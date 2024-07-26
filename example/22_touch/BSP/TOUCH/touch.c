/**
 ****************************************************************************************************
 * @file        touch.c
 * @author      ALIENTEK
 * @brief       touch drive code
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

#include "touch.h"
#include "../../SYSTEM/delay/delay.h"
#include "../../BSP/24CXX/24cxx.h"
#include "lcd.h"
#include "gt9xxx.h"
#include "ft5206.h"
#include <stdio.h>
#include <stdlib.h>

/* The touch screen controls the structure variables */
_m_tp_dev tp_dev =
{
    tp_init,
    tp_scan,
    tp_adjust,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

/**
 * @brief  SPI write data
 * @param  data: The data to be written
 * @retval None
 */
static void tp_write_byte(uint8_t data)
{
    uint8_t count;
    
    for (count=0; count<8; count++)
    {
        if (data & 0x80)    /* Send 1 */
        {
            T_MOSI(1);
        }
        else                /* Send 0 */
        {
            T_MOSI(0);
        }
        
        data <<= 1;
        T_CLK(0);
        delay_us(1);
        T_CLK(1);           /* Effective rising edge */
    }
}

/**
 * @brief  SPI read data
 * @param  cmd: directive
 * @retval the data read
 */
static uint16_t tp_read_ad(uint8_t cmd)
{
    uint8_t count;
    uint16_t num = 0;
    
    T_CLK(0);                           /* Let's pull down the clock */
    T_MOSI(0);                          /* Pull down the data line */
    T_CS(0);                            /* Select touch screen IC */
    tp_write_byte(cmd);                 /* Sending commands */
    delay_us(6);                        /* The conversion time of ADS7846 is up to 6us */
    T_CLK(0);
    delay_us(1);
    T_CLK(1);                           /* Give 1 clock, clear BUSY */
    delay_us(1);
    T_CLK(0);
    
    for (count=0; count<16; count++)    /* Read out 16 bits of data, only the high 12 bits are valid */
    {
        num <<= 1;
        T_CLK(0);                       /* Effective falling edge */
        delay_us(1);
        T_CLK(1);
        if (T_MISO)
        {
            num++;
        }
    }
    
    num >>= 4;                          /* Only the high 12 bits are valid */
    T_CS(1);                            /* Release sheet selection */
    
    return num;
}

/* Resistance touch drive related parameter definition */
#define TP_READ_TIMES   5   /* Number of reads */
#define TP_LOST_VAL     1   /* Discarding values */

/**
 * @brief Reads a coordinate value (x or y)
 * @param cmd: directive
 * @arg    0x90: Read X-axis coordinates (portrait state, landscape state and Y swapped)
 * @arg    0xD0: Read Y-axis coordinates (portrait, landscape and X swapped)
 * @retval Read (filtered), ADC (12bit)
 */
static uint16_t tp_read_xoy(uint8_t cmd)
{
    uint16_t i, j;
    uint16_t buf[TP_READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    
    for (i=0; i<TP_READ_TIMES; i++)                             /* READ the data TP READ TIMES first */
    {
        buf[i] = tp_read_ad(cmd);
    }
    
    for (i=0; i<(TP_READ_TIMES - 1); i++)                       /* Sort the data */
    {
        for (j=(i + 1); j<TP_READ_TIMES; j++)
        {
            if (buf[i] > buf[j])                                /* ascending sort */
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    
    for (i=TP_LOST_VAL; i<(TP_READ_TIMES - TP_LOST_VAL); i++)   /* Discard values at both ends */
    {
        sum += buf[i];                                          /* Sum up the data after discarding the value */
    }
    
    temp = sum / (TP_READ_TIMES - 2 * TP_LOST_VAL);             /* averaging */
    
    return temp;
}

/**
 * @brief  reads the X/Y coordinates
 * @param  x: The x-coordinate read
 * @param  y: The Y coordinate read
 * @retval None
 */
static void tp_read_xy(uint16_t *x, uint16_t *y)
{
    uint16_t xval;
    uint16_t yval;
    
    if (tp_dev.touchtype & 0x01)    /* The X/Y direction is opposite to the screen */
    {
        xval = tp_read_xoy(0x90);   /* Read the X-axis coordinate AD value */
        yval = tp_read_xoy(0xD0);   /* Read the Y-axis coordinate AD value */
    }
    else                            /* The X/Y direction is the same as the screen */
    {
        xval = tp_read_xoy(0xD0);   /* Read the X-axis coordinate AD value */
        yval = tp_read_xoy(0x90);   /* Read the Y-axis coordinate AD value */
    }
    
    *x = xval;
    *y = yval;
}

/* Definition of the maximum error allowed for two consecutive reads of coordinate data */
#define TP_ERR_RANGE    50  /* error range */

/**
 * @brief  reads the touch IC data 2 times in a row and filters it
 * @param  x: The x-coordinate read
 * @param  y: The Y coordinate read
 * @retval reads the result
 * @arg    0: failed
 * @arg    1: Success
 */
static uint8_t tp_read_xy2(uint16_t *x, uint16_t *y)
{
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
    
    tp_read_xy(&x1, &y1);   /* Read data for the first time */
    tp_read_xy(&x2, &y2);   /* Read data for the second time */
    
    /* The two samples before and after are within the +-TP_ERR_RANGE */
    if (((x2 <= x1 && x1 < x2 + TP_ERR_RANGE) || (x1 <= x2 && x2 < x1 + TP_ERR_RANGE)) &&
            ((y2 <= y1 && y1 < y2 + TP_ERR_RANGE) || (y1 <= y2 && y2 < y1 + TP_ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        
        return 1;
    }
    
    return 0;
}

/**
 * @brief  Draws a touch point (cross) for calibration
 * @param  x: The x-coordinate of the cross
 * @param  y: The y-coordinate of the cross
 * @param  color: The color of the cross
 * @retval None
 */
static void tp_draw_touch_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_line(x - 12, y, x + 13, y, color);
    lcd_draw_line(x, y - 12, x, y + 13, color);
    lcd_draw_point(x + 1, y + 1, color);
    lcd_draw_point(x - 1, y + 1, color);
    lcd_draw_point(x + 1, y - 1, color);
    lcd_draw_point(x - 1, y - 1, color);
    lcd_draw_circle(x, y, 6, color);
}

/**
 * @brief  Draw a big dot
 * @param  x: The x-coordinate of the point
 * @param  y: The y-coordinate of the point
 * @param  color: The color of the dot
 * @retval None
 */
void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_draw_point(x, y, color);
    lcd_draw_point(x + 1, y, color);
    lcd_draw_point(x, y + 1, color);
    lcd_draw_point(x + 1, y + 1, color);
}

/**
 * @brief  touch screen scanning
 * @param  mode: Coordinate mode
 * @arg    0: Screen coordinates
 * @arg    1: Physical coordinates (for special occasions such as calibration)
 * @retval The current touch state
 * @arg    0: No touch
 * @arg    1: There is touch
 */
uint8_t tp_scan(uint8_t mode)
{
    if (T_PEN == 0)                                                                                     /* There are touch presses */
    {
        if (mode)                                                                                       /* Read physical coordinates without conversion */
        {
            tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]);
        }
        else if (tp_read_xy2(&tp_dev.x[0], &tp_dev.y[0]))                                               /* To read screen coordinates, conversion is required */
        {
            tp_dev.x[0] = (signed short)(tp_dev.x[0] - tp_dev.xc) / tp_dev.xfac + lcddev.width / 2;     /* Convert X-axis physical coordinates to logical coordinates (corresponding to the X coordinate value on the LCD screen) */
            tp_dev.y[0] = (signed short)(tp_dev.y[0] - tp_dev.yc) / tp_dev.yfac + lcddev.height / 2;    /* Convert X-axis physical coordinates to logical coordinates (corresponding to the Y coordinate value on the LCD screen) */
        }
        
        if ((tp_dev.sta & TP_PRES_DOWN) == 0)                                                           /* It was not pressed before */
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;                                                   /* Press the key */
            tp_dev.x[CT_MAX_TOUCH - 1] = tp_dev.x[0];                                                   /* Record the coordinates of the first press */
            tp_dev.y[CT_MAX_TOUCH - 1] = tp_dev.y[0];
        }
    }
    else                                                                                                /* Touch free press */
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                                                  /* It was pressed before */
        {
            tp_dev.sta &= ~TP_PRES_DOWN;                                                                /* Mark button release */
        }
        else                                                                                            /* It wasn't pressed before */
        {
            tp_dev.x[CT_MAX_TOUCH - 1] = 0;
            tp_dev.y[CT_MAX_TOUCH - 1] = 0;
            tp_dev.x[0] = 0xFFFF;
            tp_dev.y[0] = 0xFFFF;
        }
    }
    
    return tp_dev.sta & TP_PRES_DOWN;                                                                   /* Returns the current touch screen state */
}

