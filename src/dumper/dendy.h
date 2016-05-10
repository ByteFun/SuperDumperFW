// Набор функций для дампинга картриджей Денди

#ifndef DUMPER_DENDY_H_
#define DUMPER_DENDY_H_

#include "stdint.h"
#include "stm32f4xx.h"

// Общие назначенные пины
#define PRG_BUS_On()	GPIOB->BSRRH = GPIO_Pin_0;		// Активируем BUSL
#define PRG_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_0;		// Деактивируем BUSL
#define CHR_BUS_On()	GPIOB->BSRRH = GPIO_Pin_1;		// Активируем BUSH
#define CHR_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_1;		// Деактивируем BUSH
#define ADR_BUS_On()	GPIOB->BSRRH = GPIO_Pin_5;		// Активируем BUSA
#define ADR_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_5;		// Деактивируем BUSA
#define CHR_In()		GPIOC->BSRRH = GPIO_Pin_8;		// Направление CHR чтение
#define CHR_Out()		GPIOC->BSRRL = GPIO_Pin_8;		// Направление CHR запись
#define PRG_In()		GPIOC->BSRRH = GPIO_Pin_9;		// Направление PRG чтение
#define PRG_Out()		GPIOC->BSRRL = GPIO_Pin_9;		// Направление PRG запись

// Особые назначенные пины
#define PRG_F2_On()		GPIOA->BSRRL = GPIO_Pin_0;		// Активируем PRG F2
#define PRG_F2_Off()	GPIOA->BSRRH = GPIO_Pin_0;		// Деактивируем PRG F2
#define PRG_RnW_Rd()	GPIOA->BSRRL = GPIO_Pin_1;		// PRG R/W - чтение
#define PRG_RnW_Wr()	GPIOA->BSRRH = GPIO_Pin_1;		// PRG R/W - запись
#define PRG_ROM_On()	GPIOA->BSRRH = GPIO_Pin_2;		// Активируем PRG ROMSEL
#define PRG_ROM_Off()	GPIOA->BSRRL = GPIO_Pin_2;		// Деактивируем PRG ROMSEL
#define CHR_PRD_On()	GPIOA->BSRRH = GPIO_Pin_3;		// Активируем CHR RD
#define CHR_PRD_Off()	GPIOA->BSRRL = GPIO_Pin_3;		// Деактивируем CHR RD
#define CHR_PWR_On()	GPIOA->BSRRH = GPIO_Pin_4;		// Активируем CHR WR
#define CHR_PWR_Off()	GPIOA->BSRRL = GPIO_Pin_4;		// Деактивируем CHR WR
#define CHR_nPA13_On()	GPIOA->BSRRL = GPIO_Pin_5;		// CHR nPA13 = 0
#define CHR_nPA13_Off()	GPIOA->BSRRH = GPIO_Pin_5;		// CHR nPA13 = 1
#define PRG_IRQ_In()	(GPIOA->IDR & GPIO_Pin_6)		// Чтение бита PRG IRQ
#define CHR_VA10_In()	(GPIOA->IDR & GPIO_Pin_7)		// Чтение бита CHR VA10 (мирроринг)
#define CHR_VRAM_In()	(GPIOA->IDR & GPIO_Pin_8)		// Чтение бита CHR VRAM (управление CIRAM)

// Отключить режим денди
void Dendy_Off();
// Включить режим денди
void Dendy_On();
// Сброс картриджа
void Dendy_Reset();
// Чтение данных из картриджа в буфер
void Dendy_Read(uint8_t *PBuf, uint32_t Start, uint32_t Size);
// Запись данных из буфера в картридж
void Dendy_Write(uint8_t *PBuf, uint32_t Start, uint32_t Size);

#endif /* DUMPER_DENDY_H_ */
