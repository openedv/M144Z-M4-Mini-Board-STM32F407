## PVD voltage monitoring example<a name="brief"></a>

### 1 Brief
The function of this program is when the MCU power supply Voltage is lower than 2.7V, the LCD screen will display **PVD Low Voltage!**, you can also see that LED1 will light up.When the supply Voltage returns to higher than 2.7V, the LCD screen will display **PVD Voltage OK!**. 
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ LED1 - PF10
+ USART1 - PA9/PA10
+ PVD
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

The PVD used in this example is an on-chip resource of STM32F407, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the project from  **11_tftlcd_mcu** and name both the project and the.ioc file **15_1_lowpower_pvd**. The **.ioc** file does not need to be modified for this example. But we will add the PWR-driver file ``pwr.c/pwr.h``.

##### code
###### pwr.c
The code for this file is as follows.
```c#
void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pwr_pvd = {0};

    __HAL_RCC_PWR_CLK_ENABLE();                      /* Enable PWR clock */
    
    pwr_pvd.PVDLevel = pls;                          /* Detection voltage level */
    pwr_pvd.Mode = PWR_PVD_MODE_IT_RISING_FALLING;   /* Use a double edge trigger of the rising and falling edge of the interrupt line */
    HAL_PWR_ConfigPVD(&pwr_pvd);

    HAL_NVIC_SetPriority(PVD_IRQn, 3 ,3);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    HAL_PWR_EnablePVD();                             /* Enable PVD detection */
}

/**
 * @brief    PVD interrupt service function
 * @param    None
 * @retval   None
 */
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

/**
 * @brief    PVD interrupt service callback function
 * @param    None
 * @retval   None
 */
void HAL_PWR_PVDCallback(void)
{
    if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO) != RESET)
    {
        /* If the voltage is lower than the set voltage level, prompt accordingly */
        LED1(0);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Low Voltage!", RED);
    }
    else
    {
        /* The voltage is not lower than the set voltage level, and the corresponding prompt is given  */
        LED1(1);
        lcd_show_string(30, 130, 200, 16, 16, "PVD Voltage OK! ", BLUE);
    }
}
```
The first is the PVD initialization function, which config the voltage threshold of the PVD according to the passed parameter values, and turns on the relevant interrupt of the PVD and the enable PVD module.

After that is the interrupt callback function of PVD. When the power supply voltage monitored by PVD is less than the set voltage threshold, a prompt message will be displayed on the LCD and you can also see that LED1 will light up. When the normal voltage is restored, the corresponding prompt information will also be provided.PVD will take corresponding operation when it detects that the supply voltage is below the set voltage threshold.

###### main.c
Add some codes between the /* USER CODE */ guards as follow:
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
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  pwr_pvd_init(PWR_PVDLEVEL_7);
  lcd_init();                                         /* Initialize LCD */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "PVD TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  /* The default LCD display voltage is normal */
  lcd_show_string(30, 120, 200, 16, 16, "PVD Voltage OK! ",RED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    t++;
    if ((t % 20) == 0)
    {
        LED0_TOGGLE();   /* flashing LED0 indicates that the system is running */
    }

    HAL_Delay(10);       /* delay 10ms */
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
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful.

If the system supply Voltage is lower than 2.7V, the LCD will display ``PVD Low Voltage!`` (The voltage is too low to see some of the phenomenon) and LED1 will light up. If the system power supply is normal, the LCD will display ``PVD Voltage OK!``, as shown below:

<img src="../../1_docs/3_figures/15_1_lowpower_pvd/01_lcd.png">

[jump to title](#brief)