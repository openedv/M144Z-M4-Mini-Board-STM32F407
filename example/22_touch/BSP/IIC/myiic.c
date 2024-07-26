/**
 ****************************************************************************************************
 * @file        myiic.c
 * @author      ALIENTEK
 * @brief       iic code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK STM32F407 development board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  : 
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#include "myiic.h"
#include "../../SYSTEM/delay/delay.h"

/**
 * @brief    Initialize the IIC
 * @param    None
 * @retval   None
 */
void iic_init(void)
{
    iic_stop();     /* Stop all devices on the bus */
}

/**
 * @brief     IIC delay function, used to control the IIC read and write speed
 * @param     None
 * @retval    None
 */
static void iic_delay(void)
{
	delay_us(2);
}

/**
 * @brief     The IIC initiation signal is generated
 * @param     None
 * @retval    None
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief     The IIC stop signal is generated
 * @param     None
 * @retval    None
 */
void iic_stop(void)
{
    IIC_SDA(0);
    iic_delay();
    IIC_SCL(1);
    iic_delay();
    IIC_SDA(1);
    iic_delay();
}

/**
 * @brief       Wait for the response signal to arrive
 * @param       None
 * @retval      1, failed to receive the reply
 *              0, the received reply was successful
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC_SDA(1);     /* The host releases the SDA wire (at this point, the external device can pull down the SDA wire) */
    iic_delay();
    IIC_SCL(1);     /* SCL=1, at which point the slave can return ACK */
    iic_delay();

    while (IIC_READ_SDA)    /* wait-for-response */
    {
        waittime++;

        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }

    IIC_SCL(0);
    iic_delay();
    return rack;
}

/**
 * @brief     Generate an ACK reply
 * @param     None
 * @retval    None
 */
void iic_ack(void)
{
    IIC_SDA(0);     /* SDA = 0 when SCL 0 -> 1, indicating a reply */
    iic_delay();
    IIC_SCL(1);     /* Generating a clock */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
    IIC_SDA(1);     /* The host releases the SDA line */
    iic_delay();
}

/**
 * @brief     No ACK response is generated
 * @param     None
 * @retval    None
 */
void iic_nack(void)
{
    IIC_SDA(1);     /* If SCL 0 -> 1, SDA = 1, indicating no response */
    iic_delay();
    IIC_SCL(1);     /* Generating a clock */
    iic_delay();
    IIC_SCL(0);
    iic_delay();
}

/**
 * @brief     The IIC sends one byte
 * @param     data: Data to send
 * @retval    None
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;

    for (t = 0; t < 8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);    /* Bit first send */
        iic_delay();
        IIC_SCL(1);
        iic_delay();
        IIC_SCL(0);
        data <<= 1;     /* The left shift is 1 bit for the next transmission */
    }
    IIC_SDA(1);         /* The sending is completed and the host releases the SDA line */
}

/**
 * @brief       The IIC reads a byte
 * @param       ack: When ack=1, ack is sent; When ack=0, nack is sent
 * @retval      Received data
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* Receives 1 byte of data */
    {
        receive <<= 1;  /* The high bits are output first, so the bits received first are shifted to the left */
        IIC_SCL(1);
        iic_delay();

        if (IIC_READ_SDA)
        {
            receive++;
        }

        IIC_SCL(0);
        iic_delay();
    }

    if (!ack)
    {
        iic_nack();     /* Sending nACK */
    }
    else
    {
        iic_ack();      /* Sending ACK */
    }

    return receive;
}


