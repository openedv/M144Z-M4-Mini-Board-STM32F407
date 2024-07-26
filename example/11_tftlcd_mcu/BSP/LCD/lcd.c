/**
 ****************************************************************************************************
 * @file        lcd.c
 * @author      ALIENTEK
 * @brief       lcd code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M144-STM32F407 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#include "stdlib.h"
#include "lcd.h"
#include "fsmc.h"
#include "lcd_ex.c"
#include "lcdfont.h"

FSMC_NORSRAM_TimingTypeDef ExtTiming = {0};

/* Background color when drawing LCD */
uint32_t g_back_color = 0xFFFF;
uint32_t g_point_color = 0XF800;

/* LCD Important parameters */
_lcd_dev lcddev;

/**
 * @brief   LCD write data
 * @param   data: data to be written
 * @retval  None
 */
void lcd_wr_data(volatile uint16_t data)
{
    data = data;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCD write register number/address function
 * @param   regno: register number/address
 * @retval  None
 */
void lcd_wr_regno(volatile uint16_t regno)
{
    regno = regno;
    LCD->LCD_REG = regno;
}

/**
 * @brief   LCD write register function
 * @param   regno: register number/address
 * @param   data: data to be written
 * @retval  None
 */
void lcd_write_reg(uint16_t regno, uint16_t data)
{
    LCD->LCD_REG = regno;
    LCD->LCD_RAM = data;
}

/**
 * @brief   LCD read register function
 * @param   None.
 * @retval  The data read
 */
static uint16_t lcd_rd_data(void)
{
    volatile uint16_t ram;
    
    ram = LCD->LCD_RAM;
    
    return ram;
}

/**
 * @brief   LCD delay function
 * @param   i: The value of the delay.
 * @retval  None
 */
static void lcd_opt_delay(uint32_t i)
{
    while (i--);
}

/**
 * @brief   LCD ready to write GRAM
 * @param   None.
 * @retval  None.
 */
void lcd_write_ram_prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/**
 * @brief   Read the color value of a point.
 * @param   x,y: coordinate
 * @retval  The color of this point.
 */
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
    
    if ((x >= lcddev.width) || (y >= (lcddev.height)))          /* If the range is exceeded, return directly. */
    {
        return 0;
    }
    
    lcd_set_cursor(x, y);                                       /* Setting coordinates */
    
    if (lcddev.id == 0x5510)                                    /* 5510 */
    {
        lcd_wr_regno(0x2E00);
    }
    else
    {
        lcd_wr_regno(0x2E);                                     /* 9341/5310/1963/7789 */
    }
    
    r = lcd_rd_data();                                          /* dummy read */
    if (lcddev.id == 0x1963)                                    /* 1963 */
    {
        return r;                                               /* 1963 just read it */
    }
    
    lcd_opt_delay(2);
    r = lcd_rd_data();                                          /* Actual coordinate color */
    lcd_opt_delay(2);
    b = lcd_rd_data();
    g = (r & 0xFF) << 8;										/* For 9341/5310/5510/7789/9806, the first read is the value of the RG R in the former, G, 8 bits of each item. */
    
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  /* RGB565 */
}

/**
 * @brief   LCD enable display
 * @param   None.
 * @retval  None.
 */
void lcd_display_on(void)
{
    if (lcddev.id == 0x5510)    /* 5510 */
    {
        lcd_wr_regno(0x2900);
    }
    else                        /* 9341/5310/1963/7789 */
    {
        lcd_wr_regno(0x29);
    }
}

/**
 * @brief   LCD disable display
 * @param   None.
 * @retval  None.
 */
void lcd_display_off(void)
{
    if (lcddev.id == 0x5510)    /* 5510 */
    {
        lcd_wr_regno(0x2800);
    }
    else                        /* 9341/5310/1963/7789 */
    {
        lcd_wr_regno(0x28);
    }
}

/**
 * @brief   Sets the cursor position
 * @param   x,y: coordinate
 * @retval  None.
 */
