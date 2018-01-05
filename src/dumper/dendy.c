// Набор функций для дампинга картриджей Денди

#include "dumper/dendy.h"

// Отключение режима Денди
void Dendy_Off()
{	// Параметры
	GPIO_InitTypeDef	GPIO_InStr;
	// Отключаем шину
	ADR_BUS_Off(); CHR_BUS_Off(); PRG_BUS_Off();
	// Деинит управляющих ног Dendy
	PRG_F2_Off(); PRG_RnW_Rd(); CHR_PRD_Off(); CHR_PWR_Off(); CHR_nPA13_Off();
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, DISABLE); TIM5->CR1 = 0x0000;
	GPIO_InStr.GPIO_Pin = 0x0001;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InStr );
	GPIO_ResetBits( GPIOA, 0x0001 );
}

// Включение режима Денди
void Dendy_On()
{	// Параметры
	GPIO_InitTypeDef	GPIO_InStr;
	// Инит общих управляющих ног
	ADR_BUS_On(); CHR_BUS_Off(); PRG_BUS_Off();
	// Инит управляющих ног Dendy
	PRG_F2_Off(); PRG_RnW_Rd(); CHR_PRD_Off(); CHR_PWR_Off(); CHR_nPA13_Off();
	GPIO_InStr.GPIO_Pin = 0x0001;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InStr );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource0, GPIO_AF_TIM5 );
	// F2: ____------- 200ns/360ns = 560ns/1.786MHz
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE); TIM5->PSC = 0x0000; TIM5->ARR = 0x0000002E; TIM5->CCR1 = 0x0000001E; TIM5->CCMR1 = 0x0060;
	TIM5->CCMR2 = 0x0000; TIM5->CCER = 0x0001; TIM5->SMCR = 0x0000; TIM5->DIER = 0x0000; TIM5->CR2 = 0x0000; TIM5->CR1 = 0x0001;
	// Сбрасываем картридж
	Dendy_Reset();
}

// Включить программный режим денди
void Dendy_Manual()
{	// Параметры
	GPIO_InitTypeDef	GPIO_InStr;
	// Инит общих управляющих ног
	ADR_BUS_On(); CHR_BUS_Off(); PRG_BUS_Off();
	// Инит управляющих ног Dendy
	PRG_F2_Off(); PRG_RnW_Rd(); CHR_PRD_Off(); CHR_PWR_Off(); CHR_nPA13_Off(); // PRG_ROM_Off();
	GPIO_InStr.GPIO_Pin = 0x0001;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOA, &GPIO_InStr );
	// Деактивация F2
	GPIO_ResetBits( GPIOA, 0x0001 );
}

// Сброс картриджа
void Dendy_Reset()
{	// Выключаем шину
	ADR_BUS_Off(); Delay( 0x3567DFE );
	GPIOC->ODR &= 0xFF00; GPIOE->ODR = 0x0000;
	ADR_BUS_On(); Delay( 0x3567DFE );
}

// Чтение данных PRG в буфер
__attribute__ ((naked)) void Read_PRG( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Начинаем
	__asm volatile
	(	"PUSH	{R0-R8}\n"
		// R0 = PBuf
		// R1 = Start
		// R2 = Size
		// R3 = GPIOA_IDR (Для чтения F2)
		"MOVW	R3, #0x0010\n"
		"MOVT	R3, #0x4002\n"
		// R4 = GPIOE_ODR (Для вывода адреса)
		"MOVW	R4, #0x1014\n"
		"MOVT	R4, #0x4002\n"
        // R5 = GPIOD_IDR (Для ввода данных)
		"MOVW	R5, #0x0C10\n"
		"MOVT	R5, #0x4002\n"
		// R6 = Маска $0000FFFF
		"MOVW	R6, #0xFFFF\n"
		"MOVT	R6, #0x0000\n"
		// Выделим
		"AND	R1, R1, R6, LSL #0\n"
		"AND	R2, R2, R6, LSL #0\n"
		// Синхронизируемся
		"CPSID	f\n"
		"Read_PRG_Syn0:\n"
		"LDR	R7, [R3, #0]\n"
		"ANDS	R7, R7, #1\n"
		"IT		EQ\n"
		"BEQ	Read_PRG_Syn0\n"
		"Read_PRG_Syn1:\n"
		"LDR	R7, [R3, #0]\n"
		"ANDS	R7, R7, #1\n"
		"IT		NE\n"
		"BNE	Read_PRG_Syn1\n"
        // Цикл чтения
		"Read_PRG_Loop:\n"
		// Выставляем адрес
		"STR	R1, [R4, #0]\n"
		// Ждем F2 = 1
		"Read_PRG_LWt0:\n"
		"LDR	R7, [R3, #0]\n"
		"ANDS	R7, R7, #1\n"
		"IT		EQ\n"
		"BEQ	Read_PRG_LWt0\n"
		// Увеличиваем адрес чтения
		"ADD	R1, R1, #1\n"
		// Ждем F2 = 0
		"Read_PRG_LWt1:\n"
		"LDR	R8, [R5, #0]\n"
		"LDR	R7, [R3, #0]\n"
		"ANDS	R7, R7, #1\n"
		"IT		NE\n"
		"BNE	Read_PRG_LWt1\n"
		// Сохраняем данные
		"AND	R8, R8, #0x00FF\n"
		"STRB	R8, [R0, #0]\n"
		// Следующий байт
		"ADDS	R0, R0, #1\n"
		// Счетчик байт
		"SUB	R2, R2, #1\n"
		"ANDS	R2, R2, R6, LSL #0\n"
		"IT		NE\n"
		"BNE	Read_PRG_Loop\n"
		"CPSIE	f\n"
		// Обнулим адрес
		"MOVW	R6, #0x0000\n"
		"STR	R6, [R4, #0]\n"
		"POP	{R0-R8}\n"
		"BX		LR\n"
	);
}