/* TP_SAVE_ADDR_BASE defines the position (start address) where touch screen calibration parameters are stored in EEPROM.
 * Footprint: 13 bytes
 */
#define TP_SAVE_ADDR_BASE   40

/**
 * @brief  holds the calibration parameters
 * @param  None
 * @retval None
 */
void tp_save_adjust_data(void)
{
    uint8_t *p;

    p = (uint8_t *)&tp_dev.xfac;                            /* Point to the first address */
    at24cxx_write(TP_SAVE_ADDR_BASE, p, 12);                /* Store 12 bytes of data (xfac, yfac, xc, yc) */
    at24cxx_write_one_byte(TP_SAVE_ADDR_BASE + 12, 0x0A);   /* Saving calibration values */
}

/**
 * @brief  gets the calibration value stored in EEPROM
 * @param  None
 * @retval Gets the result
 * @arg    0: failed
 * @arg    1: Success
 */
static uint8_t tp_get_adjust_data(void)
{
    uint8_t *p;
    uint8_t temp;
    
    p = (uint8_t *)&tp_dev.xfac;

    at24cxx_read(TP_SAVE_ADDR_BASE, p, 12);                 /* Read 12 bytes of data */
    temp = at24cxx_read_one_byte(TP_SAVE_ADDR_BASE + 12);   /* Read the calibration status marker */
    if (temp == 0x0A)
    {
        return 1;
    }
    
    return 0;
}