void lcd_set_cursor(uint16_t x, uint16_t y)
{
    if (lcddev.id == 0x1963)        /* 1963 */
    {
        if (lcddev.dir == 0)        /* In portrait mode, the x-coordinate needs to be transformed */
        {
            x = lcddev.width - 1 - x;
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(0);
            lcd_wr_data(0);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
        }
        else                        /* landscape mode */
        {
            lcd_wr_regno(lcddev.setxcmd);
            lcd_wr_data(x >> 8);
            lcd_wr_data(x & 0xFF);
            lcd_wr_data((lcddev.width - 1) >> 8);
            lcd_wr_data((lcddev.width - 1) & 0xFF);
        }
        
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else if (lcddev.id == 0x5510)   /* 5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(y & 0xFF);
    }
    else                            /* 9341/5310/7789 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(x >> 8);
        lcd_wr_data(x & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(y >> 8);
        lcd_wr_data(y & 0xFF);
    }
}

/**
 * @brief   Set the automatic scanning direction of LCD
 * @param   dir: 0 to 7, representing 8 directions (see lcd.h for definitions)
 * @retval  None.
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg;
    uint16_t temp;
    
    if (((lcddev.dir == 1) && (lcddev.id != 0x1963)) || ((lcddev.dir == 0) && (lcddev.id == 0x1963)))
    {
        switch (dir)   /* Change of direction */
        {
            case L2R_U2D:
            {
                dir = D2U_L2R;
                break;
            }
            case L2R_D2U:
            {
                dir = D2U_R2L;
                break;
            }
            case R2L_U2D:
            {
                dir = U2D_L2R;
                break;
            }
            case R2L_D2U:
            {
                dir = U2D_R2L;
                break;
            }
            case U2D_L2R:
            {
                dir = L2R_D2U;
                break;
            }
            case U2D_R2L:
            {
                dir = L2R_U2D;
                break;
            }
            case D2U_L2R:
            {
                dir = R2L_D2U;
                break;
            }
            case D2U_R2L:
            {
                dir = R2L_U2D;
                break;
            }
        }
    }
    
    /* Set the value of 0X36/0X3600 register bit 5,6,7 according to the scan mode */
    switch (dir)
    {
        case L2R_U2D:
        {
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        }
        case L2R_D2U:
        {
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        }
        case R2L_U2D:
        {
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        }
        case R2L_D2U:
        {
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        }
        case U2D_L2R:
        {
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        }
        case U2D_R2L:
        {
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        }
        case D2U_L2R:
        {
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        }
        case D2U_R2L:
        {
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
        }
    }
    
    if (lcddev.id == 0x5510)                    /* 0x5510 */
    {
        dirreg = 0x3600;
    }
    else                                        /* 9341/5310/1963/7789 */
    {
        dirreg = 0x36;
    }
    
    /* 9341 & 7789 & 7796 to set the BGR bit */
    if ((lcddev.id == 0x9341) || (lcddev.id == 0x7789))
    {
        regval |= 0x08;
    }
    
    lcd_write_reg(dirreg, regval);
    
    if (lcddev.id != 0x1963)                    /* 1963 no coordinate manipulation */
    {
        if (regval & 0x20)
        {
            if (lcddev.width < lcddev.height)   /* Swap X and Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   /* Swap X and Y */
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }
    
    /* Set the size of the display area (window) */
    if (lcddev.id == 0x5510)                    /* 0x5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(0);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
    else                                        /* 9341/5310/1963/7789 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.width - 1) >> 8);
        lcd_wr_data((lcddev.width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(0);
        lcd_wr_data(0);
        lcd_wr_data((lcddev.height - 1) >> 8);
        lcd_wr_data((lcddev.height - 1) & 0xFF);
    }
}

/**
 * @brief   Draw dots
 * @param   x,y  : coordinate
 * @param   color: Point color
 * @retval  None.
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    lcd_set_cursor(x, y);       /* Sets the cursor position */
    lcd_write_ram_prepare();    /* Start writing GRAM */
    LCD->LCD_RAM = color;
}

/**
 * @brief   SSD1963 backlight brightness setting function
 * @param   pwm  : Backlight level,0~100. The larger the brighter.
 * @retval  None.
 */
void lcd_ssd_backlight_set(uint8_t pwm)
{
    lcd_wr_regno(0xBE);         /* Configure PWM output */
    lcd_wr_data(0x05);          /* 1 Set the PWM frequency */
    lcd_wr_data(pwm * 2.55);    /* 2 Set the PWM duty cycle */
    lcd_wr_data(0x01);          /* 3 Setting C */
    lcd_wr_data(0xFF);          /* 4 Setting D */
    lcd_wr_data(0x00);          /* 5 Setting E */
    lcd_wr_data(0x00);          /* 6 Setting F */
}

/**
 * @brief   Set the LCD display orientation
 * @param   dir  : 0, portrait; 1. Landscape
 * @retval  None.
 */
void lcd_display_dir(uint8_t dir)
{
    lcddev.dir = dir;					/* Portrait/landscape */
    
    if (dir == 0)                       /* Portrait */
    {
        lcddev.width = 240;
        lcddev.height = 320;
        
        if (lcddev.id == 0x5510)        /* 5510 */
        {
            lcddev.wramcmd = 0x2C00;  	/* Set the instruction to write GRAM */
            lcddev.setxcmd = 0x2A00;  	/* Set the write x-coordinate instruction */
            lcddev.setycmd = 0x2B00;  	/* Set the write y-coordinate instruction */
            lcddev.width = 480;     	/* Set width 480 */
            lcddev.height = 800;    	/* Set height 800 */
        }
        else if (lcddev.id == 0x1963)   /* 1963 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2B;
            lcddev.setycmd = 0x2A;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else                            /* 9341/5310/7789 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
            if (lcddev.id == 0x5310)    /* 5310 */
            {
                lcddev.width = 320;
                lcddev.height = 480;
            }
        }
    }
    else                                /* landscape */
    {
        lcddev.width = 320;
        lcddev.height = 240;
        
        if (lcddev.id == 0x5510)        /* 5510 */
        {
            lcddev.wramcmd = 0x2C00;
            lcddev.setxcmd = 0x2A00;
            lcddev.setycmd = 0x2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0x1963)   /* 1963 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else                            /* 9341/5310/7789 */
        {
            lcddev.wramcmd = 0x2C;
            lcddev.setxcmd = 0x2A;
            lcddev.setycmd = 0x2B;
            if (lcddev.id == 0x5310)    /* 5310 */
            {
                lcddev.width = 480;
                lcddev.height = 320;
            }
        }
    }
    
    lcd_scan_dir(DFT_SCAN_DIR);         /* Default scan direction */
}

