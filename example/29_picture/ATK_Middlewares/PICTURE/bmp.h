/**
 ****************************************************************************************************
 * @file        bmp.h
 * @author      ALIENTEK
 * @brief       bmp code
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

#ifndef __BMP_H
#define __BMP_H

#include "main.h"


/******************************************************************************************/
/* User configuration area */
#define BMP_USE_MALLOC      1           /* Define whether to use malloc, which we choose here */
#define BMP_DBUF_SIZE       2048        /* Define the size of the bmp decoder array (minimum should be LCD width *3) */

/******************************************************************************************/


/* BMP information header */
typedef __PACKED_STRUCT
{
    uint32_t biSize;           /* Indicates the number of words required for the BITMAPINFOHEADER structure */
    long  biWidth;             /* Indicates the width of the image in pixels */
    long  biHeight;            /* Indicates the height of the image in pixels */
    uint16_t  biPlanes;        /* Specifies the number of bit planes for the target device, which will always be set to 1 */
    uint16_t  biBitCount;      /* Indicates the number of bits/pixel; the value is 1, 4, 8, 16, 24, or 32 */
    uint32_t biCompression;    /* Describes the types of image data compression. Its value can be one of the following
                                * BI_RGB  : No compression
                                * BI_RLE8 : 8 bits per pixel RLE compression encoding, the compression format consists of 2 bytes (repeat pixel count and color index)
                                * BI_RLE4 : RLE compression encoding of 4 bits per pixel, the compression format consists of 2 bytes
                                * BI_BITFIELDS : The number of bits per pixel is determined by a specified mask
                                */
    uint32_t biSizeImage;      /* Indicates the size of the image in bytes. This value can be set to 0 when using BI_RGB format */
    long  biXPelsPerMeter;     /* Indicates horizontal resolution, expressed in pixels/meter */
    long  biYPelsPerMeter;     /* Indicates vertical resolution, expressed in pixels/meter */
    uint32_t biClrUsed;        /* Indicates the number of color indices in the actual color table used by the bitmap */
    uint32_t biClrImportant;   /* Indicates the number of color indexes that are important to the display of the image. If 0, it is important */
}BITMAPINFOHEADER ;

/* BMP header file */
typedef __PACKED_STRUCT
{
    uint16_t  bfType ;          /* File flag. Only for 'BM', used to identify the BMP bitmap type */
    uint32_t  bfSize ;          /* File size, four bytes */
    uint16_t  bfReserved1 ;     /* reserve */
    uint16_t  bfReserved2 ;     /* reserve */
    uint32_t  bfOffBits ;       /* The offset from the beginning of the file to the beginning of the bitmap data */
}BITMAPFILEHEADER ;

/* color table */
typedef __PACKED_STRUCT
{
    uint8_t rgbBlue ;           /* Specify the blue intensity */
    uint8_t rgbGreen ;          /* ָSpecify the green intensity */
    uint8_t rgbRed ;            /* Specify the red intensity */
    uint8_t rgbReserved ;       /* Keep, set to 0 */
}RGBQUAD ;

/* Bitmap information header */
typedef __PACKED_STRUCT
{ 
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bmiHeader;  
    uint32_t RGB_MASK[3];       /* The color palette is used to hold the RGB mask */
    //RGBQUAD bmiColors[256];
}BITMAPINFO; 

typedef RGBQUAD * LPRGBQUAD;    /* color table */


/* Type of image data compression */
#define BI_RGB          0       /* No compression.RGB 5,5,5 */
#define BI_RLE8         1       /* 8 bits per pixel RLE compression encoding, the compression format consists of 2 bytes (repeat pixel count and color index) */
#define BI_RLE4         2       /* RLE compression encoding of 4 bits per pixel, the compression format consists of 2 bytes */
#define BI_BITFIELDS    3       /* The number of bits per pixel is determined by a specified mask */



uint8_t stdbmp_decode(const char *filename);
uint8_t minibmp_decode(uint8_t *filename,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t acolor,uint8_t mode);
uint8_t bmp_encode(uint8_t *filename,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t mode);

#endif








