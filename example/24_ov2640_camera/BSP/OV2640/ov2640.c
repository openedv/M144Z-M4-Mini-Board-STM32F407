/**
 ****************************************************************************************************
 * @file        ov2640.c
 * @author      ALIENTEK
 * @brief       This file provides the driver for the ov2640
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform   : ALIENTEK M144-STM32F407 board
 * website    : https://www.alientek.com
 * forum      : http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240222	the first version
 *
 ****************************************************************************************************
 */

#include "../../BSP/OV2640/ov2640.h"
#include "../../BSP/OV2640/sccb.h"
#include "../../BSP/OV2640/ov2640cfg.h"
#include "../../SYSTEM/delay/delay.h"

/**
 * @brief       Read OV2640 register
 * @param       reg: Register address
 * @retval      Value of the register
 */
uint8_t ov2640_read_reg(uint16_t reg)
{
    uint8_t data;
    
    sccb_start();
    sccb_send_byte(OV2640_ADDR);        /* Write command */
    delay_us(100);
    sccb_send_byte(reg);
    delay_us(100);
    sccb_stop();
    delay_us(100);
    
    sccb_start();
    sccb_send_byte(OV2640_ADDR | 0x01); /* Read command */
    delay_us(100);
    data = sccb_read_byte();
    sccb_nack();
    sccb_stop();
    
    return data;
}


/**
 * @brief       Write to OV2640 register
 * @param       reg: Register address
 * @param       data: Value to be written to the register
 * @retval      Write result
 *              0: Success
 *              1: Failure
 */
uint8_t ov2640_write_reg(uint16_t reg, uint8_t data)
{
    uint8_t res = 0;
    
    sccb_start();
    delay_us(100);
    if (sccb_send_byte(OV2640_ADDR) != 0)
    {
        res = 1;
    }
    delay_us(100);
    if (sccb_send_byte(reg) != 0)
    {
        res = 1;
    }
    delay_us(100);
    if (sccb_send_byte(data) != 0)
    {
        res = 1;
    }
    delay_us(100);
    sccb_stop();
    
    return res;
}

/**
 * @brief       Initialize OV2640
 * @param       None
 * @retval      Initialization result
 *              0: Success
 *              1: Failure
 */
uint8_t ov2640_init(void)
{
    uint16_t reg;
    uint16_t i;

    OV2640_PWDN(0);                                         /* Power on */
    delay_ms(10);
    OV2640_RST(0);                                          /* Enable OV2640 reset */
    delay_ms(20);
    OV2640_RST(1);                                          /* Release OV2640 reset */
    delay_ms(20);
    
    sccb_init();                                            /* Initialize SCCB */
    delay_ms(5);
    
    ov2640_write_reg(OV2640_DSP_RA_DLMT, 0x01);             /* Access Sensor register */
    ov2640_write_reg(OV2640_SENSOR_COM7, 0x80);             /* Software reset OV2640 */
    delay_ms(50);
    
    /* Check MID */
    reg = ov2640_read_reg(OV2640_SENSOR_MIDH) << 8;         /* Get OV2640 MID */
    reg |= ov2640_read_reg(OV2640_SENSOR_MIDL);
    if (reg != OV2640_MID)
    {
        return 1;
    }
    
    /* Check PID */
    reg = ov2640_read_reg(OV2640_SENSOR_PIDH) << 8;         /* Get OV2640 PID */
    reg |= ov2640_read_reg(OV2640_SENSOR_PIDL);
    if (reg != OV2640_PID)
    {
        return 1;
    }
    
    /* Initialize OV2640 registers */
    for (i=0; i<(sizeof(ov2640_uxga_init_reg_tbl) / 2); i++)
    {
        ov2640_write_reg(ov2640_uxga_init_reg_tbl[i][0], ov2640_uxga_init_reg_tbl[i][1]);
    }
    
    return 0;
}


/**
 * @brief       Configure OV2640 to JPEG mode
 * @param       None
 * @retval      None
 */
