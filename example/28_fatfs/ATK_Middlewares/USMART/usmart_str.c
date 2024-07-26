/**
 ****************************************************************************************************
 * @file        usmart_str.c
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

/**
* @brief 	Compares the strings str1 and str2
* @param 	str1: first address of string 1 (pointer)
* @param 	str2: first address of string 2 (pointer)
* @retval 	0, equal; 1, inequality;
*/
uint8_t usmart_strcmp(char *str1, char *str2)
{
    while (1)
    {
        if (*str1 != *str2)return 1; /* inequality */

        if (*str1 == '\0')break;    /* The comparison is complete */

        str1++;
        str2++;
    }

    return 0;/* Two strings equal */
}

/**
* @brief 	Copies the src content to dst
* @param 	src: indicates the source address
* @param 	dst: indicates the destination address
* @retval 	0, equal; 1, inequality;
*/
void usmart_strcopy(char *src, char *dst)
{
    while (1)
    {
        *dst = *src;            /* copy */

        if (*src == '\0')break; /* copy complete */

        src++;
        dst++;
    }
}

/**
* @brief 	Gets the length of the string (bytes)
* @param 	str: string pointer
* @retval 	The length of the string
*/
uint8_t usmart_strlen(char *str)
{
    uint8_t len = 0;

    while (1)
    {
        if (*str == '\0')break; /* copy complete */

        len++;
        str++;
    }

    return len;
}

/**
* @brief 	Squared function, m^n
* @param 	m: base number
* @param 	n: indexes
* @retval 	m to the NTH power
*/
uint32_t usmart_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}

/**
* @brief 	Converts strings to numbers
* @note		Hexadecimal conversion is supported, but hexadecimal letters must be uppercase and the format must start with 0X.
* 			Support negative numbers.
* @param 	str: string pointer
* @param 	res: address to store the converted result.
* @retval 	The results are as follows:
* @arg 		0, successful conversion completed
* @arg 		1, the data format is incorrect
* @arg 		2, the hexadecimal digit is 0
* @arg 		3, the start format is incorrect
* @arg 		4, the decimal digit is 0
*/
uint8_t usmart_str2num(char *str, uint32_t *res)
{
    uint32_t t;
    int tnum;
    uint8_t bnum = 0;   /* Number of digits */
    char *p;
    uint8_t hexdec = 10;/* The default value is decimal */
    uint8_t flag = 0;   /* 0, no symbol mark; 1 indicates a positive number; 2 is a negative number. */
    p = str;
    *res = 0;   		/* reset */

    while (1)
    {
        /* Parameter validity detection */
        if ((*p <= '9' && *p >= '0') || ((*str == '-' || *str == '+') && bnum == 0) || (*p <= 'F' && *p >= 'A') || (*p == 'X' && bnum == 1))
        {
            if (*p >= 'A')hexdec = 16;  /* The character string contains letters in hexadecimal format。 */

            if (*str == '-')
            {
                flag = 2;   /* Offset symbol */
                str += 1;
            }
            else if (*str == '+')
            {
                flag = 1;   /* Offset symbol */
                str += 1;
            }
            else
            {
                bnum++; /* The number of bits increment */
            }
        }
        else if (*p == '\0')
        {
            break;      /* Hit the terminator, exit */
        }
        else
        {
            return 1;   /* Not all decimal or hexadecimal data */
        }

        p++;
    }

    p = str;            /* Relocate to the address at the beginning of the string */

    if (hexdec == 16)   /* Hexadecimal data */
    {
        if (bnum < 3)return 2;  /* If the number of bits is less than 3, exit directly. Because 0X accounts for two, if 0X is not followed by data, the data is illegal */

        if (*p == '0' && (*(p + 1) == 'X'))   /* Must start with '0X' */
        {
            p += 2;     /* Offset to data start address */
            bnum -= 2;  /* Offset minus */
        }
        else
        {
            return 3;   /* The start header is in the wrong format */
        }
    }
    else if (bnum == 0)
    {
        return 4;       /* If the number of bits is 0, exit directly */
    }

    while (1)
    {
        if (bnum)bnum--;

        if (*p <= '9' && *p >= '0')t = *p - '0';    /* Get the value of the number */
        else t = *p - 'A' + 10; /* Get the values of A to F */

        *res += t * usmart_pow(hexdec, bnum);
        p++;

        if (*p == '\0')break;   /* Finished checking the data */
    }

    if (flag == 2)      /* Is it a negative number? */
    {
        tnum = -*res;
        *res = tnum;
    }

    return 0;   /* Successful conversion */
}

