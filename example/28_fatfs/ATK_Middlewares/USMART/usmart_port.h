/**
 ****************************************************************************************************
 * @file        usmart_port.h
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
 
#ifndef __USMART_PORT_H
#define __USMART_PORT_H

#include "usart.h"

/******************************************************************************************/
/* User configuration parameter */

#define MAX_FNAME_LEN           30      /* The maximum length of the function name, which should be set to not less than the length of the longest function name. */
#define MAX_PARM                10      /* The value contains a maximum of 10 parameters. To modify this parameter, you must modify usmart_exe. */
#define PARM_LEN                200     /* The length of the sum of all parameters should not exceed PARM_LEN bytes, and the serial port receiving part should correspond to it (not less than PARM_LEN). */


#define USMART_ENTIMX_SCAN      1       /* Use TIM's timer interrupt to scan the Scan Function, if set to 0, you need to implement your own scan function at regular intervals
                                         * Note: If you want to use runtime statistics, you must set USMART_ENTIMX_SCAN to 1!!!!
                                         */

#define USMART_USE_HELP         1       /* With help, this value is set to 0, which saves nearly 700 bytes, but results in no help information being displayed */
#define USMART_USE_WRFUNS       1       /* Using the read and write function, enable here, can read the value of any address, can also write the value of the register */

#define USMART_PRINTF           printf  /* Define the printf output */

/******************************************************************************************/

char * usmart_get_input_string(void); 		/* Gets the input data stream */
void usmart_timx_reset_time(void);      	/* Reset run time */
uint32_t usmart_timx_get_time(void);     	/* Get run time */
void usmart_timx_init(void);   				/* Initialization timer */

#endif
