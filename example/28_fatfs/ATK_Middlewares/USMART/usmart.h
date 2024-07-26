/**
 ****************************************************************************************************
 * @file        usmart.h
 * @author      ALIENTEK
 * @brief       USMART Serial debugging component
 *
 * 				USMART is a clever serial debugging interinterface developed by ALIENTEK Team, which allows you to call any function in the program through the serial assistant and execute it.
 * 				Therefore, you can freely change the input parameters of the function (support numbers (10/16, support negative numbers), strings, function entry address, etc.),
 * 				a single function supports up to 10 input parameters, and supports the function return value display.
 * 				After the V2.1 version, hex and dec were added, which can be used to set the display format of function parameters, and can also be used for data conversion, such as:
 *				Typing "hex 100" will see HEX 0X64 on the serial debugging assistant,and so on.
 *
 *              FLASH:4K to K bytes (setting by USMART_USE_HELP and USMART_USE_WRFUNS)
 *              SRAM:72 bytes (minimum case)
 *              SRAM calculation formula: SRAM=PARM_LEN+72-4 Where PARM_LEN must be greater than or equal to 4.
 *              The stack should be at least 100 bytes.
 *
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform  	: ALIENTEK STM32F103 development board
 * website		: https://www.alientek.com
 * forum		: http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240222	the first version
 *
 ****************************************************************************************************
 */



/* USER CODE BEGIN Header */

#ifndef __USMART_H
#define __USMART_H

#include "usmart_port.h"


#define USMART_OK               0       /* OK */
#define USMART_FUNCERR          1       /* Function error */
#define USMART_PARMERR          2       /* parameter error */
#define USMART_PARMOVER         3       /* Parameter overflow */
#define USMART_NOFUNCFIND       4       /* No matching function found */

#define SP_TYPE_DEC             0       /* Decimal parameter display */
#define SP_TYPE_HEX             1       /* Hexadecimal parameter display */


/* Function name list */
struct _m_usmart_nametab
{
    void *func;             /* Function pointer */
    const char *name;       /* Function name (lookup string) */
};

/* usmart Control Manager */
struct _m_usmart_dev
{
    struct _m_usmart_nametab *funs;     /* Function name pointer */

    void (*init)(uint16_t tclk);        /* Initialize */
    uint8_t (*cmd_rec)(char *str);   	/* Identify function names and parameters */
    void (*exe)(void);                  /* Execute  */
    void (*scan)(void);                 /* Scan */
    uint8_t fnum;                       /* Function quantity */
    uint8_t pnum;                       /* Arity */
    uint8_t id;                         /* Function ID */
    uint8_t sptype;                     /* Parameter display type (non-string parameter):0, decimal; 1, hexadecimal */
    uint16_t parmtype;                  /* Type of parameter */
    uint8_t  plentbl[MAX_PARM];         /* The length of each parameter is stored in a temporary table */
    uint8_t  parm[PARM_LEN];            /* Parameter of a function */
    uint8_t runtimeflag;                /* 0, do not count the function execution time; 1, statistics function execution time, note: this function must be enabled when USMART_ENTIMX_SCAN is useful */
    uint32_t runtime;                   /* Running time, unit :0.1ms, the maximum delay time is 2 times of the timer CNT value *0.1ms */
};

extern struct _m_usmart_nametab usmart_nametab[];   /* Define in usmart_config.c */
extern struct _m_usmart_dev usmart_dev;             /* Define in usmart_config.c */


void usmart_init(uint16_t tclk);        			/* Initialize */
uint8_t usmart_cmd_rec(char *str);    				/* Identify */
void usmart_exe(void);                  			/* Execute */
void usmart_scan(void);                 			/* Scan */
uint32_t read_addr(uint32_t addr);      			/* Reads the value of the specified address */
void write_addr(uint32_t addr,uint32_t val);		/* Writes the specified value to the specified address */

#endif






























