// Набор функций для дампинга картриджей Сеги

#ifndef DUMPER_SEGA_H_
#define DUMPER_SEGA_H_

#include "stdint.h"
#include "stm32f4xx.h"
#include "utils/utils.h"

// Общие назначенные пины
#define LOW_BUS_On()	GPIOB->BSRRH = GPIO_Pin_0;		// Активируем BUSL
#define LOW_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_0;		// Деактивируем BUSL
#define HI_BUS_On()		GPIOB->BSRRH = GPIO_Pin_1;		// Активируем BUSH
#define HI_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_1;		// Деактивируем BUSH
#define ADR_BUS_On()	GPIOB->BSRRH = GPIO_Pin_5;		// Активируем BUSA
#define ADR_BUS_Off()	GPIOB->BSRRL = GPIO_Pin_5;		// Деактивируем BUSA
#define HI_BUS_In()		GPIOC->BSRRH = GPIO_Pin_8;		// Направление BUSH чтение
#define HI_BUS_Out()	GPIOC->BSRRL = GPIO_Pin_8;		// Направление BUSH запись
#define LOW_BUS_In()	GPIOC->BSRRH = GPIO_Pin_9;		// Направление BUSL чтение
#define LOW_BUS_Out()	GPIOC->BSRRL = GPIO_Pin_9;		// Направление BUSL запись

// Особые назначенные пины
#define MRES_On()		GPIOB->BSRRH = GPIO_Pin_6;		// Активируем MRES
#define MRES_Off()		GPIOB->BSRRL = GPIO_Pin_6;		// Деактивируем MRES
#define VRES_On()		GPIOB->BSRRH = GPIO_Pin_7;		// Активируем VRES
#define VRES_Off()		GPIOB->BSRRL = GPIO_Pin_7;		// Деактивируем VRES
#define AS_On()			GPIOB->BSRRH = GPIO_Pin_8;		// Активируем AS
#define AS_Off()		GPIOB->BSRRL = GPIO_Pin_8;		// Деактивируем AS
#define ASEL_On()		GPIOB->BSRRH = GPIO_Pin_9;		// Активируем ASEL
#define ASEL_Off()		GPIOB->BSRRL = GPIO_Pin_9;		// Деактивируем ASEL
#define CE0_On()		GPIOB->BSRRH = GPIO_Pin_10;		// Активируем CE0
#define CE0_Off()		GPIOB->BSRRL = GPIO_Pin_10;		// Деактивируем CE0
#define TIME_On()		GPIOB->BSRRH = GPIO_Pin_11;		// Активируем TIME
#define TIME_Off()		GPIOB->BSRRL = GPIO_Pin_11;		// Деактивируем TIME
#define CAS0_On()		GPIOB->BSRRH = GPIO_Pin_12;		// Активируем CAS0
#define CAS0_Off()		GPIOB->BSRRL = GPIO_Pin_12;		// Деактивируем CAS0
#define CAS2_On()		GPIOB->BSRRH = GPIO_Pin_13;		// Активируем CAS2
#define CAS2_Off()		GPIOB->BSRRL = GPIO_Pin_13;		// Деактивируем CAS2
#define LWR_On()		GPIOB->BSRRH = GPIO_Pin_14;		// Активируем LWR
#define LWR_Off()		GPIOB->BSRRL = GPIO_Pin_14;		// Деактивируем LWR
#define UWR_On()		GPIOB->BSRRH = GPIO_Pin_15;		// Активируем UWR
#define UWR_Off()		GPIOB->BSRRL = GPIO_Pin_15;		// Деактивируем UWR
#define CART_In()		(GPIOC->IDR & GPIO_Pin_10)		// Чтение бита CART

typedef enum {
	mdByteNo = 0,
	mdByteLow,
	mdByteHigh,
	mdByteBoth
	} AccessMode;

// Отключить режим сеги
void Sega_Off();
// Включить режим сеги
void Sega_On();
// Включить программный режим сеги
//void Sega_Manual();
// Сброс картриджа
void Sega_Reset( FunctionalState Mode );
// Чтение данных из картриджа в буфер
void Sega_Read( uint8_t *PBuf, AccessMode Mode, uint32_t Start, uint32_t Size );
// Запись данных из буфера в картридж
void Sega_Write( uint8_t *PBuf, AccessMode Mode, uint32_t Start, uint32_t Size );
// Чтение данных в буфер из картриджа в ручном режиме
//void Sega_ManualRead( uint8_t *PBuf, uint32_t Start, uint32_t Size );
// Запись данных из буфера в картридж в ручном режиме
//void Sega_ManualWrite( uint8_t *PBuf, WriteMode Mode, uint32_t Start, uint32_t Size );

#endif /* DUMPER_SEGA_H_ */
