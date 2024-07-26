/**
 ****************************************************************************************************
 * @file        delay.c
 * @author      ALIENTEK
 * @brief
 * @license     Copyright (C) 2020-2032, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform  	: ALIENTEK STM32F103 development board
 * website		: https://www.alientek.com
 * forum		: http://www.openedv.com/forum.php
 *
 * change logs	:
 * version		data		notes
 * V1.0			20240410	the first version
 *
 ****************************************************************************************************
 */

#include "delay.h"


static uint32_t g_fac_us = 0;       /* Multiplication factor for us delay */

/* If SYS_SUPPORT_OS is defined, it means OS support is enabled (not limited to uC/OS) */
#if SYS_SUPPORT_OS

/* Include the common header file (required for uC/OS) */
#include "os.h"

/* Define the g_fac_ms variable, representing the multiplication factor for ms delay,
   indicating the number of ms per tick (only required when enabling OS support) */
static uint16_t g_fac_ms = 0;

/*
 * When delay_us/delay_ms needs to support an OS, three OS-related macros and functions are required:
 * First, three macros:
 *     delay_osrunning    : Used to indicate whether the OS is currently running, determining whether relevant functions can be used
 *     delay_ostickspersec: Used to indicate the clock ticks set by the OS, delay_init will initialize SysTick based on this parameter
 *     delay_osintnesting : Used to indicate the nesting level of interrupts in the OS, because scheduling is not allowed inside interrupts, delay_ms uses this parameter to determine how to operate
 * Then, three functions:
 *     delay_osschedlock  : Used to lock OS task scheduling, disabling scheduling
 *     delay_osschedunlock: Used to unlock OS task scheduling, re-enabling scheduling
 *     delay_ostimedly    : Used for OS delay, can cause task scheduling.
 *
 * This example only supports uC/OS-II, for other OSes, please refer to this for porting.
 */

/* Support for uC/OS-II */
#define delay_osrunning     OSRunning           /* OS running flag, 0: not running; 1: running */
#define delay_ostickspersec OS_TICKS_PER_SEC    /* OS clock ticks, i.e., scheduling frequency per second */
#define delay_osintnesting  OSIntNesting        /* Interrupt nesting level, i.e., number of interrupt nestings */



/**
 * @brief     Disable task scheduling during us-level delay (to prevent interruption of us-level delay)
 * @param     None
 * @retval    None
 */
void delay_osschedlock(void)
{
    OSSchedLock();  /* Disable scheduling in the UCOSII manner to prevent interruption of us-level delay */
}

/**
 * @brief     Restore task scheduling during us-level delay
 * @param     None
 * @retval    None
 */
void delay_osschedunlock(void)
{
    OSSchedUnlock();  /* Restore scheduling in the UCOSII manner */
}

/**
 * @brief     Delay at us-level, resume task scheduling
 * @param     ticks: Number of ticks to delay
 * @retval    None
 */
void delay_ostimedly(uint32_t ticks)
{
    OSTimeDly(ticks);  /* UCOSII delay */
}

/**
 * @brief     SysTick interrupt service routine, used when an OS is employed
 * @param     ticks : Number of ticks to delay
 * @retval    None
 */  
void SysTick_Handler(void)
{
    /* Execute normal scheduling processing only when the OS is running */
    if (delay_osrunning == OS_TRUE)
    {
        /* Call the SysTick interrupt service function of uC/OS-II */
        OS_CPU_SysTickHandler();
    }
    HAL_IncTick(); /* Increment HAL tick */
}
#endif

/**
 * @brief     Initialize delay function
 * @param     sysclk: System clock frequency, i.e., CPU frequency (rcc_c_ck), 168MHz
 * @retval    None
 */  
void delay_init(uint16_t sysclk)
{
#if SYS_SUPPORT_OS                                      /* If OS support is needed */
    uint32_t reload;
#endif
    g_fac_us = sysclk;                                  /* As SysTick has been configured in HAL_Init, no need to reconfigure here */
#if SYS_SUPPORT_OS                                      /* If OS support is needed */
    reload = sysclk;                                    /* Number of counts per second, in Mega */
    reload *= 1000000 / delay_ostickspersec;            /* Calculate reload value based on delay_ostickspersec, reload is 24-bit
                                                         * maximum value: 16777216, approximately 0.09986s at 168M
                                                         */
    g_fac_ms = 1000 / delay_ostickspersec;              /* Represents the smallest unit OS can delay */
    SysTick->CTRL |= 1 << 1;                            /* Enable SYSTICK interrupt */
    SysTick->LOAD = reload;                             /* Interrupt every 1/delay_ostickspersec seconds */
    SysTick->CTRL |= 1 << 0;                            /* Enable SYSTICK */
#endif 
}

/**
 * @brief     Delay in microseconds (us)
 * @note      Regardless of whether OS is used, clock counting method is used for us delay
 * @param     nus: Number of microseconds to delay
 * @note      nus range: 0 ~ (2^32 / fac_us) (fac_us is generally equal to system main frequency, calculate accordingly)
 * @retval    None
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;        /* Value of LOAD */
    ticks = nus * g_fac_us;                 /* Number of ticks needed */
    
#if SYS_SUPPORT_OS                          /* If OS support is needed */
    delay_osschedlock();                    /* Lock OS task scheduler */
#endif

    told = SysTick->VAL;                    /* Initial counter value */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;        /* Note: SYSTICK is a decrementing counter */
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) 
            {
                break;                      /* Exit if time exceeds or equals the delay time */
            }
        }
    }

#if SYS_SUPPORT_OS                          /* If OS support is needed */
    delay_osschedunlock();                  /* Restore OS task scheduler */
#endif 

}

/**
* @brief Delay nms
* @param nms: The number of ms to delay (0< nms <= (2^32 / fac_us / 1000))(fac_us is generally equal to the system frequency, self-embedding calculation)
* @retval None
*/
void delay_ms(uint16_t nms)
{
    
#if SYS_SUPPORT_OS  									/* If OS support is needed, call OS delay accordingly to release CPU */
    if (delay_osrunning && delay_osintnesting == 0)     /* If the OS is running and not in an interrupt (no task scheduling in interrupts) */
    {
        if (nms >= g_fac_ms)                            /* If the delay time is greater than the OS's minimum time period */
        {
            delay_ostimedly(nms / g_fac_ms);            /* OS delay */
        }

        nms %= g_fac_ms;                                /* OS cannot provide such a small delay, so use normal delay method */
    }
#endif

    delay_us((uint32_t)(nms * 1000));                   /* Normal delay method */
}