void ov2640_jpeg_mode(void)
{
    uint16_t i;
    
    /* Configure YUV422 format */
    for (i=0; i<(sizeof(ov2640_yuv422_reg_tbl) / 2); i++)
    {
        ov2640_write_reg(ov2640_yuv422_reg_tbl[i][0], ov2640_yuv422_reg_tbl[i][1]);
    }
    
    /* Configure output JPEG data */
    for (i=0; i<(sizeof(ov2640_jpeg_reg_tbl) / 2); i++)
    {
        ov2640_write_reg(ov2640_jpeg_reg_tbl[i][0], ov2640_jpeg_reg_tbl[i][1]);
    }
}


/**
 * @brief       Configure OV2640 to RGB565 mode
 * @param       None
 * @retval      None
 */
void ov2640_rgb565_mode(void)
{
    uint16_t i;
    
    /* Configure output RGB565 data */
    for (i=0; i<(sizeof(ov2640_rgb565_reg_tbl) / 4); i++)
    {
        ov2640_write_reg(ov2640_rgb565_reg_tbl[i][0], ov2640_rgb565_reg_tbl[i][1]);
    }
}


/* Auto exposure parameter configuration
 * Supports 5 levels
 */
static const uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8] = {
    {
        0xFF, 0x01,
        0x24, 0x20,
        0x25, 0x18,
        0x26, 0x60,
    },
    {
        0xFF, 0x01,
        0x24, 0x34,
        0x25, 0x1C,
        0x26, 0x00,
    },
    {
        0xFF, 0x01,
        0x24, 0x3E,
        0x25, 0x38,
        0x26, 0x81,
    },
    {
        0xFF, 0x01,
        0x24, 0x48,
        0x25, 0x40,
        0x26, 0x81,
    },
    {
        0xFF, 0x01,
        0x24, 0x58,
        0x25, 0x50,
        0x26, 0x92,
    },
};

/**
 * @brief       Configure OV2640 EV exposure compensation
 * @param       level: EV exposure compensation level, range: 0~4
 * @retval      None
 */
void ov2640_auto_exposure(uint8_t level)
{
    uint8_t i;
    uint8_t *p;
    
    p = (uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
    for (i=0; i<4; i++)
    { 
        ov2640_write_reg(p[i * 2], p[i * 2 + 1]); 
    }
}

/**
 * @brief     Configure OV2640 white balance mode
 * @param     mode: White balance mode, range: 0~4
 * @arg       0: Auto
 * @arg       1: Sunny
 * @arg       2: Office
 * @arg       3: Cloudy
 * @arg       4: Home
 * @retval    None
 */
void ov2640_light_mode(uint8_t mode)
{
    uint8_t regccval = 0x5E;                /* The default is Sunny */
    uint8_t regcdval = 0x41;
    uint8_t regceval = 0x54;
    
    switch (mode)
    { 
        case 0:                             /* Auto */
        {
            ov2640_write_reg(0xFF, 0x00);
            ov2640_write_reg(0xC7, 0x00);
            return;
        }
        case 1:                             /* Sunny */
        {
            regccval = 0x5E;
            regcdval = 0x41;
            regceval = 0x54;
            break;
        }
        case 2:                             /* Cloudy */
        {
            regccval = 0x65;
            regcdval = 0x41;
            regceval = 0x4F;
            break;
        }
        case 3:                             /* Office */
        {
            regccval = 0x52;
            regcdval = 0x41;
            regceval = 0x66;
            break;
        }
        case 4:                             /* Home */
        {
            regccval = 0x42;
            regcdval = 0x3F;
            regceval = 0x71;
            break;
        }
        default:
        {
            break;
        }
    }
    
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0xC7, 0x40);           /* AWB OFF  */
    ov2640_write_reg(0xCC, regccval);
    ov2640_write_reg(0xCD, regcdval);
    ov2640_write_reg(0xCE, regceval);
}

