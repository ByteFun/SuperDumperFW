//--------------------------------------------------------------
// File     : usbd_cdc_vcp.c
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "usbd_cdc_vcp.h"

LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };


//--------------------------------------------------------------
extern uint8_t  APP_Rx_Buffer []; // Буфер исходящих данных
extern uint32_t APP_Rx_ptr_in;    // Указатель на голову буфера исходящих данных
extern uint32_t APP_Rx_ptr_out;   // Указатель на хвост буфера исходящих данных

//--------------------------------------------------------------
static uint16_t VCP_Init     (void);
static uint16_t VCP_DeInit   (void);
static uint16_t VCP_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataTx   (uint8_t* Buf, uint32_t Len);
static uint16_t VCP_DataRx   (uint8_t* Buf, uint32_t Len);

CDC_IF_Prop_TypeDef VCP_fops = 
{
  VCP_Init,
  VCP_DeInit,
  VCP_Ctrl,
  VCP_DataTx,
  VCP_DataRx
};

//--------------------------------------------------------------
static uint16_t VCP_Init(void)
{	// Инит
	return USBD_OK;
}

//--------------------------------------------------------------
static uint16_t VCP_DeInit(void)
{
  return USBD_OK;
}


//--------------------------------------------------------------
static uint16_t VCP_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Not  needed for this driver */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Not  needed for this driver */
    break;

  case SET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case GET_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case CLEAR_COMM_FEATURE:
    /* Not  needed for this driver */
    break;

  case SET_LINE_CODING:
    linecoding.bitrate = (uint32_t)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
    linecoding.format = Buf[4];
    linecoding.paritytype = Buf[5];
    linecoding.datatype = Buf[6];
    break;

  case GET_LINE_CODING:
    Buf[0] = (uint8_t)(linecoding.bitrate);
    Buf[1] = (uint8_t)(linecoding.bitrate >> 8);
    Buf[2] = (uint8_t)(linecoding.bitrate >> 16);
    Buf[3] = (uint8_t)(linecoding.bitrate >> 24);
    Buf[4] = linecoding.format;
    Buf[5] = linecoding.paritytype;
    Buf[6] = linecoding.datatype; 
    break;

  case SET_CONTROL_LINE_STATE:
    /* Not  needed for this driver */
    break;

  case SEND_BREAK:
    /* Not  needed for this driver */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

//--------------------------------------------------------------
// CallBack передачи по USB
//--------------------------------------------------------------
static uint16_t VCP_DataTx (uint8_t* Buf, uint32_t Len)
{
  uint32_t i=0;

  while(i < Len) {
    APP_Rx_Buffer[APP_Rx_ptr_in] = *(Buf + i);
    APP_Rx_ptr_in++;
    i++;
    /* To avoid buffer overflow */
    if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE)
    {
      APP_Rx_ptr_in = 0;
    }
  }

  return USBD_OK;
}


//--------------------------------------------------------------
// CallBack приема по USB
//--------------------------------------------------------------
static uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{	// Байпассим необходимые данные
	USB_Read( Buf, Len );
	return USBD_OK;
}

//--------------------------------------------------------------
// Постановка байта в исходящий буфер
//--------------------------------------------------------------
void UB_VCP_DataTx (uint8_t Data)
{	// Переменные
	uint32_t NewPtr;
	// Ждем места в буфере
	NewPtr = APP_Rx_ptr_in + 1;
	if (NewPtr >= APP_RX_DATA_SIZE) { NewPtr = 0; }
	while (NewPtr == APP_Rx_ptr_out) { }
	// Выгружаем байт
	APP_Rx_Buffer[APP_Rx_ptr_in] = Data; APP_Rx_ptr_in++;
	// Коррекция
	if(APP_Rx_ptr_in >= APP_RX_DATA_SIZE) {	APP_Rx_ptr_in = 0; }
}
