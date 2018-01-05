// Набор функций для дампинга картриджей Сеги

#include "dumper/sega.h"

// Отключение режима Сега
void Sega_Off()
{	// Отключаем шину
	ADR_BUS_Off(); HI_BUS_Off(); LOW_BUS_Off();
	// Деинит управляющих ног Сеги
	MRES_Off(); VRES_Off(); AS_Off(); ASEL_Off(); CE0_Off(); TIME_Off();
	CAS0_Off(); CAS2_Off(); LWR_Off(); UWR_Off();
}

// Включение режима Сега
void Sega_On()
{	// Инит общих управляющих ног
	ADR_BUS_On(); HI_BUS_Off(); LOW_BUS_Off();
	// Инит управляющих ног Dendy
	MRES_Off(); VRES_Off(); AS_Off(); ASEL_Off(); CE0_Off(); TIME_Off();
	CAS0_Off(); CAS2_Off(); LWR_Off(); UWR_Off();
	// Сбрасываем картридж
	Sega_Reset( ENABLE );
}

// Сброс картриджа
void Sega_Reset( FunctionalState Mode )
{	// Сброс
	if (Mode) { MRES_On(); }
	VRES_On(); Delay( 0x3567DFE );
	GPIOC->ODR &= 0xFF00; GPIOE->ODR = 0x0000;
	MRES_Off(); VRES_Off(); Delay( 0x3567DFE );
}

// Чтение данных из картриджа в буфер
void Sega_Read( uint8_t *PBuf, AccessMode Mode, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	FunctionalState		AdrSet;
	// Начальные установки
	HI_BUS_In(); LOW_BUS_In();
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	HI_BUS_On(); LOW_BUS_On();
	AdrSet = ENABLE;
	// Подготовка адреса
	switch ( Mode )
	{	// Только старший байт - адрес должен быть четным
		case mdByteHigh : { Start &= 0xFFFFFE; break; }
		// Только младший байт - адрес должен быть нечетным
		case mdByteLow : { Start |= 0x000001; break; }
	}
	while (Size)
	{	// Нужно выставить адрес?
		if ( AdrSet == ENABLE )
		{	// Выставляем адрес
			GPIOC->ODR = (GPIOC->ODR & 0xFF00) | ((Start >> 16) & 0x00FF);
			GPIOE->ODR = Start & 0x0000FFFF;
			// Обрабатываем сигналы адресации
			AS_On();
			if (Start & 0x00800000) { ASEL_Off(); } else { ASEL_On(); }
			if (Start & 0x00C00000) { CE0_Off(); } else { CE0_On(); }
			if ((Start & 0x00FFFF00) == 0x00A13000) { TIME_On(); } else { TIME_Off(); }
			// Активируем строб чтения
			CAS2_On(); Delay = 1; while (Delay) { Delay--; }
			CAS0_On(); Delay = 1; while (Delay) { Delay--; }
			// Снимаем флаг
			AdrSet = DISABLE;
		}
		// Забираем данные согласно режиму
		switch ( Mode )
		{	// Только младший байт
			case mdByteLow :	{	// Считываем младший байт
									*(PBuf) = GPIOD->IDR & 0x00FF;
									// Корректируем счетчики
									PBuf++; Start += 2; Size--;
									AdrSet = ENABLE;
									break;
								}
			// Только старший байт
			case mdByteHigh :	{	// Считываем старший байт
									*(PBuf) = (GPIOD->IDR >> 8) & 0x00FF;
									// Корректируем счетчики
									PBuf++; Start += 2; Size--;
									AdrSet = ENABLE;
									break;
								}
			// Оба байта
			case mdByteBoth :	{	// Анализируем текущий адрес
									if ( Start & 0x000001 )
									{	// Это нечетный адрес (младший байт)
										*(PBuf) = GPIOD->IDR & 0x00FF;
										// Корректируем счетчики
										PBuf++; Start++; Size--;
										// Требуется смена адреса
										AdrSet = ENABLE;
									}
									else
									{	// Это четный адрес (старший байт)
										*(PBuf) = (GPIOD->IDR >> 8) & 0x00FF;
										// Корректируем счетчики
										PBuf++; Start++; Size--;
										// Требуется смена адреса
										AdrSet = DISABLE;
									}
									break;
								}
			// Ниодного - просто считаем размер
			case mdByteNo :		{	// Обнуление
									*(PBuf) = 0;
									// Корректируем счетчики
									PBuf++; Size--;
									AdrSet = ENABLE;
									break;
								}
		}
		if ( (AdrSet == ENABLE) || !(Size) )
		{	// Деактивируем чтение
			CAS0_Off(); CAS2_Off(); TIME_Off(); CE0_Off(); ASEL_Off(); AS_Off();
		}
	}
	HI_BUS_Off(); LOW_BUS_Off();
}