/**
 * @brief       Configure OV2640 color saturation
 * @param       sat: Color saturation, range: 0~4
 * @arg       	0: -2
 * @arg       	1: -1
 * @arg       	2: 0
 * @arg       	3: 1
 * @arg       	4: 2
 * @retval      None
 */
void ov2640_color_saturation(uint8_t sat)
{
    uint8_t reg7dval;
    
    reg7dval = ((sat + 2) << 4) | 0x08;
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0x7C, 0x00);
    ov2640_write_reg(0x7D, 0x02);
    ov2640_write_reg(0x7C, 0x03);
    ov2640_write_reg(0x7D, reg7dval);
    ov2640_write_reg(0x7D, reg7dval);
}


/**
 * @brief       Configure OV2640 brightness
 * @param       bright: Brightness, range: 0~4
 * @arg       	0: -2
 * @arg       	1: -1
 * @arg       	2: 0
 * @arg       	3: 1
 * @arg       	4: 2
 * @retval      None
 */
void ov2640_brightness(uint8_t bright)
{
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0x7C, 0x00);
    ov2640_write_reg(0x7D, 0x04);
    ov2640_write_reg(0x7C, 0x09);
    ov2640_write_reg(0x7D, bright << 4);
    ov2640_write_reg(0x7D, 0x00);
}

/**
 * @brief       Configure OV2640 contrast
 * @param       contrast: Contrast, range: 0~4
 * @arg       	0: -2
 * @arg       	1: -1
 * @arg       	2: 0
 * @arg       	3: 1
 * @arg       	4: 2
 * @retval      None
 */
void ov2640_contrast(uint8_t contrast)
{
    uint8_t reg7d0val = 0x20;   /* The default is 0 */
    uint8_t reg7d1val = 0x20;
    
    switch (contrast)
    {
        case 0:                 /* -2 */
        {
            reg7d0val = 0x18;
            reg7d1val = 0x34;
            break;
        }
        case 1:                 /* -1 */
        {
            reg7d0val = 0x1C;
            reg7d1val = 0x2A;
            break;
        }
        case 2:                 /* 0 */
        {
            reg7d0val = 0x20;
            reg7d1val = 0x20;
            break;
        }
        case 3:                 /* 1 */
        {
            reg7d0val = 0x24;
            reg7d1val = 0x16;
            break;
        }
        case 4:                 /* 2 */
        {
            reg7d0val = 0x28;
            reg7d1val = 0x0C;
            break;
        }
        default:
        {
            break;
        }
    }
    
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0x7C, 0x00);
    ov2640_write_reg(0x7D, 0x04);
    ov2640_write_reg(0x7C, 0x07);
    ov2640_write_reg(0x7D, 0x20);
    ov2640_write_reg(0x7D, reg7d0val);
    ov2640_write_reg(0x7D, reg7d1val);
    ov2640_write_reg(0x7D, 0x06);
}

/**
 * @brief       Configure OV2640 special effects
 * @param       eft: Special effect, range: 0~6
 * @arg       	0: Normal
 * @arg       	1: Negative
 * @arg       	2: Black and White
 * @arg       	3: Reddish
 * @arg       	4: Greenish
 * @arg       	5: Bluish
 * @arg       	6: Vintage
 * @retval      None
 */
void ov2640_special_effects(uint8_t eft)
{
    uint8_t reg7d0val = 0x00;   /* Default to normal */
    uint8_t reg7d1val = 0x80;
    uint8_t reg7d2val = 0x80;
    
    switch(eft)
    {
        case 0:                 /* Normal */
        {
            reg7d0val = 0x00;
            reg7d1val = 0x80;
            reg7d2val = 0x80;
            break;
        }
        case 1:                 /* Negative */
        {
            reg7d0val = 0x40;
            reg7d1val = 0x80;
            reg7d2val = 0x80;
            break;
        }
        case 2:                 /* Black and White */
        {
            reg7d0val = 0x18;
            reg7d1val = 0x80;
            reg7d2val = 0x80;
            break;
        }
        case 3:                 /* Reddish */
        {
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0xC0;
            break;
        }
        case 4:                 /* Greenish */
        {
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0x40;
            break;
        }
        case 5:                 /* Bluish */
        {
            reg7d0val = 0x18;
            reg7d1val = 0xA0;
            reg7d2val = 0x40;
            break;
        }
        case 6:                 /* Vintage */
        {
            reg7d0val = 0x18;
            reg7d1val = 0x40;
            reg7d2val = 0xA6;
            break;
        }
        default:
        {
            break;
        }
    }
    
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0x7C, 0x00);
    ov2640_write_reg(0x7D, reg7d0val);
    ov2640_write_reg(0x7C, 0x05);
    ov2640_write_reg(0x7D, reg7d1val);
    ov2640_write_reg(0x7D, reg7d2val);
}


