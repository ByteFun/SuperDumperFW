//--------------------------------------------------------------
// File     : usbd_cdc_vcp.h
//--------------------------------------------------------------


#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "usbd_cdc_core.h"
#include "usb-cdc/usbd_conf.h"
#include "usbd_usr.h"
#include "usb-cdc/usbd_desc.h"
#include "usb-cdc/link.h"

/* Exported typef ------------------------------------------------------------*/

typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;



#define APP_TX_BUF_SIZE         128  // Размер буфера приема (32, 64, 128, 256 и т.д.)
#define APP_TX_BUF_MASK         (APP_TX_BUF_SIZE-1)
//#define USB_CDC_RX_END_CHR      0x0D  // Endekennung (Ascii-Wert)
//#define USB_CDC_FIRST_ASCII       32    // erstes Ascii-Zeichen
//#define USB_CDC_LAST_ASCII       255   // letztes Ascii-Zeichen

// Отправить байт
void UB_VCP_DataTx (uint8_t Data);


#endif


