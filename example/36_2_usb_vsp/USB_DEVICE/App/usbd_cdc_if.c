/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include "usart.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USB virtual serial port related configuration parameters */
USBD_CDC_LineCodingTypeDef LineCoding =
{
    115200,     /* Baudrate */
    0x00,       /* Stop bit, default 1 bit */
    0x00,       /* Check bit, default none */
    0x08        /* Data bits, 8 bits by default */
};


/* usb_printf Send buffer used for sprintf */
uint8_t g_usb_usart_printf_buffer[USB_USART_REC_LEN];

/* USB received data buffer of up to USART_REC_LEN bytes for the USBD_CDC_SetRxBuffer function */
uint8_t g_usb_rx_buffer[USB_USART_REC_LEN];


/* The data received by the USB virtual serial port is processed by the method similar to the data received by the serial port 1 */
uint8_t g_usb_usart_rx_buffer[USB_USART_REC_LEN];       /* Receive buffer, maximum USART_REC_LEN bytes */


/* Receive status
 * bit15, receive complete flag
 * bit14, 0x0d received
 * bit13-0, the number of valid bytes received
 */
uint16_t g_usb_usart_rx_sta=0;  /* Receiving status marking */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/**
 * @brief   processes the data received from the USB virtual serial port
 * @param   buf : Receive data buffer
 * @param   len : The length of the data received
 * @retval  None
 */
void cdc_vcp_data_rx (uint8_t *buf, uint32_t Len)
{
    uint8_t i;
    uint8_t res;

    for (i = 0; i < Len; i++)
    {
        res = buf[i];

        if ((g_usb_usart_rx_sta & 0x8000) == 0)     /* Receipt not completed */
        {
            if (g_usb_usart_rx_sta & 0x4000)        /* 0x0d has been received */
            {
                if (res != 0x0a)
                {
                    g_usb_usart_rx_sta = 0;         /* Accept the error and start over */
                }
                else
                {
                    g_usb_usart_rx_sta |= 0x8000;   /* The reception is complete. */
                }
            }
            else    /* 0X0D has not been received yet */
            {
                if (res == 0x0d)
                {
                    g_usb_usart_rx_sta |= 0x4000;   /* The flag has received 0X0D */
                }
                else
                {
                    g_usb_usart_rx_buffer[g_usb_usart_rx_sta & 0X3FFF] = res;
                    g_usb_usart_rx_sta++;

                    if (g_usb_usart_rx_sta > (USB_USART_REC_LEN - 1))
                    {
                        g_usb_usart_rx_sta = 0; /* Receive data overflow to restart the receive */
                    }
                }
            }
        }
    }
}

/**
 * @brief   sends data over USB
 * @param   buf : A buffer of data to send
 * @param   len : The length of the data
 * @retval  None
 */
void cdc_vcp_data_tx(uint8_t *data, uint32_t Len)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, data, Len);
    USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    HAL_Delay(CDC_POLLING_INTERVAL);
}

/**
 * @brief  format output functions via USB
 * @param  format the output
 * @retval None
 */
void usb_printf(char *fmt, ...)
{
    uint16_t i;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)g_usb_usart_printf_buffer, fmt, ap);
    va_end(ap);
    i = strlen((const char *)g_usb_usart_printf_buffer);    /* The length of the data sent at this time */
    cdc_vcp_data_tx(g_usb_usart_printf_buffer, i);          /* send data */
}
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, g_usb_rx_buffer);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
		LineCoding.bitrate = (uint32_t) (pbuf[0] | (pbuf[1] << 8) |
										 (pbuf[2] << 16) | (pbuf[3] << 24));
		LineCoding.format = pbuf[4];
		LineCoding.paritytype = pbuf[5];
		LineCoding.datatype = pbuf[6];
		/* Printing configuration parameters */
		printf("linecoding.format:%d\r\n", LineCoding.format);
		printf("linecoding.paritytype:%d\r\n", LineCoding.paritytype);
		printf("linecoding.datatype:%d\r\n", LineCoding.datatype);
		printf("linecoding.bitrate:%lu\r\n", LineCoding.bitrate);
    break;

    case CDC_GET_LINE_CODING:
        pbuf[0] = (uint8_t) (LineCoding.bitrate);
        pbuf[1] = (uint8_t) (LineCoding.bitrate >> 8);
        pbuf[2] = (uint8_t) (LineCoding.bitrate >> 16);
        pbuf[3] = (uint8_t) (LineCoding.bitrate >> 24);
        pbuf[4] = LineCoding.format;
        pbuf[5] = LineCoding.paritytype;
        pbuf[6] = LineCoding.datatype;
    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  cdc_vcp_data_rx(Buf, *Len);
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