/**
 * @brief       OV2640 color bar test
 * @param       mode: Test mode
 * @arg       	0: Turn off
 * @arg       	1: Color bars
 * @retval      None
 */
void ov2640_color_bar(uint8_t mode)
{
    uint8_t reg;
    
    ov2640_write_reg(0xFF, 0x01);
    reg = ov2640_read_reg(0x12);
    reg &= ~(1 << 1);
    if (mode)
    {
        reg |= (1 << 1);
    }
    ov2640_write_reg(0x12, reg);
}

/**
 * @brief       Set the sensor output window
 * @param       sx     : Starting address X of the sensor output window
 * @param       sy     : Starting address Y of the sensor output window
 * @param       width  : Width of the sensor output window (corresponding to horizontal)
 * @param       height : Height of the sensor output window (corresponding to vertical)
 * @retval      None
 */
void ov2640_window_set(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t endx;
    uint16_t endy;
    uint8_t temp;
    
    endx = sx + (width >> 1);
    endy = sy + (height >> 1);
    
    ov2640_write_reg(0xFF, 0x01);
    temp = ov2640_read_reg(0x03);               /* Read the value before Vref */
    temp &= 0xF0;
    temp |= ((endy & 0x03) << 2) | (sy & 0x03);
    ov2640_write_reg(0x03, temp);               /* Set the lowest 2 bits of Vref's start and end */
    ov2640_write_reg(0x19, sy >> 2);            /* Set the high 8 bits of Vref's start */
    ov2640_write_reg(0x1A, endy >> 2);          /* Set the high 8 bits of Vref's end */
    
    temp = ov2640_read_reg(0x32);               /* Read the value before Href */
    temp &= 0xC0;
    temp |= ((endx & 0x07) << 3) | (sx & 0x07);
    ov2640_write_reg(0x32, temp);               /* Set the lowest 3 bits of Href's start and end */
    ov2640_write_reg(0x17, sx >> 3);            /* Set the high 8 bits of Href's start */
    ov2640_write_reg(0x18, endx >> 3);          /* Set the high 8 bits of Href's end */
}


/**
 * @brief       Set the image output size
 * @note        The size (resolution) of the output image from OV2640 is completely determined by this function.
 *              width and height must be multiples of 4
 * @param       width : Output width (corresponding to horizontal)
 * @param       height: Output height (corresponding to vertical)
 * @retval      Setting result
 *              0   : Success
 *              Others: Failure
 */
uint8_t ov2640_outsize_set(uint16_t width, uint16_t height)
{
    uint16_t outh;
    uint16_t outw;
    uint8_t temp;
    
    if ((width & 3) != 0)
    {
        return 1;
    }
    
    if ((height & 3) != 0)
    {
        return 2;
    }
    
    outw = width >> 2;
    outh = height >> 2;
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0xE0, 0x04);
    ov2640_write_reg(0x5A, outw & 0xFF);    /* Set the low eight bits of OUTW */
    ov2640_write_reg(0x5B, outh & 0xFF);    /* Set the low eight bits of OUTH */
    
    temp = (outw >> 8) & 0x03;
    temp |= (outh >> 6) & 0x04;
    ov2640_write_reg(0x5C, temp);           /* Set the high bits of OUTH/OUTW */
    ov2640_write_reg(0xE0, 0x00);
    
    return 0; // Success
}

