// Модуль передачи данных по USB

#include "usb-cdc/link.h"
#include "utils/utils.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"

//--------------------------------------------------------------
// Инициализация USB-OTG-порта как CDC-устройство
// (Виртуальный COM порт)
//--------------------------------------------------------------
void UB_USB_CDC_Init(void)
{
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
  return(USB_CDC_STATUS);
}


//--------------------------------------------------------------
// Посылка строки через интерфейс OTG USB
// команда в конце : [NONE, LFCR, CRLF, LF, CR]
// Возвращаемое значение :
//  -> ERROR   , строка не отправлена
//  -> SUCCESS , строка отправлена
//--------------------------------------------------------------
ErrorStatus UB_USB_CDC_SendString(char *ptr, USB_CDC_LASTBYTE_t end_cmd)
{

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
  // передавать только, когда соединение установлено
    return(ERROR);
  }

  // Отправить строку целиком
  while (*ptr != 0) {
    UB_VCP_DataTx(*ptr);
    ptr++;
  }
  // отправка конечного идентификатора
  if(end_cmd==LFCR) {
    UB_VCP_DataTx(0x0A); // Возврат строки
    UB_VCP_DataTx(0x0D); // Перевод каретки
  }
  else if(end_cmd==CRLF) {
    UB_VCP_DataTx(0x0D); // Перевод каретки
    UB_VCP_DataTx(0x0A); // Возврат строки
  }
  else if(end_cmd==LF) {
    UB_VCP_DataTx(0x0A); // Возврат строки
  }
  else if(end_cmd==CR) {
    UB_VCP_DataTx(0x0D); // Перевод каретки
  }

  return(SUCCESS);
}



//--------------------------------------------------------------
// получение строки через интерфейс OTG USB
// (прием реализуется с помощью прерывания)
// Эта функция должна опрашиваться циклически
// Возвращаемое значение :
//  -> если USB не готов = RX_USB_ERR
//  -> если ничего не получено = RX_EMPTY
//  -> если строка получена = RX_READY -> Строка в *ptr
//--------------------------------------------------------------
USB_CDC_RXSTATUS_t UB_USB_CDC_ReceiveString(char *ptr)
{
  uint16_t check;

  if(USB_CDC_STATUS!=USB_CDC_CONNECTED) {
    // прием только тогда, когда соединение установлено
    return(RX_USB_ERR);
  }

  check=UB_VCP_StringRx(ptr);
  if(check==0) {
    ptr[0]=0x00;
    return(RX_EMPTY);
  }

  return(RX_READY);
}

//------------------------------------------------------------------------------
// Прием данных по USB
void USB_Read(uint8_t *Buf, uint32_t Size)
{	// Локальные переменные
	uint8_t  *Ans;
	uint8_t  Temp[12];
	uint8_t  Dat,Mode;
	uint32_t Sz,Cnt,CRC32;
	// Работаем
	*(Buf) = 0xFF; Ans = Buf; Mode = 0; Sz = 0; CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { Temp[Cnt] = 0; }
	// Ожидаем приема
	while ((USB_CDC_STATUS == USB_CDC_CONNECTED) && (Mode < 3))
	{	// Примем байт
		if (VCP_DataRxE( &Dat, 1 ) == 1)
		{	// Логика работы
			switch (Mode)
			{	// Пытаемся синхронизироваться
				case 0 :
				{	// Заносим данные и вращаем барабан :)
					for (Cnt = 0; Cnt < 11; Cnt++) { Temp[Cnt] = Temp[Cnt+1]; }
					Temp[11] = Dat;
					// Проверяемся на синхротокен и размер
					if ((*((uint32_t *) &Temp[0]) == 0x4F434E49) && (*((uint32_t *) &Temp[4]) == 0x474E494D))
					{	// Синхра есть, пробуем размер
						if (*((uint32_t *) &Temp[8]) <= Size)
						{	// Можно принимать
							Sz = *((uint32_t *) &Temp[8]); Mode = 1; CRC32 = 0xFFFFFFFF; Ans = Buf;
						}
					}
					break;
				}
				// Загружаем тело
				case 1 :
				{	// Заносим данные
					*(Ans) = Dat; Calc_CRC32( Dat, &CRC32 ); Ans++; Sz--;
					if (Sz == 0)
					{	// Все загружено, чекаем сумму
						Mode = 2; Sz = 4;
					}
					break;
				}
				// Загружаем CRC32
				case 2 :
				{	// Заносим сумму в буфер со сдвигом
					for (Cnt = 0; Cnt < 3; Cnt++) { Temp[Cnt] = Temp[Cnt+1]; }
					Temp[3] = Dat; Sz--;
					if (Sz == 0)
					{	// Проверяем сумму
						if (*((uint32_t *) &Temp[0]) == CRC32)
						{	// Сумма совпала - выходим
							Mode = 3;
						}
						else
						{	// Сумма не совпала - начинай с начала
							Ans = Buf; Mode = 0; Sz = 0; CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { Temp[Cnt] = 0; }
						}
					}
					break;
				}
				// Заглушка
				default :
				{	// Возвращаем на начало
					Ans = Buf; Mode = 0; Sz = 0; CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < 12; Cnt++) { Temp[Cnt] = 0; }
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
