/**
 ****************************************************************************************************
 * @file        piclib.h
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

#ifndef __PICLIB_H
#define __PICLIB_H

#include "lcd.h"
#include "malloc.h"
#include "../../FatFs/exfuns/exfuns.h"

#include "bmp.h"
#include "gif.h"
#include "tjpgd.h"


#define PIC_FORMAT_ERR      0x27    /* format error */
#define PIC_SIZE_ERR        0x28    /* Image size error */
#define PIC_WINDOW_ERR      0x29    /* Window setting error */
#define PIC_MEM_ERR         0x11    /* memory error */

/* Are TRUE and FALSE defined? If not, define it! */
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

/* The picture shows the physical layer interface */
/* When porting, you must implement these functions yourself */
typedef struct
{
    /* uint32_t read_point(uint16_t x,uint16_t y) Read point function */
    uint32_t(*read_point)(uint16_t, uint16_t);
    
    /* void draw_point(uint16_t x,uint16_t y,uint32_t color) draw point function */
    void(*draw_point)(uint16_t, uint16_t, uint32_t);
    
    /* void fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color) Monochromatic filled functions */
    void(*fill)(uint16_t, uint16_t, uint16_t, uint16_t, uint32_t);
    
    /* void draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color) The draw horizontal line function */
    void(*draw_hline)(uint16_t, uint16_t, uint16_t, uint16_t);
    
    /* void piclib_fill_color(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *color) Color fill */
    void(*fillcolor)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t *);
} _pic_phy;

extern _pic_phy pic_phy;


/* pictorial information */
typedef struct
{
    uint16_t lcdwidth;      /* Width of LCD */
    uint16_t lcdheight;     /* height of LCD */
    uint32_t ImgWidth;      /* Actual width and height of the image */
    uint32_t ImgHeight;

    uint32_t Div_Fac;       /* The scaling factor (8192 times larger */

    uint32_t S_Height;      /* The height and width to set */
    uint32_t S_Width;

    uint32_t S_XOFF;        /* Offsets for the x and y axes */
    uint32_t S_YOFF;

    uint32_t staticx;       /* The xy coordinates currently displayed */
    uint32_t staticy;
} _pic_info;

extern _pic_info picinfo;   /* pictorial information */


void piclib_mem_free (void *paddr);
void *piclib_mem_malloc (uint32_t size);

void piclib_init(void);
void piclib_ai_draw_init(void);
uint16_t piclib_alpha_blend(uint16_t src, uint16_t dst, uint8_t alpha);
uint8_t piclib_is_element_ok(uint16_t x, uint16_t y, uint8_t chg);
uint8_t piclib_ai_load_picfile(char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fast);

#endif






