/**
 * @brief   Set the window and automatically set the point coordinates to the upper-left corner of the window (sx,sy).
 * @param   sx,sy  : Window start coordinate (top left corner)
 * @param   width,height锛?Window width and height, must be greater than 0!!
 * @note    Form size: width*height.
 * @retval  None.
 */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t twidth;
    uint16_t theight;
    
    twidth = sx + width - 1;
    theight = sy + height - 1;
    
    if ((lcddev.id == 0x1963) && (lcddev.dir != 1)) /* 1963 Portrait special treatment */
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data((sx + width - 1) >> 8);
        lcd_wr_data((sx + width - 1) & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(height >> 8);
        lcd_wr_data(height & 0xFF);
    }
    else if (lcddev.id == 0x5510)                   /* 5510 */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_regno(lcddev.setxcmd + 1);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_regno(lcddev.setxcmd + 2);
        lcd_wr_data(twidth >> 8);
        lcd_wr_regno(lcddev.setxcmd + 3);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_regno(lcddev.setycmd + 1);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_regno(lcddev.setycmd + 2);
        lcd_wr_data(theight >> 8);
        lcd_wr_regno(lcddev.setycmd + 3);
        lcd_wr_data(theight & 0xFF);
    }
    else                                            /* 9341/5310/7789/1963/7796/9806 landscape and other Settings window */
    {
        lcd_wr_regno(lcddev.setxcmd);
        lcd_wr_data(sx >> 8);
        lcd_wr_data(sx & 0xFF);
        lcd_wr_data(twidth >> 8);
        lcd_wr_data(twidth & 0xFF);
        lcd_wr_regno(lcddev.setycmd);
        lcd_wr_data(sy >> 8);
        lcd_wr_data(sy & 0xFF);
        lcd_wr_data(theight >> 8);
        lcd_wr_data(theight & 0xFF);
    }
}

/**
 * @brief   Screen clearing function
 * @param   color : To clear the screen color
 * @retval  None.
 */
void lcd_clear(uint16_t color)
{
    uint32_t index;
    uint32_t totalpoint;
    
    totalpoint = lcddev.width * lcddev.height;  /* �������������� */
    lcd_set_cursor(0x00, 0x0000);               /* ���ù��λ�� */
    lcd_write_ram_prepare();                    /* ��ʼд��GRAM */
    for (index=0; index<totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

/**
 * @brief   Fills a single color in the specified area
 * @param   (sx,sy),(ex,ey) : Fill the rectangle with diagonal coordinates, and the region size is :(ex-sx + 1) * (ey-sy + 1)
 * @param   color : The color to fill
 * @retval  None.
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint16_t i;
    uint16_t j;
    uint16_t xlen;
    
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++)
    {
        lcd_set_cursor(sx, i);      /* Sets the cursor position */
        lcd_write_ram_prepare();    /* Start writing GRAM */

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM = color;   /* display color */
        }
    }
}

