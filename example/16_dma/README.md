## DMA example<a name="brief"></a>

### 1 Brief
The function of this code is to press the KEY0 button, serial port USART1 to send data in DMA mode.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ LED1 - PF10
+ USART1 - PA9/PA10
+ DMA2 - Stream 7 
+ KEY - KEY0(PE4) 
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

The DMA used in this example is an on-chip resource of STM32F407, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration

We copy the **11_tftlcd_mcu** project and name both the project and the.ioc file **16_dma**.Next we start the DMA configuration by double-clicking the **16_dma.ioc** file.

Open **System Core > DMA** as shown below.

<img src="../../1_docs/3_figures/16_dma/01_config.png">

Click **File > Save**, and you will be asked to generate code.Click **Yes**.

##### code
###### main.c
In the main function, we have added some additional code, which is as follows.
```c#
/* USER CODE BEGIN PD */
uint8_t temp[] = {"Hello, M144Z-M4 STM32F407!\r\n"};
uint8_t buf[sizeof(temp) * 200];
uint8_t uart_ready = 1;

extern DMA_HandleTypeDef hdma_usart1_tx;
/* USER CODE END PD */

int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t t = 0;
  uint8_t key;
  uint16_t buf_index;
  uint8_t temp_index;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                                         /* Initialize LCD */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);

  /* Preparing the data */
  for (buf_index = 0; buf_index < 200; buf_index++)
  {
    for (temp_index = 0; temp_index < sizeof(temp); temp_index++)
    {
        buf[buf_index * sizeof(temp) + temp_index] = temp[temp_index];
    }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);
    if (key == KEY0_PRES)
    {
      if (uart_ready == 1)
      {
        uart_ready = 0;
        /* DMA transfers serial port data */
        HAL_UART_Transmit_DMA(&huart1, buf, sizeof(buf));
      }
	  }

    if (++t == 20)
    {
      t = 0;
      LED0_TOGGLE();
    }

    HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```

### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful. After pressing the KEY0 button, you can see the data sent by the Mini Board in the serial port debugging assistant, and the transmission will be prompted after the completion of transmission, as shown in the following figure:

<img src="../../1_docs/3_figures/16_dma/02_xcom.png">

[jump to title](#brief)