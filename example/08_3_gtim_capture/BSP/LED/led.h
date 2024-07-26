/**
 ****************************************************************************************************
 * @file        led.h
 * @author      ALIENTEK
 * @brief       This file provides the driver for the led
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M144-STM32F407 board
 * website      : https://www.alientek.com
 * forum        : http://www.openedv.com/forum.php
 *
 * change logs  :
 * version    data    notes
 * V1.0     20240410  the first version
 *
 ****************************************************************************************************
 */


#ifndef BSP_LED_LED_H_
#define BSP_LED_LED_H_

#include "main.h"

/******************************************************************************************/
/* LED port definition */
#define LED0(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET) :  \
                      HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET); \
                  }while(0)

#define LED1(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET) :  \
                      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); \
                  }while(0)

/* LED port defined in reverse */
#define LED0_TOGGLE()   do{ HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); }while(0)	/* turn over LED */
#define LED1_TOGGLE()   do{ HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); }while(0)	/* turn over LED */

#endif
