/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       photo code
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
#include "dcmi.h"
#include "dma.h"
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../ATK_Middlewares/MALLOC/malloc.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LCD/lcd.h"
#include "../../BSP/OV2640/ov2640.h"
#include "../../FatFs/exfuns/exfuns.h"
#include "../../FatFs/exfuns/fattester.h"
#include "../../ATK_Middlewares/PICTURE/piclib.h"
#include "ff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint8_t g_sd_error;
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

/* OV2640 output data format
 * 0: RGB565 format
 * 1: JPEG format
 */
static uint8_t g_ov_mode;

/* JPEG data acquisition completion mark
 * 0: The collection is not complete
 * 1: Collection completed, but not processed
 * 2: Processing is complete and the next frame can be received
 */
volatile uint8_t g_jpeg_data_ok = 0;

/**
 * @bref 	Handles JPEG data
 * @note 	Is called in DCMI capture interrupt
 * @param 	None
 * @retval 	None
 */
void jpeg_data_process(void)
{
    if (g_ov_mode == 0)             /* RGB565 mode */
    {
        lcd_set_cursor(0, 0);       /* Set the cursor position */
        lcd_write_ram_prepare();    /* Ready to write GRAM */
    }
}

/**
 * @bref 	Switches to OV2640 mode
 * @note 	Switch PC8/PC9/PC11 to DCMI multiplexing function (AF13)
 * @param 	None
 * @retval 	None
 */
static void sw_ov2640_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    OV2640_PWDN(0);
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
}

/**
 * @bref 	Switches to sdcard mode
 * @note 	Switch PC8/PC9/PC11 to DCMI multiplexing function (AF13)
 * @param 	None
 * @retval 	None
 */
static void sw_sdcard_mode(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    OV2640_PWDN(1);
    gpio_init_struct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);
}

/**
 * @brief 	File name increment (avoid overwriting)
 * @note 	Indicates the file name of the bmp combination, such as 0:PHOTO/PIC13141.bmp
			jpg combines to form a file name such as "0:PHOTO/PIC13141.jpg"
 * @param 	pname: indicates a valid file name
 * @param 	mode: indicates the file format
 * @arg     0: BMP
 * @arg     1: JPG
 * @retval 	None
 */
static void camera_new_pathname(uint8_t *pname, uint8_t mode)
{
    uint8_t res;
    uint16_t index = 0;
    FIL *ftemp;

    ftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
    if (ftemp == NULL)
    {
        return;
    }

    while (index < 0xFFFF)
    {
        if (mode == 0)                                                  /* Create a.bmp file name */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.bmp", index);
        }
        else                                                            /* Create a.jpg file name */
        {
            sprintf((char *)pname, "0:PHOTO/PIC%05d.jpg", index);
        }

        res = (uint8_t)f_open(ftemp, (const TCHAR *)pname, FA_READ);    /* Try to open this file */
        if (res == (uint8_t)FR_NO_FILE)                                 /* The file name does not exist. It is the desired file name */
        {
            break;
        }

        index++;
    }

    myfree(SRAMIN, ftemp);
}

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
    uint8_t res;
    uint8_t sd_ok = 1;
    uint8_t *pname;
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
  MX_FATFS_Init();
  MX_DCMI_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();
  piclib_init();
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  my_mem_init(SRAMCCM);               /* Initialize the CCM memory pool */
  exfuns_init();                      /* Request memory for exfuns */
  f_mount(fs[0], "0:", 1);            /* mount SD card */
  f_mount(fs[1], "1:", 1);            /* mount NOR Flash */

  while (ov2640_init() != 0)                                                              /* Initialize OV2640 */
  {
      lcd_show_string(30, 170, 240, 16, 16, "OV2640 Error!", RED);
      HAL_Delay(200);
      lcd_fill(30, 150, 239, 206, WHITE);
      HAL_Delay(200);
  }

  sw_sdcard_mode();
  while (g_sd_error == 1)
  {
  	  g_sd_error = 0;
  	  MX_SDIO_SD_Init();
      lcd_show_string(30, 130, 200, 16, 16, "SD Card Failed!", RED);
      HAL_Delay(500);
      lcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
      HAL_Delay(500);
      LED1_TOGGLE();
  }

  lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 50, 200, 16, 16, "PHOTO TEST", RED);
  lcd_show_string(30, 70, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 90, 200, 16, 16, "WKUP: Take a picture(BMP)", RED);

  res = (uint8_t)f_mkdir("0:/PHOTO");                                                     /* Create a PHOTO folder */
  if ((res != (uint8_t)FR_EXIST) && (res != 0))                                           /* PHOTO folder does not exist and creation failed */
  {
      lcd_show_string(30, 150, 240, 16, 16, "SD Card Error!", RED);
      lcd_show_string(30, 150, 240, 16, 16, "The photo function will not be available!", RED);
      sd_ok = 0;
  }

  pname = (uint8_t *)mymalloc(SRAMIN, 30);                                                /* Allocates memory for the filename with path */
  while (pname == NULL)
  {
      lcd_show_string(30, 150, 240, 16, 16, "Memory allocation failure!", RED);
      HAL_Delay(200);
      lcd_fill(30, 150, 240, 146, WHITE);
      HAL_Delay(200);
  }

  sw_ov2640_mode();
  ov2640_rgb565_mode();                                                                   /* Set OV2640 to RGB565 mode */
  dcmi_dma_init((uint32_t)&(LCD->LCD_RAM), 0, 1);										  /* Configure DCMI DMA */
  ov2640_outsize_set(lcddev.width, lcddev.height);                                        /* Full screen zoom */
  dcmi_start();                                                                           /* Start DCMI transfer */
  ov2640_flash_intctrl();                                                                 /* Internal control flash */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
      t++;
      key = key_scan(0);

      if (key != 0)                                                                       /* There are keys to press */
      {
          dcmi_stop();                                                                    /* First disable DCMI transmission */
          if (sd_ok == 1)                                                                 /* SD card is normal */
          {
              sw_sdcard_mode();                                                           /* Switch to SD card mode */
              switch (key)
              {
                  case WKUP_PRES:
                  {
                      camera_new_pathname(pname, 0);                                      /* The file name is in BMP format */
                      res = bmp_encode(pname, 0, 0, lcddev.width, lcddev.height, 0);      /* Encode and save the BMP file */
                      break;
                  }
                  default:
                  {
                      break;
                  }
              }
              sw_ov2640_mode();                                                           /* Switch to OV2640 mode */
              if (res != 0)
              {
                  lcd_show_string(30, 130, 240, 16, 16, "Write file error!", RED);

              }
              else
              {
                  lcd_show_string(30, 130, 240, 16, 16, "Photo success!", RED);
                  lcd_show_string(30, 130, 240, 16, 16, "Save as!", RED);
                  lcd_show_string(30, 130, 240, 16, 16, (char*)pname, RED);
              }

              HAL_Delay(1000);
          }
          else                                                                            /* SD card is not available */
          {
              lcd_show_string(30, 130, 240, 16, 16, "SD Card Error!", RED);
              lcd_show_string(30, 130, 240, 16, 16, "The photo function is unavailable!", RED);

          }
          HAL_Delay(2000);
          dcmi_start();                                                                   /* Resume DCMI transmission */
      }

      if (t == 20)
      {
          LED0_TOGGLE();
          t = 0;
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
