/**
 ****************************************************************************************************
 * @file        gif.h
 * @author      ALIENTEK
 * @brief       gif code
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

#ifndef __GIF_H
#define __GIF_H

#include "main.h"
#include "ff.h"

/******************************************************************************************/
/* User configuration area */

#define GIF_USE_MALLOC          1       /* Define whether to use malloc, which we choose here */

/******************************************************************************************/


#define LCD_MAX_LOG_COLORS      256
#define MAX_NUM_LWZ_BITS        12


#define GIF_INTRO_TERMINATOR    ';'     /* 0X3B   End of GIF file */
#define GIF_INTRO_EXTENSION     '!'     /* 0X21 */
#define GIF_INTRO_IMAGE         ','     /* 0X2C */

#define GIF_COMMENT             0xFE
#define GIF_APPLICATION         0xFF
#define GIF_PLAINTEXT           0x01
#define GIF_GRAPHICCTL          0xF9

typedef struct
{
    uint8_t    aBuffer[258];                    /*  Input buffer for data block */
    short aCode  [(1 << MAX_NUM_LWZ_BITS)];     /*  This array stores the LZW codes for the compressed strings */
    uint8_t    aPrefix[(1 << MAX_NUM_LWZ_BITS)];/*  Prefix character of the LZW code */
    uint8_t    aDecompBuffer[3000];             /*  Decompression buffer. The higher the compression, the more bytes are needed in the buffer */
    uint8_t   *sp;                              /*  Pointer into the decompression buffer */
    int   CurBit;
    int   LastBit;
    int   GetDone;
    int   LastByte;
    int   ReturnClear;
    int   CodeSize;
    int   SetCodeSize;
    int   MaxCode;
    int   MaxCodeSize;
    int   ClearCode;
    int   EndCode;
    int   FirstCode;
    int   OldCode;
} LZW_INFO;

/* Logical screen description block */
typedef __PACKED_STRUCT
{
    uint16_t width;     /* GIF width */
    uint16_t height;    /* GIF height */
    uint8_t flag;       /* Identifier 1:3:1:3 = global logo (1) : the color table color depth (3) : the marking (1) : the global color table size (3) */
    uint8_t bkcindex;   /* Index of the background color in the global color table (only if a global color table exists) */
    uint8_t pixratio;   /* Pixel aspect ratio */
} LogicalScreenDescriptor;


/* Image caption block */
typedef __PACKED_STRUCT
{
    uint16_t xoff;      /* Offset in the x direction */
    uint16_t yoff;      /* Offset in the y direction */
    uint16_t width;
    uint16_t height;
    uint8_t flag;       /* Local color chart sign identifier: : 2:3 = (1) : weaving mark (1) : keep (2) : local color table size (3) */
} ImageScreenDescriptor;

/* picture description */
typedef __PACKED_STRUCT
{
    LogicalScreenDescriptor gifLSD; /* Logical screen description block */
    ImageScreenDescriptor gifISD;   /* Fast image captioning */
    uint16_t colortbl[256];         /* Currently using a color table */
    uint16_t bkpcolortbl[256];      /* Backup color table. Used when a partial color table exists */
    uint16_t numcolors;             /* Color table size */
    uint16_t delay;                 /* delay time */
    LZW_INFO *lzw;                  /* LZW information */
} gif89a;

extern uint8_t g_gif_decoding;      /* The GIF is decoding the tag */





/* GIF�����ӿں��� */
void gif_quit(void);    /* �˳���ǰ���� */
uint8_t gif_getinfo(FIL *file, gif89a *gif);    /* ��ȡGIF��Ϣ */
uint8_t gif_decode(const char *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height);/* ��ָ���������һ��GIF�ļ� */


#endif




















