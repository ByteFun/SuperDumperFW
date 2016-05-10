// Модуль передачи данных по USB

#include "link.h"
#include "utils/utils.h"
#include "usbd_hid_core.h"

//------------------------------------------------------------------------------
// Прием данных по USB
void USB_Read(uint8_t *Buf, uint32_t Size)
{	// Локальные переменные
	uint8_t  *Ans;
	uint8_t  Cnt,Sum,Sz,Cont,Copy;
	uint32_t Loaded;
	// Работаем
	Sz = 0; Ans = Buf; Loaded = 0; Cont = 0; Copy = 0;
	// Ожидаем приема
	while (1)
	{	// Пытаемся загрузить
		USB_Rx[0] = 0; while (USB_Rx[0] == 0) {}
		Sz = USB_Rx[0];
		// Анализируем
		if ((Sz & 0x3F)> 0)
		{	// Есть данные, логика склейки пакетов
			if (Cont == 0)
			{	// Начинается прием
				if (((Sz & 0xC0) == 0x00) || ((Sz & 0xC0) == 0xC0))
				{	// Первый или последний пакет
					Cont = 1; Copy = 1;
				}
				else
				{	// Ошибка = обнуляемся
					Ans = Buf; Loaded = 0; Copy = 0;
				}
			}
			else
			{	// Продолжается прием
				if (((Sz & 0xC0) == 0x40) || ((Sz & 0xC0) == 0xC0))
				{	// Все верно, можно копировать
					Copy = 1;
				}
				else
				{	// Ошибка = обнуляемся
					Ans = Buf; Loaded = 0; Cont = 0; Copy = 0;
				}
			}
			// Перенос данных
			if (Copy != 0)
			{	// Копируем данные
				for (Cnt = 0; Cnt < (Sz & 0x3F); Cnt++)
				{	// Если есть место, грузим
					if (Loaded <= Size)
					{	// Копируем данные
						*(Ans) = USB_Rx[Cnt+1]; Ans++; Loaded++;
					}
				}
			}
			// Условие выхода
			if ((Loaded >= Size) || ((Sz & 0xC0) == 0xC0)) { break; }
		}
	}
}

// Передача пакета по USB
void USB_Write(uint8_t *Buf, uint32_t Size)
{	// Переменные
	uint8_t Cnt,Sum,Sz;
	uint32_t Send;
	// Работаем
	Send = 0; if (Size > 0x10000) { Size = 0x10000; }
	USB_Tx[1] = 0x00;
	if (Size > 0)
	{	// Размер корректен, работаем
		while (Send < Size)
		{	// Размер текущего пакета
			if ((Size-Send) > 60)
			{	// Заполняем по максимуму
				Sz = 61;
			}
			else
			{	// Заполняем остаток
				Sz = Size - Send;
			}
			// Заполняем пакет
			if ((Size > 60) && (Send == 0))
			{	// Размер первоначального пакета
				USB_Tx[1] = (Sz & 0x3F);
			}
			else
			if (((Size-Send) > 61) && (Send > 0))
			{	// Размер очередного пакета
				USB_Tx[1] = (Sz & 0x3F) | 0x40;
			}
			else
			{	// Размер последнего пакета
				USB_Tx[1] = Sz | 0xC0;
			}
			USB_Tx[0] = (0x100-USB_Tx[1]) & 0xFF; Sum = 0;
			for (Cnt = 0; Cnt < Sz; Cnt++)
			{	// Перенос данных
				USB_Tx[Cnt+2] = *(Buf); Sum = Sum + *(Buf); Buf++;
			}
			USB_Tx[Sz+2] = Sum; Send += Sz;
			// Посылаем
			USBD_HID_SendReport( &USB_Device_dev, &USB_Tx[0], HID_IN_PACKET );
			// Ждем предыдущую операцию
			while ((USBD_HID_Empty( &USB_Device_dev ) == 0) && (Send < Size)) { Delay(1); }
		}
	}
}
