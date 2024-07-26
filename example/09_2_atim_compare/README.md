## Advanced timer output comparison mode example<a name="brief"></a>

### 1 Brief
The function of this code is the timer 8 channels 1, 2, 3 and 4 output PWM with phase of 25%, 50%, 75% and 100% duty cycle of 50%, respectively.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ TIM8-channel1(PC6)
+ TIM8-channel2(PC7)
+ TIM8-channel3(PC8)
+ TIM8-channel4(PC9)
+ ALIENTEK DS100 oscilloscope

The TIM8 used in this example is the on-chip resource of STM32F103, so there is no corresponding connection schematic diagram.

### 3 STM32CubeIDE Configuration


Let's copy the project from **04_uart** and name both the project and the.ioc file **09_2_atim_compare**. Next we start the ATIM configuration by double-clicking the **09_2_atim_compare.ioc** file.

First, we will reuse pins PC6 and PC7 as TIM8_CH1 and TIM8_CH2 in a similar way as we did in the previous chapter, as shown in the following figure:

<img src="../../1_docs/3_figures/09_2_atim_compare/01_pin.png">


Click **Timers > TIM8** and configure as shown in the following figure.

<img src="../../1_docs/3_figures/09_2_atim_compare/02_config.png">

Click **Timers > TIM8 > Parameter Settings** .

<img src="../../1_docs/3_figures/09_2_atim_compare/03_config.png">

Click **File > Save** and click **Yes** to generate code.

##### code
We add some code to the tim.c file's initialization function ``MX_TIM8_Init``, as follows:
###### tim.c
```c#
  /* USER CODE BEGIN TIM8_Init 2 */
  HAL_TIM_OC_Start(&htim8, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim8, TIM_CHANNEL_2);
  /* USER CODE END TIM8_Init 2 */
```
The above function ``HAL_TIM_OC_Start`` is used to enable the output of TIM8 channels 1-2.

###### main.c
Your finished main() function should look like the following:
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t t = 0;
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
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  stm32f103ve_show_mesg();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (++t == 20)
    {
       t = 0;
       LED0_TOGGLE();
    }

    HAL_Delay(10);                 /* delay 10ms */
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
Press the **RESET** button to begin running the program on your Mini Board. We need to observe the PWM output of PC6 and PC7 pins with the help of an oscilloscope. The phases of these 2-channel PWMS differ by 25% pairwise.

Here, we use ALIENTEK DS100 oscilloscope to observe the waveforms of PC6 and PC7. The yellow waveform is that of PC6, and the green waveform is that of PC7.

<img src="../../1_docs/3_figures/09_2_atim_compare/04_wave.png">

[jump to title](#brief)
