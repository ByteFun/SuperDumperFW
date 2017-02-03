// Модуль передачи данных по USB

#include "usb-cdc/link.h"
#include "utils/utils.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"

// Окружение приемника
typedef struct
{
	uint8_t  Busy;
	uint8_t  Mode;
	uint8_t  Force;
	uint8_t  Temp[12];
	uint32_t AnsPtr;
	uint32_t Sz;
	uint32_t CRC32;
} TUSBRecv;
static TUSBRecv USBRecv;

//--------------------------------------------------------------
// Инициализация USB-OTG-порта как CDC-устройство
// (Виртуальный COM порт)
//--------------------------------------------------------------
void UB_USB_CDC_Init(void)
{	// Инит
	for (USBRecv.Mode = 0; USBRecv.Mode < 12; USBRecv.Mode++) { USBRecv.Temp[USBRecv.Mode] = 0; }
	USBRecv.Busy = 0; USBRecv.Mode = 0;
	USB_CDC_STATUS=USB_CDC_DETACHED;
	USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
}

//--------------------------------------------------------------
// читать состояние интерфейса USB
// Возвращаемое значение :
// ->  USB_CDC_NO_INIT =0, // интерфейс USB не инициализирован
// ->  USB_CDC_DETACHED,   // USB-соединение инициализировано
// ->  USB_CDC_CONNECTED   // USB-Соединение установлено
//--------------------------------------------------------------
USB_CDC_STATUS_t UB_USB_CDC_GetStatus(void)
{
	return (USB_CDC_STATUS);
}

// Проверка доступных данных
uint8_t USB_Check()
{
	if (USBRecv.Mode == 3)
	{	// Принято - выдаем флаг освобождаем
		for (USBRecv.Mode = 0; USBRecv.Mode < 12; USBRecv.Mode++) { USBRecv.Temp[USBRecv.Mode] = 0; }
		USBRecv.Busy = 0xFF; USBRecv.Mode = 0;
		return ( 0xFF );
	}
	else
	{	// Не готово еще
		return ( 0 );
	}
}
// Установка флага занятости
void USB_ClearBusy()
{
	USBRecv.Busy = 0;
}

//------------------------------------------------------------------------------
// Прием данных по USB
void USB_Read(uint8_t *Buf, uint32_t Size)
{	// Локали
	uint32_t Pos,Cnt;
	// Работаем
	while ((USBRecv.Mode < 3) && (Size > 0))
	{	// Перебираем буфер
		for (Pos = 0; Pos < Size; Pos++)
		{	// Логика работы
			switch (USBRecv.Mode)
			{	// Пытаемся синхронизироваться
				case 0 :
				{	// Заносим данные и вращаем барабан :)
					for (Cnt = 0; Cnt < 11; Cnt++) { USBRecv.Temp[Cnt] = USBRecv.Temp[Cnt + 1]; }
					USBRecv.Temp[11] = *(Buf + Pos);
					// Проверяемся на синхротокен и размер
					if ((*((uint32_t *) &USBRecv.Temp[0]) == 0x4F434E49) && (*((uint32_t *) &USBRecv.Temp[4]) == 0x474E494D))
					{	// Синхра есть, пробуем размер
						if (*((uint32_t *) &USBRecv.Temp[8]) <= 0x10000)
						{	// Можно принимать
							USBRecv.Sz = *((uint32_t *) &USBRecv.Temp[8]); USBRecv.Mode = 1; USBRecv.Force = 0; USBRecv.CRC32 = 0xFFFFFFFF; USBRecv.AnsPtr = 0;
						}
					}
					break;
				}
				// Загружаем тело
				case 1 :
				{	// Установим признак системной команды
					if ((USBRecv.AnsPtr == 0) && (*(Buf + Pos) < 0x10)) { USBRecv.Force = 0xFF; }
					// Проверяем, не заняты ли мы?
					if ( !(USBRecv.Busy) || (USBRecv.Force))
					{	// Если свободны или заняты, но команда системная - заносим данные
						Data_Buf[USBRecv.AnsPtr] = *(Buf + Pos); Calc_CRC32( Data_Buf[USBRecv.AnsPtr], &USBRecv.CRC32 ); USBRecv.AnsPtr++; USBRecv.Sz--;
						if (USBRecv.Sz == 0)
						{	// Все загружено, чекаем сумму
							USBRecv.Mode = 2; USBRecv.Sz = 4;
						}
					}
					else
					{	// Иначе - бахнем ошибку
						Ans_Buf[0] = 0xFF; Ans_Buf[1] = 0xFF; USB_Write( &Ans_Buf[0], 2 );
						// И вернемся
						USBRecv.Mode = 0; USBRecv.Sz = 0; USBRecv.CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { USBRecv.Temp[Cnt] = 0; }
					}
					break;
				}
				// Загружаем CRC32
				case 2 :
				{	// Заносим сумму в буфер со сдвигом
					for (Cnt = 0; Cnt < 3; Cnt++) { USBRecv.Temp[Cnt] = USBRecv.Temp[Cnt + 1]; }
					USBRecv.Temp[3] = *(Buf + Pos); USBRecv.Sz--;
					if (USBRecv.Sz == 0)
					{	// Проверяем сумму
						if (*((uint32_t *) &USBRecv.Temp[0]) == USBRecv.CRC32)
						{	// Сумма совпала - выходим
							USBRecv.Mode = 3;
						}
						else
						{	// Сумма не совпала - начинай с начала
							USBRecv.Mode = 0; USBRecv.Sz = 0; USBRecv.CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { USBRecv.Temp[Cnt] = 0; }
						}
					}
					break;
				}
				// Заглушка
				default :
				{	// Возвращаем на начало
					USBRecv.Mode = 0; USBRecv.Sz = 0; USBRecv.CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { USBRecv.Temp[Cnt] = 0; }
				}
			}
		}
	}
}

// Передача пакета по USB
void USB_Write(uint8_t *Buf, uint32_t Size)
{	// Переменные
	uint32_t Send,CRC32;
	uint8_t  Temp[12];
	uint8_t  Dat;
	// Можно слать?
	if ((USB_CDC_STATUS == USB_CDC_CONNECTED) && (Size > 0))
	{	// Работаем, посылаем синхру и размер
		*((uint32_t *) &Temp[0]) = 0x4F434E49;
		*((uint32_t *) &Temp[4]) = 0x474E494D;
		*((uint32_t *) &Temp[8]) = Size;
		for (Send = 0; Send < 12; Send++) { UB_VCP_DataTx(Temp[Send]); }
		Send = 0; CRC32 = 0xFFFFFFFF;
		while (Send < Size)
		{	// Посылаем данные
			Dat = *(Buf); Buf++; Send++;
			Calc_CRC32( Dat, &CRC32 ); UB_VCP_DataTx( Dat );
			// Задержка
			Delay(750);
		}
		// Посылаем контрольку
		*((uint32_t *) &Temp[0]) = CRC32;
		for (Send = 0; Send < 4; Send++) { UB_VCP_DataTx(Temp[Send]); }
		CRC32 = 0x00000000;
	}
}
