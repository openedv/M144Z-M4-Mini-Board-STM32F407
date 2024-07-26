/**
 ****************************************************************************************************
 * @file        pwr.c
 * @author      ALIENTEK Team
 * @version     V1.0
 * @date        Feb 22, 2024
 * @brief       Low power mode driver code
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * Experimental platform  : ALIENTEK STM32F103 development board
 * Online video           : https://www.yuanzige.com
 * Technical Forum        : http://www.openedv.com/forum.php
 * Company website        : https://www.alientek.com
 * Buy from               : https://www.aliexpress.com/store/1102909571
 *
 ****************************************************************************************************
 */

#include "pwr.h"
#include "../LED/led.h"
#include "../LCD/lcd.h"
#include "usart.h"

/**
* @brief 	WK_UP Keys the external interrupt service program
* @param 	None
* @retval 	None
*/
void EXTI0_IRQHandler(void)
{
    HAL_ResumeTick();                                   /* Restore the ticking clock */
    HAL_GPIO_EXTI_IRQHandler(WK_UP_Pin);
}

/**
* @brief 	Enter CPU sleep mode
* @param 	None
* @retval 	None
*/
void pwr_enter_sleep(void)
{
    HAL_SuspendTick();                                                  /* Pause the ticking clock to prevent the wake up from being interrupted by the ticking clock */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);   /* Execute WFI and enter sleep mode */
}

/**
* @brief 	External interrupt callback function
* @param 	GPIO_Pin: pin of the interrupt line
* @note 	This is called by PWR_WKUP_INT_IRQHandler()
* @retval 	None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == WK_UP_Pin)
    {
        /* HAL_GPIO_EXTI_IRQHandler(),the function has cleared the interrupt flag bit for us, so we can enter the callback function without doing anything */
    	SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
    }
}
