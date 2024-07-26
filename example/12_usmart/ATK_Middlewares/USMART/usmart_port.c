/**
 ****************************************************************************************************
 * @file        usmart_port.c
 * @author      ALIENTEK
 * @brief       USMART Serial debugging component
 *
 * 				USMART is a clever serial debugging interinterface developed by ALIENTEK Team, which allows you to call any function in the program through the serial assistant and execute it.
 * 				Therefore, you can freely change the input parameters of the function (support numbers (10/16, support negative numbers), strings, function entry address, etc.),
 * 				a single function supports up to 10 input parameters, and supports the function return value display.
 * 				After the V2.1 version, hex and dec were added, which can be used to set the display format of function parameters, and can also be used for data conversion, such as:
 *				Typing "hex 100" will see HEX 0X64 on the serial debugging assistant,and so on.
 *
 *              FLASH:4K to K bytes (setting by USMART_USE_HELP and USMART_USE_WRFUNS)
 *              SRAM:72 bytes (minimum case)
 *              SRAM calculation formula: SRAM=PARM_LEN+72-4 Where PARM_LEN must be greater than or equal to 4.
 *              The stack should be at least 100 bytes.
 *
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
 * V1.0			20240222	the first version
 *
 ****************************************************************************************************
 */


/* USER CODE BEGIN Header */

#include "usmart.h"
#include "usmart_port.h"
#include "tim.h"

/**
 * @brief       Get input data stream (string)
 * @note        USMART parses the string returned by the function to obtain information such as the function name and parameters
 * @param       none
 * @retval
 * @arg         0: No data was received
 * @arg      	Other, data stream head address (cannot be 0)
 */
char *usmart_get_input_string(void)
{
    uint8_t len;
    char *pbuf = 0;

    if (g_usart_rx_sta & 0x8000)        /* Serial port reception complete?  */
    {
        len = g_usart_rx_sta & 0x3fff;  /* Get the length of the received data */
        g_usart_rx_buf[len] = '\0';     /* Add terminator at the end */
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;             /* Open next receive */
    }

    return pbuf;
}

/* If timer scanning is enabled, you need to define the following functions */
#if USMART_ENTIMX_SCAN == 1

/**
 * Porting Note: This example is the stm32 as an example, if you want to port to other MCUS, please make changes accordingly.
   usmart_reset_runtime：Clears the function runtime, along with the timer's count register and flag bits. And set the reload value to the maximum to extend the timing time to the maximum.
   usmart_get_runtime: Obtains the running time of the function by reading the CNT value. Because usmart is called by the interrupt function, the interrupt of the timer is no longer valid
   Only 2 CNT values can be counted, that is, after clearing zero + overflow once, when overflow exceeds 2 times, it cannot be processed, so the maximum delay is controlled at :2* counter CNT*0.1ms. For STM32, it is: around 13.1s
   Others :USMART_TIMX_IRQHandler and Timer4_Init, which must be modified based on MCU features. Ensure that the counter counting frequency is :10Khz. In addition, do not turn on the automatic reload function of the timer!!
 */

/**
 * @brief      Reset runtime
 * @note       You need to modify the timer parameters based on the MCU you are migrating to
 * @param      none
 * @retval     none
 */
void usmart_timx_reset_time(void)
{
    __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE); /* Clear the interrupt flag bit */
    __HAL_TIM_SET_AUTORELOAD(&htim4, 0XFFFF);      /* Set the reload value to the maximum */
    __HAL_TIM_SET_COUNTER(&htim4, 0);              /* Clear the CNT of the timer */
    usmart_dev.runtime = 0;
}

/**
 * @brief       Get runtime time
 * @note        You need to modify the timer parameters based on the MCU you are migrating to
 * @param       none
 * @retval      Execution time, unit :0.1ms. The maximum delay time is twice the CNT value of the timer *0.1ms
 */
uint32_t usmart_timx_get_time(void)
{
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) == SET)	/* During operation, a timer overflow occurred */
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&htim4);
    return usmart_dev.runtime;                              /* Return count value */
}


/**
* @brief	timer initialization function
* @param 	arr: Auto reload value
*        	psc: timer frequency division coefficient
* @retval 	none
*/
void TIM4_IRQHandler(void)
{
	/* Overflow interrupt */
	if(__HAL_TIM_GET_IT_SOURCE(&htim4,TIM_IT_UPDATE)==SET)
	{

		usmart_dev.scan();                              	/* Execute usmart scan */
		__HAL_TIM_SET_COUNTER(&htim4, 0);;    				/* Clear the CNT of the timer */
		__HAL_TIM_SET_AUTORELOAD(&htim4, 100);				/* Restore the original Settings */
	}
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);				/* Clear the interrupt flag bit */
}

#endif
