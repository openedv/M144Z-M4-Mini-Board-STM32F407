## FLASH_EEPROM example<a name="brief"></a>

### 1 Brief
The function of this program is to press the WKUP and KEY0 keys, respectively, the Flash data write and read operations, read the data will be displayed to the LCD screen.
### 2 Hardware Hookup
The hardware resources used in this example are:
+ LED0 - PF9
+ USART1 - PA9/PA10
+ KEY - WKUP(PA0)
+ KEY - KEY0(PE4)
+ ALIENTEK  2.8/3.5/4.3/7 inch TFTLCD module

The FLASH used in this example is an on-chip resource of STM32F407, so there is no corresponding connection schematic.

### 3 STM32CubeIDE Configuration

We copy the **12_usmart** project and name both the project and the.ioc file **23_flash_eeprom**.
This example does not need to be configured on STM32CubeIDE, so after copying the project, you can directly add the user driver code.

##### code

##### stmflash.c
```c#
void stmflash_read(uint32_t addr, uint32_t *buf, uint32_t length)
{
    uint32_t index;
    
    for (index = 0; index < length; index++)
    {
        buf[index] = stmflash_read_word(addr);
        addr += sizeof(uint32_t);
    }
}
```
The above function is the FLASH driver in the read function, the function is mainly to read the value of the address, used to write function call check, to ensure that the success of the write.
```c#
void stmflash_write(uint32_t addr, uint32_t *buf, uint16_t length)
{
    uint32_t offaddr;
    uint16_t pagepos;
    uint16_t pageoff;
    uint16_t pageremain;
    uint16_t data_index;
    FLASH_EraseInitTypeDef flash_erase_init_struct = {0};
    uint32_t pageerr;
    
    /* Check the validity of writing to the address range */
    if ((!IS_FLASH_PROGRAM_ADDRESS(addr)) || (!IS_FLASH_PROGRAM_ADDRESS(addr + (length * sizeof(uint32_t)) - 1)))
    {
        return;
    }
    
    offaddr = addr - FLASH_BASE;
    pagepos = offaddr / FLASH_PAGE_SIZE;
    pageoff = (offaddr % FLASH_PAGE_SIZE) / sizeof(uint32_t);
    pageremain = (FLASH_PAGE_SIZE / sizeof(uint32_t)) - pageoff;
    if (length <= pageremain)
    {
        pageremain = length;
    }
    
    while (1)
    {
        stmflash_read(FLASH_BASE + pagepos * FLASH_PAGE_SIZE, g_flash_buf, sizeof(g_flash_buf) / sizeof(uint32_t));
        for (data_index=0; data_index<pageremain; data_index++)
        {
            /* Determine if erasure is required */
            if (g_flash_buf[data_index + pageoff] != 0xFFFFFFFF)
            {
                break;
            }
        }
        if (data_index < pageremain)
        {
            /* Erasing pages */
            flash_erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
            flash_erase_init_struct.Banks = FLASH_BANK_1;
            flash_erase_init_struct.PageAddress = FLASH_BASE + pagepos * FLASH_PAGE_SIZE;
            flash_erase_init_struct.NbPages = 1;
            HAL_FLASH_Unlock();
            HAL_FLASHEx_Erase(&flash_erase_init_struct, &pageerr);
            HAL_FLASH_Lock();
            
            for (data_index=0; data_index<pageremain; data_index++)
            {
                g_flash_buf[pageoff + data_index] = buf[data_index];
            }
            stmflash_write_nocheck(FLASH_BASE + pagepos * FLASH_PAGE_SIZE, g_flash_buf, sizeof(g_flash_buf) / sizeof(uint32_t));
        }
        else
        {
            stmflash_write_nocheck(addr, buf, pageremain);
        }
        
        /* Determines whether the write is complete */
        if (pageremain == length)
        {
            break;
        }
        else
        {
            pagepos++;
            pageoff = 0;
            buf += pageremain;
            addr += (pageremain * sizeof(uint32_t));
            length -= pageremain;
            
            if (length > (FLASH_PAGE_SIZE / sizeof(uint32_t)))
            {
                pageremain = FLASH_PAGE_SIZE / sizeof(uint32_t);
            }
            else
            {
                pageremain = length;
            }
        }
    }
}
```
Before writing FLASH, it needs to determine whether the bit to be written is 1. If it is not 1, it needs to erase the operation first, otherwise the writing will fail. After ensuring that the bit to be written is all 0, the function ``HAL_FLASH_Program()``can be called for programming.

