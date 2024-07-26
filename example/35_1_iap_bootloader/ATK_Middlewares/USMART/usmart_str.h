/**
 ****************************************************************************************************
 * @file        usmart_str.h
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



/* USER CODE BEGIN Header */

#ifndef __USMART_STR_H
#define __USMART_STR_H

#include "usmart_port.h"


uint8_t usmart_get_parmpos(uint8_t num);                								/* Gets the starting address of the specified parameter */
uint8_t usmart_strcmp(char *str1, char *str2);    										/* Compares the strings str1 and str2 */
uint32_t usmart_pow(uint8_t m, uint8_t n);              								/* Squared function, m^n */
uint8_t usmart_str2num(char *str, uint32_t *res);    									/* Converts strings to numbers */
uint8_t usmart_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen);	/* Gets the command name */
uint8_t usmart_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval); 		/* Gets the function name from str */
uint8_t usmart_get_aparm(char *str, char *fparm, uint8_t *ptype); 						/* Takes an argument to a function from str */
uint8_t usmart_get_fparam(char *str, uint8_t *parn); 									/* Takes function arguments from str. */

#endif











