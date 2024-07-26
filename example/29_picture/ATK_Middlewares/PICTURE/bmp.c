/**
 ****************************************************************************************************
 * @file        bmp.c
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

#include "string.h"
#include "bmp.h"
#include "piclib.h"



/* No memory allocation is used */
#if BMP_USE_MALLOC == 0
FIL f_bfile;
uint8_t bmpreadbuf[BMP_DBUF_SIZE];
#endif


/**
 * @brief   standard bmp decode, decode filename this BMP file
 * @param   filename: The filename containing the path (.bmp)
 * @retval  operation result
 * @arg     0, success
 * @arg     other, error code
 */
uint8_t stdbmp_decode(const char *filename)
{
    FIL *f_bmp;
    uint16_t br;

    uint16_t count;
    volatile uint8_t  rgb, color_byte;
    uint16_t x, y, color;
    uint16_t countpix = 0;      /* Recording pixels */

    /* x, the actual coordinates of y */
    uint16_t  realx = 0;
    uint16_t realy = 0;
    uint8_t  yok = 1;
    uint8_t res;


    uint8_t *databuf;           /* The address where data is read and stored */
    uint16_t readlen = BMP_DBUF_SIZE;   /* The length of bytes read from the SD card at a time */

    uint8_t *bmpbuf;            /* Data decoding address */
    uint8_t biCompression = 0;  /* Record compression mode */

    uint16_t rowlen;            /* Horizontal bytes */
    BITMAPINFO *pbmp;           /* temporary pointer */

#if BMP_USE_MALLOC == 1         /* use malloc */
    databuf = (uint8_t *)piclib_mem_malloc(readlen); /* A memory area of readlen bytes is opened */

    if (databuf == NULL)return PIC_MEM_ERR;          /* Memory request failed */

    f_bmp = (FIL *)piclib_mem_malloc(sizeof(FIL));   /* A FIL byte memory area is opened */

    if (f_bmp == NULL)          /* Memory request failed */
    {
        piclib_mem_free(databuf);
        return PIC_MEM_ERR;
    }

#else   /* malloc is not used */
    databuf = bmpreadbuf;
    f_bmp = &f_bfile;
#endif
    res = f_open(f_bmp, (const TCHAR *)filename, FA_READ);  /* open file */

    if (res == 0)   /* Opened successfully */
    {
        f_read(f_bmp, databuf, readlen, (UINT *)&br);   /* Read out readlen bytes */
        pbmp = (BITMAPINFO *)databuf;                   /* Get the BMP head information */
        count = pbmp->bmfHeader.bfOffBits;              /* Data offset to get the start address of the data segment */
        color_byte = pbmp->bmiHeader.biBitCount / 8;    /* Color bits 16/24/32 */
        biCompression = pbmp->bmiHeader.biCompression;  /* compress mode */
        picinfo.ImgHeight = pbmp->bmiHeader.biHeight;   /* Get the height of the image */
        picinfo.ImgWidth = pbmp->bmiHeader.biWidth;     /* Get the width of the image */
        piclib_ai_draw_init();                          /* Initialize the smart drawing */

        /* Horizontal pixels must be multiples of 4!! */
        if ((picinfo.ImgWidth * color_byte) % 4)
        {
            rowlen = ((picinfo.ImgWidth * color_byte) / 4 + 1) * 4;
        }
        else
        {
            rowlen = picinfo.ImgWidth * color_byte;
        }

        /* Start decoding BMP */
        color = 0;  /* Color clear */
        x = 0 ;
        y = picinfo.ImgHeight;
        rgb = 0;

        /* For images whose size is less than or equal to the set size, fast decoding is performed */
        realy = (y * picinfo.Div_Fac) >> 13;
        bmpbuf = databuf;

        while (1)
        {
            while (count < readlen)     /* Read a cluster of 1024 sectors (SectorsPerClust sectors per cluster) */
            {
                if (color_byte == 3)    /* 24-bit color map */
                {
                    switch (rgb)
                    {
                        case 0:
                            color = bmpbuf[count] >> 3; /* B */
                            break ;

                        case 1:
                            color += ((uint16_t)bmpbuf[count] << 3) & 0X07E0;   /* G */
                            break;

                        case 2 :
                            color += ((uint16_t)bmpbuf[count] << 8) & 0XF800;   /* R */
                            break ;
                    }
                }
                else if (color_byte == 2)   /* 16-bit color map */
                {
                    switch (rgb)
                    {
                        case 0 :
                            if (biCompression == BI_RGB)    /* RGB:5,5,5 */
                            {
                                color = ((uint16_t)bmpbuf[count] & 0X1F);           /* R */
                                color += (((uint16_t)bmpbuf[count]) & 0XE0) << 1;   /* G */
                            }
                            else    /* RGB:5,6,5 */
                            {
                                color = bmpbuf[count];  /* G,B */
                            }

                            break ;

                        case 1 :
                            if (biCompression == BI_RGB)/* RGB:5,5,5 */
                            {
                                color += (uint16_t)bmpbuf[count] << 9;  /* R,G */
                            }
                            else    /* RGB:5,6,5 */
                            {
                                color += (uint16_t)bmpbuf[count] << 8;  /* R,G */
                            }

                            break ;
                    }
                }
                else if (color_byte == 4)   /* 32-bit color map */
                {
                    switch (rgb)
                    {
                        case 0:
                            color = bmpbuf[count] >> 3; /* B */
                            break ;

                        case 1:
                            color += ((uint16_t)bmpbuf[count] << 3) & 0X07E0;   /* G */
                            break;

                        case 2 :
                            color += ((uint16_t)bmpbuf[count] << 8) & 0XF800;   /* R */
                            break ;

                        case 3 :
                            //alphabend=bmpbuf[count]; /* ALPHA channels are not read */
                            break ;
                    }
                }
                else if (color_byte == 1)   /* 8-bit color, temporarily not supported, need to use color table */
                {
                }

                rgb++;
                count++ ;

                if (rgb == color_byte)  /* Horizontal direction to read 1 pixel number of data after display */
                {
                    if (x < picinfo.ImgWidth)
                    {
                        realx = (x * picinfo.Div_Fac) >> 13; /* The actual value of the x axis */

                        if (piclib_is_element_ok(realx, realy, 1) && yok)   /* eligibility */
                        {
                            pic_phy.draw_point(realx + picinfo.S_XOFF, realy + picinfo.S_YOFF - 1, color);  /* display picture */
                        }
                    }

                    x++;    /* Increase the X-axis by one pixel */
                    color = 0x00;
                    rgb = 0;
                }

                countpix++; /* Pixel accumulation */

                if (countpix >= rowlen) /* The horizontal pixel value is reached. Line wrapping */
                {
                    y--;

                    if (y == 0)break;

                    realy = (y * picinfo.Div_Fac) >> 13;        /* The actual y value changes */

                    if (piclib_is_element_ok(realx, realy, 0))yok = 1; /* The values of picinfo.staticx,y are not changed */
                    else yok = 0;

                    if ((realy + picinfo.S_YOFF) == 0)break;    /* The y-coordinate has to be greater than 0 */

                    x = 0;
                    countpix = 0;
                    color = 0x00;
                    rgb = 0;
                }
            }

            res = f_read(f_bmp, databuf, readlen, (UINT *)&br); /* Read out readlen bytes */

            if (br != readlen)readlen = br; /* Last batch of data */

            if (res || br == 0)break;       /* Read error */

            bmpbuf = databuf;
            count = 0;
        }

        f_close(f_bmp); /* closed file */
    }

#if BMP_USE_MALLOC == 1 /* use malloc */
    piclib_mem_free(databuf);
    piclib_mem_free(f_bmp);
#endif
    return res;         /* End of BMP display */
}

