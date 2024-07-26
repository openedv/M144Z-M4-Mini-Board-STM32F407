/**
 ****************************************************************************************************
 * @file        gt9xxx.h
 * @author      ALIENTEK
 * @brief       gt9xxx code
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

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "main.h"

#define GT9XXX_RST_GPIO_PORT            GPIOC
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_13
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define GT9XXX_INT_GPIO_PORT            GPIOB
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_1
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)


#define GT9XXX_RST(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define GT9XXX_INT                      ((HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* GT9XXX IIC read and write address definition */
#define GT9XXX_CMD_WR                   0x28    /* write address */
#define GT9XXX_CMD_RD                   0x29    /* read address */

/* GT9XXX partial register address definition */
#define GT9XXX_CTRL_REG                 0x8040  /* GT9XXX control register */
#define GT9XXX_CFGS_REG                 0x8047  /* GT9XXX config the start address register */
#define GT9XXX_CHECK_REG                0x80FF  /* GT9XXX checksum register */
#define GT9XXX_PID_REG                  0x8140  /* GT9XXX product ID register */
#define GT9XXX_GSTID_REG                0x814E  /* GT9XXX The currently detected touch */
#define GT9XXX_TP1_REG                  0x8150  /* The first touch point data address */
#define GT9XXX_TP2_REG                  0x8158  /* The second touch point data address */
#define GT9XXX_TP3_REG                  0x8160  /* The third touch point data address */
#define GT9XXX_TP4_REG                  0x8168  /* The fourth touch point data address */
#define GT9XXX_TP5_REG                  0x8170  /* The fifth touch point data address */
#define GT9XXX_TP6_REG                  0x8178  /* The sixth touch point data address */
#define GT9XXX_TP7_REG                  0x8180  /* The seventh touch point data address */
#define GT9XXX_TP8_REG                  0x8188  /* The eirhth touch point data address */
#define GT9XXX_TP9_REG                  0x8190  /* The ninth touch point data address */
#define GT9XXX_TP10_REG                 0x8198  /* The tenth touch point data address */

uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode);

#endif
