
////////////////////////////////////////////////////////////////////
// Super Dumper
////////////////////////////////////////////////////////////////////

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "misc.h"

#include "utils/utils.h"
#include "dumper/dendy.h"

#include "usb-hid/usbd_desc.h"
#include "usbd_usr.h"
#include "usbd_hid_core.h"
#include "usb_dcd_int.h"

// USB HID
USB_OTG_CORE_HANDLE  USB_Device_dev;

// Прерывание USB
void OTG_FS_IRQHandler(void)
{
	USBD_OTG_ISR_Handler( &USB_Device_dev );
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
// Главный старт
int main(void)
{	// Инит оборудования
	HWInit(); Dendy_Off(); UpdateLEDs(0x040404, 0x040404);
    USBD_Init( &USB_Device_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_cb, &USR_cb );
	// Работаем
    while (1)
    {

    }
}

