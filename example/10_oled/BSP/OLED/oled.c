/**
 ****************************************************************************************************
 * @file        oled.c
 * @author      ALIENTEK
 * @brief       oled code
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
 * V1.0         20240410     the first version
 *
 ****************************************************************************************************
 */

#include "oled.h"
#include "oledfont.h"
#include "../../SYSTEM/delay/delay.h"

/* 
 * OLED video memory
 * Each byte represents 8 pixels, 128 means there are 128 columns, 8 means there are 64 rows, and the high bit means the high number of rows.
 * For example: g_oled_gram[0][0], which contains the first column, rows 1 through 8, and g_oled_gram[0][0].0, which represents the coordinate (0,0).
 * Similarly: g_oled_gram[1][0].1 for the coordinate (1,1), and g_oled_gram[10][1].2 for the coordinate (10,10).
 * 
 * The format is as follows (high bits indicate high rows)
 * [0]0 1 2 3 ... 127
 * [1]0 1 2 3 ... 127
 * [2]0 1 2 3 ... 127
 * [3]0 1 2 3 ... 127
 * [4]0 1 2 3 ... 127
 * [5]0 1 2 3 ... 127
 * [6]0 1 2 3 ... 127
 * [7]0 1 2 3 ... 127
 */
static uint8_t g_oled_gram[128][8];

#if (OLED_MODE == 1)    /* OLED uses 8080 parallel port mode */

/**
 * @brief   outputs an 8-bit data to the OLED using a patchwork method
 * @param   data : The data to output
 * @retval  None
 */
static void oled_data_out(uint8_t data)
{
    GPIOC->ODR &= ~(0xF << 6);                          /* GPIOC[9:6] Reset */
    GPIOC->ODR |= ((uint16_t)data & 0xF) << 6;          /* data[3:0] --> GPIOC[9:6] */
    
    GPIOC->ODR &= ~(0x1 << 11);                         /* GPIOC[11] Reset */
    GPIOC->ODR |= ((uint16_t)(data >> 4) & 0x1) << 11;  /* data[4] --> GPIOC[11] */
    
    GPIOB->ODR &= ~(0x1 << 6);                          /* GPIOB[6] Reset */
    GPIOB->ODR |= ((uint16_t)(data >> 5) & 0x1) << 6;   /* data[5] --> GPIOB[6] */
    
    GPIOB->ODR &= ~(0x3 << 8);                          /* GPIOB[9:8] Reset */
    GPIOB->ODR |= ((uint16_t)(data >> 6) & 0x3) << 8;   /* data[7:6] --> GPIOB[9:8] */
}

/**
 * @brief   writes a byte to the OLED
 * @param   data : The data to output
 * @param   cmd  : command/data flag
 * @arg     OLED_CMD: command
 * @arg     OLED_DATA: Data
 * @retval  None
 */
static void oled_wr_byte(uint8_t data, uint8_t cmd)
{
    oled_data_out(data);
    OLED_RS(cmd);
    OLED_CS(0);
    OLED_WR(0);
    OLED_WR(1);
    OLED_CS(1);
    OLED_RS(1);
}

#else   /* The OLED uses a 4-wire SPI mode */

/**
 * @brief   writes a byte to the OLED
 * @param   data : The data to output
 * @param   cmd  : command/data flag
 * @arg     OLED_CMD: command
 * @arg     OLED_DATA: Data
 * @retval  none
 */
static void oled_wr_byte(uint8_t data, uint8_t cmd)
{
    uint8_t i;
    
    OLED_RS(cmd);
    OLED_CS(0);
    
    for (i=0; i<8; i++)
    {
        OLED_SCLK(0);
        if ((data & 0x80) != 0)
        {
            OLED_SDIN(1);
        }
        else
        {
            OLED_SDIN(0);
        }
        OLED_SCLK(1);
        data <<= 1;
    }
    
    OLED_CS(1);
    OLED_RS(1);
}

#endif

/**
 * @brief  updates video memory to OLED
 * @param  None
 * @retval None
 */
void oled_refresh_gram(void)
{
    uint8_t i;
    uint8_t n;
    
    for (i=0; i<8; i++)
    {
        oled_wr_byte(0xB0 + i, OLED_CMD);   /* Set page address (0-7) */
        oled_wr_byte(0x00, OLED_CMD);       /* Set display location - column low address */
        oled_wr_byte(0x10, OLED_CMD);       /* Set display location - column high address */
        for (n=0; n<128; n++)
        {
            oled_wr_byte(g_oled_gram[n][i], OLED_DATA);
        }
    }
}

/**
 * @brief  turns on the OLED display
 * @param  None
 * @retval None
 */
void oled_display_on(void)
{
    oled_wr_byte(0x8D, OLED_CMD);   /* SET DCDC command */
    oled_wr_byte(0x14, OLED_CMD);   /* DCDC ON */
    oled_wr_byte(0xAF, OLED_CMD);   /* DISPLAY ON */
}

