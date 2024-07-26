/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : FSMC.h
  * Description        : This file provides code for the configuration
  *                      of the FSMC peripheral.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FSMC_H
#define __FSMC_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SRAM_HandleTypeDef hsram1;
extern SRAM_HandleTypeDef hsram2;

/* USER CODE BEGIN Private defines */

/* FSMC相关参数定义
 * 默认通过FSMC的存储块1来连接外部SRAM，存储块1有4个片选（SMC_NE1~4）对应FSMC存储块1的4个区域块
 */
#define SRAM_FSMC_NEX               3   /* 使用FSMC_NE3连接SRAM_CS，范围：1~4 */

/* 外部SRAM基地址的详细计算方法：
 * 一般使用FSMC的存储块1来驱动TFTLCD液晶屏（MCU屏），存储块1地址范围总大小为256MB，均分成4区域块：
 * 存储块1区域块1（FSMC_NE1）地址范围：0x60000000~0x63FFFFFF
 * 存储块1区域块2（FSMC_NE2）地址范围：0x64000000~0x67FFFFFF
 * 存储块1区域块3（FSMC_NE3）地址范围：0x68000000~0x6BFFFFFF
 * 存储块1区域块4（FSMC_NE4）地址范围：0x6C000000~0x6FFFFFFF
 */
#define SRAM_BASE_ADDR              (uint32_t)(0x60000000 + (0x4000000 * (SRAM_FSMC_NEX - 1)))

/* USER CODE END Private defines */

void MX_FSMC_Init(void);
void HAL_SRAM_MspInit(SRAM_HandleTypeDef* hsram);
void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef* hsram);

/* USER CODE BEGIN Prototypes */

void sram_read(uint8_t *pbuf, uint32_t addr, uint32_t datalen);     /* 外部SRAM读数据 */
void sram_write(uint8_t *pbuf, uint32_t addr, uint32_t datalen);    /* 外部SRAM写数据 */
uint8_t sram_test_read(uint32_t addr);                              /* 外部SRAM读测试 */
void sram_test_write(uint32_t addr, uint8_t data);                  /* 外部SRAM写测试 */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__FSMC_H */

/**
  * @}
  */

/**
  * @}
  */