// Запись данных из буфера в картридж
void Sega_Write( uint8_t *PBuf, AccessMode Mode, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	// Начальные установки
	HI_BUS_Out(); LOW_BUS_Out();
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	HI_BUS_On(); LOW_BUS_On();
	// Подготовка адреса
	switch ( Mode )
	{	// Только старший байт - адрес должен быть четным
		case mdByteHigh : { Start &= 0xFFFFFE; break; }
		// Только младший байт - адрес должен быть нечетным
		case mdByteLow : { Start |= 0x000001; break; }
	}
	while (Size)
	{	// Выставляем адрес
		GPIOC->ODR = (GPIOC->ODR & 0xFF00) | ((Start >> 16) & 0x00FF);
		GPIOE->ODR = Start & 0x0000FFFF;
		// Обрабатываем сигналы адресации
		AS_On();
		if (Start & 0x00800000) { ASEL_Off(); } else { ASEL_On(); }
		if (Start & 0x00C00000) { CE0_Off(); } else { CE0_On(); }
		if ((Start & 0x00FFFF00) == 0x00A13000) { TIME_On(); } else { TIME_Off(); }
		// Снимаем флаг
		// Пишем данные согласно режима
		switch (Mode)
		{	// Запись по младшим байтам
			case mdByteLow :	{	// Выставляем данные
									GPIOD->ODR = (GPIOD->ODR & 0xFF00) | *(PBuf);
									// Активируем строб записи младшего байта
									Delay = 1; while (Delay) { Delay -= 1; }
									LWR_On();
									// Деактивируем строб записи младшего байта
									Delay = 1; while (Delay) { Delay -= 1; }
									LWR_Off();
									// Корректируем счетчики
									PBuf++; Start += 2; Size--;
									break;
								}
			// Запись по младшим байтам
			case mdByteHigh :	{	// Выставляем данные
									GPIOD->ODR = (GPIOD->ODR & 0x00FF) | (*(PBuf) * 0x100);
									// Активируем строб записи старшего байта
									Delay = 1; while (Delay) { Delay -= 1; }
									UWR_On();
									// Деактивируем строб записи старшего байта
									Delay = 1; while (Delay) { Delay -= 1; }
									UWR_Off();
									// Корректируем счетчики
									PBuf++; Start += 2; Size--;
									break;
								}
			// Запись по обим байтам
			case mdByteBoth :	{	// Анализируем адрес
									if ( Start & 0x000001 )
									{	// Это нечетный адрес, следует записать только младший байт
										GPIOD->ODR = (GPIOD->ODR & 0xFF00) | *(PBuf);
										// Активируем строб записи младшего байта
										Delay = 1; while (Delay) { Delay -= 1; }
										LWR_On();
										// Деактивируем строб записи младшего байта
										Delay = 1; while (Delay) { Delay -= 1; }
										LWR_Off();
										// Корректируем счетчики
										PBuf++; Start++; Size--;
									}
									else
									{	// Это четный адрес, либо пишем слово целиком, либо только старший байт
										if ( Size > 1 )
										{	// Осталось больше одного байта - пишем слово целиком
											GPIOD->ODR = *(PBuf) * 0x100; PBuf++;
											GPIOD->ODR = (GPIOD->ODR & 0xFF00) | *(PBuf); PBuf++;
											// Активируем оба строба записи
											Delay = 1; while (Delay) { Delay -= 1; }
											LWR_On(); UWR_On();
											// Деактивируем оба строба записи
											Delay = 1; while (Delay) { Delay -= 1; }
											LWR_Off(); UWR_Off();
											// Корректируем счетчики
											Start += 2; Size -= 2;
										}
										else
										{	// Остался один байт - пишем только его (старший байт)
											GPIOD->ODR = (GPIOD->ODR & 0x00FF) | (*(PBuf) * 0x100);
											// Активируем строб записи старшего байта
											Delay = 1; while (Delay) { Delay -= 1; }
											UWR_On();
											// Деактивируем строб записи старшего байта
											Delay = 1; while (Delay) { Delay -= 1; }
											UWR_Off();
											// Корректируем счетчики
											PBuf++; Start++; Size--;
										}
									}
									break;
								}
		}
		// Деактивация выбора
		AS_Off(); ASEL_Off(); CE0_Off(); TIME_Off();
	}
	// Начальное состояние
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	HI_BUS_Off(); LOW_BUS_Off(); HI_BUS_In(); LOW_BUS_In();
}