/**
 * @brief  turns off the OLED display
 * @param  None
 * @retval None
 */
void oled_display_off(void)
{
    oled_wr_byte(0x8D, OLED_CMD);   /* SET DCDC command */
    oled_wr_byte(0x10, OLED_CMD);   /* DCDC OFF */
    oled_wr_byte(0xAE, OLED_CMD);   /* DISPLAY OFF */
}

/**
 * @brief  screen cleaner
 * @note   clear the screen, the entire screen is black, as if it is not lit
 * @param  None
 * @retval None
 */
void oled_clear(void)
{
    uint8_t i;
    uint8_t n;
    
    for (i=0; i<8; i++)                 /* Video memory reset */
    {
        for (n=0; n<128; n++)
        {
            g_oled_gram[n][i] = 0x00;
        }
    }
    
    oled_refresh_gram();                /* Update the display */
}

/**
 * @brief   OLED dots
 * @param   x   : The x-coordinate of the point in the range 0-127
 * @param   y   : The y-coordinate of the point in the range 0-63
 * @param   dot : The color of the dot
 * @arg     0   : Zero clear
 * @arg     1   : Fill
 * @retval  none
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos;
    uint8_t bx;
    uint8_t temp;
    
    if ((x > 127) || (y > 63))      /* Coordinates out of range */
    {
        return;
    }
    
    pos = y >> 3;                   /* Compute the byte in which the y-coordinate of the GRAM is located; each byte can store eight row coordinates */
    bx = y & 0x7;                   /* The remainder is used to calculate the position of the Y coordinate in the corresponding byte, that is, the position of the row coordinate (Y) */
    temp = 1 << bx;                 /* The high bit represents the high line number, and the bit position corresponding to Y is obtained, and the bit is set to 1 first */
    
    if (dot == 1)                   /* filling point */
    {
        g_oled_gram[x][pos] |= temp;
    }
    else                            /* Zero clearing point */
    {
        g_oled_gram[x][pos] &= ~temp;
    }
}

/**
 * @brief   OLED fill area fill
 * @param   x1  : The starting x-coordinate in the range 0-x2
 * @param   y1  : The Y coordinate of the starting point, range: 0-y2
 * @param   x2  : Endpoint x-coordinate, range: x1 to 127
 * @param   y2  : Endpoint y-coordinate, range: y1 to 64
 * @param   dot : The color of the dot
 * @arg     0: Zero clear
 * @arg     1: Fill
 * @retval  none
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x;
    uint8_t y;
    
    for (x=x1; x<=x2; x++)              /* Draw points one by one within the region */
    {
        for (y=y1; y<=y2; y++)
        {
            oled_draw_point(x, y, dot);
        }
    }
    
    oled_refresh_gram();                /* Update the display */
}

/**
 * @brief   Displays a character at the specified position
 * @param   x    : The x-coordinate of the specified position in the range 0-127
 * @param   y    : The y-coordinate of the specified position in the range 0-63
 * @param   chr  : The character to display, range: "~'~'
 * @param   size : Font
 * @arg     12: 12*12 ASCII characters
 * @arg     16: 16*16 ASCII characters
 * @arg     24: 24*24 ASCII characters
 * @param   mode : Display mode
 * @arg     0: Inverted white display
 * @arg     1: Display normally
 * @retval  none
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t csize;
    uint8_t *pfont;
    uint8_t t;
    uint8_t t1;
    uint8_t temp;
    uint8_t y0;
    
    csize = ((size >> 3) + (((size & 0x7) != 0) ? 1 : 0)) * (size >> 1);    /* Counts the number of bytes for a character in the selected font */
    chr -= ' ';                                                             /* The offset is calculated because the font starts with a space */
    
    if (size == 12)                                                         /* 12*12 ASCII characters */
    {
        pfont = (uint8_t *)oled_asc2_1206[chr];
    }
    else if (size == 16)                                                    /* 16*16 ASCII characters */
    {
        pfont = (uint8_t *)oled_asc2_1608[chr];
    }
    else if (size == 24)                                                    /* 24*24 ASCII characters */
    {
        pfont = (uint8_t *)oled_asc2_2412[chr];
    }
    else                                                                    /* There is no font for it */
    {
        return;
    }
    
    y0 = y;
    for (t=0; t<csize; t++)
    {
        temp = pfont[t];
        for (t1=0; t1<8; t1++)
        {
            if ((temp & 0x80) != 0)
            {
                oled_draw_point(x, y, mode);
            }
            else
            {
                oled_draw_point(x, y, !mode);
            }
            
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief   squared function
 * @param   m : The base number
 * @param   n : Exponent
 * @retval  m^n
 */
static uint32_t oled_pow(uint8_t m, uint8_t n)
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
 * @param   x    : The starting x-coordinate in the range 0-127
 * @param   y    : The starting Y coordinate, range: 0-63
 * @param   num  : Number in the range 0-2 ^32
 * @param   len  : Displays the number of bits
 * @param   size : Font
 * @arg     12: 12*12 ASCII characters
 * @arg     16: 16*16 ASCII characters
 * @arg     24: 24*24 ASCII characters
 * @retval  None
 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t;
    uint8_t temp;
    uint8_t enshow = 0;
    
    for (t=0; t<len; t++)                                               /* Cycle by the total number of display bits */
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;                  /* Gets the number of the corresponding bit */
        if((enshow == 0) && (t < (len - 1)))                            /* Display is not enabled, and there is still bit to display */
        {
            if (temp == 0)
            {
                oled_show_char(x + (size >> 1) * t, y, ' ', size, 1);   /* Display space, placeholder */
                continue;                                               /* Move on to the next bit */
            }
            else
            {
                enshow = 1;                                             /* Enable display */
            }
        }
        
        oled_show_char(x + (size >> 1) * t, y, temp + '0', size, 1);    /* display character */
    }
}

