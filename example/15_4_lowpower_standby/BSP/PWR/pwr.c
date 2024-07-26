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
#include "../../BSP/KEY/key.h"
#include "usart.h"

/**
 * @brief       Enter CPU standby mode
 * @param       None
 * @retval      None
 */
void pwr_enter_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();               /* The power clock was enabled */

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* The wake UP function of the WK_UP pin was enabled */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);        /* This flag needs to be cleared, otherwise it will remain awake */
    HAL_PWR_EnterSTANDBYMode();               /* Enter standby mode */
}

/**
 * @brief   HAL library external interrupt callback function
 * @param   GPIO_Pin: The pin that triggered the interrupt
 * @retval  None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == WK_UP_Pin)
    {
        /* Re-enable SysTick interrupt */
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    }
}

/**
 * @brief   Wake-up pin external interrupt service function
 * @param   None
 * @retval  None
 */
void PWR_WKUP_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WK_UP_Pin);
}

