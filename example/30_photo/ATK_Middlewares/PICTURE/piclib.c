/**
 ****************************************************************************************************
 * @file        piclib.c
 * @author      ALIENTEK
 * @brief       piclib code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M100-STM32F103 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  ：
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#include "piclib.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"

_pic_info picinfo;      /* pictorial information */
_pic_phy pic_phy;       /* The picture shows the physical interface */


/**
 * @brief   fill color
 * @param   x, y          : the starting coordinate
 * @param   width, height : The width and height
 * @param   color         : An array of colors
 * @retval  None
 */
static void piclib_fill_color(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t *color)
{
    lcd_color_fill(x, y, x + width - 1, y + height - 1, color);     /* fill */
}

/**
 * @brief  Draw initialization
 * @param  None
 * @retval None
 */
void piclib_init(void)
{
    pic_phy.read_point = lcd_read_point;    /* Read point function implementation, only required by BMP */
    pic_phy.draw_point = lcd_draw_point;    /* Dot function implementation */
    pic_phy.fill = lcd_fill;                /* Fill function implementation, required only for GIF */
    pic_phy.draw_hline = lcd_draw_hline;    /* Line drawing function implementation, only needed for GIF */
    pic_phy.fillcolor = piclib_fill_color;  /* Color fill function implementation, only required for TJPGD */

    picinfo.lcdwidth = lcddev.width;        /* Get the width of the LCD in pixels */
    picinfo.lcdheight = lcddev.height;      /* Get the height of the LCD in pixels */

    picinfo.ImgWidth = 0;                   /* Initialize the width to 0 */
    picinfo.ImgHeight = 0;                  /* Initialize the height to 0 */
    picinfo.Div_Fac = 0;                    /* Initialize the scaling factor to 0 */
    picinfo.S_Height = 0;                   /* The initial height is set to 0 */
    picinfo.S_Width = 0;                    /* The initial width is set to 0 */
    picinfo.S_XOFF = 0;                     /* Initialize the X-axis offset to 0 */
    picinfo.S_YOFF = 0;                     /* Initialize the Y-axis offset to 0 */
    picinfo.staticx = 0;                    /* Initialize the currently displayed x-coordinate to 0 */
    picinfo.staticy = 0;                    /* Initialize the currently displayed y-coordinate to 0 */
}

/**
 * @brief    Fast ALPHA BLENDING algorithm
 * @param    src   : number of colors
 * @param    dst   : The target color
 * @param    alpha : Transparency (0-32)
 * @retval   blended colors
 */
uint16_t piclib_alpha_blend(uint16_t src, uint16_t dst, uint8_t alpha)
{
    uint32_t src2;
    uint32_t dst2;
    /* Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB| */
    src2 = ((src << 16) | src) & 0x07E0F81F;
    dst2 = ((dst << 16) | dst) & 0x07E0F81F;
    /* Perform blending R:G:B with alpha in range 0..32
     * Note that the reason that alpha may not exceed 32 is that there are only
     * 5bits of space between each R:G:B value, any higher value will overflow
     * into the next component and deliver ugly result. 
     */
    dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
    return (dst2 >> 16) | dst2;
}

/**
 * @brief  Initializes the smart points
 * @param  None
 * @retval None
 */
void piclib_ai_draw_init(void)
{
    float temp, temp1;
    temp = (float)picinfo.S_Width / picinfo.ImgWidth;
    temp1 = (float)picinfo.S_Height / picinfo.ImgHeight;

    if (temp < temp1)temp1 = temp;  /* Take the smaller one */

    if (temp1 > 1)temp1 = 1;

    /* Place the picture in the middle of the given area */
    picinfo.S_XOFF += (picinfo.S_Width - temp1 * picinfo.ImgWidth) / 2;
    picinfo.S_YOFF += (picinfo.S_Height - temp1 * picinfo.ImgHeight) / 2;
    temp1 *= 8192;  /* 8,192 times larger */
    picinfo.Div_Fac = temp1;
    picinfo.staticx = 0xffff;
    picinfo.staticy = 0xffff;   /* Put it on an impossible value */
}

/**
 * @brief  Determines if the pixel can be displayed
 * @param  x, y : original coordinates of the pixel
 * @param  chg  : Function variable
 * @param  None
 * @retval operation result
 * @arg    0, need not be displayed
 * @arg    1, required to be displayed
 */
__inline uint8_t piclib_is_element_ok(uint16_t x, uint16_t y, uint8_t chg)
{
    if (x != picinfo.staticx || y != picinfo.staticy)
    {
        if (chg == 1)
        {
            picinfo.staticx = x;
            picinfo.staticy = y;
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief    Intelligent drawing
 * @param    filename: The filename containing the path (.bmp/.jpg/.jpeg/.gif, etc.)
 * @param    x, y          : the starting coordinate
 * @param    width, height : The display area
 * @param    fast          : Enables fast decoding
 * @arg      0, not enabled
 * @arg      1, enable
 * @retval   none
 */
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fast)
{
    uint8_t	res;  /* returned valueֵ */
    uint8_t temp;

    if ((x + width) > picinfo.lcdwidth)return PIC_WINDOW_ERR;   /* The x-coordinate is out of range */

    if ((y + height) > picinfo.lcdheight)return PIC_WINDOW_ERR; /* The y-coordinate is out of range */

    /* Gets the display box size */
    if (width == 0 || height == 0)return PIC_WINDOW_ERR;        /* Window setting error */

    picinfo.S_Height = height;
    picinfo.S_Width = width;

    /* Display area is invalid */
    if (picinfo.S_Height == 0 || picinfo.S_Width == 0)
    {
        picinfo.S_Height = lcddev.height;
        picinfo.S_Width = lcddev.width;
        return FALSE;
    }

    if (pic_phy.fillcolor == NULL)fast = 0; /* The color fill function is not implemented and cannot be displayed quickly */

    /* The start coordinate point of the display */
    picinfo.S_YOFF = y;
    picinfo.S_XOFF = x;

    /* Filename passing */
    temp = exfuns_file_type(filename);   /* Get the file type */

    switch (temp)
    {
        case T_BMP:
            res = stdbmp_decode(filename);      /* Decoding bmp */
            break;

        case T_JPG:
        case T_JPEG:
            res = jpg_decode(filename, fast);   /* Decode JPG/JPEG */
            break;

        case T_GIF:
            res = gif_decode(filename, x, y, width, height);    /* Decoding gif */
            break;

        default:
            res = PIC_FORMAT_ERR;               /* Non IMAGE FORMAT!! */
            break;
    }

    return res;
}

/**
 * @brief   dynamically allocates memory
 * @param   size : The size (in bytes) of memory to be requested
 * @retval  at the head of the allocated memory
 */
void *piclib_mem_malloc (uint32_t size)
{
    return (void *)mymalloc(SRAMIN, size);
}

/**
 * @brief   frees memory
 * @param   paddr : the first memory address
 * @retval  None
 */
void piclib_mem_free (void *paddr)
{
    myfree(SRAMIN, paddr);
}























