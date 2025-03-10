/**
 ****************************************************************************************************
 * @file        fattester.h
 * @author      ALIENTEK
 * @brief       fatfs test code
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

#ifndef __FATTESTER_H
#define __FATTESTER_H

#include "ff.h"


/* Define whether to support file system testing functionality
 * 1, support (enabled);
 * 0, do not support (disabled);
 */
#define USE_FATTESTER         1


/* If file system testing is supported, enable the following code */
#if USE_FATTESTER == 1
 
/* FATFS test structure */
typedef struct
{
    FIL *file;          /* File structure pointer 1 */
    FILINFO fileinfo;   /* File information */
    DIR dir;            /* Directory */
    uint8_t *fatbuf;    /* Read-write buffer */
    uint8_t initflag;   /* Initialization flag */
} _m_fattester;

extern _m_fattester fattester;      /* FATFS test structure */

uint8_t mf_init(void);              /* Initialize file system testing */
void mf_free(void);                 /* Free memory allocated during file system testing */


uint8_t mf_mount(uint8_t* path, uint8_t mt);
uint8_t mf_open(uint8_t*path, uint8_t mode);
uint8_t mf_close(void);
uint8_t mf_read(uint16_t len);
uint8_t mf_write(uint8_t*dat, uint16_t len);
uint8_t mf_opendir(uint8_t* path);
uint8_t mf_closedir(void);
uint8_t mf_readdir(void);
uint8_t mf_scan_files(uint8_t * path);
uint32_t mf_showfree(uint8_t *drv);
uint8_t mf_lseek(uint32_t offset);
uint32_t mf_tell(void);
uint32_t mf_size(void);
uint8_t mf_mkdir(uint8_t*pname);
uint8_t mf_fmkfs(uint8_t* path, uint8_t opt, uint16_t au);
uint8_t mf_unlink(uint8_t *pname);
uint8_t mf_rename(uint8_t *oldname, uint8_t* newname);
void mf_getlabel(uint8_t *path);
void mf_setlabel(uint8_t *path); 
void mf_gets(uint16_t size);
uint8_t mf_putc(uint8_t c);
uint8_t mf_puts(uint8_t*c);

#endif

 
#endif
