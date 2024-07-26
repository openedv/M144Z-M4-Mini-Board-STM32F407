/**
 ****************************************************************************************************
 * @file        sccb.h
 * @author      ALIENTEK
 * @brief       This file provides the driver for the sccb
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

#ifndef __SCCB_H
#define __SCCB_H

#include "main.h"

/* If open-drain mode is not used or there are no pull-up resistors on SCCB, push-pull and input switching are required */
#define OV_SCCB_TYPE_NOD            0
#if (OV_SCCB_TYPE_NOD != 0)
#define SCCB_SDA_IN()               do { GPIOD->MODE &= ~(3 << (7 * 2)); GPIOD->MODE |= 0 << (7 * 2); } while(0)
#define SCCB_SDA_OUT()              do { GPIOD->MODE &= ~(3 << (7 * 2)); GPIOD->MODE |= 1 << (7 * 2); } while(0)
#endif

/* IO operation */
#define SCCB_SCL(x)                 do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(SCCB_SCL_GPIO_Port, SCCB_SCL_Pin, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(SCCB_SCL_GPIO_Port, SCCB_SCL_Pin, GPIO_PIN_RESET);   \
                                    } while (0)
#define SCCB_SDA(x)                 do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(SCCB_SDA_GPIO_Port, SCCB_SDA_Pin, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(SCCB_SDA_GPIO_Port, SCCB_SDA_Pin, GPIO_PIN_RESET);   \
                                    } while (0)
#define SCCB_SDA_READ               ((HAL_GPIO_ReadPin(SCCB_SDA_GPIO_Port, SCCB_SDA_Pin) == GPIO_PIN_RESET) ? 0 : 1)

/* Function declaration */
void sccb_init(void);                   /* Initialize SCCB */
void sccb_start(void);                  /* Generate SCCB start signal */
void sccb_stop(void);                   /* Generate SCCB stop signal */
void sccb_nack(void);                   /* Generate SCCB NACK signal */
uint8_t sccb_send_byte(uint8_t data);   /* Send one byte via SCCB */
uint8_t sccb_read_byte(void);           /* Read one byte via SCCB */

#endif
