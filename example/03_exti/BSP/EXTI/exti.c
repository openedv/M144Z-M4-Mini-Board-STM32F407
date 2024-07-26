/**
 ****************************************************************************************************
 * @file        exti.c
 * @author      ALIENTEK
 * @brief       This file provides the driver for the exit
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


#include "../EXTI/exti.h"
#include "../KEY/key.h"
#include "../LED/led.h"
#include "../../SYSTEM/delay/delay.h"


/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	delay_ms(20);       	   /* delay 20ms to eliminate chattering */

    switch (GPIO_Pin)
    {
        case KEY0_Pin:
        {
        	LED0_TOGGLE();     /* LED0 state is flipped */
        	break;
        }

        case WK_UP_Pin:
        {
        	LED1_TOGGLE();     /* LED1 state is flipped */
        	break;
        }
    }
}
