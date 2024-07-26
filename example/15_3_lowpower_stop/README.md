## LowPower_Stop example<a name="brief"></a>

### 1 Brief
The function of this program is when the KEY0 is pressed, Enter the Stop Mode, the LCD screen will display **Enter Stop Mode...**. When WKUP is pressed, Stop Mode is exited, and the LCD screen will display **EXIT Stop Mode...**. You can see that LED1 also undergoes corresponding changes.
### 2 Hardware Description
The hardware resources used in this example are:
+ LED0 - PF9
+ LED1 - PF10
+ USART1 - PA9/PA10
+ KEY - WKUP(PA0)
+ KEY - KEY0(PE4) 
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

This example introduces the stop mode in the low power mode and does not involve the connection schematic.

### 3 STM32CubeIDE Configuration

We copy the **15_2_lowpower_sleep** project and name both the project and the.ioc file **15_3_lowpower_stop**. 
This example is replicated from the previous section, so external interrupt configuration these are already configured. No additional changes to the.ioc file are required. It is only necessary to add the PWR driver file ``pwr.c/pwr.h``. The pwr file for this example is different from the one in the previous chapter.

##### code
###### pwr.c
The code for this file is as follows.
```c#
void pwr_enter_stop(void)
{
  /* Turn off the SysTick interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); /* Execute the WFI command and enter the stop mode */
}

/**
 * @brief    External interrupt callback function
 * @param    GPIO Pin : Interrupt pin
 * @note     This function is called by PWR WKUP INT IRQHandler()
 * @retval   None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == WK_UP_Pin)
    {
      /* Restart the SysTick interrupt */
      SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
    }
}
```
The ``HAL_PWR_EnterSTOPMode`` function is called directly inside ``pwr_enter_stop`` to enter stop mode and wait for WKUP to interrupt wakeup.

In the stop mode, the system clock is provided by the HSI RC oscillator, and if we do not actively configure the system clock, the peripheral processing rate will be greatly reduced.

###### main.c
Here's the main function.
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t t = 0;
  uint8_t key = 0;
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
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                                         /* Initialize LCD */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "STOP TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "KEY0:Enter STOP MODE", RED);
  lcd_show_string(30, 130, 200, 16, 16, "WK_UP:Exit STOP MODE", RED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);

    if (key == KEY0_PRES)
    {
      lcd_show_string(30, 150, 200, 16, 16, "Enter Stop Mode...", BLUE);
      /* Press the KEY0 button to turn on LED1 and enter the stop mode */
      LED1(0);
      pwr_enter_stop();    /* Enter stop mode */
      lcd_show_string(30, 150, 200, 16, 16, "Exit  Stop Mode...", BLUE);
      LED1(1);
    }
    if ((t % 20) == 0)
    {
      LED0_TOGGLE();   /* flashing LED0 indicates that the system is running */
    }

    t++;
    HAL_Delay(10);     /* delay 10ms */
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
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful. If the LED0 stops flashing when the KEY0 button is pressed, the LCD screen will display **Enter Stop Mode...**. If you press the WKUP button, the LED0 will resume flashing, and the LCD screen will display **Exit Stop Mode...**.You can see that LED1 also undergoes corresponding changes.


[jump to title](#brief)