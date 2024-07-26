/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      ALIENTEK
 * @brief       lcd code
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
 * V1.0         20240409     the first version
 *
 ****************************************************************************************************
 */

#ifndef BSP_LCD_LCD_H_
#define BSP_LCD_LCD_H_

#include "main.h"
#include "stdlib.h"


/* LCD backlight control */
#define LCD_BL(x)                   do { (x) ?                                                             \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET):     \
                                        HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);   \
                                    } while (0)

/* Definition of FSMC-related parameters
 * Note: By default, we connect the LCD through FSMC block 1, which has 4 chip choices: FSMC_NE1~4
 *
 * Modify LCD_FSMC_NEX, The corresponding LCD_CS_GPIO related Settings also need to be changed
 * Modify LCD_FSMC_AX , The corresponding LCD_RS_GPIO related Settings also need to be changed
 */
#define LCD_FSMC_NEX         4              /* When FSMC_NE1 is connected to LCD_CS, the value range can only be: 1~4 */
#define LCD_FSMC_AX          6              /* Use FSMC_A19 connected to LCD_RS, the value range is: 0 ~ 25 */

/* LCD address structure */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE                    (uint32_t)((0x60000000 + (0x4000000 * (LCD_FSMC_NEX - 1))) | (((1 << LCD_FSMC_AX) * 2) - 2))
#define LCD                         ((LCD_TypeDef *)LCD_BASE)

/* LCD important parameter set */
typedef struct
{
    uint16_t width;     /* LCD width */
    uint16_t height;    /* LCD height */
    uint16_t id;        /* LCD ID */
    uint8_t dir;        /* Landscape or portrait control: 0, portrait; 1. Landscape. */
    uint16_t wramcmd;   /* Start writing gram instructions */
    uint16_t setxcmd;   /* Set the x-coordinate command */
    uint16_t setycmd;   /* Set the y-coordinate command */
} _lcd_dev;

/* LCD parameters */
extern _lcd_dev lcddev; /* ���ڹ���LCD��Ҫ�Ĳ��� */

/******************************************************************************************/
/* LCD scan direction and color definition */

/* Scan direction definition */
#define L2R_U2D         0           /* From left to right, from top to bottom */
#define L2R_D2U         1           /* From left to right, from bottom to top */
#define R2L_U2D         2           /* From right to left, from top to bottom */
#define R2L_D2U         3           /* From right to left, from bottom to top */
#define U2D_L2R         4           /* From top to bottom, left to right */
#define U2D_R2L         5           /* From top to bottom, right to left */
#define D2U_L2R         6           /* From bottom to top, left to right */
#define D2U_R2L         7           /* From bottom to top, right to left */
#define DFT_SCAN_DIR    L2R_U2D     /* Default scan direction */

#define WHITE           0xFFFF      /* White */
#define BLACK           0x0000      /* Black */
#define RED             0xF800      /* Red */
#define GREEN           0x07E0      /* Green */
#define BLUE            0x001F      /* Blue */
#define MAGENTA         0XF81F      /* Magenta\fuchsia = BLUE + RED */
#define YELLOW          0XFFE0      /* Yellow = GREEN + RED */
#define CYAN            0X07FF      /* Cyan = GREEN + BLUE */
#define BROWN           0XBC40      /* Brown */
#define BRRED           0XFC07      /* Brown red */
#define GRAY            0X8430      /* Gray */
#define DARKBLUE        0X01CF      /* Dark blue */
#define LIGHTBLUE       0X7D7C      /* Light blue */
#define GRAYBLUE        0X5458      /* Grey blue */
#define LIGHTGREEN      0X841F      /* Light green */
#define LGRAY           0XC618      /* Light gray (PANNEL), the form background color */
#define LGRAYBLUE       0XA651      /* Light gray blue (middle layer color) */
#define LBBLUE          0X2B12      /* Light brown blue (the reverse color of selected items) */

/******************************************************************************************/
/* SSD1963 related configuration parameters (generally do not need to change) */

/* LCD resolution setting */
#define SSD_HOR_RESOLUTION      800     /* LCD horizontal resolution */
#define SSD_VER_RESOLUTION      480     /* LCD vertical resolution */

/* LCD driver parameter setting */
#define SSD_HOR_PULSE_WIDTH     1       /* Horizontal pulse width */
#define SSD_HOR_BACK_PORCH      46      /* Horizontal front porch */
#define SSD_HOR_FRONT_PORCH     210     /* Horizontal rear gallery */

#define SSD_VER_PULSE_WIDTH     1       /* Vertical pulse width */
#define SSD_VER_BACK_PORCH      23      /* Vertical front porch */
#define SSD_VER_FRONT_PORCH     22      /* Vertical rear porch */
#define SSD_HT                      (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS                     (SSD_HOR_BACK_PORCH)
#define SSD_VT                      (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS                     (SSD_VER_BACK_PORCH)

/* LCD parameters */
extern uint32_t  g_point_color;
extern uint32_t g_back_color;               /* Background color. White by default */

/******************************************************************************************/
/* Function declaration */

void lcd_wr_data(volatile uint16_t data);            /* LCD write data */
void lcd_wr_regno(volatile uint16_t regno);          /* LCD write register number/address */
void lcd_write_reg(uint16_t regno, uint16_t data);   /* The LCD writes the value of the register */


void lcd_init(void);                        /* Initialize LCD */
void lcd_display_on(void);                  /* Open display */
void lcd_display_off(void);                 /* Off display */
void lcd_scan_dir(uint8_t dir);             /* Set screen scan direction */
void lcd_display_dir(uint8_t dir);          /* Sets the screen orientation */
void lcd_ssd_backlight_set(uint8_t pwm);    /* SSD1963 backlight control */

void lcd_write_ram_prepare(void);               /* Get some grams */
void lcd_set_cursor(uint16_t x, uint16_t y);    /* Setting the cursor */
uint32_t lcd_read_point(uint16_t x, uint16_t y);/* Read point   */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);/* Draw point */

void lcd_clear(uint16_t color);                /* LCD clear screen */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                   /* Fill the solid circle */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                  /* Draw a circle */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                  /* Draw horizontal lines */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);             /* Setting the window */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);          /* The solid color fills the rectangle */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);   /* Colored filled rectangle */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);     /* Draw a straight line */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);/* Draw the rectangle */


#endif
