/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_storage_if.h"
/* USER CODE BEGIN INCLUDE */
#include "../../BSP/NORFLASH/norflash.h"
#include "sdio.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/* Filesystem start address in external FLASH
 * Because we specified in the FATFS experiment that FATFS takes up the first 12MB of FLASH space
 * Therefore, the base address is 0, see the instructions of FATFS experiment, diskio.c for details
 */
#define USB_STORAGE_FLASH_BASE  0


/* Self defined a mark USB status register, convenient to judge the status of USB */
volatile uint8_t g_usb_state_reg = 0;

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */

#define STORAGE_LUN_NBR                  2

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_FS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t  STORAGE_Inquirydata[] ={/* 36 */

  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 4),
  0x00,
  0x00,
  0x00,
  /* Vendor Identification */
  'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',/* 9 bytes */
  /* Product Identification */
  'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',/* 15 bytes */
  ' ', 'D', 'i', 's', 'k', ' ',
  /* Product Revision Level */
  '1', '.', '0', ' ',                         /* 4 bytes */


  /* LUN 1 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 4),
  0x00,
  0x00,
  0x00,
  /* Vendor Identification */
  'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',    /* 9 bytes */
  /* Product Identification */
  'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',    /* 15 bytes */
  'D', 'i', 's', 'k', ' ', ' ',
  /* Product Revision Level */
  '1', '.', '0', ' ',                             /* 4 bytes */
};
/* USER CODE END INQUIRY_DATA_FS */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */
 int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */
USBD_StorageTypeDef USBD_DISK_fops =
{
    STORAGE_Init,
    STORAGE_GetCapacity,
    STORAGE_IsReady,
    STORAGE_IsWriteProtected,
    STORAGE_Read,
    STORAGE_Write,
    STORAGE_GetMaxLun,
    (int8_t *)STORAGE_Inquirydata,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init (uint8_t lun)
{
    /* USER CODE BEGIN 2 */
    uint8_t res = 0;

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_init();
            break;

        case 1: /* SD card */
            MX_SDIO_SD_Init();
            break;
    }

    return res;
    /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */

int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    /* USER CODE BEGIN 3 */
    switch (lun)
    {
        case 0: /* SPI FLASH */
            *block_size = 512;
            *block_num = (12 * 1024 * 1024) / 512;  /* SPI FLASH 12MB bytes, file system use */
            break;

        case 1: /* SD卡 */
        	HAL_SD_GetCardInfo(&hsd, &g_sd_card_info);
            *block_size = g_sd_card_info.LogBlockNbr - 1;
			*block_num = g_sd_card_info.LogBlockSize;	//g_sd_card_info.CardCapacity / 512;
            break;
    }
    return (USBD_OK);
    /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{
  /* USER CODE BEGIN 4 */
  g_usb_state_reg |= 0X10;    /* Marked polling */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
	int8_t res = 0;
	g_usb_state_reg |= 0X02;    /* The flag is reading data */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_read(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD card */
            res = sd_read_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
        printf("rerr:%d,%d\r\n", lun, res);
        g_usb_state_reg |= 0X08;    /* Read error */
    }

    return res;
    /* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
	int8_t res = 0;
	g_usb_state_reg |= 0X01;    /* The flag is writing data */

    switch (lun)
    {
        case 0: /* SPI FLASH */
            norflash_write(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            break;

        case 1: /* SD card */
            res = sd_write_disk(buf, blk_addr, blk_len);
            break;
    }

    if (res)
    {
		  g_usb_state_reg |= 0X04;    /* write error */
		  printf("werr:%d,%d\r\n", lun, res);
	  }

	return res;
  /* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun (void)
{
  /* USER CODE BEGIN 8 */
    /* The default is 2. */
	HAL_SD_GetCardInfo(&hsd, &g_sd_card_info);

    if (hsd.SdCard.BlockNbr && hsd.SdCard.BlockSize)    /* If the SD card is normal, 2 disks are supported */
    {
        return STORAGE_LUN_NBR - 1;
    }
    else    /* SD card is not normal, only support 1 disk */
    {
        return STORAGE_LUN_NBR - 2;
    }
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
