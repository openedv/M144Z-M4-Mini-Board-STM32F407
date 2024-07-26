/**
 ****************************************************************************************************
 * @file        norflash.h
 * @author      ALIENTEK
 * @brief       This file provides the driver for the norflash
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

#ifndef __norflash_H
#define __norflash_H

#include "main.h"


/******************************************************************************************/
/* NORFLASH chip select signal */

#define NORFLASH_CS(x)      do{ x ? \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_Port, NORFLASH_CS_Pin, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_Port, NORFLASH_CS_Pin, GPIO_PIN_RESET); \
                            }while(0)

/* FLASHоƬ�б� */
#define W25Q80      0XEF13          /* W25Q80   ID */
#define W25Q16      0XEF14          /* W25Q16   ID */
#define W25Q32      0XEF15          /* W25Q32   ID */
#define W25Q64      0XEF16          /* W25Q64   ID */
#define W25Q128     0XEF17          /* W25Q128  ID */
#define W25Q256     0XEF18          /* W25Q256  ID */
#define BY25Q64     0X6816          /* BY25Q64  ID */
#define BY25Q128    0X6817          /* BY25Q128 ID */
#define NM25Q64     0X5216          /* NM25Q64  ID */
#define NM25Q128    0X5217          /* NM25Q128 ID */

extern uint16_t norflash_TYPE;      /* Define FLASH chip model */

/* NOR Flash directive definition */
#define FLASH_WriteEnable           0x06
#define FLASH_WriteDisable          0x04
#define FLASH_ReadStatusReg1        0x05
#define FLASH_ReadStatusReg2        0x35
#define FLASH_ReadStatusReg3        0x15
#define FLASH_WriteStatusReg1       0x01
#define FLASH_WriteStatusReg2       0x31
#define FLASH_WriteStatusReg3       0x11
#define FLASH_ReadData              0x03
#define FLASH_FastReadData          0x0B
#define FLASH_FastReadDual          0x3B
#define FLASH_FastReadQuad          0xEB
#define FLASH_PageProgram           0x02
#define FLASH_PageProgramQuad       0x32
#define FLASH_BlockErase            0xD8
#define FLASH_SectorErase           0x20
#define FLASH_ChipErase             0xC7
#define FLASH_PowerDown             0xB9
#define FLASH_ReleasePowerDown      0xAB
#define FLASH_DeviceID              0xAB
#define FLASH_ManufactDeviceID      0x90
#define FLASH_JedecDeviceID         0x9F
#define FLASH_Enable4ByteAddr       0xB7
#define FLASH_Exit4ByteAddr         0xE9
#define FLASH_SetReadParam          0xC0
#define FLASH_EnterQPIMode          0x38
#define FLASH_ExitQPIMode           0xFF

/* Ordinary functions */
void norflash_init(void);                            					/* Initialize 25QXX */
uint16_t norflash_read_id(void);                     					/* Read FLASH ID */
void norflash_write_enable(void);                    					/* Enable write */
uint8_t norflash_read_sr(uint8_t regno);             					/* Read status register */
void norflash_write_sr(uint8_t regno, uint8_t sr);   					/* Write status register */
void norflash_erase_chip(void);                      					/* Erase entire chip */
void norflash_erase_sector(uint32_t saddr);          					/* Erase sector */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);    	/* Read flash */
void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);   	/* Write to flash */

#endif