/* Prompt string */
static char *TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

/**
 * @brief  Prompt calibration results (for each parameter)
 * @param  xy[5][2]: Five physical coordinates
 * @param  px: x-direction scaling factor (around 1 is better)
 * @param  py: Scale factor in the Y direction (around 1 is better)
 * @retval None
 */
static void tp_adjust_info_show(uint16_t xy[5][2], double px, double py)
{
    uint8_t i;
    char sbuf[20];
    
    for (i=0; i<5; i++)                                                                         /* Display five physical coordinate values */
    {
        sprintf(sbuf, "x%d:%d", i + 1, xy[i][0]);
        lcd_show_string(40, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
        sprintf(sbuf, "y%d:%d", i + 1, xy[i][1]);
        lcd_show_string(40 + 80, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
    }
    
    /* Displays the scale factor in the X/Y direction */
    lcd_fill(40, 160 + (i * 20), lcddev.width - 1, 16, WHITE);                                  /* Clear the previous px, py display */
    sprintf(sbuf, "px:%0.2f", px);
    sbuf[7] = 0;                                                                                /* Adding a terminator */
    lcd_show_string(40, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
    sprintf(sbuf, "py:%0.2f", py);
    sbuf[7] = 0;                                                                                /* Adding a terminator */
    lcd_show_string(40 + 80, 160 + (i * 20), lcddev.width, lcddev.height, 16, sbuf, RED);
}

/**
 * @brief  touch screen calibration
 * @param  None
 * @retval None
 */
void tp_adjust(void)
{
    uint16_t pxy[5][2];                                                                                         /* Physical coordinates cache values */
    uint8_t cnt = 0;
    short s1;                                                                                                   /* The coordinate difference of the first point */
    short s2;                                                                                                   /* The coordinate difference of the second point */
    short s3;                                                                                                   /* The coordinate difference of the third point */
    short s4;                                                                                                   /* The coordinate difference of the fourth point */
    double px;                                                                                                  /* The X-axis physical coordinate scale is used to determine whether the calibration is successful */
    double py;                                                                                                  /* The Y-axis physical coordinate scale is used to determine whether the calibration is successful */
    uint16_t outtime = 0;
    
    lcd_clear(WHITE);                                                                                           /* Clear screen */
    lcd_show_string(40, 40, 160, 100, 16, TP_REMIND_MSG_TBL, RED);                                              /* Display prompt information */
    tp_draw_touch_point(20, 20, RED);                                                                           /* Draw point 1 */
    tp_dev.sta = 0;                                                                                             /* Eliminate trigger signals */
    
    while (1)                                                                                                   /* If it is not pressed for 10 seconds, it will exit automatically */
    {
        tp_dev.scan(1);                                                                                         /* Scanning physical coordinates */
        
        if ((tp_dev.sta & 0xC000) == TP_CATH_PRES)                                                              /* The key is pressed once (the key is released) */
        {
            outtime = 0;
            tp_dev.sta &= ~TP_CATH_PRES;                                                                        /* The marking key has already been handled */
            pxy[cnt][0] = tp_dev.x[0];                                                                          /* Save the X physical coordinates */
            pxy[cnt][1] = tp_dev.y[0];                                                                          /* Save the Y physical coordinates */
            cnt++;
            switch (cnt)
            {
                case 1:
                {
                    tp_draw_touch_point(20, 20, WHITE);                                                         /* Clear point 1 */
                    tp_draw_touch_point(lcddev.width - 20, 20, RED);                                            /* Draw point 2 */
                    break;
                }
                case 2:
                {
                    tp_draw_touch_point(lcddev.width - 20, 20, WHITE);                                          /* Clear point 2 */
                    tp_draw_touch_point(20, lcddev.height - 20, RED);                                           /* Draw point 3 */
                    break;
                }
                case 3:
                {
                    tp_draw_touch_point(20, lcddev.height - 20, WHITE);                                         /* Clear point 3 */
                    tp_draw_touch_point(lcddev.width - 20, lcddev.height - 20, RED);                            /* Draw point 4 */
                    break;
                }
                case 4:
                {
                    lcd_clear(WHITE);                                                                           /* Let's draw the fifth point. Let's clear the screen */
                    tp_draw_touch_point(lcddev.width / 2, lcddev.height / 2, RED);                              /* Draw point 5 */
                    break;
                }
                case 5:                                                                                         /* All five points have been obtained */
                {
                    s1 = pxy[1][0] - pxy[0][0];                                                                 /* The difference in X-axis physical coordinates between the second point and the first point (AD value) */
                    s3 = pxy[3][0] - pxy[2][0];                                                                 /* The difference in X-axis physical coordinates between the fourth point and the third point (AD value) */
                    s2 = pxy[3][1] - pxy[1][1];                                                                 /* The difference in X-axis physical coordinates between the fourth point and the second point (AD value) */
                    s4 = pxy[2][1] - pxy[0][1];                                                                 /* The difference in X-axis physical coordinates between the third point and the first point (AD value) */
                    
                    px = (double)s1 / s3;                                                                       /* X-axis scaling factor */
                    py = (double)s2 / s4;                                                                       /* Y-axis scaling factor */
                    
                    if (px < 0)
                    {
                        px = -px;                                                                               /* Corrections for negative numbers */
                    }
                    if (py < 0)
                    {
                        py = -py;                                                                               /* Corrections for negative numbers */
                    }
                    
                    if ((px < 0.95) || (px > 1.05) || (py < 0.95) || (py > 1.05) ||                             /* Unqualified proportion */
                        (abs(s1) > 4095) || (abs(s2) > 4095) || (abs(s3) > 4095) || (abs(s4) > 4095) ||         /* The difference is not qualified and is larger than the coordinate range */
                        (abs(s1) == 0) || (abs(s2) == 0) || (abs(s3) == 0) || (abs(s4) == 0)                    /* The difference is not qualified and is equal to 0 */
                       )
                    {
                        cnt = 0;
                        tp_draw_touch_point(lcddev.width / 2, lcddev.height / 2, WHITE);                        /* Clear point 5 */
                        tp_draw_touch_point(20, 20, RED);                                                       /* Let me redraw point 1 */
                        tp_adjust_info_show(pxy, px, py);                                                       /* Display current information, easy to find problems */
                        continue;
                    }
                    
                    tp_dev.xfac = (float)(s1 + s3) / (2 * (lcddev.width - 40));
                    tp_dev.yfac = (float)(s2 + s4) / (2 * (lcddev.height - 40));
                    
                    tp_dev.xc = pxy[4][0];                                                                      /* X-axis, physical center coordinates */
                    tp_dev.yc = pxy[4][1];                                                                      /* Y-axis, physical center coordinates */
                    
                    lcd_clear(WHITE);                                                                           /* Clear screen */
                    lcd_show_string(35, 110, lcddev.width, lcddev.height, 16, "Touch Screen Adjust OK!", BLUE); /* Correction completed */
                    HAL_Delay(1000);
                    tp_save_adjust_data();
                    lcd_clear(WHITE);                                                                           /* Clear screen */
                    
                    return;                                                                                     /* Correction completed */
                }
            }
        }
        
        HAL_Delay(10);
        outtime++;
        if (outtime > 1000)
        {
            tp_get_adjust_data();
            break;
        }
    }
}

/**
 * @brief  Initializes the touch screen
 * @param  None
 * @retval Whether the touch screen is calibrated
 * @arg    0: No
 * @arg    1: Yes
 */
uint8_t tp_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    tp_dev.touchtype = 0;                                                               /* Default Settings (resistor, portrait) */
    tp_dev.touchtype |= lcddev.dir & 0x01;                                              /* Determine whether the screen is landscape or portrait based on the LCD */
    
    if ((lcddev.id == 0x5510) ||                                                        /* Capacitive touch screen, 4.3 "/10.1" screen */
        (lcddev.id == 0x9806) ||
        (lcddev.id == 0x7796) ||
        (lcddev.id == 0x4342) ||
        (lcddev.id == 0x4384) ||
        (lcddev.id == 0x1018))
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;                                                      /* Set the touch screen scan function */
        tp_dev.touchtype |= 0x80;                                                       /* capacitive touch screen */
        return 0;
    }
    else if ((lcddev.id == 0x1963) || (lcddev.id == 0x7084) || (lcddev.id == 0x7016))   /* SSD1963 7 inch screen or 7 inch 800*480/1024*600 RGB screen */
    {
        if (ft5206_init() == 0)
        {
            tp_dev.scan = ft5206_scan;                                                  /* Set the touch screen scan function */
        }
        else
        {
            gt9xxx_init();
            tp_dev.scan = gt9xxx_scan;                                                  /* Set the touch screen scan function */
        }
        tp_dev.touchtype |= 0x80;                                                       /* capacitive touch screen */
        return 0;
    }
    else                                                                                /* resistive touch screen */
    {
        T_PEN_GPIO_CLK_ENABLE();
        T_CS_GPIO_CLK_ENABLE();
        T_MISO_GPIO_CLK_ENABLE();
        T_MOSI_GPIO_CLK_ENABLE();
        T_CLK_GPIO_CLK_ENABLE();
        
        gpio_init_struct.Pin = T_PEN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_INPUT;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(T_PEN_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = T_CS_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(T_CS_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = T_MISO_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_INPUT;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(T_MISO_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = T_MOSI_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(T_MOSI_GPIO_PORT, &gpio_init_struct);
        
        gpio_init_struct.Pin = T_CLK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(T_CLK_GPIO_PORT, &gpio_init_struct);
        
        tp_read_xy(&tp_dev.x[0], &tp_dev.y[0]);                                         /* First read initialization */
        at24cxx_init();                                                                 /* Initialize AT24CXX */
        
        if (tp_get_adjust_data())                                                       /* Calibrated touch */
        {
            return 0;
        }
        else                                                                            /* Uncalibrated touch */
        {
            tp_adjust();                                                                /* Touch screen calibration */
            tp_save_adjust_data();                                                      /* Saving calibration parameters */
        }
        
        tp_get_adjust_data();                                                           /* Obtain the touch screen calibration parameters */
    }
    
    return 1;
}
