/**
 ****************************************************************************************************
 * @file        ctiic.c
 * @author      ALIENTEK
 * @brief       ctiic code
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

#include "ctiic.h"
#include "../../SYSTEM/delay/delay.h"

/**
 * @brief  Capacitive touch screen IIC delay function
 * @param  None
 * @retval None
 */
static void ct_iic_delay(void)
{
    delay_us(2);
}

/**
 * @brief  Initializes the capacitive touch screen IIC
 * @param  None
 * @retval None
 */
void ct_iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    CT_IIC_SCL_GPIO_CLK_ENABLE();
    CT_IIC_SDA_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = CT_IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SCL_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = CT_IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CT_IIC_SDA_GPIO_PORT, &gpio_init_struct);
    
    ct_iic_stop();
}

/**
 * @brief  generates the capacitive touchscreen IIC onset signal
 * @param  None
 * @retval None
 */
void ct_iic_start(void)
{
    CT_IIC_SDA(1);
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
}

/**
 * @brief  generates the capacitive touchscreen IIC stop signal
 * @param  None
 * @retval None
 */
void ct_iic_stop(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

/**
 * @brief  waits for the capacitive touch screen IIC to answer the signal
 * @param  None
 * @retval Wait for the result
 * @arg    0: Wait for the capacitive touch screen IIC to answer the signal successfully
 * @arg    1: Failed to wait for the reply signal of the capacitive touch screen IIC
 */
uint8_t ct_iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;
    
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    
    while (CT_IIC_SDA_READ != 0)
    {
        waittime++;
        if (waittime > 250)
        {
            ct_iic_stop();
            rack = 1;
            break;
        }
    }
    
    CT_IIC_SCL(0);
    ct_iic_delay();
    
    return rack;
}

/**
 * @brief  generates capacitive touchscreen IIC ACK signals
 * @param  None
 * @retval None
 */
void ct_iic_ack(void)
{
    CT_IIC_SDA(0);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
    CT_IIC_SDA(1);
    ct_iic_delay();
}

/**
 * @brief   The capacitive touch screen IIC NACK signal is generated
 * @param   None
 * @retval  None
 */
void ct_iic_nack(void)
{
    CT_IIC_SDA(1);
    ct_iic_delay();
    CT_IIC_SCL(1);
    ct_iic_delay();
    CT_IIC_SCL(0);
    ct_iic_delay();
}

/**
 * @brief  Capacitive touch screen IIC sends one byte
 * @param  data: A byte of data to be sent
 * @retval None
 */
void ct_iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t=0; t<8; t++)
    {
        CT_IIC_SDA((data & 0x80) >> 7);
        ct_iic_delay();
        CT_IIC_SCL(1);
        ct_iic_delay();
        CT_IIC_SCL(0);
        data <<= 1;
    }
    
    CT_IIC_SDA(1);
}

/**
 * @brief  Capacitive touch screen IIC reads one byte
 * @param  ack: The response
 * @arg    0: Sends an ACK response
 * @arg    1: Sends a NACK response
 * @retval read one byte of data
 */
uint8_t ct_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;
    
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        CT_IIC_SCL(1);
        ct_iic_delay();
        
        if (CT_IIC_SDA_READ)
        {
            receive++;
        }
        
        CT_IIC_SCL(0);
        ct_iic_delay();
    }
    
    if (ack == 0)
    {
        ct_iic_nack();
    }
    else
    {
        ct_iic_ack();
    }
    
    return receive;
}