// Чтение данных из картриджа в буфер
void Dendy_Read( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	// Начальные установки
	PRG_RnW_Rd(); CHR_In(); PRG_In(); GPIOC->ODR &= 0xFF00;
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_On(); PRG_BUS_On();
	// Это PRG или CHR?
	if ((Start & 0x00FF0000) == 0)
	{
		Read_PRG( PBuf, Start, Size );
	}
	else
	{	// Маски адресов и размеров
		Start &= 0x00003FFF; Size &= 0x00003FFF;
		while ((Size & 0x00003FFF) != 0)
		{	// Выставляем адрес
			GPIOE->ODR = Start & 0x0000FFFF;
			// Активируем строб чтения CHR
			CHR_PRD_On();
			// Ждем
			Delay = 3; while (Delay) { Delay -= 1; }
			// Забираем данные и отключаем строб
			*(PBuf) = (GPIOD->IDR >> 8) & 0x00FF;
			// Деактивируем чтение
			CHR_PRD_Off();
			// Счетчики
			PBuf += 1; Start += 1; Size -= 1;
		}
	}
	CHR_BUS_Off(); PRG_BUS_Off();
}

// Запись данных буфера в PRG
__attribute__ ((naked)) void Write_PRG( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Начинаем
	__asm volatile
	(	"PUSH	{R0-R11}\n"
		// R0 = PBuf
		// R1 = Start
		// R2 = Size
		// R3 = GPIOA_IDR (Для чтения F2)
		"MOVW	R3, #0x0010\n"
		"MOVT	R3, #0x4002\n"
		// R4 = GPIOA_BSRR (Для управления R/W)
		"MOVW	R4, #0x0018\n"
		"MOVT	R4, #0x4002\n"
		// R5 = GPIOE_ODR (Для вывода адреса)
		"MOVW	R5, #0x1014\n"
		"MOVT	R5, #0x4002\n"
		// R6 = GPIOD_ODR (Для вывода данных)
		"MOVW	R6, #0x0C14\n"
		"MOVT	R6, #0x4002\n"
		// R7 = Маска $0000FFFF
		"MOVW	R7, 0xFFFF\n"
		"MOVT	R7, 0x0000\n"
		// R8 = Маска $00020000
		"MOVW	R8, 0x0000\n"
		"MOVT	R8, 0x0002\n"
		// R9 = Маска $00000002
		"MOVW	R9, 0x0002\n"
		"MOVT	R9, 0x0000\n"
		// R10 - данные записи
		"LDRB	R10, [R0, #0]\n"
		"AND	R10, R10, 0x00FF\n"
		// Выделим
		"AND	R1, R1, R7, LSL #0\n"
		"AND	R2, R2, R7, LSL #0\n"
		"CPSID	f\n"
		// Синхронизируемся
		"Write_PRG_Syn0:\n"
		"LDR	R11, [R3, #0]\n"
		"ANDS	R11, R11, #1\n"
		"IT		EQ\n"
		"BEQ	Write_PRG_Syn0\n"
		"Write_PRG_Syn1:\n"
		"LDR	R11, [R3, #0]\n"
		"ANDS	R11, R11, #1\n"
		"IT		NE\n"
		"BNE	Write_PRG_Syn1\n"
		// Выставляем запись
		"STR	R8, [R4, #0]\n"
		// Цикл записи
		"Write_PRG_Loop:\n"
		// Выставляем адрес
		"STR	R1, [R5, #0]\n"
		// Выставляем данные
		"STR	R10, [R6, #0]\n"
		// Ждем F2 = 1
		"Write_PRG_LWt0:\n"
		"LDR	R11, [R3, #0]\n"
		"ANDS	R11, R11, #1\n"
		"IT		EQ\n"
		"BEQ	Write_PRG_LWt0\n"
		// Увеличиваем адрес записи
		"ADD	R1, R1, #1\n"
		// Считываем новые данные
		"ADD	R0, R0, #1\n"
		"LDRB	R10, [R0, #0]\n"
		"AND	R10, R10, 0x00FF\n"
		// Ждем F2 = 0
		"Write_PRG_LWt1:\n"
		"LDR	R11, [R3, #0]\n"
		"ANDS	R11, R11, #1\n"
		"IT		NE\n"
		"BNE	Write_PRG_LWt1\n"
		// Счетчик байт
		"SUB	R2, R2, #1\n"
		"ANDS	R2, R2, R7, LSL #0\n"
		"IT		NE\n"
		"BNE	Write_PRG_Loop\n"
		// Убираем запись
		"STR	R9, [R4, #0]\n"
		"CPSIE	f\n"
		// Обнулим адрес
		"MOVW	R7, 0x0000\n"
		"STR	R7, [R5, #0]\n"
		"POP	{R0-R11}\n"
		"BX		LR\n"
	);
}

