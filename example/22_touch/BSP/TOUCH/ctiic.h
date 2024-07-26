/**
 ****************************************************************************************************
 * @file        ctiic.h
 * @author      ALIENTEK
 * @brief       ctiic code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK STM32F407 development board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  ：
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#ifndef __CTIIC_H
#define __CTIIC_H

#include "main.h"

#define CT_IIC_SCL_GPIO_PORT            GPIOB
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_0
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)
#define CT_IIC_SDA_GPIO_PORT            GPIOF
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_11
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOF_CLK_ENABLE(); } while (0)

/* IO操作 */
#define CT_IIC_SCL(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define CT_IIC_SDA_READ                 ((HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)


void ct_iic_init(void);
void ct_iic_start(void);
void ct_iic_stop(void);
uint8_t ct_iic_wait_ack(void);
void ct_iic_ack(void);
void ct_iic_nack(void);
void ct_iic_send_byte(uint8_t data);
uint8_t ct_iic_read_byte(uint8_t ack);

#endif
