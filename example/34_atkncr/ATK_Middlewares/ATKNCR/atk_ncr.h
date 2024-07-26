/**
 ****************************************************************************************************
 * @file        atk_ncr.h
 * @author      ALIENTEK
 * @brief       ATKNCR code
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

#ifndef __ATK_NCR_H
#define __ATK_NCR_H


#define ATK_NCR_TRACEBUF1_SIZE      500*4           /* Define the first tracebuf size (in bytes) and make it larger if there is a crash */
#define ATK_NCR_TRACEBUF2_SIZE      250*4           /* Define the size (in bytes) of the second tracebuf array and make it larger if there is a crash */


/* Enter the trajectory coordinate type */
typedef __PACKED_STRUCT _atk_ncr_point
{
    short x;        /* X-axis coordinates */
    short y;        /* Y-axis coordinates */
}atk_ncr_point;



/* External function calls
 * Initialize the recognizer
 * Return value: 0, initialization was successful
 * 1, failed initialization
 */
unsigned char alientek_ncr_init(void);


/* Stop the recognizer */
void alientek_ncr_stop(void);

void alientek_ncr(atk_ncr_point * track,int potnum,int charnum,unsigned char mode,char*result);

void alientek_ncr_memset(char *p,char c,unsigned long len);

void *alientek_ncr_malloc(unsigned int size);

void alientek_ncr_free(void *ptr);

#endif



