// Запись данных из буфера в картридж
void Dendy_Write( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	// Начальные установки
	PRG_RnW_Rd(); CHR_Out(); PRG_Out(); GPIOC->ODR &= 0xFF00;
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_On(); PRG_BUS_On();
	// Это PRG или CHR?
	if ((Start & 0x00FF0000) == 0)
	{
		Write_PRG( PBuf, Start, Size );
	}
	else
	{	// Маски адресов и размеров
		Start &= 0x00003FFF; Size &= 0x00003FFF;
		while ((Size & 0x00003FFF) != 0)
		{	// Выставляем адрес и данные
			GPIOE->ODR = Start & 0x0000FFFF; GPIOD->ODR = (GPIOD->ODR & 0x00FF) | (*(PBuf) * 0x100);
			// Активируем строб записи CHR
			CHR_PWR_On();
			// Ждем
			Delay = 3; while (Delay) { Delay -= 1; }
			// Деактивируем чтение
			CHR_PWR_Off();
			// Счетчики
			PBuf += 1; Start += 1; Size -= 1;
		}
	}
	// Начальное состояние
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_Off(); PRG_BUS_Off(); CHR_In(); PRG_In();
}

// Чтение данных в буфер из картриджа в ручном режиме
void Dendy_ManualRead( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	// Начальные установки
	PRG_F2_Off(); PRG_RnW_Rd(); CHR_In(); PRG_In(); GPIOC->ODR &= 0xFF00;
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_On(); PRG_BUS_On();
	// Маски адресов и размеров
	Start &= 0x0000FFFF; Size &= 0x0000FFFF;
	while ((Size & 0x0000FFFF) != 0)
	{	// Выставляем адрес
		GPIOE->ODR = Start & 0x0000FFFF;
		// Активируем строб чтения CHR
		PRG_F2_On();
		// Ждем
		Delay = 3; while (Delay) { Delay -= 1; }
		// Забираем данные и отключаем строб
		*(PBuf) = GPIOD->IDR & 0x00FF;
		// Деактивируем чтение
		PRG_F2_Off();
		// Счетчики
		PBuf += 1; Start += 1; Size -= 1;
	}
	// Начальное состояние
	PRG_F2_Off(); CHR_BUS_Off(); PRG_BUS_Off();
}

// Запись данных из буфера в картридж в ручном режиме
void Dendy_ManualWrite( uint8_t *PBuf, uint32_t Start, uint32_t Size )
{	// Переменные
	uint32_t			Delay;
	GPIO_InitTypeDef	GPIO_InStr;
	// Начальные установки
	PRG_F2_Off(); PRG_RnW_Wr(); CHR_Out(); PRG_Out(); GPIOC->ODR &= 0xFF00;
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_On(); PRG_BUS_On();
	// Маски адресов и размеров
	Start &= 0x0000FFFF; Size &= 0x0000FFFF;
	while ((Size & 0x0000FFFF) != 0)
	{	// Выставляем адрес и данные
		GPIOE->ODR = Start & 0x0000FFFF; GPIOD->ODR = (GPIOD->ODR & 0xFF00) | *(PBuf);
		// Активируем строб чтения CHR
		PRG_F2_On();
		// Ждем
		Delay = 3; while (Delay) { Delay -= 1; }
		// Деактивируем чтение
		PRG_F2_Off();
		// Счетчики
		PBuf += 1; Start += 1; Size -= 1;
	}
	// Начальное состояние
	PRG_F2_Off(); PRG_RnW_Rd();
	GPIO_InStr.GPIO_Pin = 0xFFFF;
	GPIO_InStr.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InStr.GPIO_Speed = GPIO_High_Speed;
	GPIO_InStr.GPIO_OType = GPIO_OType_PP;
	GPIO_InStr.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOD, &GPIO_InStr );
	CHR_BUS_Off(); PRG_BUS_Off(); CHR_In(); PRG_In();
}
