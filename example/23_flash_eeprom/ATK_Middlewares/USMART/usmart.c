/**
 ****************************************************************************************************
 * @file        usmart.c
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

#include "usmart.h"
#include "usmart_str.h"
#include "usmart_port.h"


/* system command */
char *sys_cmd_tab[] =
{
    "?",
    "help",
    "list",
    "id",
    "hex",
    "dec",
    "runtime",
};

/**
* @brief 	handles system commands
* @param 	str: string pointer
* @retval 	0, successfully processed; Other, error code;
*/
uint8_t usmart_sys_cmd_exe(char *str)
{
    uint8_t i;
    char sfname[MAX_FNAME_LEN];                     /* Store local function names */
    uint8_t pnum;
    uint8_t rval;
    uint32_t res;
    res = usmart_get_cmdname(str, sfname, &i, MAX_FNAME_LEN);   /* Get the instruction and the instruction length */

    if (res)return USMART_FUNCERR;                  /* Wrong instruction */

    str += i;

    for (i = 0; i < sizeof(sys_cmd_tab) / 4; i++)   /* Supported system directives */
    {
        if (usmart_strcmp(sfname, sys_cmd_tab[i]) == 0)break;
    }

    switch (i)
    {
        case 0:
        case 1: /* help command */
            USMART_PRINTF("\r\n");
#if USMART_USE_HELP
            USMART_PRINTF("------------------------USMART V3.5------------------------ \r\n");
            USMART_PRINTF("    USMART is a clever serial debugging interinterface developed by ALIENTEK Team, \r\n");
            USMART_PRINTF("which allows you to call any function in the program through the serial assistant and execute it.\r\n");
            USMART_PRINTF("Therefore, you can freely change the input parameters of the function (support numbers (10/16, support negative numbers),\r\n"),
            USMART_PRINTF("strings, function entry address, etc.),a single function supports up to 10 input parameters, and supports the function return value display.\r\n"),
            USMART_PRINTF("After the V2.1 version, hex and dec were added, which can be used to set the display format of function parameters, \r\n");
            USMART_PRINTF("and can also be used for data conversion \r\n");
            USMART_PRINTF("Technical support : www.openedv.com\r\n");
            USMART_PRINTF("USMART has 7 system commands (must be in lower case) : \r\n");
            USMART_PRINTF("?:      Get help information\r\n");
            USMART_PRINTF("help:   Get help information\r\n");
            USMART_PRINTF("list:   List of available functions\r\n\n");
            USMART_PRINTF("id:     List of IDs of available functions\r\n\n");
            USMART_PRINTF("hex:    The parameter is displayed in hexadecimal format, followed by a space + a number to perform a decimal conversion\r\n\n");
            USMART_PRINTF("dec:    The parameter is displayed in decimal, followed by a space + a number to perform a decimal conversion\r\n\n");
            USMART_PRINTF("runtime : 1, start the function runtime; 0, turn off the function runtime;\r\n\n");
            USMART_PRINTF("Please enter the function name and parameters in the format written by the program and end with the enter key.\r\n");
            USMART_PRINTF("--------------------------ALIENTEK------------------------- \r\n");
#else
            USMART_PRINTF("Instruction failure\r\n");
#endif
            break;

        case 2: /* Query instruction */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------Function list--------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)USMART_PRINTF("%s\r\n", usmart_dev.funs[i].name);

            USMART_PRINTF("\r\n");
            break;

        case 3: /* Query ID */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------Function ID --------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)
            {
                usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &pnum, &rval); /* Get the name of the local function */
                USMART_PRINTF("%s id is:\r\n0X%08X\r\n", sfname, (unsigned int)usmart_dev.funs[i].func);  /* Show ID */
            }

            USMART_PRINTF("\r\n");
            break;

        case 4: /* hex command */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* Normal parameter */
            {
                i = usmart_str2num(sfname, &res);       /* Record the parameter */

                if (i == 0) /* Decimal conversion function */
                {
                    USMART_PRINTF("HEX:0X%X\r\n", res); /* Convert to hexadecimal */
                }
                else if (i != 4)return USMART_PARMERR;  /* Parameter error */
                else        /* Parameter display setting function */
                {
                    USMART_PRINTF("Hexadecimal parameter display !\r\n");
                    usmart_dev.sptype = SP_TYPE_HEX;
                }

            }
            else return USMART_PARMERR; /* Parameter error */

            USMART_PRINTF("\r\n");
            break;

        case 5: /* dec command */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0)     /* Normal parameter */
            {
                i = usmart_str2num(sfname, &res);       /* Record the parameter */

                if (i == 0) /* Decimal conversion function */
                {
                    USMART_PRINTF("DEC:%lu\r\n", (unsigned long)res);  /* Convert to base 10 */
                }
                else if (i != 4)
                {
                    return USMART_PARMERR;  /* Parameter error */
                }
                else        /* Parameter display setting function */
                {
                    USMART_PRINTF("Decimal parameter display !\r\n");
                    usmart_dev.sptype = SP_TYPE_DEC;
                }

            }
            else 
            {
                return USMART_PARMERR;  /* Parameter error */
            }
                
            USMART_PRINTF("\r\n");
            break;

        case 6: /* runtime command that sets whether the function execution time is displayed */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* Normal parameter */
            {
                i = usmart_str2num(sfname, &res);   /* Record the parameter */

                if (i == 0) /* Read the specified address data function */
                {
                    if (USMART_ENTIMX_SCAN == 0)
                    {
                        USMART_PRINTF("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");/* Error */
                    }
                    else
                    {
                        usmart_dev.runtimeflag = res;

                        if (usmart_dev.runtimeflag)
                        {
                            USMART_PRINTF("Run Time Calculation ON\r\n");
                        }
                        else 
                        {
                            USMART_PRINTF("Run Time Calculation OFF\r\n");
                        }
                    }
                }
                else 
                {
                    return USMART_PARMERR;  /* No parameters are included, or the parameters are incorrect */
                }
            }
            else 
            {
                return USMART_PARMERR;      /* Parameter error */
            }
            
            USMART_PRINTF("\r\n");
            break;

        default:/* illegal command */
            return USMART_FUNCERR;
    }

    return 0;
}

