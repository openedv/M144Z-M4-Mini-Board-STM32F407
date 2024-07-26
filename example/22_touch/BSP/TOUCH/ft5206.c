/**
 ****************************************************************************************************
 * @file        ft5206.c
 * @author      ALIENTEK
 * @brief       ft5206 code
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

#include "ft5206.h"
#include "../../SYSTEM/delay/delay.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"

/**
 * @brief   reads the data once from the FT5206
 * @param   reg: the start register address
 * @param   buf: the starting address of the data store
 * @param   len: The length of the data to be read out
 * @retval  None
 */
static void ft5206_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);                        /* Send write command */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* Send the lower 8 bit address of the register */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_RD);                        /* Send read command */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* Loop out the data */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief   writes data once from FT5206
 * @param   reg: the start register address
 * @param   buf: The start address of the data to be written
 * @param   len: The length of the data to be written
 * @retval  writes the result
 * @arg     0: Success
 * @arg     1: Failure
 */
static uint8_t ft5206_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(FT5206_CMD_WR);    /* Send write command */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);       /* Send the lower 8 bit address of the register */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)           /* Cyclic writing of data */
    {
        ct_iic_send_byte(buf[i]);
        ret = ct_iic_wait_ack();
        if (ret)
        {
            break;
        }
    }
    ct_iic_stop();
    
    return ret;
}

/**
 * @brief  Initializes FT5206
 * @param  None
 * @retval initializes the result
 *         0: Success
 *         1: Failure
 */
uint8_t ft5206_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp[2];
    
    FT5206_RST_GPIO_CLK_ENABLE();
    FT5206_INT_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = FT5206_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_RST_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = FT5206_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(FT5206_INT_GPIO_PORT, &gpio_init_struct);
    
    ct_iic_init();
    
    FT5206_RST(0);
    HAL_Delay(20);
    FT5206_RST(1);
    HAL_Delay(50);
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_DEVIDE_MODE, temp, 1);             /* Configuration enters normal operation mode */
    
    temp[0] = 0;
    ft5206_wr_reg(FT5206_ID_G_MODE, temp, 1);               /* query mode */
    
    temp[0] = 22;
    ft5206_wr_reg(FT5206_ID_G_THGROUP, temp, 1);            /* Touch the effective value, the smaller the more sensitive */
    
    temp[0] = 12;
    ft5206_wr_reg(FT5206_ID_G_PERIODACTIVE, temp, 1);       /* Activation period, cannot be less than 12, maximum 14 */
    
    ft5206_rd_reg(FT5206_ID_G_LIB_VERSION, temp, 2);        /* Reading the version number */
    if (((temp[0] == 0x30) && (temp[1] == 0x03)) ||         /* 0x3003 */
        ((temp[0] == 0x00) && (temp[1] == 0x01)) ||         /* 0x0001 */
        ((temp[0] == 0x00) && (temp[1] == 0x02)) ||         /* 0x0002 */
        ((temp[0] == 0x00) && (temp[1] == 0x00)))           /* CST340 */
    {
        return 0;
    }
    
    return 1;
}

/* The FT5206 reads the touch coordinate register */
static const uint16_t FT5206_TPX_TBL[5] = {
    FT5206_TP1_REG,
    FT5206_TP2_REG,
    FT5206_TP3_REG,
    FT5206_TP4_REG,
    FT5206_TP5_REG,
};

/**
 * @brief  scan FT5206 read touch screen
 * @param  mode: Capacitor screen is not used, compatible with resistive screen
 * @retval The current touch state
 * @arg    0: No touch
 * @arg    1: There is touch
 */
uint8_t ft5206_scan(uint8_t mode)
{
    static uint8_t t = 0;
    uint16_t temp;
    uint8_t i;
    uint8_t buf[4];
    uint8_t res = 0;
    
    t++;
    if (((t % 10) == 0) || (t < 10))
    {
        ft5206_rd_reg(FT5206_REG_NUM_FINGER, &mode, 1);                                             /* Read the state of the touch point */
        if ((mode & 0x0F) && ((mode & 0x0F) < 6))
        {
            temp = 0xFFFF << (mode & 0x0F);                                                         /* The number of points is converted to the number of bits of 1, matching the tp dev.sta definition */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            HAL_Delay(4);
            
            for (i=0; i<5; i++)
            {
                if (tp_dev.sta & (1 << i))                                                          /* Touch-point valid */
                {
                    ft5206_rd_reg(FT5206_TPX_TBL[i], buf, 4);                                       /* Read the XY coordinates */
                    
                    if (tp_dev.touchtype & 0x01)                                                    /* landscape */
                    {
                        tp_dev.y[i] = ((uint16_t)(buf[0] & 0x0F) << 8) + buf[1];
                        tp_dev.x[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    else                                                                            /* portrait screen */
                    {
                        tp_dev.x[i] = lcddev.width - (((uint16_t)(buf[0] & 0x0F) << 8) + buf[1]);
                        tp_dev.y[i] = ((uint16_t)(buf[2] & 0x0F) << 8) + buf[3];
                    }
                    
                    if ((buf[0] & 0xF0) != 0x80)
                    {
                        tp_dev.x[i] = tp_dev.y[i] = 0;                                              /* It must be a contact event to be considered valid */
                    }
                }
            }
            res = 1;
            
            if (tp_dev.x[0] == 0 && tp_dev.y[0] == 0)                                               /* All values read are 0, so we ignore this one */
            {
                mode = 0;
            }
            
            t = 0;                                                                                  /* If triggered once, it will be monitored at least 10 times in a row, increasing the hit rate */
        }
    }
    
    if ((mode & 0x1F) == 0)                                                                         /* No touch point to press */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                                              /* It was pressed before */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                                            /* Mark button release */
        }
        else                                                                                        /* It wasn't pressed before */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                                   /* Clear the point valid marker */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                                     /* Start counting again at 10 */
    }
    
    return res;
}
