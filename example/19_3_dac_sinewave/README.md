## DAC_Sine_Wave example<a name="brief"></a>

### 1 Brief
The function of this code is to press the KEY0 button and the PA4 pin outputs the sine wave 1. Press the WKUP button, pin PA4 outputs sine wave 2. The LCD displays the DAC conversion value and the voltage value.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ USART1 - PA9/PA10
+ DAC1 - Channel1(PA4)
+ TIM7
+ ALIENTEK DS100 oscilloscope
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

The DAC used in this example is an on-chip resource of STM32F407, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration


We copy the **11_tftlcd** project and name both the project and the.ioc file **19_3_dac_sine_wave**.Next we start the DAC configuration by double-clicking the **19_3_dac_sine_wave.ioc** file.

In **Pinout&Configuration**, click **PA4** pin to set mode.

<img src="../../1_docs/3_figures/19_3_dac_sine_wave/01_pin.png">

Then open **Analog->DAC1** to configure.

<img src="../../1_docs/3_figures/19_3_dac_sine_wave/02_config.png">

Then configure TIM7 as shown.

<img src="../../1_docs/3_figures/19_3_dac_sine_wave/04_tim.png">

Click **File > Save**, and you will be asked to generate code.Click **Yes**.

Among them, TIM files have been introduced in the previous chapters, and the added content is basically the same. You can open the source code to check it by yourself. The logic code added for this example is described below.

##### code
###### dac.c
```c#
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint16_t i;
    double w;
    uint16_t outdata;

    w = (2 * 3.1415926) / samples;                  /* ω=2π/T */

    for (i = 0; i < samples; i++)
    {
        outdata = maxval * sin(w * i + 0) + maxval; /* y=Asin(ωx+φ)+b */
        if (outdata > 4095)                         /* Limit upper bound */
        {
            outdata = 4095;
        }
        g_dac_sin_buf[i] = outdata;
    }
}

void dac_dma_wave_enable(uint16_t cndtr, uint16_t arr, uint16_t psc)
{
    __HAL_TIM_SET_PRESCALER(&htim7, psc);
    __HAL_TIM_SET_AUTORELOAD(&htim7, arr);
    HAL_TIM_Base_Init(&htim7);
    HAL_TIM_Base_Start(&htim7);                                    /* Start Timer 7 */

    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);                        /* Stop previous transfer first */
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)g_dac_sin_buf, cndtr, DAC_ALIGN_12B_R);
}
```
The function ``dac_creat_sin_buf`` is used to generate the sine wave sequence, which is stored in the **g_dac_sin_buf** array for DAC conversion.
The function ``dac_dma_wave_enable`` is used to configure the TIM7 overflow frequency, so the TIM7 overflow frequency determines the DAC output sine wave frequency and also enables DAC DMA transfer.

##### main.c
Here's the main function.
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint8_t t = 0;
    uint8_t key;
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
  MX_DAC_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();
  dac_dma_wave_enable(100, 10 - 1, 84 - 1);

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "DAC DMA Sine Wave TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "WK_UP:Wave1 KEY0:Wave2", RED);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	  key = key_scan(0);

	  if (key == WKUP_PRES)
	  {
	      dac_creat_sin_buf(2048 - 1, 100);           /* Generate sine wave data */
	      dac_dma_wave_enable(100, 10 - 1, 84 - 1);   /* Enable DAC DMA waveform output */
	  }
	  else if (key == KEY0_PRES)
	  {
	      dac_creat_sin_buf(2048 - 1, 10);            /* Generate sine wave data */
	      dac_dma_wave_enable(10, 10 - 1, 84 - 1);    /* Enable DAC DMA waveform output */
	  }

	  if (++t == 20)
	  {
	      t = 0;
	      LED0_TOGGLE();
	  }

      HAL_Delay(10);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
When this part of the code is initialized, a set of sine wave data is generated to control the DAC channel 1 to output the specified sine wave. Then, according to the scanned keys, the sine waves with different frequencies are output.


### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful. Connect the **PA4** pin with a **ALIENTEK DS100 oscilloscope** and press the KEY0 and WKUP keys respectively. You can see two different types of sine waves, as shown in the following figure.

<img src="../../1_docs/3_figures/19_3_dac_sine_wave/05_wave1.png">

<img src="../../1_docs/3_figures/19_3_dac_sine_wave/06_wave2.png">

[jump to title](#brief)