/**
* @brief 	Initializes USMART
* @param 	tclk: Working frequency of the timer (unit :Mhz)
* @retval 	None
*/
void usmart_init(uint16_t tclk)
{
#if USMART_ENTIMX_SCAN == 1
	MX_TIM4_Init();			/* Initialization of the timer */
#endif
    usmart_dev.sptype = 1;  /* Display parameters in hexadecimal format */
}

/**
* @brief 	gets the function name,id, and parameter information from str
* @param 	str: string pointer.
* @retval 	0, identification success; Other, error code.
*/
uint8_t usmart_cmd_rec(char *str)
{
    uint8_t sta, i, rval;   /* status */
    uint8_t rpnum, spnum;
    char rfname[MAX_FNAME_LEN];  /* A temporary storage space for the received function names */
    char sfname[MAX_FNAME_LEN];  /* Store local function names */
    sta = usmart_get_fname(str, rfname, &rpnum, &rval); /* Get the function name and number of parameters for the received data */

    if (sta)return sta;     /* Error */

    for (i = 0; i < usmart_dev.fnum; i++)
    {
        sta = usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &spnum, &rval); /* Get the local function name and the number of arguments */

        if (sta)return sta; /* Local parsing error */

        if (usmart_strcmp(sfname, rfname) == 0) /* Equality */
        {
            if (spnum > rpnum)return USMART_PARMERR;/* Parameter errors (fewer input parameters than source function parameters) */

            usmart_dev.id = i;  /* Record function ID */
            break;  /*Jump out */
        }
    }

    if (i == usmart_dev.fnum)return USMART_NOFUNCFIND;  /* No matching function was found */

    sta = usmart_get_fparam(str, &i);   /* Get the number of function parameters */

    if (sta)return sta;     /* Error */

    usmart_dev.pnum = i;    /* Parameter number record */
    return USMART_OK;
}

