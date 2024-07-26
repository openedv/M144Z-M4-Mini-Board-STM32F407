/**
 ****************************************************************************************************
 * @file        oled.h
 * @author      ALIENTEK
 * @brief       oled code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK M144-STM32F407 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240410     the first version
 *
 ****************************************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include "main.h"

/* OLED interface mode Settings
 * 0:4-wire SPI mode (BS1 and BS2 of OLED module are connected to GND)
 * 1: 8080 parallel port mode (OLED module BS1, BS2 are connected to VCC)
 */
#define OLED_MODE   1   /* By default, 8080 parallel port mode is used */


#define OLED_RST(x)                 do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(OLED_SPI_RST_GPIO_Port, OLED_SPI_RST_Pin, GPIO_PIN_SET):       \
                                        HAL_GPIO_WritePin(OLED_SPI_RST_GPIO_Port, OLED_SPI_RST_Pin, GPIO_PIN_RESET);     \
                                    } while (0)

#define OLED_CS(x)                  do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(OLED_SPI_CS_GPIO_Port, OLED_SPI_CS_Pin, GPIO_PIN_SET):         \
                                        HAL_GPIO_WritePin(OLED_SPI_CS_GPIO_Port, OLED_SPI_CS_Pin, GPIO_PIN_RESET);       \
                                    } while (0)

#define OLED_RS(x)                  do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(OLED_SPI_RS_GPIO_Port, OLED_SPI_RS_Pin, GPIO_PIN_SET):         \
                                        HAL_GPIO_WritePin(OLED_SPI_RS_GPIO_Port, OLED_SPI_RS_Pin, GPIO_PIN_RESET);       \
                                    } while (0)

#define OLED_SCLK(x)                do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(OLED_SPI_SCLK_GPIO_Port, OLED_SPI_SCLK_Pin, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(OLED_SPI_SCLK_GPIO_Port, OLED_SPI_SCLK_Pin, GPIO_PIN_RESET);   \
                                    } while (0)

#define OLED_SDIN(x)                do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(OLED_SPI_SDIN_GPIO_Port, OLED_SPI_SDIN_Pin, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(OLED_SPI_SDIN_GPIO_Port, OLED_SPI_SDIN_Pin, GPIO_PIN_RESET);   \
                                    } while (0)

/* OLED 8080 parallel port mode pin operation definition */
#define OLED_WR(x)                  do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET):                         \
                                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);                       \
                                    } while (0)

#define OLED_RD(x)                  do { (x) ?                                                                      \
                                        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET):                         \
                                        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);                       \
                                    } while (0)

/* Command/data definition */
#define OLED_CMD    0   /* write command */
#define OLED_DATA   1   /* write data */

void oled_refresh_gram(void);
void oled_display_on(void);
void oled_display_off(void);
void oled_clear(void);
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot);
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);
void oled_init(void);

#endif