/**
 * @brief   Fills the specified color block in the specified area
 * @param   (sx,sy),(ex,ey) : Fill the rectangle with diagonal coordinates, and the region size is :(ex-sx + 1) * (ey-sy + 1)
 * @param   color : address at the beginning of the color array to fill
 * @retval  None.
 */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height;
    uint16_t width;
    uint16_t i;
    uint16_t j;
    
    width = ex - sx + 1;            /* Get the width of the fill */
    height = ey - sy + 1;           /* height */

    for (i = 0; i < height; i++)
    {
        lcd_set_cursor(sx, sy + i); /* Sets the cursor position */
        lcd_write_ram_prepare();    /* Start writing GRAM */

        for (j = 0; j < width; j++)
        {
            LCD->LCD_RAM = color[i * width + j]; /* write in data */
        }
    }
}

/**
 * @brief   draw line
 * @param   x1,y1 : Starting point coordinates
 * @param   x2,y2 : End point coordinates
 * @param   color : line color
 * @retval  None.
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx;
    int incy;
    int row;
    int col;
    
    /* Calculating coordinate increments */
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    
    row = x1;
    col = y1;
    
    /* Set the single-step direction */
    if (delta_x > 0)
    {
        incx = 1;
    }
    else if (delta_x == 0)
    {
        incx = 0;	/* perpendicular line */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;	/* horizontal line */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    /* Select the basic increment axis */
    if (delta_x > delta_y)
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }
    
    for (t=0; t<=(distance+1); t++)
    {
        lcd_draw_point(row, col, color);	/* Draw dots */
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief   Draw horizontal lines
 * @param   x,y : Starting point coordinates
 * @param   len : line length
 * @param   color : The color of the rectangle
 * @retval  None.
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcddev.width) || (y > lcddev.height))
    {
        return;
    }
    
    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief   Draw the rectangle
 * @param   x1,y1 : Starting point coordinates
 * @param   x2,y2 : End point coordinates
 * @param   color : The color of the rectangle
 * @retval  None.
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief   Draw a circle
 * @param   x,y   : Circle center coordinate
 * @param   r     : radius
 * @param   color : The color of the circle
 * @retval  None.
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a;
    int b;
    int di;
    
    a = 0;
    b = r;
    di = 3 - (r << 1);  /* Flag to determine the next point position */
    
    while (a <= b)
    {
        lcd_draw_point(x0 + a, y0 - b, color);
        lcd_draw_point(x0 + b, y0 - a, color);
        lcd_draw_point(x0 + b, y0 + a, color);
        lcd_draw_point(x0 + a, y0 + b, color);
        lcd_draw_point(x0 - a, y0 + b, color);
        lcd_draw_point(x0 - b, y0 + a, color);
        lcd_draw_point(x0 - a, y0 - b, color);
        lcd_draw_point(x0 - b, y0 - a, color);
        a++;
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief   Fill the solid circle
 * @param   x,y   : Circle center coordinate
 * @param   r     : radius
 * @param   color : The color of the circle
 * @retval  None.
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax;
    uint32_t sqmax;
    uint32_t xr;
    
    imax = ((uint32_t)r * 707) / 1000 + 1;
    sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    xr = r;
    
    lcd_draw_hline(x - r, y, 2 * r, color);
    for (i=1; i<=imax; i++)
    {
        if ((i * i + xr * xr) > sqmax)
        {
            if (xr > imax)
            {
                lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
                lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
            }
            xr--;
        }
        lcd_draw_hline(x - xr, y + i, 2 * xr, color);
        lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief   Displays a character at the specified position
 * @param   x,y   : Coordinates
 * @param   chr   : The character to display :" "-- >"~"
 * @param   size  : Font size 12/16/24/32
 * @param   mode  : Overlay mode (1); Non-superposition (0);
 * @param   color : The color of the character;
 * @retval  None.
 */
void lcd_show_char(uint16_t x, uint16_t y, unsigned char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t csize;
    uint8_t *pfont;
    uint16_t y0;
    uint8_t t;
    uint8_t t1;
    uint8_t temp;
    
    csize = ((size >> 3) + (((size & 0x7) != 0) ? 1 : 0)) * (size >> 1);    /* Get the number of bytes of a font character corresponding to the dot matrix set */
    chr -= ' ';                                                             /* Get the offset value (ASCII fonts start modulo Spaces, so - "is the font for the corresponding character) */
    
    switch (size)
    {
        case 12:                                                            /* Call 1206 font */
        {
            pfont = (uint8_t *)asc2_1206[chr];
            break;
        }
        case 16:                                                            /* Call 1608 font */
        {
            pfont = (uint8_t *)asc2_1608[chr];
            break;
        }
        case 24:                                                            /* Call 2412 font */
        {
            pfont = (uint8_t *)asc2_2412[chr];
            break;
        }
        case 32:                                                            /* Call 3216 font */
        {
            pfont = (uint8_t *)asc2_3216[chr];
            break;
        }
        default:
        {
            return;
        }
    }
    
    y0 = y;
    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* Retrieves the dot matrix data of the character */

        for (t1 = 0; t1 < 8; t1++)  /* There are 8 points in a byte */
        {
            if ((temp & 0x80) != 0)        /* Valid points, need to be displayed */
            {
                lcd_draw_point(x, y, color);        /* Let me draw the dot, let me show the dot */
            }
            else if (mode == 0)     /* Invalid point, not shown */
            {
                lcd_draw_point(x, y, g_back_color); /* Drawing the background color is equivalent to not showing the point (note that the background color is controlled by a global variable). */
            }

            temp <<= 1; /* Shift in order to get the state of the next bit */
            y++;

            if (y >= lcddev.height)return;  /* Hyper region */

            if ((y - y0) == size)
            {
                y = y0;          /* y coordinate reset */
                x++;             /* The x-coordinate is increasing */

                if (x >= lcddev.width)
                {
                    return;     /* The x-coordinate hyperregion */
                }

                break;
            }
        }
    }
}

/**
 * @brief   squared function, m^n
 * @param   m   : The base number
 * @param   n   : Exponent
 * @retval  None.
 */
static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    
    while (n--)
    {
        result *= m;
    }
    
    return result;
}

