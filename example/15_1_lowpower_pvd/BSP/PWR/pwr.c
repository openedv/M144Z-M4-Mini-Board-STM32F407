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
 * @brief       Initialize the PVD voltage monitor
 * @param       pls: voltage level(PWR_PVD_detection_level)
 * @arg       	PWR_PVDLEVEL_0,2.2V;
 * @arg       	PWR_PVDLEVEL_1,2.3V;
 * @arg       	PWR_PVDLEVEL_2,2.4V;
 * @arg       	PWR_PVDLEVEL_3,2.5V;
 * @arg       	PWR_PVDLEVEL_4,2.6V;
 * @arg       	PWR_PVDLEVEL_5,2.7V;
 * @arg       	PWR_PVDLEVEL_6,2.8V;
 * @arg       	PWR_PVDLEVEL_7,2.9V;
 * @retval      None
 */
void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pwr_pvd = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                      /* Enable PWR clock */
    
    pwr_pvd.PVDLevel = pls;                          /* Detection voltage level */
    pwr_pvd.Mode = PWR_PVD_MODE_IT_RISING_FALLING;   /* Use a double edge trigger of the rising and falling edge of the interrupt line */
    HAL_PWR_ConfigPVD(&pwr_pvd);

    HAL_NVIC_SetPriority(PVD_IRQn, 3 ,3);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    HAL_PWR_EnablePVD();                             /* Enable PVD detection */
}

/**
* @brief 	PVD interrupt the service function
* @param 	None
* @retval 	None
*/
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
* @brief 	PVD Interrupt service callback function
* @param 	None
* @retval 	None
*/
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO))  							/* The voltage is lower than the voltage selected by PLS */
    {
    	LED1(0);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!",BLUE);  	/* The LCD displays low voltage */
    }
    else
    {
    	LED1(1);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ",BLUE);  	/* The LCD voltage is normal */
    }
}
