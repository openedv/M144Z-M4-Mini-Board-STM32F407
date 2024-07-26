/**
 ****************************************************************************************************
 * @file        malloc.h
 * @author      ALIENTEK
 * @brief       malloc code
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

#ifndef __MALLOC_H
#define __MALLOC_H

#include "main.h"

/* memory pools */
#define SRAMIN      0       /* Internal SRAM, 64KB in total */
#define SRAMCCM     1       /* CCM memory pool (This part of SRAM is only accessible by the CPU!!) */
#define SRAMEX      2       /* External SRAM pool */
#define SRAMBANK    3       /* Defines the number of SRAM blocks supported. */


#define MT_TYPE     uint16_t

 
/* mem1 memory parameter setting.mem1 is the SRAM inside F103. */
#define MEM1_BLOCK_SIZE         32                              /* The memory block size is 32 bytes */
#define MEM1_MAX_SIZE           100 * 1024                      /* The maximum managed memory is 40K, with a total of 512KB of F103 internal SRAM */
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   /* Memory table size */

/* Memory parameters for mem2. mem2 is located in CCM and is used to manage CCM. (Note: This part of SRAM can only be accessed by the CPU!) */
#define MEM2_BLOCK_SIZE         32                              /* Memory block size is 32 bytes */
#define MEM2_MAX_SIZE           60 * 1024                       /* Maximum managed memory is 60K */
#define MEM2_ALLOC_TABLE_SIZE   MEM2_MAX_SIZE / MEM2_BLOCK_SIZE /* Memory table size */

/* Memory parameters setting for mem3. mem3 is external SRAM expansion */
#define MEM3_BLOCK_SIZE         32                              /* Memory block size is 32 bytes */
#define MEM3_MAX_SIZE           963 *1024                       /* Maximum managed memory is 963K */
#define MEM3_ALLOC_TABLE_SIZE   MEM3_MAX_SIZE/MEM3_BLOCK_SIZE   /* Memory table size */

#ifndef NULL
#define NULL 0
#endif

/* Memory Management controller */
struct _m_mallco_dev
{
    void (*init)(uint8_t);          /* initialize */
    uint16_t (*perused)(uint8_t);   /* memory usage */
    uint8_t *membase[SRAMBANK];     /* The memory pool manages the memory of the SRAMBANK regions */
    MT_TYPE *memmap[SRAMBANK];      /* Memory management status table */
    uint8_t  memrdy[SRAMBANK];      /* Memory management in place */
};

extern struct _m_mallco_dev mallco_dev;

void my_mem_init(uint8_t memx);
uint16_t my_mem_perused(uint8_t memx) ;
void my_mem_set(void *s, uint8_t c, uint32_t count);
void my_mem_copy(void *des, void *src, uint32_t n);

void myfree(uint8_t memx, void *ptr);
void *mymalloc(uint8_t memx, uint32_t size);
void *myrealloc(uint8_t memx, void *ptr, uint32_t size);

#endif
