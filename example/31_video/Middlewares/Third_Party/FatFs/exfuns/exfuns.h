/**
 ****************************************************************************************************
 * @file        exfuns.h
 * @author      ALIENTEK
 * @brief       fatfs extension code
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M144-STM32F407 board
 * website      : https://www.alientek.com
 * forum        : http://www.openedv.com/forum.php
 *
 * change logs  :
 * version    data    notes
 * V1.0     20240410  the first version
 *
 ****************************************************************************************************
 */

#ifndef __EXFUNS_H
#define __EXFUNS_H

#include "ff.h"


extern FATFS *fs[_VOLUMES];
extern FIL *file;
extern FIL *ftemp;
extern UINT br, bw;
extern FILINFO fileinfo;
extern DIR dir;
extern uint8_t *fatbuf;   /* SD card data cache */

/* exfuns_file_type Specifies the returned type definition
 * Obtained according to FILE_TYPE_TBL. Defined in exfuns.c
 * /
#define T_BIN       0X00    /* bin file */
#define T_LRC       0X10    /* lrc file */

#define T_NES       0X20    /* nes file */
#define T_SMS       0X21    /* sms file */

#define T_TEXT      0X30    /* .txt file */
#define T_C         0X31    /* .c file */
#define T_H         0X32    /* .h file */

#define T_WAV       0X40    /* WAV file */
#define T_MP3       0X41    /* MP3 file */
#define T_OGG       0X42    /* OGG file */
#define T_FLAC      0X43    /* FLAC file */
#define T_AAC       0X44    /* AAC file */
#define T_WMA       0X45    /* WMA file */
#define T_MID       0X46    /* MID file */

#define T_BMP       0X50    /* bmp file */
#define T_JPG       0X51    /* jpg file */
#define T_JPEG      0X52    /* jpeg file */
#define T_GIF       0X53    /* gif file */

#define T_AVI       0X60    /* avi file */

/* Function declaration */
uint8_t exfuns_init(void);
uint8_t exfuns_file_type(char *fname);

uint8_t exfuns_get_free(char *pdrv, uint32_t *total, uint32_t *free);
uint32_t exfuns_get_folder_size(uint8_t *fdname);
uint8_t *exfuns_get_src_dname(uint8_t *dpfn);
uint8_t exfuns_file_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);
uint8_t exfuns_folder_copy(uint8_t(*fcpymsg)(uint8_t *pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode);
#endif
