/**
 ****************************************************************************************************
 * @file        atk_ncr.c
 * @author      ALIENTEK
 * @brief       ATKNCR code
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

#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../ATK_Middlewares/ATKNCR/atk_ncr.h"

/**
 * @brief   memory setup function
 * @param  *p   : Memory head address
 * @param   c   : The value to set
 * @param   len : The amount of memory (in bytes) to set
 * @retval  None
 */
void alientek_ncr_memset(char *p, char c, unsigned long len)
{
    my_mem_set((uint8_t*)p, (uint8_t)c, (uint32_t)len);
}

/**
 * @brief   allocates memory
 * @param   size : The size (in bytes) of memory to allocate
 * @retval  at the head of the allocated memory.
 */
void *alientek_ncr_malloc(unsigned int size) 
{
    return mymalloc(SRAMIN,size);
}

/**
 * @brief   frees memory
 * @param   ptr : Memory head address
 * @retval  None
 */
void alientek_ncr_free(void *ptr) 
{
    myfree(SRAMIN,ptr);
}




















