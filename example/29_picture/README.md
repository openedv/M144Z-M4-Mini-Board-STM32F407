## Pictures example<a name="brief"></a>


### 1 Brief
The function of this example is to display BMP, JPG and GIF on the LCD.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ USART1 - PA9/PA10
+ KEY - WKUP(PA0)
+ KEY - KEY0(PE4)
+ Micro SD card
+ NORFLASH
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

The image decoding library used in this example is a software library, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration

We copy the project from  **28_fatfs** and name both the project and the.ioc file **29_pictures**. The **.ioc** file does not need to be modified for this example. But we will add the PICTURES-driver files.

<img src="../../1_docs/3_figures/29_pictures/pic.png">


##### code

###### main.c
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint8_t t = 0;
    uint8_t key;
    uint8_t res;
    DIR picdir;
    uint16_t totpicnum;
    FILINFO *picfileinfo;
    char *pname;
    uint32_t *picoffsettbl;
    uint16_t curindex;
    uint16_t temp;
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
  MX_TIM4_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  lcd_init();

  my_mem_init(SRAMIN);                /* Initialize the internal SRAM memory pool */
  my_mem_init(SRAMCCM);               /* Initialize the CCM memory pool */
  exfuns_init();                      /* Request memory for exfuns */
  f_mount(fs[0], "0:", 1);            /* mount SD card */
  f_mount(fs[1], "1:", 1);            /* mount NOR Flash */


  lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 50, 200, 16, 16, "PICTURE TEST", RED);
  lcd_show_string(30, 70, 200, 16, 16, "ATOM@ALIENTEK", RED);
  lcd_show_string(30, 90, 200, 16, 16, "WKUP: PREV", RED);
  lcd_show_string(30, 110, 200, 16, 16, "KEY0: NEXT", RED);

  while (sd_init() != 0)
	    /* Open the images folder */
	    while (f_opendir(&picdir, "0:/PICTURE") != FR_OK)
	    {
	        lcd_show_string(30, 130, 200, 16, 16, "PICTURE folder error!", RED);
	        delay_ms(200);
	        lcd_fill(30, 130, 200, 16, WHITE);
	        delay_ms(200);
	    }

  	    /* Get the number of valid image files */
	    totpicnum = pic_get_tnum("0:/PICTURE");
	    while (totpicnum == 0)
	    {
	        lcd_show_string(30, 130, 200, 16, 16, "No pictures!", RED);
	        delay_ms(200);
	        lcd_fill(30, 130, 200, 16, WHITE);
	        delay_ms(200);
	    }

	    /* request memory */
	    picfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));
	    pname = (char *)mymalloc(SRAMIN, _MAX_LFN * 2 + 1);
	    picoffsettbl = (uint32_t *)mymalloc(SRAMIN, 4 * totpicnum);
	    while ((picfileinfo == NULL) || (pname == NULL) || (picoffsettbl == NULL))
	    {
	        lcd_show_string(30, 130, 200, 16, 16, "Memory allocation failure!", RED);
	        delay_ms(200);
	        lcd_fill(30, 130, 200, 16, WHITE);
	        delay_ms(200);
	    }

	    /* Open the directory and record the image index */
	    res = (uint8_t)f_opendir(&picdir, "0:/PICTURE");
	    if (res == 0)
	    {
	        curindex = 0;
	        while (1)
	        {
	            temp = picdir.dptr;
	            res = (uint8_t)f_readdir(&picdir, picfileinfo);
	            if ((res != 0) || (picfileinfo->fname[0] == 0))
	            {
	                break;
	            }

	            res = exfuns_file_type(picfileinfo->fname);
	            if ((res & 0xF0) == 0x50)
	            {
	                picoffsettbl[curindex] = temp;
	                curindex++;
	            }
	        }
	    }

	    /* Start displaying images */
        lcd_show_string(30, 130, 200, 16, 16, "Start display...", RED);

	    delay_ms(1500);
	    piclib_init();
	    curindex = 0;
	    res = (uint8_t)f_opendir(&picdir, (const TCHAR *)"0:/PICTURE");
	    while (res == 0)
	    {
	    	/* Get the next image file information */
	        dir_sdi(&picdir, picoffsettbl[curindex]);
	        res = (uint8_t)f_readdir(&picdir, picfileinfo);
	        if ((res != 0) || (picfileinfo->fname[0] == 0))
	        {
	            break;
	        }

	        /* Display images according to the image path */
	        strcpy((char *)pname, "0:/PICTURE/");
	        strcat((char *)pname, (const char *)picfileinfo->fname);
	        lcd_clear(BLACK);
	        piclib_ai_load_picfile(pname, 0, 0, lcddev.width, lcddev.height, 1);
	        lcd_show_string(2, 2, lcddev.width, 16, 16, (char *)pname, RED);


	        while (1)
	        {
	            key = key_scan(0);
	            if (key == KEY0_PRES)
	            {
	            	/* Switch to the previous screen */
	                if (curindex != 0)
	                {
	                    curindex--;
	                }
	                else
	                {
	                    curindex = totpicnum - 1;
	                }
	                break;
	            }
	            else if (key == WKUP_PRES)
	            {
	            	/* Switch to the next screen */
	                curindex++;
	                if (curindex >= totpicnum)
	                {
	                    curindex = 0;
	                }
	                break;
	            }

	            if (++t == 20)
	            {
	                t = 0;
	                LED0_TOGGLE();
	            }

	            delay_ms(10);
	        }
	    }

	    /* free the memory */
	    myfree(SRAMIN, picfileinfo);
	    myfree(SRAMIN, pname);
	    myfree(SRAMIN, picoffsettbl);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
You can see that the whole idea is designed according to the image decoding library, ``piclib_ai_load_picfile()`` is the core of this code, other interaction is around it and image decoded image information for display. Let's look at the CD Source code to learn more about the whole setup idea.

### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, open the serial port and the host computer ATK-XCOM can see the prompt information of the example, indicating that the code download is successful. 

We take a microSD card to test(Assuming the SD and files are ready, create a PICTURE folder in the root of the SD card and store some image files (.bmp/.jpg/.gif).), you can see the screen will show the picture.

Press WKUP and KEY0 to quickly switch between the previous or next screen, and note that the size of the picture must not exceed the size of the LCD screen.

[jump to title](#brief)
