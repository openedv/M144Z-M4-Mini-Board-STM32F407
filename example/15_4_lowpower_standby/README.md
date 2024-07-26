## LowPower_Standby example<a name="brief"></a>

### 1 Brief
The function of this program is that when KEY0 is pressed, it enters the standby mode, at this time the LCD and LED0 are off. When WKUP is pressed, the standby mode is exited, the LCD lights up, and the LED0 blinks normally.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ LED1 - PF10
+ USART1 - PA9/PA10
+ KEY - WKUP(PA0)
+ KEY - KEY0(PE4) 
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

This example introduces the standby mode in the low power mode and does not involve the connection schematic.

### 3 STM32CubeIDE Configuration

We copy the **15_3_lowpower_stop** project and name both the project and the.ioc file **15_4_lowpower_standby**. 
This example is replicated from the previous section, so external interrupt configuration these are already configured. No additional changes to the.ioc file are required. It is only necessary to add the PWR driver file ``pwr.c/pwr.h``. The pwr file for this example is different from the one in the previous chapter.

##### code
###### pwr.c
The code for this file is as follows.
```c#
void pwr_enter_standby(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();                /* The power clock was enabled */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);  /* Enable the wake-up function of the WK_UP pin */
  /* Clear the standby mode flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  HAL_PWR_EnterSTANDBYMode();                /* Enter standby mode */
}
```
The function needs to clear the wake flag before going into standby mode, otherwise it stays awake, and then calls the ``HAL_PWR_EnterSTANDBYMode`` function to go into standby mode.

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
  lcd_show_string(30, 70, 200, 16, 16, "STANDBY TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "KEY0:Enter STANDBY MODE", RED);
  lcd_show_string(30, 130, 200, 16, 16, "WK_UP:Exit STANDBY MODE", RED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);

    if (key == KEY0_PRES)
    {
      pwr_enter_standby(); /* Enter standby mode */
    }
    if (++t == 20)
    {
      t = 0;
      LED0_TOGGLE();   /* flashing LED0 indicates that the system is running */
    }

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
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful. If the **KEY0** button is pressed, the LED0 and LCD will go off. If the **WKUP** button is pressed, the LED0 will resume flashing, the LCD screen will resume display, and the example information will be displayed again on the serial port, as shown in the following figure.


[jump to title](#brief)