/**
 * @brief small size bmp decode, decode filename this BMP file
 * @param  filename     : The filename containing the path (.bmp)
 * @param  x, y         : Displays the starting coordinates
 * @param  width,height : The size of the display area
 * @param  acolor       : The color of the additional alphablend (this only works for 32-bit color BMPS!!)
 * @param  mode         : Mode (except bit5, all other are only valid for 32-bit color bmp!!)
 *         bit[7:6]: 00, using only the image and alphablend;
 *                   01, alphablend only for images and acolor, and no additional transparency applies;
 *                   10, base color, acolor, image, alphablend together;
 *         bit5: Keep
 *         bit4~0:0~31, transparency using additional alphablend
 *
 * @retval  operation result
 * @arg     0, success
 * @arg     other, error code
 */
uint8_t minibmp_decode(uint8_t *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t acolor, uint8_t mode)
{
    FIL *f_bmp;
    uint16_t br;
    uint8_t  color_byte;
    uint16_t tx, ty, color;

    /* tx, the actual coordinates of ty */
    uint8_t res;
    uint16_t i, j;
    uint8_t *databuf;                   /* Data read storage address */
    uint16_t readlen = BMP_DBUF_SIZE;   /* The length of bytes read from the SD card at a time cannot be less than the LCD width *3!! */

    uint8_t *bmpbuf;                    /* Data decoding address */
    uint8_t biCompression = 0;          /* Record compression modeʽ */

    uint16_t rowcnt;                    /* The number of rows read at a time */
    uint16_t rowlen;                    /* Horizontal bytes */
    uint16_t rowpix = 0;                /* Number of pixels in the horizontal direction */
    uint8_t rowadd;                     /* Number of padded bytes per line */

    uint16_t tmp_color;

    uint8_t alphabend = 0xff;           /* This means transparency is 0 and completely opaque */
    uint8_t alphamode = mode >> 6;      /* You get the pattern value,0/1/2 */
    BITMAPINFO *pbmp;                   /* temporary pointer */

    /* Get the form dimensions */
    picinfo.S_Height = height;
    picinfo.S_Width = width;

#if BMP_USE_MALLOC == 1                 /* use malloc */
    databuf = (uint8_t *)piclib_mem_malloc(readlen); /* A memory area of readlen bytes is opened */

    if (databuf == NULL)return PIC_MEM_ERR;          /* Memory request failed */

    f_bmp = (FIL *)piclib_mem_malloc(sizeof(FIL));   /* A FIL byte memory area is opened */

    if (f_bmp == NULL)                               /* Memory request failed */
    {
        piclib_mem_free(databuf);
        return PIC_MEM_ERR;
    }

#else
    databuf = bmpreadbuf;
    f_bmp = &f_bfile;
#endif
    res = f_open(f_bmp, (const TCHAR *)filename, FA_READ);  /* open file */

    if (res == 0)   /* Opened successfully */
    {
        f_read(f_bmp, databuf, sizeof(BITMAPINFO), (UINT *)&br);/* Read out the BITMAPINFO information */
        pbmp = (BITMAPINFO *)databuf;                   /* Get the BMP head information */
        color_byte = pbmp->bmiHeader.biBitCount / 8;    /* Color bits 16/24/32 */
        biCompression = pbmp->bmiHeader.biCompression;  /* compress mode */
        picinfo.ImgHeight = pbmp->bmiHeader.biHeight;   /* Get the height of the image */
        picinfo.ImgWidth = pbmp->bmiHeader.biWidth;     /* Get the width of the image */

        /* Horizontal pixels must be multiples of 4!! */
        if ((picinfo.ImgWidth * color_byte) % 4)
        {
            rowlen = ((picinfo.ImgWidth * color_byte) / 4 + 1) * 4;
        }
        else
        {
            rowlen = picinfo.ImgWidth * color_byte;
        }

        rowadd = rowlen - picinfo.ImgWidth * color_byte;/* Number of padded bytes per line */
        /* Start decoding BMP */
        color = 0; /* Color clear */
        tx = 0 ;
        ty = picinfo.ImgHeight - 1;

        if (picinfo.ImgWidth <= picinfo.S_Width && picinfo.ImgHeight <= picinfo.S_Height)
        {
            x += (picinfo.S_Width - picinfo.ImgWidth) / 2;      /* Offset to the center */
            y += (picinfo.S_Height - picinfo.ImgHeight) / 2;    /* Offset to the center */
            rowcnt = readlen / rowlen;  /* The number of rows read at a time */
            readlen = rowcnt * rowlen;  /* The number of bytes read at a time */
            rowpix = picinfo.ImgWidth;  /* The number of horizontal pixels is the width */
            f_lseek(f_bmp, pbmp->bmfHeader.bfOffBits);          /* Offset to start of data */

            while (1)
            {
                res = f_read(f_bmp, databuf, readlen, (UINT *)&br); /* Read out readlen bytes */
                bmpbuf = databuf;       /* Data head address */

                if (br != readlen)rowcnt = br / rowlen; /* The number of rows left at the end */

                if (color_byte == 3)    /* 24-bit BMP images */
                {
                    for (j = 0; j < rowcnt; j++)        /* The number of lines read at a time */
                    {
                        for (i = 0; i < rowpix; i++)    /* Write a row of pixels */
                        {
                            color = (*bmpbuf++) >> 3;		   		 	/* B */
                            color += ((uint16_t)(*bmpbuf++) << 3) & 0X07E0;	/* G */
                            color += (((uint16_t) * bmpbuf++) << 8) & 0XF800;	/* R */
                            pic_phy.draw_point(x + tx, y + ty, color); /* display picture */
                            tx++;
                        }

                        bmpbuf += rowadd;   /* Skip the fill area */
                        tx = 0;
                        ty--;
                    }
                }
                else if (color_byte == 2)   /* 16-bit BMP images */
                {
                    for (j = 0; j < rowcnt; j++)        /* The number of lines read at a time */
                    {
                        if (biCompression == BI_RGB)    /* RGB:5,5,5 */
                        {
                            for (i = 0; i < rowpix; i++)
                            {
                                color = ((uint16_t) * bmpbuf & 0X1F);           /* R */
                                color += (((uint16_t) * bmpbuf++) & 0XE0) << 1; /* G */
                                color += ((uint16_t) * bmpbuf++) << 9;          /* R,G */
                                pic_phy.draw_point(x + tx, y + ty, color);      /* display picture */
                                tx++;
                            }
                        }
                        else     /* RGB 565 */
                        {
                            for (i = 0; i < rowpix; i++)
                            {
                                color = *bmpbuf++;  /* G,B */
                                color += ((uint16_t) * bmpbuf++) << 8;      /* R,G */
                                pic_phy.draw_point(x + tx, y + ty, color);  /* display picture */
                                tx++;
                            }
                        }

                        bmpbuf += rowadd;   /* Skip the fill area */
                        tx = 0;
                        ty--;
                    }
                }
                else if (color_byte == 4)   /* 32-bit BMP images */
                {
                    for (j = 0; j < rowcnt; j++)    /* The number of lines read at a time */
                    {
                        for (i = 0; i < rowpix; i++)
                        {
                            color = (*bmpbuf++) >> 3;   /* B */
                            color += ((uint16_t)(*bmpbuf++) << 3) & 0X07E0;     /* G */
                            color += (((uint16_t) * bmpbuf++) << 8) & 0XF800;   /* R */
                            alphabend = *bmpbuf++;  /* ALPHA channel */

                            if (alphamode != 1)     /* We need to read the base color */
                            {
                                tmp_color = pic_phy.read_point(x + tx, y + ty); /* Reading colors */

                                if (alphamode == 2) /* Additional alphablend is required */
                                {
                                    tmp_color = piclib_alpha_blend(tmp_color, acolor, mode & 0X1F); /* blend with the specified color */
                                }

                                color = piclib_alpha_blend(tmp_color, color, alphabend / 8);    /* alphablend with the base color */
                            }
                            else
                            {
                                tmp_color = piclib_alpha_blend(acolor, color, alphabend / 8);   /* blend with the specified color */
                            }

                            pic_phy.draw_point(x + tx, y + ty, color);  /* display picture */
                            tx++;           /* Increase the X-axis by one pixel */
                        }

                        bmpbuf += rowadd;   /* Skip the fill area */
                        tx = 0;
                        ty--;
                    }

                }

                if (br != readlen || res)break;
            }
        }

        f_close(f_bmp);     /* closed file */
    }
    else
    {
        res = PIC_SIZE_ERR; /* Image size error */
    }

#if BMP_USE_MALLOC == 1     /* use malloc */
    piclib_mem_free(databuf);
    piclib_mem_free(f_bmp);
#endif
    return res;
}

