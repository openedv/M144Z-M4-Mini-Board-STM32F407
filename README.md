# ALIENTEK STM32F407 Mini Board User Guide<a name="catalogue"></a>


## 1 Repository overview

The file structure of the data disk is as follows:

| Root directory | Describe                                                     |
| -------------- | ------------------------------------------------------------ |
| 1_docs         | Mini Board related documentation,includes schematic diagrams, data sheets, pictures of each example configuration, pin elicitation diagrams, and pin assignment tables. |
| 2_tools        | Development tools. Such as Flash download software, USB virtual serial port driver, and serial port terminal, etc |
| examples       | Explains how to use STM32CubeIDE and 33 examples of different peripherals. |
| libraries      | HAL library for STM32F4. |



## 2 Board Resources

This manual aims to introduce the **hardware and software resources** of the M144Z-M4 Mini Board, including the introduction of chip resources, the introduction of experimental platform hardware resources, and the supporting resource use guide. By studying this manual, the reader will have a comprehensive understanding of the resources of the M144Z-M4 Mini Board, which will be very helpful for subsequent program design.

It is intended to provide readers with a comprehensive introduction to the M144Z-M4 Mini Board resources. It is highly recommended that you read this manual before learning the examples!

### 2.1 Hardware Description

<img src="./1_docs/3_figures/image/PM144.png">

| Resource          | Quantity   | Description                                                         |
| ----------------- | ---------- | ------------------------------------------------------------------- |
| CPU               | 1          | STM32F404ZGT6; FLASH:1024KB; SRAM:192KB                               |
| SRAM              | 1          | 1MB, 16-bit parallel port access                                    |
| SPI FLASH         | 1          | 16MB                                                                |
| EEPROM            | 1          | 256B                                                                |
| Power Light       | 1          | BLUE                                                                |
| LED               | 2          | RED(LED0), GREEN(LED1)                                              |
| RESET BUTTON      | 1          | Used for MCU/LCD reset.                                             |
| KEY               | 2          | KEY0 (sets the boot mode function), WKUP (has the wake-up function) |
| SWD Interface     | 1          | Used for simulation debugging, downloading code, etc.               |
| 5V power input/output outlet     | 1 | For 5V power supply access/external supply of 5V voltage.     |
| 3.3V power input/output outlet   | 1 | For 3.3V power supply access/external supply of 5V voltage.   |
| Select the serial port interface | 1 | Used to select serial connection.                             |
| One click download circuit       | 1 | Easy to download code with serial port.                       |

<img src="./1_docs/3_figures/image/main.png">

+ **EEPROM**:The EEPROM chip, with a capacity of 2Kb, which is equivalent to 256 bytes. It is used to store important data that cannot be lost during power loss, such as system settings parameters/touch screen calibration data, etc. With this, it is convenient to achieve non-volatile data storage.
+ **NOR FLASH**:The SPI FLASH chip expanded externally on the mini board, with a capacity of 128Mbit, which is equivalent to 16M bytes. It can be used to store user data, meeting the demand for large-capacity data storage. Of course, if you find that 16M bytes are not sufficient, you can store the data on an external TF card.
+ **USB slave**: this interface can not only be used as a USB slave communication interface, but also has a power supply function, which can supply power for the Mini Board. USB_Slave can be used to connect the computer to realize the USB virtual serial port experiment.
+ **TF card interface**:The TF card interface driven via SDIO. The TF card capacity selection range is very wide (up to TB level). With this interface, it can meet the demand for massive data storage.
+ **TFTLCD interface**:This is the onboard LCD module interface on the mini board, which is connected to the TFTLCD module via FPC ribbon cable and an adapter board. This interface is compatible with the entire series of TFTLCD modules from Elecrow, including 2.4-inch, 3.5-inch, 4.3-inch, and 7-inch TFTLCD modules, and supports both resistive and capacitive touch functionalities.

### 2.2 Introduction to Software Resources

There are up to **55** examples provided. Before learning each example, carefully read the **README.md**, which includes a brief introduction to the example function, hardware hookup, STM32CubeIDE configuration instructions, and running phenomena. Here are all the examples.