/**
* @brief 	Gets the command name
* @param 	str 	: source string
* @param 	cmdname	: indicates the command name
* @param 	nlen	: indicates the length of the command name
* @param 	maxlen	: Maximum length (do limit, instruction cannot be too long)
* @retval 	0, successful; Other, failure.
*/
uint8_t usmart_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen)
{
    *nlen = 0;

    while (*str != ' ' && *str != '\0')   /* If a space or terminator is found, it is considered over */
    {
        *cmdname = *str;
        str++;
        cmdname++;
        (*nlen)++;  /* Statistical command length */

        if (*nlen >= maxlen)return 1;   /* Wrong instruction */
    }

    *cmdname = '\0';/* Add terminator */
    return 0;       /* Normal return */
}

/**
* @brief 	Gets the next character (when there are many spaces in between, this function simply ignores the space and finds the first character after the space)
* @param 	str : string pointer
* @retval 	Next character
*/
uint8_t usmart_search_nextc(char *str)
{
    str++;

    while (*str == ' ' && str != 0)str++;

    return *str;
}

/**
* @brief 	Gets the function name from str
* @param 	str: source string pointer
* @param 	fname: pointer to the obtained function name
* @param 	pnum: indicates the number of parameters of the function
* @param 	rval: whether to display the return value (0, no; 1, need)
* @retval 	0, successful; Other, error code.
*/
uint8_t usmart_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval)
{
    uint8_t res;
    uint8_t fover = 0;  /* Bracket depth */
    char *strtemp;
    uint8_t offset = 0;
    uint8_t parmnum = 0;
    uint8_t temp = 1;
    char fpname[6];  	/* void+X+'/0' */
    uint8_t fplcnt = 0; /* The length counter of the first argument */
    uint8_t pcnt = 0;   /* Parameter counter */
    uint8_t nchar;

    /* Determines whether the function has a return value */
    strtemp = str;

    while (*strtemp != '\0')    /* Do not end */
    {
        if (*strtemp != ' ' && (pcnt & 0X7F) < 5)   /* Record a maximum of 5 characters */
        {
            if (pcnt == 0)pcnt |= 0X80; /* Set to the highest bit, marking the start of receiving the return value type */

            if (((pcnt & 0x7f) == 4) && (*strtemp != '*'))break;    /* The last character, must be" * " */

            fpname[pcnt & 0x7f] = *strtemp; /* Records the return value type of the function */
            pcnt++;
        }
        else if (pcnt == 0X85)
        {
            break;
        }

        strtemp++;
    }

    if (pcnt)   /* received */
    {
        fpname[pcnt & 0x7f] = '\0'; /* Add terminator */

        if (usmart_strcmp(fpname, "void") == 0)
        {
            *rval = 0;  /* No return value is required */
        }
        else
        {
            *rval = 1;  /* Required return value */
        }

        pcnt = 0;
    }

    res = 0;
    strtemp = str;

    while (*strtemp != '(' && *strtemp != '\0')   /* This code finds the true starting position of the function name */
    {
        strtemp++;
        res++;

        if (*strtemp == ' ' || *strtemp == '*')
        {
            nchar = usmart_search_nextc(strtemp);   /* Gets the next character */

            if (nchar != '(' && nchar != '*')offset = res;  /* Skip space and " * " */
        }
    }

    strtemp = str;

    if (offset)strtemp += offset + 1;   /* Skip to the beginning of the function name */

    res = 0;
    nchar = 0;  /* Whether the flag is in the string,0, is not in the string; 1, in the string */

    while (1)
    {
        if (*strtemp == 0)
        {
            res = USMART_FUNCERR;   /* Function error */
            break;
        }
        else if (*strtemp == '(' && nchar == 0)
        {
            fover++;    /* The bracket depth is increased by one level */
        }
        else if (*strtemp == ')' && nchar == 0)
        {
            if (fover)
            {
                fover--;
            }
            else
            {
                res = USMART_FUNCERR;  /* Error ended, did not receive '(' */
            }

            if (fover == 0)break;       /* That's the end，exit */
        }
        else if (*strtemp == '"')
        {
            nchar = !nchar;
        }

        if (fover == 0)   /* Function names haven't been received yet */
        {
            if (*strtemp != ' ')    /* Spaces do not belong to function names */
            {
                *fname = *strtemp;  /* Get function name */
                fname++;
            }
        }
        else     /* Already accepted the function name. */
        {
            if (*strtemp == ',')
            {
                temp = 1;           /* Enable to add a parameter */
                pcnt++;
            }
            else if (*strtemp != ' ' && *strtemp != '(')
            {
                if (pcnt == 0 && fplcnt < 5)    /* When the first parameter comes, in order to avoid counting parameters of type void, you must make a judgment. */
                {
                    fpname[fplcnt] = *strtemp;  /* Record parameter feature. */
                    fplcnt++;
                }

                temp++;     /* Get valid parameters (non-spaces) */
            }

            if (fover == 1 && temp == 2)
            {
                temp++;     /* Prevent duplication */
                parmnum++;  /* Add one parameter */
            }
        }

        strtemp++;
    }

    if (parmnum == 1)       /* Only 1 parameter. */
    {
        fpname[fplcnt] = '\0';  /* Add terminator */

        if (usmart_strcmp(fpname, "void") == 0)parmnum = 0; /* The argument is void, which means there are no arguments. */
    }

    *pnum = parmnum;/* Number of recorded parameters */
    *fname = '\0';  /* Add terminator */
    return res;     /* Return execution result */
}

