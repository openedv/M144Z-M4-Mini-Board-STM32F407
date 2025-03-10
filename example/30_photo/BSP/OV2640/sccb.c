/**
 ****************************************************************************************************
 * @file        sccb.c
 * @author      ALIENTEK
 * @brief       This file provides the driver for the sccb
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

#include "../../BSP/OV2640/sccb.h"
#include "../../SYSTEM/delay/delay.h"


/**
 * @brief   SCCB delay function
 * @note    Used to control the SCCB communication
 * @param   None
 * @retval  None
 */
static void sccb_delay(void)
{
    delay_us(50);
}

/**
 * @brief   Initialize SCCBInitialize SCCB
 * @param   None
 * @retval  None
 */
void sccb_init(void)
{
    sccb_stop();
}

/**
 * @brief   Generate SCCB start signal
 * @param   None
 * @retval  None
 */
void sccb_start(void)
{
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(0);
}

/**
 * @brief   Generate SCCB stop signal
 * @param   None
 * @retval  None
 */
void sccb_stop(void)
{
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(1);
    sccb_delay();
}

/**
 * @brief   Generate SCCB NACK signal
 * @param   None
 * @retval  None
 */
void sccb_nack(void)
{
    sccb_delay();
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SCL(0);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
}

/**
 * @brief   Send one byte via SCCB
 * @param   data: One byte of data to be sent
 * @retval  Sending result
 * @arg     0: Successful transmission
 * @arg     1: Transmission failed
 */
uint8_t sccb_send_byte(uint8_t data)
{
    uint8_t t;
    uint8_t res;
    
    for (t=0; t<8; t++)
    {
        SCCB_SDA((data & 0x80) >> 7);
        sccb_delay();
        SCCB_SCL(1);
        sccb_delay();
        SCCB_SCL(0);
        data <<= 1;
    }
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_IN();
    sccb_delay();
#endif
    SCCB_SDA(1);
    sccb_delay();
    
    SCCB_SCL(1);
    sccb_delay();
    if (SCCB_SDA_READ)
    {
        res = 1;
    }
    else
    {
        res = 0;
    }
    SCCB_SCL(0);
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_OUT();
    sccb_delay();
#endif
    
    return res;
}

/**
* @brief 	SCCB reads a byte
* @param 	None
* @retval 	One byte of data read
*/
uint8_t sccb_read_byte(void)
{
    uint8_t i;
    uint8_t receive = 0;
    
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_IN();
    sccb_delay();
#endif
    for (i=0; i<8; i++)
    {
        receive <<= 1;
        SCCB_SCL(1);
        sccb_delay();
        
        if (SCCB_SDA_READ)
        {
            receive++;
        }
        
        SCCB_SCL(0);
        sccb_delay();
    }
    
#if (OV_SCCB_TYPE_NOD != 0)
    SCCB_SDA_OUT();         /* Set SDA to output */
    sccb_delay();
#endif
    
    return receive;
}
