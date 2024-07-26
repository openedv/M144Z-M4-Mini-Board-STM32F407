/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       USB_Card_Reader code
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
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_msc.h"
#include "usbd_desc.h"
#include "usb_device.h"
#include "usbd_storage_if.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../SYSTEM/delay/delay.h"
#include "../../BSP/NORFLASH/norflash.h"
#include "../../ATK_Middlewares/MALLOC/malloc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


extern uint8_t g_sd_error;
extern uint8_t g_sd_sta;
extern volatile uint8_t g_usb_state_reg;    /* USB status */
extern volatile uint8_t g_device_state;     /* USB connection condition */

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
    uint16_t norflash_id;
    uint8_t offline_cnt = 0;
    uint8_t t = 0;
    uint8_t usb_sta;
    uint8_t device_sta;
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
  MX_TIM4_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();

  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  my_mem_init(SRAMCCM);				  /* Initialize the internal SRAMCCM memory pool */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "USB MSC TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  if (sd_init() != 0)
  {

      lcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
  }
  else
  {
      lcd_show_string(30, 110, 200, 16, 16, "SD Card Size:     MB", RED);
      lcd_show_num(134, 110, SD_TOTAL_SIZE_MB(&hsd), 5, 16, RED);
  }

  /* Detects NOR Flash */
  norflash_id = norflash_read_id();
  if ((norflash_id == 0x0000) || (norflash_id == 0xFFFF))
  {
      lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Error!", RED);
  }
  else
  {
      lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Size:12MB", RED);
  }

  lcd_show_string(30, 150, 200, 16, 16, "USB Connecting...", RED);    			/* Indicates that a connection is being established */
  USBD_Init(&hUsbDeviceFS, &FS_Desc, 0);                              			/* Initializes USB */
  USBD_RegisterClass(&hUsbDeviceFS, USBD_MSC_CLASS);                  			/* Adds class */
  USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_DISK_fops);     				/* Add a callback function to the MSC class */
  USBD_Start(&hUsbDeviceFS);                                           			/* Enable USB */
  HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
      HAL_Delay(1);

      /* Status changed */
      if (usb_sta != g_usb_state_reg)
      {
          lcd_fill(30, 170, 240, 186, WHITE);

          /* Writing */
          if (g_usb_state_reg & 0x01)
          {
              LED1(0);
              lcd_show_string(30, 170, 200, 16, 16, "USB Writing...", RED);
          }

          /* Reading */
          if (g_usb_state_reg & 0x02)
          {
              LED1(0);
              lcd_show_string(30, 170, 200, 16, 16, "USB Reading...", RED);
          }

          /* Write error */
          if (g_usb_state_reg & 0x04)
          {
              lcd_show_string(30, 190, 200, 16, 16, "USB Write Err ", RED);
          }
          else
          {
              lcd_fill(30, 190, 240, 206, WHITE);
          }

          /* Read error */
          if (g_usb_state_reg & 0x08)
          {
              lcd_show_string(30, 210, 200, 16, 16, "USB Read  Err ", RED);
          }
          else
          {
              lcd_fill(30, 210, 240, 226, WHITE);
          }

          usb_sta = g_usb_state_reg;
      }

      /* Connection status changes */
      if (device_sta != g_device_state)
      {
          if (g_device_state == 1)
          {
              lcd_show_string(30, 150, 200, 16, 16, "USB Connected    ", RED);
          }
          else
          {
              lcd_show_string(30, 150, 200, 16, 16, "USB DisConnected ", RED);
          }

          device_sta = g_device_state;
      }

      if (++t == 200)
      {
          t = 0;
          LED1(1);
          LED0_TOGGLE();

          if (g_usb_state_reg & 0x10)
          {
              offline_cnt = 0;
              g_device_state = 1;
          }
          else
          {
              offline_cnt++;

              if (offline_cnt > 10)
              {
                  g_device_state = 0;
              }
          }

          g_usb_state_reg = 0;
      }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