/**
* @brief 	Takes an argument to a function from str
* @param 	str		: source string pointer
* @param 	fparm	: parameter string pointer
* @param 	ptype	: indicates the parameter type
* @arg 		0, number
* @arg 		1, a character string
* @arg 		0XFF, the parameter is incorrect
* @retval
* @arg 		0, there are no parameters
* @arg 		Other, offset of the next parameter.
*/
uint8_t usmart_get_aparm(char *str, char *fparm, uint8_t *ptype)
{
    uint8_t i = 0;
    uint8_t enout = 0;
    uint8_t type = 0;   /* Default is number */
    uint8_t string = 0; /* Mark whether str is reading */

    while (1)
    {
        if (*str == ',' && string == 0)enout = 1;   /* Suspend immediate exit in order to find the starting address of the next argument */

        if ((*str == ')' || *str == '\0') && string == 0)break; /* Exit the identifier immediately */

        if (type == 0)   /* Default is number */
        {
            /* Digit string detection */
            if ((*str >= '0' && *str <= '9') || *str == '-' || *str == '+' || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F') || *str == 'X' || *str == 'x')
            {
                if (enout)break;    /* Found the next parameter, just exit. */

                if (*str >= 'a')
                {
                    *fparm = *str - 0X20;   /* Convert lowercase to uppercase */
                }
                else
                {
                    *fparm = *str; /* Lowercase or digits remain the same */
                }

                fparm++;
            }
            else if (*str == '"')     /* Find the start flag of the string */
            {
                if (enout)break;    /* Find it, find it later ", thinks it's over. */

                type = 1;
                string = 1;         /* Register STRING is being read */
            }
            else if (*str != ' ' && *str != ',')     /* Invalid character found, parameter error */
            {
                type = 0XFF;
                break;
            }
        }
        else     /* string class */
        {
            if (*str == '"')string = 0;

            if (enout)break;    /* Found the next parameter, just exit. */

            if (string)         /* String reading */
            {
                if (*str == '\\')   /* Escape character encountered (do not copy escape character) */
                {
                    str++;      /* Offset the character after the escape character, no matter what character, straight COPY */
                    i++;
                }

                *fparm = *str;  /* Lowercase or digits remain the same */
                fparm++;
            }
        }

        i++;    /* Offset increase */
        str++;
    }

    *fparm = '\0';  /* Add terminator */
    *ptype = type;  /* Return parameter type */
    return i;       /* Return parameter length */
}

/**
* @brief 	Gets the starting address of the specified parameter
* @param 	num: specifies the num parameter. The value ranges from 0 to 9.
* @retval 	Specifies the start address of this parameter
*/
uint8_t usmart_get_parmpos(uint8_t num)
{
    uint8_t temp = 0;
    uint8_t i;

    for (i = 0; i < num; i++)
    {
        temp += usmart_dev.plentbl[i];
    }

    return temp;
}

/**
* @brief 	Takes function arguments from str
* @param 	str: source string
* @param 	parn: specifies the number of parameters.0 indicates the void type with no parameters
* @retval 	0, successful; Other, error code.
*/
uint8_t usmart_get_fparam(char *str, uint8_t *parn)
{
    uint8_t i, type;
    uint32_t res;
    uint8_t n = 0;
    uint8_t len;
    char tstr[PARM_LEN + 1]; /* A byte-length cache that can hold a string of up to PARM_LEN characters */

    for (i = 0; i < MAX_PARM; i++)
    {
        usmart_dev.plentbl[i] = 0;  /* Clear the parameter length table */
    }

    while (*str != '(')   /* Offset to the start of the parameter */
    {
        str++;

        if (*str == '\0')return USMART_FUNCERR; /* The terminator is encountered */
    }

    str++;  /* Offset to the first byte after "(" */

    while (1)
    {
        i = usmart_get_aparm(str, tstr, &type); /* Get the first parameter */
        str += i;

        switch (type)
        {
            case 0: /* number */
                if (tstr[0] != '\0')    /* The received parameter is valid */
                {
                    i = usmart_str2num(tstr, &res); /* Record the parameter */

                    if (i)return USMART_PARMERR;    /* parameter error. */

                    *(uint32_t *)(usmart_dev.parm + usmart_get_parmpos(n)) = res;   /* Record the result of successful conversion. */
                    usmart_dev.parmtype &= ~(1 << n);   /* Marked digit */
                    usmart_dev.plentbl[n] = 4;  /* The length of this parameter is 4 */
                    n++;    /* Parameter increment */

                    if (n > MAX_PARM)return USMART_PARMOVER;    /* Too many parameters */
                }

                break;

            case 1:/* character string */
                len = usmart_strlen(tstr) + 1;  /* Contains the terminator '\0' */
                usmart_strcopy(tstr, (char *)&usmart_dev.parm[usmart_get_parmpos(n)]);  /* Copy tstr data to usmart dev.parm[n] */
                usmart_dev.parmtype |= 1 << n;  /* Tag string */
                usmart_dev.plentbl[n] = len;    /* The length of this parameter is len */
                n++;

                if (n > MAX_PARM)return USMART_PARMOVER;    /* Too many parameters */

                break;

            case 0XFF:/* error */
                return USMART_PARMERR;  /* parameter error */
        }

        if (*str == ')' || *str == '\0')break;  /* Got the end sign. */
    }

    *parn = n;  /* Records the number of parameters */
    return USMART_OK;   /* The parameters are obtained correctly */
}














