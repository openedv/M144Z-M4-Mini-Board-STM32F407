/* USER CODE BEGIN Header */
/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ALIENTEK
 * @brief       TOUCH code
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
#include "../../BSP/TOUCH/touch.h"
#include "../../SYSTEM/delay/delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @brief   Clear the screen and display "RST" in the upper right corner
 * @param   None
 * @retval  None
 */
static void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* Clear the screen */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* Display "RST" in the upper right corner */
}

/**
 * @brief   Draw a thick line
 * @param   x1: Starting X coordinate
 * @param   y1: Starting Y coordinate
 * @param   x2: Ending X coordinate
 * @param   y2: Ending Y coordinate
 * @param   size: Thickness of the line
 * @param   color: Color of the line
 * @retval  None
 */
static void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx, incy, row, col;

    if ((x1 < size) || (x2 < size) || (y1 < size) || (y2 < size))
    {
        return; // Ensure that the line is not drawn outside the screen
    }

    delta_x = x2 - x1;                          /* Calculate coordinate increments */
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)
    {
        incx = 1;                               /* Set the step direction */
    }
    else if (delta_x == 0)
    {
        incx = 0;                               /* Vertical line */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;                               /* Horizontal line */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x;                     /* Select the basic increment coordinate axis */
    }
    else
    {
        distance = delta_y;
    }

    for (t=0; t<=(distance + 1); t++)           /* Draw the line */
    {
        lcd_fill_circle(row, col, size, color); /* Draw a point */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief   Resistive Touch Screen Test
 * @param   None
 * @retval  None
 */
void rtp_test(void)
{
    uint8_t key;
    uint8_t i = 0;

    while (1)
    {
        key = key_scan(0);
        tp_dev.scan(0);

        if (tp_dev.sta & TP_PRES_DOWN)                                          /* Touch screen is pressed */
        {
            if ((tp_dev.x[0] < lcddev.width) && (tp_dev.y[0] < lcddev.height))
            {
                if ((tp_dev.x[0] > (lcddev.width - 24)) && (tp_dev.y[0] < 16))
                {
                    load_draw_dialog();                                         /* Clear */
                }
                else
                {
                    tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);           /* Draw point */
                }
            }
        }
        else
        {
            delay_ms(10);                                                       /* When no key is pressed */
        }

        if (key == KEY0_PRES)                                                   /* KEY0 pressed, execute calibration procedure */
        {
            tp_adjust();                                                        /* Screen calibration */
            tp_save_adjust_data();
            load_draw_dialog();
        }

        i++;
        if ((i % 20) == 0)
        {
            LED0_TOGGLE();
        }
    }
}

/* Color of 10 touch points (for capacitive touch screen) */
static const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW,
                                             MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};

/**
 * @brief   Capacitive Touch Screen Test
 * @param   None
 * @retval  None
 */
static void ctp_test(void)
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];
    uint8_t maxp = 5;

    if (lcddev.id == 0x1018)
    {
        maxp = 10;
    }

    while (1)
    {
        tp_dev.scan(0);
        for (t = 0; t < maxp; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if ((tp_dev.x[t] < lcddev.width) && (tp_dev.y[t] < lcddev.height))                                  /* Coordinates are within the screen range */
                {
                    if (lastpos[t][0] == 0xFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }

                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]);  /* Draw line */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];

                    if (tp_dev.x[t] > (lcddev.width - 24) && tp_dev.y[t] < 20)
                    {
                        load_draw_dialog();                                                                         /* Clear */
                    }
                }
            }
            else
            {
                lastpos[t][0] = 0xFFFF;
            }
        }

        delay_ms(5);
        i++;
        if ((i % 20) == 0)
        {
            LED0_TOGGLE();
        }
    }
}


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
  tp_dev.init();                      /* Initialize the touch screen */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "TOUCH TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  /* Resistive screen displays touch calibration prompt */
  if ((tp_dev.touchtype & 0x80) == 0)
  {
      lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED);
  }
  HAL_Delay(1500);
  load_draw_dialog();

  if (tp_dev.touchtype & 0x80)
  {
	  /* Capacitive screen test */
      ctp_test();
  }
  else
  {
	  /* Resistive screen test */
      rtp_test();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
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
