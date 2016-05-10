// Передача данных по USB HID репортами

#ifndef USB_HID_LINK_H_
#define USB_HID_LINK_H_

#include "stdint.h"
#include "stm32f4xx.h"
#include "usbd_conf.h"

// USB HID
USB_OTG_CORE_HANDLE USB_Device_dev;
volatile uint8_t USB_Rx[ HID_OUT_PACKET ];
uint8_t USB_Tx[ HID_IN_PACKET ];

// Прием данных по USB
void USB_Read(uint8_t *Buf, uint32_t Size);
// Передача пакета по USB
void USB_Write(uint8_t *Buf, uint32_t Size);

#endif /* USB_HID_LINK_H_ */
