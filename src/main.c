
////////////////////////////////////////////////////////////////////
// Super Dumper
////////////////////////////////////////////////////////////////////

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "misc.h"

#include "usbd_usr.h"
#include "usbd_cdc_core.h"
#include "usb_dcd_int.h"
#include "usb-cdc/usbd_desc.h"

#include "utils/utils.h"
#include "dumper/dendy.h"
#include "dumper/sega.h"
#include "usb-cdc/link.h"

// Версия прошивки и билда
const	uint8_t  FWVer     = 0x03;
		uint8_t  BDVer     = 0x01;
		uint32_t clBlack   = 0x000000;
		uint32_t clBlue    = 0x0000FF;
		uint32_t clRed     = 0x00FF00;
		uint32_t clMagenta = 0x00FFFF;
		uint32_t clGreen   = 0xFF0000;
		uint32_t clCyan    = 0xFF00FF;
		uint32_t clYellow  = 0xFFFF00;
		uint32_t clWhite   = 0xFFFFFF;
		uint32_t clGray    = 0x101010;

// Глобальный буфер данных
static uint8_t  Mode, Status;
static uint32_t DendyLED, SegaLED, BSize, Cnt, CRC32, Adr, Size;
static AccessMode AcMode;

// Прерывание USB
void OTG_FS_IRQHandler(void)
{
	USBD_OTG_ISR_Handler( &USB_OTG_dev );
}

// Начальная настройка оборудования
void HWInit()
{	// Локальные переменные
	GPIO_InitTypeDef	GPIO_InStr;
	// Разрешим питальник
	RCC_APB1PeriphClockCmd( RCC_APB1ENR_PWREN, ENABLE );
	// Настроим клоки SYSCFG
	RCC_APB2PeriphClockCmd( RCC_APB2ENR_SYSCFGEN, ENABLE );
	// Тактируем все порты от A до E,
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE );
	// Настраиваем порт GPIOA (вход)
	GPIO_InStr.GPIO_Pin = 0x01C0;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InStr );
	// Настраиваем порт GPIOA (выход)
	GPIO_InStr.GPIO_Pin = 0x003F;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InStr );
	GPIO_ResetBits( GPIOA, 0x0001 );
	GPIO_SetBits( GPIOA, 0x003E );
	// Настраиваем порт GPIOB (выход)
	GPIO_InStr.GPIO_Pin = 0xFFE7;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOB, &GPIO_InStr );
	GPIO_ResetBits( GPIOB, 0x0004 );
	GPIO_SetBits( GPIOB, 0xFFE3 );
	// Настраиваем порт GPIOC (вход)
	GPIO_InStr.GPIO_Pin = 0x0400;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InStr );
	// Настраиваем порт GPIOC (выход)
	GPIO_InStr.GPIO_Pin = 0x03FF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InStr );
	GPIO_ResetBits( GPIOC, 0x00FF );
	GPIO_SetBits( GPIOC, 0x0300 );
	// Настраиваем порт GPIOD (вход)
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	// Настраиваем порт GPIOE (выход)
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOE, &GPIO_InStr );
	GPIO_ResetBits( GPIOE, 0xFFFF );
}
// Посылаем ошибку - 61 байт 0xFF
void SendError()
{	// Переменные
	uint8_t Cnt;
	// Заполняем
	for (Cnt = 0; Cnt < 61; Cnt++) { Data_Buf[Cnt] = 0xFF; }
	USB_Write( &Data_Buf[0], 61 );
}

