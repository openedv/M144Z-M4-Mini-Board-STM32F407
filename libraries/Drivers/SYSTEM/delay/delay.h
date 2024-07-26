/**
 ****************************************************************************************************
 * @file        delay.h
 * @author      ALIENTEK
 * @brief
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform  	: ALIENTEK STM32F103 development board
 * website		: https://www.alientek.com
 * forum		: http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240410	the first version
 *
 ****************************************************************************************************
 */
 
#ifndef __DELAY_H
#define __DELAY_H
#include "main.h"


void delay_init(uint16_t sysclk);       /* Initialize delay function */
void delay_ms(uint16_t nms);            /* Delay in milliseconds */
void delay_us(uint32_t nus);            /* Delay in microseconds */

#if (!SYS_SUPPORT_OS)                   /* If OS is not supported */
    void HAL_Delay(uint32_t Delay);     /* HAL library's delay function, used internally by HAL */

#endif

#endif
