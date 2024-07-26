/**
 ****************************************************************************************************
 * @file        myiic.h
 * @author      ALIENTEK
 * @brief       This file provides the driver for the IIC
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform   : ALIENTEK M144-STM32F407 board
 * website    : https://www.alientek.com
 * forum      : http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240222	the first version
 *
 ****************************************************************************************************
 */
 
#ifndef __MYIIC_H
#define __MYIIC_H
#include "main.h"

/******************************************************************************************/

/* LED port definition */
#define IIC_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define IIC_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define IIC_READ_SDA     HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) /*  Read SDA */


/* All IIC operation functions */
void iic_init(void);                     	/* Initialize the IIC IO ports */
void iic_start(void);                    	/* Send the IIC start signal */
void iic_stop(void);                     	/* Send the IIC stop signal */
void iic_ack(void);                      	/* Send ACK signal on IIC */
void iic_nack(void);                     	/* Do not send ACK signal on IIC */
uint8_t iic_wait_ack(void);              	/* Wait for ACK signal on IIC */
void iic_send_byte(uint8_t txd);         	/* Send one byte on IIC */
uint8_t iic_read_byte(unsigned char ack);	/* Read one byte from IIC */

#endif

