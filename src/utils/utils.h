// Утилиты

#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include "stdint.h"
#include "stm32f4xx.h"

// Типы

// Процедуры и функции
// Вычисляем CRC32
void Calc_CRC32(uint8_t Dat, uint32_t *CRC32);
// Управляем лампочками
__attribute__ ((naked)) void UpdateLEDs(uint32_t DendyLED, uint32_t SegaLED);

#endif /* UTILS_UTILS_H_ */
