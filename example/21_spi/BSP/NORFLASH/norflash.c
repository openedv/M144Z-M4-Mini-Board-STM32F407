/**
 ****************************************************************************************************
 * @file        norflash.c
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

#include "spi.h"
#include "../../SYSTEM/delay/delay.h"
#include "usart.h"
#include "norflash.h"


/* Static functions */
static void norflash_wait_busy(void);                    /* Wait for idle */
static void norflash_send_address(uint32_t address);     /* Send address */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen);    /* Write page */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen); /* Write to flash without erase */

uint16_t g_norflash_type = NM25Q128;                     /* Default is NM25Q128 */


/**
 * @brief       Initialize SPI NOR FLASH
 * @param       None
 * @retval      None
 */
void norflash_init(void)
{
    uint8_t temp;

    NORFLASH_CS(1);                     /* Deselect the chip */

    MX_SPI1_Init();                   	/* Initialize spi1 */
    spi1_set_speed(SPI_BAUDRATEPRESCALER_4);/* Switch spi1 to high-speed mode 18MHz */
    
    g_norflash_type = norflash_read_id();   /* Read FLASH ID */
    printf("ID:%x\r\n", g_norflash_type);
    if (g_norflash_type == W25Q256)     /* If SPI FLASH is W25Q256, 4-byte address mode must be enabled */
    {
        temp = norflash_read_sr(3);     /* Read status register 3 to determine address mode */

        if ((temp & 0X01) == 0)         /* If not in 4-byte address mode, switch to 4-byte address mode */
        {
            norflash_write_enable();    /* Enable write */
            temp |= 1 << 1;             /* ADP=1, enable 4-byte address mode */
            norflash_write_sr(3, temp); /* Write to SR3 */
            
            NORFLASH_CS(0);
            spi1_read_write_byte(FLASH_Enable4ByteAddr);    /* Enable 4-byte address instruction */
            NORFLASH_CS(1);
        }
    }
}


/**
 * @brief       Wait for the flash to become idle
 * @param       None
 * @retval      None
 */
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /* Wait for the BUSY bit to clear */
}

/**
 * @brief       Enable write operation for 25QXX
 *   @note      Set the WEL bit in the status register S1
 * @param       None
 * @retval      None
 */
void norflash_write_enable(void)
{
    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_WriteEnable);   /* Send write enable command */
    NORFLASH_CS(1);
}

/**
 * @brief       Send address for 25QXX
 *   @note      Send 24-bit or 32-bit address depending on the chip type
 * @param       address : Address to send
 * @retval      None
 */
static void norflash_send_address(uint32_t address)
{
    if (g_norflash_type == W25Q256) /* Only W25Q256 supports 4-byte address mode */
    {
        spi1_read_write_byte((uint8_t)((address) >> 24)); /* Send bits 31 to 24 of the address */
    } 
    spi1_read_write_byte((uint8_t)((address) >> 16));     /* Send bits 23 to 16 of the address */
    spi1_read_write_byte((uint8_t)((address) >> 8));      /* Send bits 15 to 8 of the address */
    spi1_read_write_byte((uint8_t)address);               /* Send bits 7 to 0 of the address */
}

/**
* @brief 		Reads the status register of 25QXX. There are 3 status registers in 25QXX
* @note 		Status Register 1:
*              	BIT   7  6   5   4   3   2   1   0
*              	SPR   RV  TB BP2 BP1 BP0 WEL BUSY
* 				SPR: Default 0, status register protection bit, used with WP
* 				TB,BP2,BP1,BP0: indicates the write protection Settings of the FLASH area
* 				WEL: Write enable lock
* 				BUSY: indicates the busy flag bit (1, busy; 0, idle)
* 				Default :0x00
*
* 				Status 	Register 2:
*              	BIT7  6   5   4   3   2   1   0
*              	SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
*
* 				Status 	Register 3:
*              	BIT7      6    5    4   3   2   1   0
*              	HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
*
* @param 		regno: indicates the status register number. The value ranges from 1 to 3
* @retval 		Status register value
*/
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte;
    uint8_t command;

    switch (regno)
    {
        case 1:
        {
            command = FLASH_ReadStatusReg1;  /* Command to read status register 1 */
            break;
        }
        case 2:
        {
            command = FLASH_ReadStatusReg2;  /* Command to read status register 2 */
            break;
        }
        case 3:
        {
            command = FLASH_ReadStatusReg3;  /* Command to read status register 3 */
            break;
        }
        default:
        {
            command = FLASH_ReadStatusReg1;  /* Default command to read status register 1 */
            break;
        }
    }
    
    NORFLASH_CS(0);                             /* Enable NOR Flash chip select */
    spi1_read_write_byte(command);              /* Send command to read status register */
    byte = spi1_read_write_byte(0xFF);          /* Read one byte */
    NORFLASH_CS(1);                             /* Disable NOR Flash chip select */

    return byte;
}