/**
 * @brief   displays len numbers
 * @param   x,y  : The starting coordinate
 * @param   num  : Number (0-2 ^32)
 * @param   len  : Displays the number of bits
 * @param   size : Select font 12/16/24/32
 * @param   color: The color of the number;
 * @retval  None.
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* Cycle by the total number of display bits */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;   /* Gets the number of the corresponding bit */

        if (enshow == 0 && t < (len - 1))   /* Display is not enabled, and there is still bit to display */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color);/* Display space, placeholder */
                continue;   /* Go to the next one */
            }
            else
            {
                enshow = 1; /* Enable display */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color); /* display character */
    }
}

/**
 * @brief   extension displays len numbers (high bits of 0 are also displayed)
 * @param   x,y  : the starting coordinate
 * @param   num  : Number (0-2 ^32)
 * @param   len  : Displays the number of bits
 * @param   size : Select font 12/16/24/32
 * @param   mode : Display mode
 *           [7]  :0, no padding; 1, fill with 0.
 *           [6:1]: Reserved
 *           [0]  :0, non-overlay display; 1, overlay display.
 * @param   color : The color of the number;
 * @retval  None.
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)   /* Cycle by the total number of display bits */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;    /* Gets the number of the corresponding bit */

        if (enshow == 0 && t < (len - 1))   /* Display is not enabled, and there is still bit to display */
        {
            if (temp == 0)
            {
                if (mode & 0X80)   /* The high bits need to be filled with zeros */
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color);  /* put a 0 in there */
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color);  /* Use Spaces for space */
                }

                continue;
            }
            else
            {
                enshow = 1; /* Enable display */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief   Displays the string
 * @param   x,y          : the starting coordinate
 * @param   width,height : The size of the region
 * @param   size         : Select font 12/16/24/32
 * @param   p            : The first address of the string
 * @param   color        : The color of the string;
 * @retval  None
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* Check whether it is an illegal character! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* quit */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

/**
 * @brief   Initialize LCD
 * @note    This initialization function can initialize various types of LCD
 * @param   None.
 * @retval  None.
 */
void lcd_init(void)
{
    HAL_Delay(50);
    
    /* Try a read of 9341 ID */
    lcd_wr_regno(0xD3);
    lcddev.id = lcd_rd_data();                                                              /* dummy read */
    lcddev.id = lcd_rd_data();                                                              /* Read to 0X00 */
    lcddev.id = lcd_rd_data() << 8;                                                         /* Read 0X93 */
    lcddev.id |= lcd_rd_data();																/* Read 0X41 */
    if (lcddev.id != 0x9341)																/* Not 9341, try to see if it's ST7789 */
    {
        lcd_wr_regno(0x04);
        lcddev.id = lcd_rd_data();                                                          /* dummy read */
        lcddev.id = lcd_rd_data();                                                          /* Read to 0X85 */
        lcddev.id = lcd_rd_data() << 8;                                                     /* Read 0X85 */
        lcddev.id |= lcd_rd_data();                                                         /* Read 0X52 */
        if (lcddev.id == 0x8552)                                                            /* Convert the ID of 8552 to 7789 */
        {
            lcddev.id = 0x7789;
        }
        if (lcddev.id != 0x7789)                                                            /* Also not ST7789, try not NT35310 */
        {
            lcd_wr_regno(0xD4);
            lcddev.id = lcd_rd_data();                                                      /* dummy read */
            lcddev.id = lcd_rd_data();                                                      /* Read back 0x01 */
            lcddev.id = lcd_rd_data() << 8;                                                 /* Read back 0x53 */
            lcddev.id |= lcd_rd_data();                                                     /* Read back 0x10 */
            if (lcddev.id != 0x5310)                                                        /* Also not NT35310, try to see if it is ST7796 */
            {
            	 /* Send key (provided by manufacturer) */
                lcd_write_reg(0xF000, 0x0055);
                lcd_write_reg(0xF001, 0x00AA);
                lcd_write_reg(0xF002, 0x0052);
                lcd_write_reg(0xF003, 0x0008);
                lcd_write_reg(0xF004, 0x0001);
                
                lcd_wr_regno(0xC500);
                lcddev.id = lcd_rd_data() << 8;                                             /* Read the lower eight bits of the ID */
                lcd_wr_regno(0xC501);
                lcddev.id |= lcd_rd_data();                                                 /* Read back 0x55 */
                HAL_Delay(5);
                if (lcddev.id != 0x5510)                                                   	/* Read the high eight bits of the ID */
                {
                    lcd_wr_regno(0xA1);
                    lcddev.id = lcd_rd_data();
                    lcddev.id = lcd_rd_data() << 8;                                         /* Read back 0x57 */
                    lcddev.id |= lcd_rd_data();                                             /* Read back 0x61 */
                    if (lcddev.id == 0x5761)                                                /* The ID read back by SSD1963 is 5761H, which we force to be 1963 for convenience */
                    {
                        lcddev.id = 0x1963;
                    }
                }
            }
        }
    }
    
    if (lcddev.id == 0x7789)
    {
        lcd_ex_st7789_reginit();                                                            /* Perform ST7789 initialization */
    }
    else if (lcddev.id == 0x9341)
    {
        lcd_ex_ili9341_reginit();                                                           /* Perform ILI9341 initialization */
    }
    else if (lcddev.id == 0x5310)
    {
        lcd_ex_nt35310_reginit();                                                           /* Perform NT35310 initialization */
    }
    else if (lcddev.id == 0x5510)
    {
        lcd_ex_nt35510_reginit();                                                           /* Perform ST7796 initialization */
    }
    else if (lcddev.id == 0x1963)
    {
        lcd_ex_ssd1963_reginit();                                                           /* Perform SSD1963 initialization */
        lcd_ssd_backlight_set(100);                                                         /* The backlight is set to its brightest */
    }
    
    if ((lcddev.id == 0x9341) || (lcddev.id == 0x1963) || (lcddev.id == 0x7789))            /* 9341/1963/7789 */
    {
    	ExtTiming.AddressSetupTime = 3;
    	ExtTiming.DataSetupTime = 3;
        FSMC_NORSRAM_Extended_Timing_Init(hsram1.Extended, &ExtTiming, hsram1.Init.NSBank, hsram1.Init.ExtendedMode);
    }
    else if ((lcddev.id == 0x5310) || (lcddev.id == 0x5510))                                /* 5310/5510 */
    {
    	ExtTiming.AddressSetupTime = 2;
    	ExtTiming.DataSetupTime = 2;
        FSMC_NORSRAM_Extended_Timing_Init(hsram1.Extended, &ExtTiming, hsram1.Init.NSBank, hsram1.Init.ExtendedMode);
    }
    
    lcd_display_dir(0); /* Portrait is the default */
    LCD_BL(1);          /* Light up the backlight */
    lcd_clear(WHITE);
}
