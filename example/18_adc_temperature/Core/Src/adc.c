/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief   Set up ADC channel
 * @param   adc_handle: ADC handle
 * @param   channel: ADC channel
 * @param   rank: Rank of regular conversion
 * @param   sampling_time: Sampling time
 * @retval  None
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t channel, uint32_t rank, uint32_t sampling_time)
{
    ADC_ChannelConfTypeDef adc_channel_conf_struct = {0};

    /* Configure ADC channel */
    adc_channel_conf_struct.Channel = channel;
    adc_channel_conf_struct.Rank = rank;
    adc_channel_conf_struct.SamplingTime = sampling_time;
    adc_channel_conf_struct.Offset = 0;
    HAL_ADC_ConfigChannel(adc_handle, &adc_channel_conf_struct);
}


/**
 * @brief   Get ADC result
 * @param   channel: ADC channel
 * @retval  ADC result
 */
uint16_t adc_get_result(uint32_t channel)
{
    uint16_t result;

    adc_channel_set(&hadc1, channel, 1, ADC_SAMPLETIME_480CYCLES);   /* Set up ADC channel */
    HAL_ADC_Start(&hadc1);                                           /* Start ADC */
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);                /* Poll for ADC conversion */
    result = HAL_ADC_GetValue(&hadc1);                               /* Get ADC value */

    return result;
}

/**
 * @brief   Get ADC result using moving average filtering
 * @param   channel: ADC channel
 * @param   times: Number of raw data for moving average filtering
 * @retval  ADC result
 */
uint16_t adc_get_result_average(uint32_t channel, uint8_t times)
{
    uint32_t sum_result = 0;
    uint8_t index;
    uint16_t result;

    for (index = 0; index < times; index++)
    {
        sum_result += adc_get_result(channel);
    }

    result = sum_result / times;

    return result;
}

/**
 * @brief   Get internal temperature sensor result
 * @param   None
 * @retval  Internal temperature sensor result (multiplied by 100)
 */
int16_t adc_get_temperature(void)
{
    uint16_t result;
    double voltage;
    double temperature;
    int16_t temperature_x100;

    result = adc_get_result_average(ADC_CHANNEL_TEMPSENSOR, 10);
    voltage = ((double)result * 3.3) / 4095;
    temperature = (voltage - 0.76) * 400 + 25;
    temperature_x100 = (int16_t)(temperature * 100);

    return temperature_x100;
}

/* USER CODE END 1 */
