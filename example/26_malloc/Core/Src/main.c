/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       MALLOC code
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
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../SYSTEM/delay/delay.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"
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
    uint8_t t = 0;
    uint8_t key;
    uint8_t *p_sramin = NULL;
    uint8_t *p_sramccm = NULL;
    uint8_t *p_sramex = NULL;
    uint32_t tp_sramin = 0;
    uint32_t tp_sramccm = 0;
    uint32_t tp_sramex = 0;
    uint8_t paddr[32];
    uint16_t memused;
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
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  my_mem_init(SRAMCCM);               /* Initialize the CCM memory pool */
  my_mem_init(SRAMEX);                /* Initialize the external SRAM memory pool */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "MALLOC TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "KEY0:Malloc & WR & Show", RED);
  lcd_show_string(30, 130, 200, 16, 16, "KEY_UP:Free", RED);

  lcd_show_string(30, 162, 200, 16, 16, "SRAMIN USED:", BLUE);
  lcd_show_string(30, 178, 200, 16, 16, "SRAMCCM USED:", BLUE);
  lcd_show_string(30, 194, 200, 16, 16, "SRAMEX USED:", BLUE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

      key = key_scan(0);
      if (key == WKUP_PRES)
      {
          /* Apply for memory */
          p_sramin = mymalloc(SRAMIN, 2048);
          p_sramccm = mymalloc(SRAMCCM, 2048);
          p_sramex = mymalloc(SRAMEX, 2048);

          /* Check whether memory application is successful */
          if ((p_sramin != NULL) && (p_sramccm != NULL) && (p_sramex != NULL))
          {
              /* Uses the allocated memory */
              sprintf((char *)p_sramin, "SRAMIN: Malloc Test%03d", t + SRAMIN);
              lcd_show_string(30, 258, 239, 16, 16, (char *)p_sramin, BLUE);
              sprintf((char *)p_sramccm, "SRAMCCM: Malloc Test%03d", t + SRAMCCM);
              lcd_show_string(30, 274, 239, 16, 16, (char *)p_sramccm, BLUE);
              sprintf((char *)p_sramex, "SRAMEX: Malloc Test%03d", t + SRAMEX);
              lcd_show_string(30, 290, 239, 16, 16, (char *)p_sramex, BLUE);
          }
          else
          {
              myfree(SRAMIN, p_sramin);
              myfree(SRAMCCM, p_sramccm);
              myfree(SRAMEX, p_sramex);
              p_sramin = NULL;
              p_sramccm = NULL;
              p_sramex = NULL;
          }
      }
      else if (key == KEY0_PRES)
      {
          /* Release memory */
          myfree(SRAMIN, p_sramin);
          myfree(SRAMCCM, p_sramccm);
          myfree(SRAMEX, p_sramex);
          p_sramin = NULL;
          p_sramccm = NULL;
          p_sramex = NULL;
      }

      /* Displays the first address applied to memory */
      if ((tp_sramin != (uint32_t)p_sramin) || (tp_sramccm != (uint32_t)p_sramccm) || (tp_sramex != (uint32_t)p_sramex))
      {
          tp_sramin = (uint32_t)p_sramin;
          tp_sramccm = (uint32_t)p_sramccm;
          tp_sramex = (uint32_t)p_sramex;

          sprintf((char *)paddr, "SRAMIN: Addr: 0x%08X", (uint32_t)p_sramin);
          lcd_show_string(30, 210, 239, 16, 16, (char *)paddr, BLUE);
          sprintf((char *)paddr, "SRAMCCN: Addr: 0x%08X", (uint32_t)p_sramccm);
          lcd_show_string(30, 226, 239, 16, 16, (char *)paddr, BLUE);
          sprintf((char *)paddr, "SRAMEX: Addr: 0x%08X", (uint32_t)p_sramex);
          lcd_show_string(30, 242, 239, 16, 16, (char *)paddr, BLUE);
      }
      else if ((p_sramin == NULL) || (p_sramccm == NULL) || (p_sramex == NULL))
      {
          lcd_fill(30, 210, 239, 319, WHITE);
      }

      if (++t == 20)
      {
          t = 0;

          /* Displays internal SRAM usage */
          memused = my_mem_perused(SRAMIN);
          sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
          lcd_show_string(30 + 96, 162, 200, 16, 16, (char *)paddr, BLUE);

          /* Displays CCM usage */
          memused = my_mem_perused(SRAMCCM);
          sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
          lcd_show_string(30 + 104, 178, 200, 16, 16, (char *)paddr, BLUE);

          /* Displays external SRAM usage */
          memused = my_mem_perused(SRAMEX);
          sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
          lcd_show_string(30 + 96, 194, 200, 16, 16, (char *)paddr, BLUE);

          LED0_TOGGLE();
      }

      HAL_Delay(10);

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
