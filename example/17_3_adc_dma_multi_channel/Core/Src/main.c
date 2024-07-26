/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       adc_dma_multi_channel code
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M144-STM32F407 board
 * website      : https://www.alientek.com
 * forum        : http://www.openedv.com/forum.php
 *
 * change logs  :
 * version    data    notes
 * V1.0     20240410  the first version
 *
 ****************************************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../SYSTEM/delay/delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

#define ADC_NCH_DMA_BUF_SIZE (50 * 5)
uint16_t g_adc_nch_dma_buf[ADC_NCH_DMA_BUF_SIZE];
extern uint8_t g_adc_nch_dma_sta;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint16_t adc_result[5];
    uint16_t voltage;
    uint16_t index;
    uint32_t result_sum;
    uint8_t ch_index;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  delay_init(168);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_ADC1_Init((uint32_t)g_adc_nch_dma_buf);
  /* USER CODE BEGIN 2 */

  lcd_init();
  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "ADC 5CH DMA TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "ADC1_CH1_VAL:", BLUE);
  lcd_show_string(30, 130, 200, 16, 16, "ADC1_CH1_VOL:0.000V", BLUE);

  lcd_show_string(30, 150, 200, 16, 16, "ADC1_CH2_VAL:", BLUE);
  lcd_show_string(30, 170, 200, 16, 16, "ADC1_CH2_VOL:0.000V", BLUE);

  lcd_show_string(30, 190, 200, 16, 16, "ADC1_CH3_VAL:", BLUE);
  lcd_show_string(30, 210, 200, 16, 16, "ADC1_CH3_VOL:0.000V", BLUE);

  lcd_show_string(30, 230, 200, 16, 16, "ADC1_CH4_VAL:", BLUE);
  lcd_show_string(30, 250, 200, 16, 16, "ADC1_CH4_VOL:0.000V", BLUE);

  lcd_show_string(30, 270, 200, 16, 16, "ADC1_CH5_VAL:", BLUE);
  lcd_show_string(30, 290, 200, 16, 16, "ADC1_CH5_VOL:0.000V", BLUE);

  adc_nch_dma_enable(ADC_NCH_DMA_BUF_SIZE);   /* Turn on the ADC and transmit the result by DMA */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

      if (g_adc_nch_dma_sta == 1)
      {
          g_adc_nch_dma_sta = 0;

          for (ch_index=0; ch_index < 5; ch_index++)
          {
        	  /* Average filtering of multiple ADC data read by DMA */
              for (result_sum = 0, index=0; index<(ADC_NCH_DMA_BUF_SIZE / 5); index++)
              {
                  result_sum += g_adc_nch_dma_buf[(5 * index) + ch_index];
              }
              adc_result[ch_index] = result_sum / (ADC_NCH_DMA_BUF_SIZE / 5);
          }

          voltage = (adc_result[0] * 3300) / 4095;
          lcd_show_xnum(134, 110, adc_result[0], 5, 16, 0, BLUE);
          lcd_show_xnum(134, 130, voltage / 1000, 1, 16, 0, BLUE);
          lcd_show_xnum(150, 130, voltage % 1000, 3, 16, 0x80, BLUE);

          voltage = (adc_result[1] * 3300) / 4095;
          lcd_show_xnum(134, 150, adc_result[1], 5, 16, 0, BLUE);
          lcd_show_xnum(134, 170, voltage / 1000, 1, 16, 0, BLUE);
          lcd_show_xnum(150, 170, voltage % 1000, 3, 16, 0x80, BLUE);

          voltage = (adc_result[2] * 3300) / 4095;
          lcd_show_xnum(134, 190, adc_result[2], 5, 16, 0, BLUE);
          lcd_show_xnum(134, 210, voltage / 1000, 1, 16, 0, BLUE);
          lcd_show_xnum(150, 210, voltage % 1000, 3, 16, 0x80, BLUE);

          voltage = (adc_result[3] * 3300) / 4095;
          lcd_show_xnum(134, 230, adc_result[3], 5, 16, 0, BLUE);
          lcd_show_xnum(134, 250, voltage / 1000, 1, 16, 0, BLUE);
          lcd_show_xnum(150, 250, voltage % 1000, 3, 16, 0x80, BLUE);

          voltage = (adc_result[4] * 3300) / 4095;
          lcd_show_xnum(134, 270, adc_result[4], 5, 16, 0, BLUE);
          lcd_show_xnum(134, 290, voltage / 1000, 1, 16, 0, BLUE);
          lcd_show_xnum(150, 290, voltage % 1000, 3, 16, 0x80, BLUE);

          adc_nch_dma_enable(ADC_NCH_DMA_BUF_SIZE);
      }

      LED0_TOGGLE();

      HAL_Delay(100);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