// Главный старт
int main(void)
{	// Инит оборудования
	HWInit(); Status = 0x00;
	// Стартуем
	Dendy_Off(); Sega_Off(); DendyLED = clGray; SegaLED = clGray;
	USBD_Init( &USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb );
	// Работаем
	while (1)
	{	// Обновим лампочки
		UpdateLEDs( DendyLED, SegaLED );
		// Прием данных с ожидаем
		while (!USB_Check()) { }
		// Анализ данных, исполнение команд
		switch ( Data_Buf[0] )
		{	// =================================================================
			// Системные команды
			// =================================================================
			case 0x00 :
			{	// Команда 0x00: Сброс
				Dendy_Off(); Sega_Off();
				Data_Buf[1] = FWVer; Data_Buf[2] = BDVer;
				USB_Write( &Data_Buf[0], 3 );
				DendyLED = clGray; SegaLED = clGray;
				Status = 0x00; Mode = 0x00;
				USB_ClearBusy();
				break;
			}

			case 0x01 :
			{	// Команда 0x01: Установить режим
				switch ( Data_Buf[1] )
				{	// Режимы
					case 0x01 : { Mode = 0x01; DendyLED = clBlue; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED ); Dendy_On(); break; }
					case 0x02 : { Mode = 0x02; DendyLED = clBlack; SegaLED = clBlue; UpdateLEDs( DendyLED, SegaLED ); Sega_On(); break; }
					default : { Mode = 0x00; DendyLED = clGray; SegaLED = clGray; UpdateLEDs( DendyLED, SegaLED ); Dendy_Off(); Sega_Off(); break; }
				}
				USB_Write( &Data_Buf[0], 2 );
				USB_ClearBusy();
				break;
			}

			case 0x02 :
			{	// Команда 0x02: Получить статус
				switch ( Mode )
				{	// Режимы
					case 0x01 : { Data_Buf[1] = 0x01; break; }
					case 0x02 : { Data_Buf[1] = 0x02; break; }
					default : { Data_Buf[1] = 0x00; break; }
				}
				Data_Buf[2] = Status & 0xF0;
				if (CART_In()) { Data_Buf[2] |= 0x01; }
				if (PRG_IRQ_In())  { Data_Buf[2] |= 0x02; }
				if (CHR_VA10_In()) { Data_Buf[2] |= 0x04; }
				if (CHR_VRAM_In()) { Data_Buf[2] |= 0x08; }
				Data_Buf[3] = (SegaLED >> 16) & 0xFF; Data_Buf[4] = (SegaLED >> 8) & 0xFF; Data_Buf[5] = SegaLED & 0xFF;
				Data_Buf[6] = (DendyLED >> 16) & 0xFF; Data_Buf[7] = (DendyLED >> 8) & 0xFF; Data_Buf[8] = DendyLED & 0xFF;
				USB_Write( &Data_Buf[0], 9 );
				// Статус
				if ((Status & 0x80) == 0x00)
				{	// Отложенная задача закончилась, возвращаем лампочки
					switch ( Mode )
					{	// Режимы
						case 0x01 : { DendyLED = clBlue; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED ); break; }
						case 0x02 : { DendyLED = clBlack; SegaLED = clBlue; UpdateLEDs( DendyLED, SegaLED ); break; }
						default : { Mode = 0x00; DendyLED = clGray; SegaLED = clGray; UpdateLEDs( DendyLED, SegaLED ); Dendy_Off(); Sega_Off(); break; }
					}
				}
				USB_ClearBusy();
				break;
			}

			// =================================================================
			// Команды режима Денди
			// =================================================================
			case 0x40 :
			{	// Команда 0x40: Получить карту CRC32 в пространстве PRG
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clGreen; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем пространство
					Dendy_Read( &Data_Buf[0], 0x000000, 0x10000 );
					// Вычисляем CRC32
					for (BSize = 0; BSize < 8; BSize++)
					{	// Перебираем блоки
						CRC32 = 0xFFFFFFFF;
						for (Cnt = 0x0000; Cnt < 0x2000; Cnt++) { Calc_CRC32( Data_Buf[BSize * 0x2000 + Cnt], &CRC32 ); }
						*( (uint32_t *) &Data_Buf[BSize * 4 + 1] ) = CRC32;
					}
					// Посылаем
					Data_Buf[0] = 0x40; USB_Write( &Data_Buf[0], 33 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x41 :
			{	// Получить карту CRC32 в пространстве CHR
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clGreen; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем пространство
					Dendy_Read( &Data_Buf[0], 0x010000, 0x2000 );
					// Вычисляем CRC32
					for (BSize = 0; BSize < 8; BSize++)
					{	// Перебираем блоки
						CRC32 = 0xFFFFFFFF;
						for (Cnt = 0x0000; Cnt < 0x0400; Cnt++) { Calc_CRC32( Data_Buf[BSize * 0x0400 + Cnt], &CRC32 ); }
						*( (uint32_t *) &Data_Buf[BSize * 4 + 1] ) = CRC32;
					}
					// Посылаем
					Data_Buf[0]  = 0x41; USB_Write( &Data_Buf[0], 33 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x42 :
			{	// Прочитать основную память
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clGreen; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint16_t *) &Data_Buf[1] );
					Size = *( (uint16_t *) &Data_Buf[3] ); if (Size == 0) { Size = 0x10000; }
					Dendy_Read( &Data_Buf[9], Adr, Size );
					// Заголовок
					Data_Buf[0] = 0x42;
					*((uint16_t *)&Data_Buf[1]) = Adr & 0xFFFF;
					*((uint16_t *)&Data_Buf[3]) = Size & 0xFFFF;
					// Считаем контрольку
					CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < Size; Cnt++) { Calc_CRC32( Data_Buf[Cnt + 9], &CRC32 ); }
					*( (uint32_t *) &Data_Buf[5] ) = CRC32;
					// Выдаем результат
					USB_Write( &Data_Buf[0], Size + 9 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x43 :
			{	// Прочитать дополнительную память
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clGreen; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint16_t *) &Data_Buf[1] );
					Size = *( (uint16_t *) &Data_Buf[3] ); if (Size == 0) { Size = 0x10000; }
					Dendy_Read( &Data_Buf[9], (Adr & 0x3FFF) + 0x10000, Size );
					// Заголовок
					Data_Buf[0] = 0x43;
					*( (uint16_t *) &Data_Buf[1] ) = Adr & 0xFFFF;
					*( (uint16_t *) &Data_Buf[3] ) = Size & 0xFFFF;
					// Считаем контрольку
					CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < Size; Cnt++) { Calc_CRC32( Data_Buf[Cnt + 9], &CRC32 ); }
					*( (uint32_t *) &Data_Buf[5] ) = CRC32;
					// Выдаем результат
					USB_Write( &Data_Buf[0], Size + 9 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x44 :
			{	// Записать основную память
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clRed; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint16_t *) &Data_Buf[1] );
					Size = *( (uint16_t *) &Data_Buf[3] ); if (Size == 0) { Size = 0x10000; }
					Dendy_Write( &Data_Buf[5], Adr, Size );
					// Заголовок
					Data_Buf[0] = 0x44;
					*( (uint16_t *) &Data_Buf[1] ) = Adr & 0xFFFF;
					*( (uint16_t *) &Data_Buf[3] ) = Size & 0xFFFF;
					// Выдаем результат
					USB_Write( &Data_Buf[0], 5 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x45 :
			{	// Записать дополнительную память
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clRed; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint16_t *) &Data_Buf[1] );
					Size = *( (uint16_t *) &Data_Buf[3] ); if (Size == 0) { Size = 0x10000; }
					Dendy_Write( &Data_Buf[5], (Adr & 0x3FFF) + 0x10000, Size );
					// Заголовок
					Data_Buf[0] = 0x45;
					*( (uint16_t *) &Data_Buf[1] ) = Adr & 0xFFFF;
					*( (uint16_t *) &Data_Buf[3] ) = Size & 0xFFFF;
					// Выдаем результат
					USB_Write( &Data_Buf[0], 5 );
					DendyLED = clBlue; SegaLED = clBlack;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
				break;
			}

			case 0x46 :
			{	// Запись команды и данных во FLASH, проверка команд
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clRed; SegaLED = clBlack; UpdateLEDs( DendyLED, SegaLED );
					// Заполняем переменные
					Status = Data_Buf[1]; Data_Buf[1] = 0x00;
					// Запускаем Delayed

					// Отвечаем
					USB_Write( &Data_Buf[0], 2 );
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
		    	break;
			}

			case 0x47 :
			{	// Имитация сброса DENDY
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					Dendy_Reset();
					// Отвечаем
					USB_Write( &Data_Buf[0], 1 );
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
		    	break;
			}

			// =================================================================
			// Команды режима Сега
			// =================================================================
			case 0x80 :
			{	// Имитация сброса SEGA
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					if ( Data_Buf[1] ) { Sega_Reset( ENABLE ); } else { Sega_Reset( DISABLE ); }
					// Отвечаем
					USB_Write( &Data_Buf[0], 1 );
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
		    	break;
			}

			case 0x81 :
			{	// Прочитать память SEGA
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clBlack; SegaLED = clGreen; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint32_t *) &Data_Buf[1] ); Adr &= 0xFFFFFF;
					Size = *( (uint16_t *) &Data_Buf[5] ); if (Size == 0) { Size = 0x10000; }
					switch ( Data_Buf[7] & 0x03 )
					{	// Режим доступа
						case 0x0 : { AcMode = mdByteNo; break; }
						case 0x1 : { AcMode = mdByteLow; break; }
						case 0x2 : { AcMode = mdByteHigh; break; }
						case 0x3 : { AcMode = mdByteBoth; break; }
					}
					Sega_Read( &Data_Buf[11], AcMode, Adr, Size );
					// Заголовок
					Data_Buf[0] = 0x81;
					*( (uint32_t *) &Data_Buf[1] ) = Adr & 0xFFFFFF;
					*( (uint16_t *) &Data_Buf[5] ) = Size & 0xFFFF;
					// Считаем контрольку
					CRC32 = 0xFFFFFFFF; for (Cnt = 0; Cnt < Size; Cnt++) { Calc_CRC32( Data_Buf[Cnt + 11], &CRC32 ); }
					*( (uint32_t *) &Data_Buf[7] ) = CRC32;
					// Выдаем результат
					USB_Write( &Data_Buf[0], Size + 11 );
					DendyLED = clBlack; SegaLED = clBlue;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
		    	break;
			}

			case 0x82 :
			{	// Записать память SEGA
				if ((Status & 0x80) == 0x00)
				{	// Отложенных задач нет
					DendyLED = clBlack; SegaLED = clRed; UpdateLEDs( DendyLED, SegaLED );
					// Считываем данные
					Adr = *( (uint32_t *) &Data_Buf[1] ); Adr &= 0xFFFFFF;
					Size = *( (uint16_t *) &Data_Buf[5] ); if (Size == 0) { Size = 0x10000; }
					switch ( Data_Buf[7] & 0x03 )
					{	// Режим доступа
						case 0x0 : { AcMode = mdByteNo; break; }
						case 0x1 : { AcMode = mdByteLow; break; }
						case 0x2 : { AcMode = mdByteHigh; break; }
						case 0x3 : { AcMode = mdByteBoth; break; }
					}
					Sega_Write( &Data_Buf[8], AcMode, Adr, Size );
					// Заголовок
					Data_Buf[0] = 0x82;
					*( (uint32_t *) &Data_Buf[1] ) = Adr & 0xFFFFFF;
					*( (uint16_t *) &Data_Buf[5] ) = Size & 0xFFFF;
					// Выдаем результат
					USB_Write( &Data_Buf[0], 7 );
					DendyLED = clBlack; SegaLED = clBlue;
				}
				else
				{	// Мы заняты
					SendError();
				}
				USB_ClearBusy();
		    	break;
			}

			// =================================================================
			//	Заглушка
			// =================================================================
			default :
			{	// Посылаем ошибку
				SendError(); USB_ClearBusy(); break;
			}
		}
	}
}

