/**
 ****************************************************************************************************
 * @file        touch.h
 * @author      ALIENTEK
 * @brief       touch drive code
 * @license     Copyright (C) 2012-2024, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK STM32F407 development board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  ：
 * version      data         notes
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#ifndef __TOUCH_H
#define __TOUCH_H

#include "main.h"

#define T_PEN_GPIO_PORT                 GPIOB
#define T_PEN_GPIO_PIN                  GPIO_PIN_1
#define T_PEN_GPIO_CLK_ENABLE()         do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define T_CS_GPIO_PORT                  GPIOC
#define T_CS_GPIO_PIN                   GPIO_PIN_13
#define T_CS_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)

#define T_MISO_GPIO_PORT                GPIOB
#define T_MISO_GPIO_PIN                 GPIO_PIN_2
#define T_MISO_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define T_MOSI_GPIO_PORT                GPIOF
#define T_MOSI_GPIO_PIN                 GPIO_PIN_11
#define T_MOSI_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

#define T_CLK_GPIO_PORT                 GPIOB
#define T_CLK_GPIO_PIN                  GPIO_PIN_0
#define T_CLK_GPIO_CLK_ENABLE()         do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)


#define T_MOSI(x)                   do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(T_MOSI_GPIO_PORT, T_MOSI_GPIO_PIN, GPIO_PIN_RESET);   \
                                    } while (0)
#define T_CLK(x)                    do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_SET):       \
                                        HAL_GPIO_WritePin(T_CLK_GPIO_PORT, T_CLK_GPIO_PIN, GPIO_PIN_RESET);     \
                                    } while (0)
#define T_CS(x)                     do { (x) ?                                                                  \
                                        HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_SET):         \
                                        HAL_GPIO_WritePin(T_CS_GPIO_PORT, T_CS_GPIO_PIN, GPIO_PIN_RESET);       \
                                    } while (0)
#define T_PEN                       ((HAL_GPIO_ReadPin(T_PEN_GPIO_PORT, T_PEN_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)
#define T_MISO                      ((HAL_GPIO_ReadPin(T_MISO_GPIO_PORT, T_MISO_GPIO_PIN) == GPIO_PIN_RESET) ? 0 : 1)

/* Touch-related Definitions */
#define TP_PRES_DOWN                0x8000  /* The touch screen is pressed */
#define TP_CATH_PRES                0x4000  /* There's a button pressed */
#define CT_MAX_TOUCH                10      /* The number of points supported by the capacitive screen is fixed to 5 points */

/* Touch screen control structure */
typedef struct
{
    uint8_t (*init)(void);      /* Initialize the touch screen controller */
    uint8_t (*scan)(uint8_t);   /* Scan touch screen, 0: screen scan; 1: Physical coordinates */
    void (*adjust)(void);       /* Touch screen calibration */
    uint16_t x[CT_MAX_TOUCH];   /* Current coordinates */
    uint16_t y[CT_MAX_TOUCH];   /* Capacitive screens have up to 10 sets of coordinates, while resistive screens use x[0] and y[0] to represent the coordinates of the touch screen at the time of the scan
                                 * Use x[9] and y[9] to store the coordinates of the first press
                                 */
    uint16_t sta;               /* State of pen״̬
                                 * b15:1: Press down; 0: Let go
                                 * b14:0: No key pressed; 1: There are keys to press
                                 * b13~b10: Reserved
                                 * b9~b0: the number of points pressed by the capacitive touch screen (0: not pressed; 1: Press)
                                 */
    /* 5-point calibration touch screen calibration parameters (capacitor screen does not need to be calibrated) */
    float xfac;                 /* X-direction scaling factor for 5-point calibration method */
    float yfac;                 /* Y-direction scaling factor for 5-point calibration method */
    short xc;                   /* Center X-coordinate physical value (AD value) */
    short yc;                   /* Center Y-coordinate physical value (AD value)*/
    uint8_t touchtype;          /* Center Y-coordinate physical value (AD value)
                                 * b0:0: Portrait (suitable for TP with left and right X coordinates and top and bottom Y coordinates)
                                 *     1: Landscape (suitable for the left and right Y coordinate, up and down for the X coordinate of TP)
                                 * b1~6: Keep
                                 * b7:0: Resistance screen
                                 *    1: Capacitive touch screen
                                 */
} _m_tp_dev;
extern _m_tp_dev tp_dev;


void tp_draw_big_point(uint16_t x, uint16_t y, uint16_t color);
void tp_save_adjust_data(void);
uint8_t tp_scan(uint8_t mode);
void tp_adjust(void);
uint8_t tp_init(void);

#endif
