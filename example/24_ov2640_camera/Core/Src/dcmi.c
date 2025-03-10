/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dcmi.c
  * @brief   This file provides code for the configuration
  *          of the DCMI instances.
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
/* Includes ------------------------------------------------------------------*/
#include "dcmi.h"

/* USER CODE BEGIN 0 */

#include "../../BSP/LED/led.h"
#include "../../BSP/LCD/lcd.h"

uint8_t g_ov_frame = 0;                     /* 帧率 */
extern void jpeg_data_process(void);        /* JPEG数据处理函数 */

/* USER CODE END 0 */

DCMI_HandleTypeDef g_dcmi_handle;
DMA_HandleTypeDef hdma_dcmi;

/* DCMI init function */
void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  g_dcmi_handle.Instance = DCMI;
  g_dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  g_dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  g_dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
  g_dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  g_dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  g_dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  g_dcmi_handle.Init.JPEGMode = DCMI_JPEG_DISABLE;
  if (HAL_DCMI_Init(&g_dcmi_handle) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  DCMI->IER = 0x0;

  __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);        /* 使能帧中�? */
  __HAL_DCMI_ENABLE(&g_dcmi_handle);                          /* 使能DCMI */

  /* USER CODE END DCMI_Init 2 */

}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* dcmiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(dcmiHandle->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspInit 0 */

  /* USER CODE END DCMI_MspInit 0 */
    /* DCMI clock enable */
    __HAL_RCC_DCMI_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**DCMI GPIO Configuration
    PE5     ------> DCMI_D6
    PE6     ------> DCMI_D7
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PC6     ------> DCMI_D0
    PC7     ------> DCMI_D1
    PC8     ------> DCMI_D2
    PC9     ------> DCMI_D3
    PC11     ------> DCMI_D4
    PB6     ------> DCMI_D5
    PB7     ------> DCMI_VSYNC
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN DCMI_MspInit 1 */

    /* DCMI interrupt Init */
    HAL_NVIC_SetPriority(DCMI_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);

  /* USER CODE END DCMI_MspInit 1 */
  }
}

void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* dcmiHandle)
{

  if(dcmiHandle->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspDeInit 0 */

  /* USER CODE END DCMI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_CLK_DISABLE();

    /**DCMI GPIO Configuration
    PE5     ------> DCMI_D6
    PE6     ------> DCMI_D7
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PC6     ------> DCMI_D0
    PC7     ------> DCMI_D1
    PC8     ------> DCMI_D2
    PC9     ------> DCMI_D3
    PC11     ------> DCMI_D4
    PB6     ------> DCMI_D5
    PB7     ------> DCMI_VSYNC
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* DCMI DMA DeInit */
    HAL_DMA_DeInit(dcmiHandle->DMA_Handle);

    /* DCMI interrupt Deinit */
    HAL_NVIC_DisableIRQ(DCMI_IRQn);
  /* USER CODE BEGIN DCMI_MspDeInit 1 */

  /* USER CODE END DCMI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void dcmi_dma_init(uint32_t mem0addr, uint32_t mem1addr, uint16_t memsize, uint32_t memblen, uint32_t meminc)
{
    /* DCMI DMA Init */
    /* DCMI Init */
    hdma_dcmi.Instance = DMA2_Stream1;
    hdma_dcmi.Init.Channel = DMA_CHANNEL_1;
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dcmi.Init.MemInc = meminc;
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dcmi.Init.MemDataAlignment = memblen;
    hdma_dcmi.Init.Mode = DMA_CIRCULAR;
    hdma_dcmi.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_dcmi) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&g_dcmi_handle,DMA_Handle,hdma_dcmi);

    if (mem1addr == 0)
    {
        HAL_DMA_Start(&hdma_dcmi, (uint32_t)&DCMI->DR, mem0addr, memsize);
    }
    else
    {
        HAL_DMAEx_MultiBufferStart(&hdma_dcmi, (uint32_t)&DCMI->DR, mem0addr, mem1addr, memsize);
        __HAL_DMA_ENABLE_IT(&hdma_dcmi, DMA_IT_TC);
    }
}

/**
 * @brief   Start the DCMI transfer
 * @param   None
 * @retval  None
 */
void dcmi_start(void)
{
    lcd_set_cursor(0, 0);
    lcd_write_ram_prepare();
    __HAL_DMA_ENABLE(&hdma_dcmi);   		/* Enable DMA */
    DCMI->CR |= DCMI_CR_CAPTURE;            /* DCMI capture is enabled */
}

/**
 * @brief   Stop the DCMI transfer
 * @param   None
 * @retval  None
 */
void dcmi_stop(void)
{
    DCMI->CR &= ~(DCMI_CR_CAPTURE);         /* Disable DCMI capture */
    while (DCMI->CR & 0X01);                /* Wait for transfer to end */
    __HAL_DMA_DISABLE(&hdma_dcmi);          /* Disable DMA */
}

/**
 * @brief   DCMI中断回调服务函数
 * @param   g_dcmi_handle: DCMI句柄
 * @retval  None
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    __HAL_DCMI_CLEAR_FLAG(&g_dcmi_handle, DCMI_FLAG_FRAMERI);	/* Clears frame interrupt */
    jpeg_data_process();    /* jpeg data processing */
    LED1_TOGGLE();          /* LED1 blinking */
    g_ov_frame++;

    __HAL_DCMI_ENABLE_IT(&g_dcmi_handle, DCMI_IT_FRAME);
}

/* DCMI DMA receive callback function, only used in double buffering mode, used in conjunction with interrupt service function */
void (*dcmi_rx_callback)(void);

/**
 * @brief   DMA2 Stream1 interrupt service function
 * @param   None
 * @retval  None
 */
void DMA2_Stream1_IRQHandler(void)
{
    if (__HAL_DMA_GET_FLAG(&hdma_dcmi, DMA_FLAG_TCIF1_5) != RESET)  /* DMA transfer complete */
    {
        __HAL_DMA_CLEAR_FLAG(&hdma_dcmi, DMA_FLAG_TCIF1_5);/* Clear the DMA transfer complete interrupt flag */
        dcmi_rx_callback();     /* Perform camera receive callback function, read data and other operations in this processing */
    }
}

/* USER CODE END 1 */