/**
 * @brief       Set the image window size
 * @note        The size (resolution) of the output image from OV2640 is completely determined by this function.
 *              Because the scaling function is enabled, the image window used for output is: xsize - 2 * offx, ysize - 2 * offy
 *              The actual output (width, height) is then scaled based on xsize - 2 * offx, ysize - 2 * offy
 *              Generally, the values of offx and offy are set to 16 and 4, smaller values can also be used, but the defaults are 16 and 4
 * @param       offx : The offset in the X direction of the window set by ov2640_image_window_set for the output image
 * @param       offy: The offset in the Y direction of the window set by ov2640_image_window_set for the output image
 * @param       width : Output width (corresponding to horizontal)
 * @param       height: Output height (corresponding to vertical)
 * @retval      Setting result
 *              0   : Success
 *              Others: Failure
 */
uint8_t ov2640_image_win_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{
    uint16_t hsize;
    uint16_t vsize;
    uint8_t temp;
    
    if ((width & 3) != 0)
    {
        return 1;
    }
    
    if ((height & 3) != 0)
    {
        return 2;
    }
    
    hsize = width >> 2;
    vsize = height >> 2;

    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0xE0, 0x04);
    ov2640_write_reg(0x51, hsize & 0xFF);           // Set low 8 bits of H_SIZE
    ov2640_write_reg(0x52, vsize & 0xFF);           // Set low 8 bits of V_SIZE
    ov2640_write_reg(0x53, offx & 0xFF);            // Set low 8 bits of offx
    ov2640_write_reg(0x54, offy & 0xFF);            // Set low 8 bits of offy
    temp = (vsize >> 1) & 0x80;
    temp |= (offy >> 4) & 0x70;
    temp |= (hsize>>5) & 0x08;
    temp |= (offx >> 8) & 0x07; 
    ov2640_write_reg(0x55, temp);                   // Set high bits of H_SIZE/V_SIZE/OFFX,OFFY
    ov2640_write_reg(0x57, (hsize >> 2) & 0x80);    // Set high bit of H_SIZE/V_SIZE/OFFX,OFFY
    ov2640_write_reg(0xE0, 0x00);
    
    return 0;
}


/**
 * @brief       Set the size of the image
 * @note        UXGA: 1600*1200
 *              SVGA: 800*600
 *              CIF : 352*288
 * @param       width : Width of the image
 * @param       height: Height of the image
 * @retval      Setting result
 *              0   : Success
 *              Others: Failure
 */
uint8_t ov2640_imagesize_set(uint16_t width, uint16_t height)
{ 
    uint8_t temp;
    
    ov2640_write_reg(0xFF, 0x00);
    ov2640_write_reg(0xE0, 0x04);
    ov2640_write_reg(0xC0, (width) >> 3 & 0xFF);    /* Set bits 10:3 of HSIZE */
    ov2640_write_reg(0xC1, (height) >> 3 & 0xFF);   /* Set bits 10:3 of VSIZE */
    temp = (width & 0x07) << 3;
    temp |= height & 0x07;
    temp |= (width >> 4) & 0x80;
    ov2640_write_reg(0x8C, temp);
    ov2640_write_reg(0xE0, 0x00);
    
    return 0;
}

/**
 * @brief       Control external flash
 * @param       sw: Control signal
 *              0: Off
 *              1: On
 * @retval      None
 */
void ov2640_flash_extctrl(uint8_t sw)
{
    if(sw != 0)
    {
        OV2640_FLASH(1);
    }
    else
    {
        OV2640_FLASH(0);
    }
}


/**
 * @brief       Control internal flash
 * @note        The control signal is automatically output from the STROBE pin of OV2640
 * @retval      None
 */
void ov2640_flash_intctrl(void)
{
    ov2640_write_reg(0xFF, 0x01);
    ov2640_write_reg(0x4B, 0x81);
    ov2640_write_reg(0x4B, 0x01);
}