/**
 * @brief BMP encoding function
 * @param   filename: The filename containing the storage path (.bmp)
 * @param   x, y         : the starting coordinate
 * @param   width,height : The size of the region
 * @param   acolor       : The color of the additional alphablend (this only works for 32-bit color BMPS!!)
 * @param   mode         : Save mode
 * @arg     0, encoding only how to create a new file;
 * @arg     1, overwrites the previous file if one existed before. If not, a new file is created.
 * @retval  operation result
 * @arg     0, success
 * @arg     other, error code
 */
uint8_t bmp_encode(uint8_t *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t mode)
{
    FIL *f_bmp;
    uint32_t bw = 0;
    uint16_t bmpheadsize;   /* bmp head size */
    BITMAPINFO hbmp;        /* bmp head */
    uint8_t res = 0;
    uint16_t tx, ty;        /* picture size */
    uint16_t *databuf;      /* Data cache address */
    uint16_t pixcnt;        /* Pixel counter */
    uint16_t bi4width;      /* Number of horizontal pixels in bytes */

    if (width == 0 || height == 0)return PIC_WINDOW_ERR;        /* Region error */

    if ((x + width - 1) > lcddev.width)return PIC_WINDOW_ERR;   /* Region error */

    if ((y + height - 1) > lcddev.height)return PIC_WINDOW_ERR; /* Region error */

#if BMP_USE_MALLOC == 1     /* use malloc */

    /* Open up a memory area of at least bi4width bytes, or 480 bytes for a 240-wide screen. The maximum support for 1024 width bmp encoding */
    databuf = (uint16_t *)piclib_mem_malloc(2048);

    if (databuf == NULL)return PIC_MEM_ERR;     /* Memory request failed. */

    f_bmp = (FIL *)piclib_mem_malloc(sizeof(FIL));   /* A FIL byte memory area is opened */

    if (f_bmp == NULL)      /* Memory request failed */
    {
        piclib_mem_free(databuf);
        return PIC_MEM_ERR;
    }

#else
    databuf = (uint16_t *)bmpreadbuf;
    f_bmp = &f_bfile;
#endif
    bmpheadsize = sizeof(hbmp);         /* Get the size of the bmp file header */
    my_mem_set((uint8_t *)&hbmp, 0, sizeof(hbmp));      /* Set zero to empty the requested memory */
    hbmp.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   /* Information header size */
    hbmp.bmiHeader.biWidth = width;     /* Width of bmp */
    hbmp.bmiHeader.biHeight = height;   /* height of bmp */
    hbmp.bmiHeader.biPlanes = 1;        /* It's always 1 */
    hbmp.bmiHeader.biBitCount = 16;     /* bmp is a 16-bit color bmp */
    hbmp.bmiHeader.biCompression = BI_BITFIELDS;        /* The number of bits per pixel is determined by a specified mask */
    hbmp.bmiHeader.biSizeImage = hbmp.bmiHeader.biHeight * hbmp.bmiHeader.biWidth * hbmp.bmiHeader.biBitCount / 8;  /* bmp data area size */

    hbmp.bmfHeader.bfType = ((uint16_t)'M' << 8) + 'B'; /* BM format flag */
    hbmp.bmfHeader.bfSize = bmpheadsize + hbmp.bmiHeader.biSizeImage; /* The size of the entire bmp */
    hbmp.bmfHeader.bfOffBits = bmpheadsize; /* Offset to the data area */

    hbmp.RGB_MASK[0] = 0X00F800;        /* Red mask */
    hbmp.RGB_MASK[1] = 0X0007E0;        /* Green mask */
    hbmp.RGB_MASK[2] = 0X00001F;        /* Blue mask */

    if (mode == 1)
    {
        res = f_open(f_bmp, (const TCHAR *)filename, FA_READ | FA_WRITE);       /* Try opening the previous file */
    }

    if (mode == 0 || res == 0x04)
    {
        res = f_open(f_bmp, (const TCHAR *)filename, FA_WRITE | FA_CREATE_NEW); /* Mode 0, or if the opening attempt fails, a new file is created */
    }

    if ((hbmp.bmiHeader.biWidth * 2) % 4)   /* Horizontal pixels (bytes) are not multiples of 4 */
    {
        bi4width = ((hbmp.bmiHeader.biWidth * 2) / 4 + 1) * 4;  /* The actual width, in pixels, must be a multiple of 4 */
    }
    else
    {
        bi4width = hbmp.bmiHeader.biWidth * 2;  /* It's just a multiple of 4 */
    }

    if (res == FR_OK)   /* Successfully created */
    {
        res = f_write(f_bmp, (uint8_t *)&hbmp, bmpheadsize, &bw);   /* Write the BMP header */

        for (ty = y + height - 1; hbmp.bmiHeader.biHeight; ty--)
        {
            pixcnt = 0;

            for (tx = x; pixcnt != (bi4width / 2);)
            {
                if (pixcnt < hbmp.bmiHeader.biWidth)
                {
                    databuf[pixcnt] = pic_phy.read_point(tx, ty);   /* Read the value of the coordinate point */
                }
                else
                {
                    databuf[pixcnt] = 0Xffff;   /* Fill in the white pixels */
                }

                pixcnt++;
                tx++;
            }

            hbmp.bmiHeader.biHeight--;
            res = f_write(f_bmp, (uint8_t *)databuf, bi4width, &bw);    /* read-in data */
        }

        f_close(f_bmp);
    }

#if BMP_USE_MALLOC == 1     /* use malloc */
    piclib_mem_free(databuf);
    piclib_mem_free(f_bmp);
#endif
    return res;
}














