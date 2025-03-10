/**
 ****************************************************************************************************
 * @file        stmflash.c
 * @author      ALIENTEK
 * @brief       stmflash code
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

#include "stmflash.h"

/**
 * @brief   reads a single word of data from the specified address
 * @param   addr: Specifies the address from which to read the data
 * @retval  the single word read
 */
uint32_t stmflash_read_word(uint32_t addr)
{
    return (*(volatile uint32_t *)addr);
}

/**
 * @brief   reads the specified word from the specified address
 * @param   addr   : Specifies the address from which to read the data
 * @param   buf    : Store the starting address for reading data
 * @param   length : Specifies the length, in words, of the data to be read
 * @retval  None
 */
void stmflash_read(uint32_t addr, uint32_t *buf, uint32_t length)
{
    uint32_t index;
    
    for (index=0; index<length; index++)
    {
        buf[index] = stmflash_read_word(addr);
        addr += sizeof(uint32_t);
    }
}

/**
 * @brief   writes the specified word of data to the specified address without checking
 * @param   addr: Specifies the address to write the data to
 * @param   buf: The start address of the store to write data to
 * @param   length: This specifies the length of the data to be written in words
 * @retval  None
 */
void stmflash_write_nocheck(uint32_t addr, uint32_t *buf, uint16_t length)
{
    uint16_t index;
    
    HAL_FLASH_Unlock();
    __HAL_FLASH_DATA_CACHE_DISABLE();
    for (index=0; index<length; index++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + (index << 2), buf[index]);
    }
    __HAL_FLASH_DATA_CACHE_ENABLE();
    HAL_FLASH_Lock();
}

/**
 * @brief   获取Flash地址所在的Flash扇区
 * @param   addr: Flash地址
 * @retval  Flash扇区
 */
static uint32_t stmflash_get_flash_sector(uint32_t addr)
{
    if (addr < 0x08004000)
    {
        return FLASH_SECTOR_0;
    }
    else if (addr < 0x08008000)
    {
        return FLASH_SECTOR_1;
    }
    else if (addr < 0x0800C000)
    {
        return FLASH_SECTOR_2;
    }
    else if (addr < 0x08010000)
    {
        return FLASH_SECTOR_3;
    }
    else if (addr < 0x08020000)
    {
        return FLASH_SECTOR_4;
    }
    else if (addr < 0x08040000)
    {
        return FLASH_SECTOR_5;
    }
    else if (addr < 0x08060000)
    {
        return FLASH_SECTOR_6;
    }
    else if (addr < 0x08080000)
    {
        return FLASH_SECTOR_7;
    }
    else if (addr < 0x080A0000)
    {
        return FLASH_SECTOR_8;
    }
    else if (addr < 0x080C0000)
    {
        return FLASH_SECTOR_9;
    }
    else if (addr < 0x080E0000)
    {
        return FLASH_SECTOR_10;
    }
    else
    {
        return FLASH_SECTOR_11;
    }
}


/**
 * @brief   往指定地址写入指定字的数据
 * @param   addr: 指定写入数据的地址
 * @param   buf: 存储写入数据的起始地址
 * @param   length: 指定写入数据的长度，单位：字
 * @retval  无
 */
void stmflash_write(uint32_t addr, uint32_t *buf, uint16_t length)
{
    uint32_t addrx;
    uint32_t endaddr;
    FLASH_EraseInitTypeDef flash_erase_init_struct = {0};
    uint32_t sectorerr;
    HAL_StatusTypeDef status = HAL_OK;
    
    /* 检查写入地址范围的合法性 */
    if ((!IS_FLASH_ADDRESS(addr)) ||
        (!IS_FLASH_ADDRESS(addr + (length * sizeof(uint32_t)) - 1)) ||
        ((addr & 3) != 0))
    {
        return;
    }
    
    addrx = addr;
    endaddr = addr + (length << 2);
    if (addrx <= FLASH_END)
    {
        while (addrx < endaddr)
        {
            /* 判断是否需要擦除 */
            if (stmflash_read_word(addrx) != 0xFFFFFFFF)
            {
                /* 擦除扇区 */
                flash_erase_init_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
                flash_erase_init_struct.Banks = FLASH_BANK_1;
                flash_erase_init_struct.Sector = stmflash_get_flash_sector(addrx);
                flash_erase_init_struct.NbSectors = 1;
                flash_erase_init_struct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
                HAL_FLASH_Unlock();
                status = HAL_FLASHEx_Erase(&flash_erase_init_struct, &sectorerr);
                HAL_FLASH_Lock();
                if (status != HAL_OK)
                {
                    break;
                }
            }
            else
            {
                addrx += sizeof(uint32_t);
            }
        }
    }

    if (status == HAL_OK)
    {
        while (addr < endaddr)
        {
            /* 写入数据 */
            stmflash_write_nocheck(addr, buf, 1);
            addr += sizeof(uint32_t);
            buf++;
        }
    }
}
