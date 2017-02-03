// Передача данных по USB CDC через vCOM

#ifndef USB_CDC_LINK_H_
#define USB_CDC_LINK_H_

#include "stdint.h"
#include "stm32f4xx.h"
#include "usb-cdc/usbd_conf.h"
#include "usb_core.h"

// USB CDC
USB_OTG_CORE_HANDLE  USB_OTG_dev;

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "usbd_cdc_vcp.h"


//--------------------------------------------------------------
// Статус соединения USB
//--------------------------------------------------------------
typedef enum {
  USB_CDC_NO_INIT =0, // интерфейс USB не инициализирован
  USB_CDC_DETACHED,   // USB-соединение инициализировано
  USB_CDC_CONNECTED   // USB-Соединение установлено
}USB_CDC_STATUS_t;
USB_CDC_STATUS_t USB_CDC_STATUS;

//--------------------------------------------------------------
// Окончание ID при отправке
//--------------------------------------------------------------
typedef enum {
  NONE = 0,  // без идентификатора
  LFCR,      // возврат строки и перевод каретки (0x0A,0x0D)
  CRLF,      // перевод каретки и возврат строки (0x0D,0x0A)
  LF,        // перевод строки (0x0A)
  CR         // возврат каретки (0x0D)
}USB_CDC_LASTBYTE_t;

//--------------------------------------------------------------
// Статус при получении
//--------------------------------------------------------------
typedef enum {
  RX_USB_ERR =0, // нет подключения к USB
  RX_EMPTY,      // данные не приходят
  RX_READY       // данные, принятые в буфер
}USB_CDC_RXSTATUS_t;


//--------------------------------------------------------------
// Глобальная функция
//--------------------------------------------------------------
void UB_USB_CDC_Init(void);
USB_CDC_STATUS_t UB_USB_CDC_GetStatus(void);
ErrorStatus UB_USB_CDC_SendString(char *ptr, USB_CDC_LASTBYTE_t end_cmd);
USB_CDC_RXSTATUS_t UB_USB_CDC_ReceiveString(char *ptr);
//--------------------------------------------------------------

// Буфер данных приема-передачи
uint8_t  Data_Buf[ 0x10010 ];
uint8_t  Ans_Buf[ 0x10 ];

// CallBack приема данных по USB
void USB_Read(uint8_t *Buf, uint32_t Size);
// Проверка доступных данных
uint8_t USB_Check();
// Установка флага занятости
void USB_ClearBusy();
// Передача пакета по USB
void USB_Write(uint8_t *Buf, uint32_t Size);

#endif /* USB_CDC_LINK_H_ */