/**
* @brief 	USMART execution function
* @note
* 			This function is used to finally execute a valid function received from the serial port.
* 			Functions with up to 10 parameters are supported, and more parameter support is easy to implement. However, it is rarely used. Generally, functions with about 5 parameters are very rare.
* 			This function prints the execution in the serial port. Start with :" Function name (parameter 1, parameter 2... Parameter N)= return value ".
* 			When the executed function does not return a value, the return value printed is a meaningless data.
*
* @param None
* @retval None
*/
void usmart_exe(void)
{
    uint8_t id, i;
    uint32_t res;
    uint32_t temp[MAX_PARM];        /* The parameter is converted to support strings */
    char sfname[MAX_FNAME_LEN];     /* Store local function names */
    uint8_t pnum, rval;
    id = usmart_dev.id;

    if (id >= usmart_dev.fnum)return;   /* not execute */

    usmart_get_fname((char *)usmart_dev.funs[id].name, sfname, &pnum, &rval);    /* Get the name of the local function and the number of arguments */
    USMART_PRINTF("\r\n%s(", sfname);   /* Outputs the name of the function being executed */

    for (i = 0; i < pnum; i++)          /* Output parameter */
    {
        if (usmart_dev.parmtype & (1 << i)) /* Argument is a string */
        {
            USMART_PRINTF("%c", '"');
            USMART_PRINTF("%s", usmart_dev.parm + usmart_get_parmpos(i));
            USMART_PRINTF("%c", '"');
            temp[i] = (uint32_t) & (usmart_dev.parm[usmart_get_parmpos(i)]);
        }
        else    /* Parameters are numbers */
        {
            temp[i] = *(uint32_t *)(usmart_dev.parm + usmart_get_parmpos(i));

            if (usmart_dev.sptype == SP_TYPE_DEC)
            {
                USMART_PRINTF("%ld", (long)temp[i]);  /* Decimal parameter display */
            }
            else 
            {
                USMART_PRINTF("0X%X", temp[i]); /* Hexadecimal parameter display */
            }
        }

        if (i != pnum - 1)USMART_PRINTF(",");
    }

    USMART_PRINTF(")");
#if USMART_ENTIMX_SCAN==1
    usmart_timx_reset_time();   /* Timer reset and start counting */
#endif

    switch (usmart_dev.pnum)
    {
        case 0: /* NONE */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)();
            break;

        case 1: /* There are 1 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0]);
            break;

        case 2: /* There are 2 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1]);
            break;

        case 3: /* There are 3 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2]);
            break;

        case 4: /* There are 4 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3]);
            break;

        case 5: /* There are 5 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4]);
            break;

        case 6: /* There are 6 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5]);
            break;

        case 7: /* There are 7 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6]);
            break;

        case 8: /* There are 8 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7]);
            break;

        case 9: /* There are 9 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8]);
            break;

        case 10:/* There are 10 parameters */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8], temp[9]);
            break;
    }

#if USMART_ENTIMX_SCAN==1
    usmart_timx_get_time(); /*  */
#endif

    if (rval == 1)  /* Required return value */
    {
        if (usmart_dev.sptype == SP_TYPE_DEC)USMART_PRINTF("=%lu;\r\n", (unsigned long)res);   /* Output execution result (decimal parameter display) */
        else USMART_PRINTF("=0X%X;\r\n", res);  /* Output execution result (hexadecimal parameter display) */
    }
    else USMART_PRINTF(";\r\n");    /* Do not need to return the value, directly output the end */

    if (usmart_dev.runtimeflag)     /* Need to display function execution time */
    {
        USMART_PRINTF("Function Run Time:%d.%1dms\r\n", usmart_dev.runtime / 10, usmart_dev.runtime % 10);  /* Print function execution time */
    }
}

/**
* @brief 	USMART scan function
* @note
* 			By calling this function, each control of USMART is realized. This function needs to be called at regular intervals
* 			Execute the functions sent from the serial port in a timely manner.
* 			This function can be called inside the interrupt, so as to achieve automatic management.
* 			If it is not a positive atomic user, USART_RX_STA and USART_RX_BUF[] need to be implemented by the user
*
* @param 	None
* @retval 	None
*/
void usmart_scan(void)
{
    uint8_t sta, len;
    char *pbuf = 0;

    pbuf = usmart_get_input_string();   /* Get data data flow */
    if (pbuf == 0) return ; /* The data stream is empty and is returned directly */
     
    sta = usmart_dev.cmd_rec(pbuf);     /* Get the function information */

    if (sta == 0)
    {
        usmart_dev.exe();  /* Executive function */
    }
    else
    {
        len = usmart_sys_cmd_exe(pbuf);

        if (len != USMART_FUNCERR)sta = len;

        if (sta)
        {
            switch (sta)
            {
                case USMART_FUNCERR:
                    USMART_PRINTF("parameter error !\r\n");
                    break;

                case USMART_PARMERR:
                    USMART_PRINTF("parameter error !\r\n");
                    break;

                case USMART_PARMOVER:
                    USMART_PRINTF("parameter number too much !\r\n");
                    break;

                case USMART_NOFUNCFIND:
                    USMART_PRINTF("No matching function was found !\r\n");
                    break;
            }
        }
    } 
 
}

#if USMART_USE_WRFUNS == 1  /* If read and write operations are enabled */

/**
* @brief 	Reads the value of the specified address
* @param 	None
* @retval 	None
*/
uint32_t read_addr(uint32_t addr)
{
    return *(uint32_t *)addr;
}

/**
* @brief 	Writes the specified value at the specified address
* @param 	None
* @retval 	None
*/
void write_addr(uint32_t addr, uint32_t val)
{
    *(uint32_t *)addr = val;
}

#endif





















