/**
 ****************************************************************************************************
 * @file        24cxx.c
 * @author      ALIENTEK
 * @brief       24cxx code
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


#include "../IIC/myiic.h"
#include "24cxx.h"


/**
 * @brief     Initialize the IIC interface
 * @param     None
 * @retval    None
 */
void at24cxx_init(void)
{
    iic_init();
}

/**
 * @brief       Read out a data at the address specified by AT24CXX
 * @param       addr: The address to start reading
 * @retval      The data you read
 */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t temp = 0;
    iic_start();                 /* Send start signal */

    if (EE_TYPE > AT24C16)       /* For models above 24C16, the address is sent in 2 bytes */
    {
        iic_send_byte(0XA0);     /* The write command is sent, and the IIC stipulates that the least significant bit is 0, indicating writing */
        iic_wait_ack();          /* Every time a byte is sent, it waits for an ACK */
        iic_send_byte(addr >> 8);/* Send high-byte addresses */
    }
    else
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* Send device 0XA0 + high bit a8/a9/a10 address, write data */
    }

    iic_wait_ack();             /* Every time a byte is sent, it waits for an ACK */
    iic_send_byte(addr % 256);  /* Send low address */
    iic_wait_ack();             /* Wait for the ACK, at which point the address is sent */

    iic_start();                /* Retransmit the start signal */
    iic_send_byte(0XA1);        /* Enter receive mode, the IIC specifies that the least bit is 0, indicating read */
    iic_wait_ack();             /* Every time a byte is sent, it waits for an ACK */
    temp = iic_read_byte(0);    /* Receives one byte of data */
    iic_stop();                 /* Produces a stopping condition */
    return temp;
}

/**
 * @brief    writes a data at the address specified by AT24CXX
 * @param    addr: The destination to write the data to
 * @param    data: The data to be written
 * @retval   None
 */
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    iic_start();                /* Send start signal */

    if (EE_TYPE > AT24C16)      /* For models above 24C16, the address is sent in 2 bytes */
    {
        iic_send_byte(0XA0);    /* The write command is sent, and the IIC stipulates that the least significant bit is 0, indicating writing */
        iic_wait_ack();         /* Every time a byte is sent, it waits for an ACK */
        iic_send_byte(addr >> 8);/* Send high-byte addresses */
    }
    else
    {
        iic_send_byte(0XA0 + ((addr >> 8) << 1));   /* Send device 0XA0 + high bit a8/a9/a10 address, write data */
    }

    iic_wait_ack();             /* Every time a byte is sent, it waits for an ACK */
    iic_send_byte(addr % 256);  /* Send low address */
    iic_wait_ack();             /* Wait for the ACK, at which point the address is sent */

    /* There is no need to re-send the start signal because we no longer need to enter receive mode when writing data */
    iic_send_byte(data);        /* Send 1 byte */
    iic_wait_ack();             /* Waiting for ACK */
    iic_stop();                 /* Produces a stopping condition */
    HAL_Delay(10);              /* Note: EEPROM writes are slow and must wait until 10ms before writing a word */
}

/**
 * @brief  checks if AT24CXX works
 * @param  None
 * @retval detection result
 *         0: Detection was successful
 *         1: Detection failed
 */
uint8_t at24cxx_check(void)
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;
    temp = at24cxx_read_one_byte(addr);

    if (temp == 0X55)   /* Reading data is normal. */
    {
        return 0;
    }
    else    /* Exclude the first initialization */
    {
        at24cxx_write_one_byte(addr, 0X55); /* Write data first */
        temp = at24cxx_read_one_byte(255);  /* Rereading data */

        if (temp == 0X55)return 0;
    }

    return 1;
}

/**
 * @brief   starts reading the specified amount of data at the specified address in AT24CXX
 * @param   addr    : Start reading out the address pair 24c02 from 0 to 255
 * @param   pbuf    : the address at the beginning of the data array
 * @param   datalen : The number of data to read out
 * @retval  None
 */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        *pbuf++ = at24cxx_read_one_byte(addr++);
    }
}

/**
 * @brief   starts writing the specified amount of data at the specified address in AT24CXX
 * @param   addr    : Start writing at address 0-255 for 24c02
 * @param   pbuf    : the address at the beginning of the data array
 * @param   datalen : The amount of data to write
 * @retval  None
 */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
{
    while (datalen--)
    {
        at24cxx_write_one_byte(addr, *pbuf);
        addr++;
        pbuf++;
    }
}