// Задача обмена по USB
void vTaskDelayed ( void *pvParameters )
{	// Переменные
	uint8_t Count, Dat;
	// Начинаемся
	Status |= 0x80;
	if (Mode == 0x01) { Dendy_Manual(); }
//	if (Mode == 0x02) { Sega_Manual(); }
	// Ветка алгоритмов
	switch (Status)
	{	// Алгоритм 0x00: простая запись команд
		case 0x00 :
		{	// Работаем с буфером, забираем счетчик команд
			if (Data_Buf[2])
			{	// Команды есть
				for (Count = 0; Count < Data_Buf[2]; Count++)
				{	// Вычитываем адрес
					Adr = (Data_Buf[(Count * 3) + 4] * 0x100 + Data_Buf[(Count * 3) + 3]) | 0x8000;
					// Посылаем запись
					Dendy_ManualWrite( &Data_Buf[(Count * 3) + 5], Adr, 1 );
				}
				// Опрашиваем биты
				while ( 1 )
				{	// Считываем статус
					Dendy_ManualRead( &Dat, Adr, 1);
					// Проверяем DQ7:
					if (Dat & 0x80)
					{	// DQ7 = 1
						Status = 0x00; break;
					}
					else
					{	// DQ7 = 0
						// Проверяем DQ5:
						if (Dat & 0x20)
						{	// DQ5 = 1
							// Повторно считываем статус
							Dendy_ManualRead( &Dat, Adr, 1);
							// Проверяем DQ7:
							if (Dat & 0x80)
							{	// DQ7 = 1
								Status = 0x00; break;
							}
							else
							{	// DQ7 = 0
								Status = 0x70; break;
							}
						}
					}
				}
			}
			else
			{	// Команд нет - ошибка
				Status = 0x70;
			}
			break;
		}
		// Если ничего нет
		default :
		{	// Ничего не делаем
			Status = 0x00; break;
		}
	}
	// Восстановим режим
	if (Mode == 0x01) { Dendy_On(); }
	USB_ClearBusy();
}
