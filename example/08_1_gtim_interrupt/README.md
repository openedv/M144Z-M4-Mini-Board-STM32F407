## General-purpose timer interrupt example<a name="brief"></a>

### 1 Brief

The function of this code is to flip the state of the LED1 in the interrupt service function of the general-purpose timer 3.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PB5
+ LED1 - PE5
+ USART1 - PA9\PA10
+ TIM3

The TIM3 used in this example is the on-chip resource of STM32F103, so there is no corresponding connection schematic diagram.

### 3 STM32CubeIDE Configuration

Let's copy the project from **04_uart** and name both the project and the.ioc file **08_1_gtim_interrupt**. Next we start the GTIM configuration by double-clicking the **08_1_gtim_interrupt.ioc** file.

Click **Timers->TIM3** and configure as shown in the following figure.

<img src="../../1_docs/3_figures/08_1_gtim_interrupt/01_config.png">

As can be seen from the figure above, we configure the automatic reload value as (5,000-1), the pre-divider value as (7,200-1), and the clock frequency of TIM3 is 72MHz, so the count frequency of TIM3 is 2KHz.

The configuration of the NVIC is shown below.

<img src="../../1_docs/3_figures/08_1_gtim_interrupt/02_nvic.png">

Click **File > Save**, and you will be asked to generate code.Click **Yes**.

##### code
We add some code to the tim.c file's initialization function ``MX_TIM3_Init``, as follows.
###### tim.c
```c#
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_TIM_Base_Start_IT(&htim3);    /* Enable timer 3 and its update interrupt. */
  /* USER CODE END TIM3_Init 2 */
```
The above function ``HAL_TIM_Base_Start_IT`` is used to enable TIM3 and start the update interrupt.

An interrupt callback function has also been added, as shown below:
```c#
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    LED1_TOGGLE(); /* turn over LED1 */
  }
}
```
 The LED1 is flipped in the update interrupt callback function ``HAL_TIM_PeriodElapsedCallback``. We will determine whether the update is interrupted by TIM3, and if so, perform an LED1 flip operation.

###### main.c
Your finished main() function should look like the following:
```c#
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
     LED0_TOGGLE();     /* flashing LED0 indicates that the system is running */
     HAL_Delay(200);    /* delay 200ms */
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
Press the **RESET** button to begin running the program on your Mini Board. Observe the LED1 flashing every 500 milliseconds on the Mini Board.

[jump to title](#brief)