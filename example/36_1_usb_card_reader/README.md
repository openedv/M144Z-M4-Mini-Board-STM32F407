## USB card reader example<a name="brief"></a>


### 1 Brief
The function of this example is to simulate a USB card reader with STM32F407.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ LED1 - PF10
+ USART1 - PA9/PA10
+ KEY - KEY0(PE4)
+ Micro SD card
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

In this example, USB interface is used to connect with PC, and the connection schematic diagram of USB SLAVE interface and MCU is shown below:

<img src="../../1_docs/3_figures/36_1_usb_card_reader/01_sch.png">

### 3 STM32CubeIDE Configuration

We copy the project from **27_sd** and name both the project and the.ioc file **36_1_usb_card_reader**. Next we start the configuration by double-clicking the **36_1_usb_card_reader.ioc** file.

Click **Connectivity -> USB**, activate the USB slave device:

<img src="../../1_docs/3_figures/36_1_usb_card_reader/02_config.png">

Then start configuring the USB middleware, click **Middleware and Software Packs -> USB_DEVICE**:

<img src="../../1_docs/3_figures/36_1_usb_card_reader/03_usb.png">

The device descriptor is left to its default Settings as shown in the figure below:

<img src="../../1_docs/3_figures/36_1_usb_card_reader/04_device.png">

After the above configuration, it is found that there is an error in the USB clock configuration, and the clock frequency exceeds 48MHz. So we need to make the following changes.

<img src="../../1_docs/3_figures/36_1_usb_card_reader/05_clock.png">

We need to get the USB connection status and configure the interrupt priority in the USB interrupt callback function.

<img src="../../1_docs/3_figures/36_1_usb_card_reader/06_nvic.png">

Click **File > Save**, and you will be asked to generate code.Click **Yes**.The generated project has two more folders, USB_DEVICE and Middlewares.

<img src="../../1_docs/3_figures/36_1_usb_card_reader/07_file.png">


##### code

In this example, several files need to be modified in the USB_DEVICE folder: **usbd_storage_if.c**,**usbd_conf.c**, and **usbd_conf.h**.

**usbd_conf.c/.h** mainly implements USB hardware initialization and interrupt operations.
**usbd_storage_if.c** mainly realizes an identification information of mass storage device.


###### main.c
```c#
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_SDIO_SD_Init();
  MX_SPI2_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                         /* Initialize LCD */
  norflash_init();                    /* Initialize NOR FLASH */
  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "USB Card Reader TEST", RED);
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
  USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_Storage_Interface_fops_FS);     /* Add a callback function to the MSC class */
  USBD_Start(&hUsbDeviceFS);                                           			/* Enable USB */
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
After calling the **MX_USB_DEVICE_Init** function, the USB read/write status and connection status are continuously obtained and displayed on the LCD.

### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful. 

Insert the SD card into the SD card holder on the Mini Board, and you can see the capacity information of the SD card and the connection status of USB on the LCD.

<img src="../../1_docs/3_figures/36_1_usb_card_reader/08_lcd.png">


[jump to title](#brief)