/**
 * @brief       Write to the 25QXX status register
 * @note        See the norflash_read_sr function for register details
 * @param       regno: Status register number, range: 1~3
 * @param       sr   : Value to write to the status register
 * @retval      None
 */
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1;  /* Command to write to status register 1 */
            break;

        case 2:
            command = FLASH_WriteStatusReg2;  /* Command to write to status register 2 */
            break;

        case 3:
            command = FLASH_WriteStatusReg3;  /* Command to write to status register 3 */
            break;

        default:
            command = FLASH_WriteStatusReg1;
            break;
    }

    NORFLASH_CS(0);
    spi1_read_write_byte(command);  /* Send command to read status register */
    spi1_read_write_byte(sr);       /* Write one byte */
    NORFLASH_CS(1);
}


/**
 * @brief       Read the chip ID
 * @param       None
 * @retval      FLASH chip ID
 * @note        See chip ID list in norflash.h, Chip List section
 */
uint16_t norflash_read_id(void)
{
    uint16_t deviceid;

    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_ManufactDeviceID);   /* Send read ID command */
    spi1_read_write_byte(0);                        /* Write one byte */
    spi1_read_write_byte(0);
    spi1_read_write_byte(0);
    deviceid = spi1_read_write_byte(0xFF) << 8;     /* Read high 8-bit byte */
    deviceid |= spi1_read_write_byte(0xFF);         /* Read low 8-bit byte */
    NORFLASH_CS(1);

    return deviceid;
}


/**
 * @brief       Read from SPI FLASH
 * @note        Read specified length of data starting from the specified address
 * @param       pbuf    : Data storage area
 * @param       addr    : Starting address to read from (up to 32 bits)
 * @param       datalen : Number of bytes to read (up to 65535)
 * @retval      None
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_ReadData);       /* Send read command */
    norflash_send_address(addr);                /* Send address */
    
    for(i = 0; i < datalen; i++)
    {
        pbuf[i] = spi1_read_write_byte(0xFF);   /* Read in a loop */
    }
    
    NORFLASH_CS(1);
}


/**
 * @brief       Write less than 256 bytes of data within a page (0~65535) via SPI
 * @note        Write up to 256 bytes of data starting from the specified address
 * @param       pbuf    : Data storage area
 * @param       addr    : Starting address to write to (up to 32 bits)
 * @param       datalen : Number of bytes to write (up to 256), this number should not exceed the remaining bytes of the page!!!
 * @retval      None
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    norflash_write_enable();                    /* Enable write */

    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_PageProgram);    /* Send write page command */
    norflash_send_address(addr);                /* Send address */

    for(i = 0; i < datalen; i++)
    {
        spi1_read_write_byte(pbuf[i]);          /* Write in a loop */
    }
    
    NORFLASH_CS(1);
    norflash_wait_busy();                       /* Wait for write to finish */
}