| Num                  | Example Name                      | Num  	| Example Name                        	|
| -------------------- | --------------------------------- | ---- 	| ---------------------- 				|
| 00                   | 00_f407_test                      | 19_1   | 19_1_dac_output 						|
| 01                   | 01_led                            | 19_2   | 19_2_dac_triangular_wave 				|
| 02                   | 02_key                            | 19_3 	| 19_3_dac_sinewave                     |
| 03                   | 03_exti                           | 20 	| 20_iic                              	|
| 04                   | 04_uart                           | 21 	| 21_spi                                |
| 05                   | 05_iedg                           | 22 	| 22_touch                            	|
| 06                   | 06_wwdg                           | 23   	| 23_flash_eeprom                       |
| 07                   | 07_btim                           | 24 	| 24_ov2640_camera                      |
| 08_1                 | 08_1_gtim_interrupt               | 25 	| 25_sram                        		|
| 08_2                 | 08_2_gtim_pwm                     | 26 	| 26_malloc                       		|
| 08_3                 | 08_3_gtim_capture                 | 27   	| 27_sd                               	|
| 08_4                 | 08_4_gtim_count                   | 28   	| 28_fatfs                              |                     																	
| 09_1                 | 09_1_atim_npwm                    | 29   	| 29_picture      						|
| 09_2                 | 09_2_atim_compare                 | 30 	| 30_photo                              |
| 09_3                 | 09_3_atim_cplm_pwm                | 31 	| 31_video                              |
| 09_4                 | 09_4_atim_pwm_in                  | 32_1   | 32_1_julia_enable_fpu                 |
| 10                   | 10_oled                		   | 32_2   | 32_2_julia_disable_fpu                |
| 11                   | 11_tftlcd_mcu            		   | 33_1   | 33_1_dsp_basicmath                    |                                         
| 12                   | 12_usmart         	               | 33_2   | 33_2_dsp_fft      					|
| 13                   | 13_rtc                 		   | 34   	| 34_atkncr                     	    |
| 14                   | 14_rng                 		   | 35_1   | 35_1_iap_bootloader                   |
| 15_1                 | 15_1_lowpower_pvd        		   | 35_2   | 35_2_led_flash_app                    |
| 15_2                 | 15_2_lowpower_sleep               | 36_1   | 36_1_usb_card_reader                  |
| 15_3                 | 15_3_lowpower_stop         	   | 36_2   | 36_2_usb_vsp                          |
| 15_4                 | 15_4_lowpower_standby             | 37   	| 37_freertos_demo                      |
| 16                   | 16_dma		            		   |      	|                           			|
| 17_1                 | 17_1_adc_single_channel           |    	|                           			|
| 17_2                 | 17_2_adc_dma_single_channel       |   		|                           			|
| 17_3                 | 17_3_adc_dma_multi_channel        |    	|                                     	|
| 18                   | 18_adc_temperature                |    	|                                     	|
     						                                            												                                                                                       
														   																
## 3 How to use STM32CubeIDE
If you have not set up **STM32CubeIDE** or can't use it, you will need to do so following the [steps outlined in this tutorial](./1_docs/STM32CubeIDE_Usage_Guide.md).

## 4 Accessories
When using the MIni Board, you can use the following accessories.
 
| LCD module         | Dap Debugger   | USB to Serial Port module  | Digital Oscilloscope
| ------------------ | ----------------|---------------------------------------|------------------------------------------------ |
| <a href="https://github.com/openedv/LCD-module_2.8-inch_ATK-MD0280"><img src="./1_docs/3_figures/image/1.LCD.png" width="250" height="200" /></a>     |    <a href="https://github.com/openedv/alientek-tools_DAP_Debugger"><img src="./1_docs/3_figures/image/Mini_HSDAP.png" width="200" height="100" /></a>     |    <a href="https://github.com/openedv/USB-to-Serial-Port-module-ATK-MO340P"><img src="./1_docs/3_figures/image/USB.png" width="280" /></a>  |  <a href="https://github.com/openedv/Digital-Oscilloscope_DS100"><img src="./1_docs/3_figures/image/digital.png" width="300" /></a>