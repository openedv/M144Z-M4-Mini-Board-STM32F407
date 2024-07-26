/**
 ****************************************************************************************************
 * @file        videoplayer.h
 * @author      ALIENTEK
 * @brief       Video player application code
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

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include "main.h"

/* ����ռ���ض��� */
#define AVI_VIDEO_BUF_SIZE (60 * 1024)
#define AVI_AUDIO_BUF_SIZE (5 * 1024)

/* �������� */
void video_play(void);  /* ������Ƶ */

#endif
