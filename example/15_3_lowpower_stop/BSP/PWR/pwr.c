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
    HAL_ResumeTick();										/* Restore the ticking clock */

    /* To wake up from stop mode, reset the system clock, 72Mhz */
    SystemClock_Config();                                 	/* set clock, 72Mhz */
    HAL_GPIO_EXTI_IRQHandler(WK_UP_Pin);
}

/**
 * @brief       Enter CPU stop mode
 * @param       None
 * @retval      None
 */
void pwr_enter_stop(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();                                         /* Enable power clock */
    HAL_SuspendTick();                                                  /* Pause the ticking clock to prevent the wake up from being interrupted by the ticking clock */

    /* Enter stop mode, set the regulator to low power mode, and wait for interrupted wake up */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
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
        /* HAL_GPIO_EXTI_IRQHandler()，the function has cleared the interrupt flag bit for us, so we can enter the callback function without doing anything */
    }
}
