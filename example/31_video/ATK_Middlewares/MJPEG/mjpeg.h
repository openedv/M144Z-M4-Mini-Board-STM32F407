/**
 ****************************************************************************************************
 * @file        mjpeg.h
 * @author      ALIENTEK
 * @brief       MJPEG video processing code
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
 
#ifndef __MJPEG_H
#define __MJPEG_H 

#include "stdio.h" 
#include "stdint.h"
#include "../../ATK_Middlewares/MJPEG/JPEG/cdjpeg.h"
#include <setjmp.h>



struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

uint8_t mjpegdec_init(uint16_t offx,uint16_t offy);
void mjpegdec_free(void);
uint8_t mjpegdec_decode(uint8_t* buf,uint32_t bsize);

#endif
