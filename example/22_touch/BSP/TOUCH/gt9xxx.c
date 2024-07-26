/**
 ****************************************************************************************************
 * @file        gt9xxx.c
 * @author      ALIENTEK
 * @brief       gt9xxx code
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

#include "gt9xxx.h"
#include "../../SYSTEM/delay/delay.h"
#include "ctiic.h"
#include "touch.h"
#include "lcd.h"
#include <string.h>

/* The default support for touch screen 5 touch
 * Except for GT9271, which supports 10 touches, the rest only support 5 touches
 */
static uint8_t g_gt_tnum = 5;

/**
 * @brief  reads the data once from GT9XXX
 * @param  reg: the start register address
 * @param  buf: the starting address of the data store
 * @param  len: The length of the data to be read out
 * @retval None
 */
static void gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);                        /* Send write command */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);                             /* Send the high 8-bit address of the register */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg & 0xFF);                           /* Send the lower 8 bit address of the register */
    ct_iic_wait_ack();
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_RD);                        /* Send read command */
    ct_iic_wait_ack();
    for (i = 0; i < len; i++)                               /* Loop out the data */
    {
        buf[i] = ct_iic_read_byte(i == (len - 1) ? 0 : 1);
    }
    
    ct_iic_stop();
}

/**
 * @brief  writes data from GT9XXX once
 * @param  reg: the start register address
 * @param  buf: The start address of the data to be written
 * @param  len: The length of the data to be written
 * @retval writes the result
 * @arg    0: Success
 * @arg    1: Failure
 */
static uint8_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ret = 0;
    
    ct_iic_start();
    ct_iic_send_byte(GT9XXX_CMD_WR);    /* Send write command */
    ct_iic_wait_ack();
    ct_iic_send_byte(reg >> 8);         /* Send the high 8-bit address of the register */
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
 * @brief  Initializes GT9XXX
 * @param  None
 * @retval initializes the result
 * @arg    0: Success
 * @arg    1: Failure
 */
uint8_t gt9xxx_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    uint8_t temp[5];
    
    GT9XXX_RST_GPIO_CLK_ENABLE();
    GT9XXX_INT_GPIO_CLK_ENABLE();
    
    gpio_init_struct.Pin = GT9XXX_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_RST_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);
    
    /* Initialize the touch screen IIC */
    ct_iic_init();
    
    /* Reset GT9XXX */
    GT9XXX_RST(0);
    HAL_Delay(10);
    GT9XXX_RST(1);
    HAL_Delay(10);
    
    gpio_init_struct.Pin = GT9XXX_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GT9XXX_INT_GPIO_PORT, &gpio_init_struct);
    
    HAL_Delay(100);
    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);                 /* Read the GT9XXX PID */
    temp[4] = 0;

    if ((strcmp((char *)temp, "911") != 0) &&
        (strcmp((char *)temp, "9147") != 0) &&
        (strcmp((char *)temp, "1158") != 0) &&
        (strcmp((char *)temp, "9271") != 0))
    {
        return 1;
    }

    if (strcmp((char *)temp, "9271") == 0)                  /* GT9271 supports 10 touches */
    {
        g_gt_tnum = 10;
    }
    
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* Software reset GT9XXX */
    HAL_Delay(10);
    
    temp[0] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);                /* The reset is finished and the read coordinate state is entered */
    
    return 0;
}

/* GT9XXX reads the touch coordinate register */
static const uint16_t GT9XXX_TPX_TBL[10] = {
    GT9XXX_TP1_REG,
    GT9XXX_TP2_REG,
    GT9XXX_TP3_REG,
    GT9XXX_TP4_REG,
    GT9XXX_TP5_REG,
    GT9XXX_TP6_REG,
    GT9XXX_TP7_REG,
    GT9XXX_TP8_REG,
    GT9XXX_TP9_REG,
    GT9XXX_TP10_REG,
};

/**
 * @brief  scan GT9XXX touch screen
 * @param  mode: Capacitor screen is not used, compatible with resistive screen
 * @retval The current touch state
 * @arg    0: No touch
 * @arg    1: There is touch
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    static uint8_t t = 0;
    uint8_t data;
    uint16_t temp;
    uint16_t tempsta;
    uint8_t i;
    uint8_t buf[4];
    uint8_t res = 0;
    
    t++;
    if (((t % 10) == 0) || (t < 10))
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);                                  /* Read the state of the touch point */
        if ((mode & 0x80) && ((mode & 0x0F) <= g_gt_tnum))
        {
            data = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &data, 1);                              /* Clear flag */
        }
        
        if ((mode & 0x0F) && ((mode & 0x0F) <= g_gt_tnum))
        {
            temp = 0xFFFF << (mode & 0x0F);                                         /* The number of points is converted to the number of bits of 1, matching the tp dev.sta definition */
            tempsta = tp_dev.sta;                                                   /* Save the current value of tp dev.sta */
            tp_dev.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];                                  /* Save the data for contact 0, save it on the last one */
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];
            for (i=0; i<g_gt_tnum; i++)
            {
                if (tp_dev.sta & (1 << i))                                          /* Touch-point valid */
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);                       /* Read the XY coordinates */
                    
                    if ((lcddev.id == 0x5510) ||                                    /* 4.3 inch 800*480 MCU screen */
                        (lcddev.id == 0x9806) ||
                        (lcddev.id == 0x7796))
                    {
                        if (tp_dev.touchtype & 0x01)                                /* landscape */
                        {
                            tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        }
                        else                                                        /* portrait screen */
                        {
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                    }
                    else                                                            /* Other models */
                    {
                        if (tp_dev.touchtype & 0x01)                                /* landscape */
                        {
                            tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                        else                                                        /* portrait screen */
                        {
                            tp_dev.x[i] = lcddev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                            tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        }
                    }
                }
            }
            res = 1;
            
            if ((tp_dev.x[0] > lcddev.width) || (tp_dev.y[0] > lcddev.height))      /* Invalid data (coordinates out of order) */
            {
                if ((mode & 0x0F) > 1)                                              /* If some other point has data, copy the data from the second contact to the first. */
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;                                                          /* If triggered once, it will be monitored at least 10 times in a row, increasing the hit rate */
                }
                else                                                                /* Illegal data, then ignore the data (restore the original) */
                {
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum - 1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum - 1];
                    mode = 0x80;
                    tp_dev.sta = tempsta;                                           /* Restore tp dev.sta */
                }
            }
            else
            {
                t = 0;                                                              /* If triggered once, it will be monitored at least 10 times in a row, increasing the hit rate */
            }
        }
    }
    
    if ((mode & 0x8F) == 0x80)                                                      /* No touch point to press */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                              /* It was pressed before */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                            /* Mark button release */
        }
        else                                                                        /* It wasn't pressed before */
        {
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
            tp_dev.sta &= 0xE000;                                                   /* Clear the point valid marker */
        }
    }
    
    if (t > 240)
    {
        t = 10;                                                                     /* Start counting again at 10 */
    }
    
    return res;
}