/**
 * @brief   Displays the string
 * @param   x    : The starting x-coordinate in the range 0-127
 * @param   y    : The starting Y coordinate, range: 0-63
 * @param   *p   : String pointer
 * @param   size : Font
 * @arg     12: 12*12 ASCII characters
 * @arg     16: 16*16 ASCII characters
 * @arg     24: 24*24 ASCII characters
 * @retval  None
 */
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))      /* Determines whether the character is invalid */
    {
        if (x > (128 - (size >> 1)))        /* Width out of bounds */
        {
            x = 0;                          /* line feed */
            y += size;
        }
        
        if (y > (64 - size))                /* Height out of bounds */
        {
            x = 0;
            y = 0;
            oled_clear();
        }
        
        oled_show_char(x, y, *p, size, 1);  /* Display a character */
        x += (size >> 1);                   /* ASCII characters are half as wide as they are tall */
        p++;
    }
}

/**
 * @brief    Initializing OLED
 * @param    None
 * @retval   None
 */
void oled_init(void)
{
	OLED_CS(1);
	OLED_RS(1);

	OLED_RST(0);
	delay_ms(100);
	OLED_RST(1);

	oled_wr_byte(0xAE, OLED_CMD);    /* Turn off display */
	oled_wr_byte(0xD5, OLED_CMD);    /* Set clock division factor and oscillator frequency */
	oled_wr_byte(0x50, OLED_CMD);    /* [3:0]: Division factor; [7:4]: Oscillator frequency */
	oled_wr_byte(0xA8, OLED_CMD);    /* Set multiplex ratio */
	oled_wr_byte(0x3F, OLED_CMD);    /* Default 0x3F (1/64) */
	oled_wr_byte(0xD3, OLED_CMD);    /* Set display offset */
	oled_wr_byte(0x00, OLED_CMD);    /* Default to 0 */
	oled_wr_byte(0x40, OLED_CMD);    /* Set display start line, [5:0]: Line number */
	oled_wr_byte(0x8D, OLED_CMD);    /* Set charge pump */
	oled_wr_byte(0x14, OLED_CMD);    /* bit2: On/Off */
	oled_wr_byte(0x20, OLED_CMD);    /* Set memory addressing mode */
	oled_wr_byte(0x02, OLED_CMD);    /* [1:0]: 00: Column addressing mode; 01: Row addressing mode; 10: Page addressing mode, default 10 */
	oled_wr_byte(0xA1, OLED_CMD);    /* Set segment re-map, bit0: 0: 0->0; 1: 0->127 */
	oled_wr_byte(0xC8, OLED_CMD);    /* Set COM scan direction, bit3: 0: Normal mode; 1: Remapped mode COM[N-1]->COM0, N: Multiplex ratio */
	oled_wr_byte(0xDA, OLED_CMD);    /* Set COM hardware pin configuration */
	oled_wr_byte(0x12, OLED_CMD);    /* [5:4]: Configuration */
	oled_wr_byte(0x81, OLED_CMD);    /* Set contrast control */
	oled_wr_byte(0xEF, OLED_CMD);    /* 1~255, default 0x7F (brightness setting, larger value for brighter) */
	oled_wr_byte(0xD9, OLED_CMD);    /* Set pre-charge period */
	oled_wr_byte(0xF1, OLED_CMD);    /* [3:0]: PHASE 1; [7:4]: PHASE 2; */
	oled_wr_byte(0xDB, OLED_CMD);    /* Set VCOMH voltage multiplier */
	oled_wr_byte(0x30, OLED_CMD);    /* [6:4]: 000: 0.65*vcc; 001: 0.77*vcc; 011: 0.83*vcc */
	oled_wr_byte(0xA4, OLED_CMD);    /* Enable global display, bit0: 1: On; 0: Off (White screen/Black screen) */
	oled_wr_byte(0xA6, OLED_CMD);    /* Set display mode, bit0: 1: Reverse display; 0: Normal display */
	oled_wr_byte(0xAF, OLED_CMD);    /* Turn on display */
	oled_clear();                    /* Clear screen */
}