/**
 * @brief       Unverified write to SPI FLASH
 * @note        Ensure that all data within the address range to be written is 0xFF, otherwise data written at non-0xFF locations will fail!
 *              With automatic page swapping feature
 *              Write data of specified length starting from specified address, but ensure address does not exceed boundaries!
 *
 * @param       pbuf    : Data storage area
 * @param       addr    : Starting address to write to (up to 32 bits)
 * @param       datalen : Number of bytes to write (up to 65535)
 * @retval      None
 */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* Number of bytes remaining in the page */

    if (datalen <= pageremain)      /* Not more than 256 bytes */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* If the number of bytes to be written is less than the remaining bytes in the page, write all at once
         * If the number of bytes to be written is more than the remaining bytes in the page, write the entire remaining bytes first, then handle differently based on the remaining length
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* Writing is complete */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* Offset the pbuf pointer address, already written pageremain bytes */
            addr += pageremain;         /* Offset the write address, already written pageremain bytes */
            datalen -= pageremain;      /* Subtract the number of bytes already written from the total length to be written */

            if (datalen > 256)          /* There is still more data than a page, can write one page at a time */
            {
                pageremain = 256;       /* Can write 256 bytes at once */
            }
            else     /* Remaining data is less than a page, can write all at once */
            {
                pageremain = datalen;   /* Less than 256 bytes */
            }
        }
    }
}

/* Sector cache */
static uint8_t g_norflash_buf[4096];

/**
 * @brief       Write to SPI FLASH
 * @note        Write data of specified length starting from specified address, with erase operation!
 *              SPI FLASH is typically organized as follows: 256 bytes per Page, 4 Kbytes per Sector, 16 sectors per Block
 *              Erasure occurs at the minimum unit of a Sector.
 *
 * @param       pbuf    : Data storage area
 * @param       addr    : Starting address to write to (up to 32 bits)
 * @param       datalen : Number of bytes to write (up to 65535)
 * @retval      None
 */
void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* Sector address */
    secoff = addr % 4096;       /* Offset within the sector */
    secremain = 4096 - secoff;  /* Remaining space within the sector */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* For testing */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* Not more than 4096 bytes */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* Read the entire sector */

        for (i = 0; i < secremain; i++)   /* Check data */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* Erase needed, exit the for loop */
            }
        }

        if (i < secremain)   /* Erase required */
        {
            norflash_erase_sector(secpos);  /* Erase this sector */

            for (i = 0; i < secremain; i++)   /* Copy */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* Write the entire sector */
        }
        else        /* Write to already erased sector, directly write to remaining area of the sector */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* Directly write to the sector */
        }

        if (datalen == secremain)
        {
            break;  /* Writing completed */
        }
        else        /* Writing not completed */
        {
            secpos++;               /* Increment sector address */
            secoff = 0;             /* Offset position is 0 */

            pbuf += secremain;      /* Pointer offset */
            addr += secremain;      /* Increment write address */
            datalen -= secremain;   /* Decrement the number of bytes */

            if (datalen > 4096)
            {
                secremain = 4096;   /* Next sector still not enough to write */
            }
            else
            {
                secremain = datalen;/* Next sector can be fully written */
            }
        }
    }
}

/**
 * @brief       Erase the entire chip
 *   @note      Long waiting time...
 * @param       None
 * @retval      None
 */
void norflash_erase_chip(void)
{
    norflash_write_enable();    			/* Enable write */
    norflash_wait_busy();       			/* Wait for idle */
    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_ChipErase);  /* Send chip erase command */
    NORFLASH_CS(1);
    norflash_wait_busy();       			/* Wait for chip erase to complete */
}

/**
 * @brief       Erase a sector
 * @note        Note: This is sector address, not byte address!!
 *              Minimum time to erase a sector: 150ms
 *
 * @param       saddr: Sector address (set according to actual capacity)
 * @retval      None
 */
void norflash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* Monitor flash erase status, for testing */
    saddr *= 4096;
    norflash_write_enable();        /* Enable write */
    norflash_wait_busy();           /* Wait for idle */

    NORFLASH_CS(0);
    spi1_read_write_byte(FLASH_SectorErase);    /* Send sector erase command */
    norflash_send_address(saddr);   /* Send address */
    NORFLASH_CS(1);
    norflash_wait_busy();           /* Wait for sector erase to complete */
}