###### usmart_port.c
The **usmart_nametab** array is modified in this file to facilitate the call validation of the relevant functions as follows:
```c#
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS == 1      /* If read and write operations are enabled */
    {(void *)read_addr, "uint32_t read_addr(uint32_t addr)"},
    {(void *)write_addr, "void write_addr(uint32_t addr,uint32_t val)"},
#endif
};
```
The related functions of stmflash are added to USMART, so that the value written or read by FLASH can be set directly through the serial port.
###### main.c
Here's the main function.
```c#
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t t = 0;
  uint8_t key;
  uint8_t data[TEXT_SIZE];
  uint8_t wdata[TEXT_SIZE] = {0};
  uint8_t index;
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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();                                         /* Initialize LCD */

  lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
  lcd_show_string(30, 70, 200, 16, 16, "STMFLASH TEST", RED);
  lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

  lcd_show_string(30, 110, 200, 16, 16, "WK_UP:Write KEY0:Read", RED);

  for (index=0; index<sizeof(g_text_buf); index++)
  {
     wdata[index] = g_text_buf[index];
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    key = key_scan(0);
    if (key == WKUP_PRES)
    {
        /* Writing data to Flash */
        lcd_fill(0, 130, 239, 319, WHITE);
        lcd_show_string(30, 130, 200, 16, 16, "Start Write Flash....", BLUE);
        stmflash_write(FLASH_BANK1_END + 1 - TEXT_SIZE, (uint32_t *)wdata, TEXT_SIZE / sizeof(uint32_t));
        lcd_show_string(30, 130, 200, 16, 16, "Flash Write Finished!", BLUE);
    }
    else if (key == KEY0_PRES)
    {
        /* Read data from Flash */
        lcd_show_string(30, 130, 200, 16, 16, "Start Read Flash.... ", BLUE);
        stmflash_read(FLASH_BANK1_END + 1 - TEXT_SIZE, (uint32_t *)data, TEXT_SIZE / sizeof(uint32_t));
        lcd_show_string(30, 130, 200, 16, 16, "The Data Readed Is:  ", BLUE);
        lcd_show_string(30, 150, 200, 16, 16, (char *)data, BLUE);
    }
    if (++t == 20)
    {
        t = 0;
        LED0_TOGGLE();        /* flashing LED0 indicates that the system is running */
    }

    HAL_Delay(10);            /* delay 10ms */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
When the key WKUP is detected to be pressed in the while loop, a piece of data is written to the FLASH space at the beginning of the specified address. When KEY0 is detected, the spatial data starting from the specified address of FLASH is read.


### 4 Running
#### 4.1 Compile & Download
After the compilation is complete, connect the DAP and the Mini Board, and then connect to the computer together to download the program to the Mini Board.
#### 4.2 Phenomenon
Press the **RESET** button to begin running the program on your Mini Board, observe the LED0 flashing on the Mini Board, indicating that the code download is successful.

Press the WKUP button to write the specified data to the specified address of FLASH, and then press the KEY0 button to read the data of the specified address of FLASH and display it on the LCD.

<img src="../../1_docs/3_figures/21_flash_eeprom/01_lcd.png">

<img src="../../1_docs/3_figures/21_flash_eeprom/02_lcd.png">

[jump to title](#brief)