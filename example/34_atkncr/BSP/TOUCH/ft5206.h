/**
 ****************************************************************************************************
 * @file        ft5206.h
 * @author      ALIENTEK
 * @brief       ft5206 code
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

#ifndef __FT5206_H
#define __FT5206_H

#include "main.h"


#define FT5206_RST_GPIO_PORT            GPIOC
#define FT5206_RST_GPIO_PIN             GPIO_PIN_13
#define FT5206_RST_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define FT5206_INT_GPIO_PORT            GPIOB
#define FT5206_INT_GPIO_PIN             GPIO_PIN_1
#define FT5206_INT_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOB_CLK_ENABLE(); } while (0)


#define FT5206_RST(x)                   do { (x) ?                                                                          \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_SET):     \
                                            HAL_GPIO_WritePin(FT5206_RST_GPIO_PORT, FT5206_RST_GPIO_PIN, GPIO_PIN_RESET);   \
                                        } while (0)
#define FT5206_INT                      ((HAL_GPIO_ReadPin(FT5206_INT_GPIO_PORT, FT5206_INT_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* FT5206 IIC read and write address definition */
#define FT5206_CMD_WR                   0x70    /* write address */
#define FT5206_CMD_RD                   0x71    /* read address */

/* FT5206 partial register address definition */
#define FT5206_DEVIDE_MODE              0x00    /* FT5206 mode control register */
#define FT5206_REG_NUM_FINGER           0x02    /* Touch the status register */
#define FT5206_TP1_REG                  0x03    /* The first touch point data address */
#define FT5206_TP2_REG                  0x09    /* The second touch point data address */
#define FT5206_TP3_REG                  0x0F    /* The third touch point data address */
#define FT5206_TP4_REG                  0x15    /* The fourth touch point data address */
#define FT5206_TP5_REG                  0x1B    /* The fifth touch point data address */
#define FT5206_ID_G_LIB_VERSION         0xA1    /* Version */
#define FT5206_ID_G_MODE                0xA4    /* FT5206 interrupt mode control register */
#define FT5206_ID_G_THGROUP             0x80    /* Touch the valid value setting register */
#define FT5206_ID_G_PERIODACTIVE        0x88    /* Activation state cycle set register */


uint8_t ft5206_init(void);
uint8_t ft5206_scan(uint8_t mode);

#endif
