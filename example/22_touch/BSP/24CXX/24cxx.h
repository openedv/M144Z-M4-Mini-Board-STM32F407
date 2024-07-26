/**
 ****************************************************************************************************
 * @file        24cxx.h
 * @author      ALIENTEK
 * @brief       24cxx code
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

#ifndef BSP_24CXX_24CXX_H_
#define BSP_24CXX_24CXX_H_

#include "main.h"

#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767


#define EE_TYPE     AT24C02

void at24cxx_init(void);
uint8_t at24cxx_check(void);
uint8_t at24cxx_read_one_byte(uint16_t addr);
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen);
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);


#endif /* 24CXX_24CXX_H_ */
