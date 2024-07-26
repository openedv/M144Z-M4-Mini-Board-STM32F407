/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

_calendar_obj calendar;         /* Time structure */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

	/* Check whether the clock is configured for the first time */
    uint16_t bkpflag = 0;

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7F;
  hrtc.Init.SynchPrediv = 0xFF;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  bkpflag = rtc_read_bkr(0);  	/* Read the value of BKP0 */

  if ((bkpflag != 0X5051) && (bkpflag != 0x5050))   	/* Previously uninitialized, reconfigure */
  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 9;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  sDate.Month = RTC_MONTH_APRIL;
  sDate.Date = 10;
  sDate.Year = 24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  rtc_write_bkr(0, 0X5050);						/* Write 0x5050 so that the RTC initialization time is not repeated */
  }

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
* @brief 	RTC writes to back-up area SRAM
* @param 	bkrx: backup register number. The value ranges from 0 to 41
			It corresponds to RTC_BKP_DR1 to RTC_BKP_DR42
* @param 	data: Data to be written,16 bits
* @retval   None
*/
void rtc_write_bkr(uint32_t bkrx, uint16_t data)
{
    HAL_PWR_EnableBkUpAccess(); /* Disable write protection for the backup area */
    HAL_RTCEx_BKUPWrite(&hrtc, bkrx + 1, data);
}

/**
* @brief 	RTC reads the backup area SRAM
* @param 	bkrx: backup register number. The value ranges from 0 to 41
			It corresponds to RTC_BKP_DR1 to RTC_BKP_DR42
* @retval 	The value read
*/
uint16_t rtc_read_bkr(uint32_t bkrx)
{
    uint32_t temp = 0;
    temp = HAL_RTCEx_BKUPRead(&hrtc, bkrx + 1);
    return (uint16_t)temp; /* Returns the read value */
}

/**
 * @brief   Set RTC time information
 * @param   hour: Hour
 * @param   minute: Minute
 * @param   second: Second
 * @param   ampm: AM/PM
 *            - 0: AM
 *            - 1: PM
 * @retval  Setting result
 *            - 0: Success
 *            - 1: Failure
 */
uint8_t rtc_set_time(uint8_t hour, uint8_t minute, uint8_t second, uint8_t ampm)
{
    RTC_TimeTypeDef rtc_time_struct = {0};

    rtc_time_struct.Hours = hour;
    rtc_time_struct.Minutes = minute;
    rtc_time_struct.Seconds = second;
    rtc_time_struct.TimeFormat = ampm;
    rtc_time_struct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    rtc_time_struct.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &rtc_time_struct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief   Set RTC date information
 * @param   year: Year
 * @param   month: Month
 * @param   date: Day
 * @param   week: Weekday
 * @retval  Setting result
 *            - 0: Success
 *            - 1: Failure
 */
uint8_t rtc_set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef rtc_date_struct = {0};

    rtc_date_struct.WeekDay = week;
    rtc_date_struct.Month = month;
    rtc_date_struct.Date = date;
    rtc_date_struct.Year = year;
    if (HAL_RTC_SetDate(&hrtc, &rtc_date_struct, RTC_FORMAT_BIN) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief   Set RTC alarm time information
 * @param   week: Weekday
 * @param   hour: Hour
 * @param   minute: Minute
 * @param   second: Second
 * @retval  None
 */
void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t minute, uint8_t second)
{
    RTC_AlarmTypeDef rtc_alarm_struct = {0};

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    /* Set alarm interrupt */
    rtc_alarm_struct.AlarmTime.Hours = hour;
    rtc_alarm_struct.AlarmTime.Minutes = minute;
    rtc_alarm_struct.AlarmTime.Seconds = second;
    rtc_alarm_struct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
    rtc_alarm_struct.AlarmTime.SubSeconds = 0;
    rtc_alarm_struct.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_alarm_struct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_alarm_struct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_alarm_struct.AlarmDateWeekDay = week;
    rtc_alarm_struct.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&hrtc, &rtc_alarm_struct, RTC_FORMAT_BIN);
}

/**
 * @brief   Get RTC time information
 * @param   hour: Hour
 * @param   minute: Minute
 * @param   second: Second
 * @param   ampm: AM/PM
 *            - 0: AM
 *            - 1: PM
 * @retval  None
 */
void rtc_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second, uint8_t *ampm)
{
    RTC_TimeTypeDef rtc_time_struct = {0};

    HAL_RTC_GetTime(&hrtc, &rtc_time_struct, RTC_FORMAT_BIN);

    *hour = rtc_time_struct.Hours;
    *minute = rtc_time_struct.Minutes;
    *second = rtc_time_struct.Seconds;
    *ampm = rtc_time_struct.TimeFormat;
}

/**
 * @brief   Get RTC date information
 * @param   year: Year
 * @param   month: Month
 * @param   date: Day
 * @param   week: Weekday
 * @retval  None
 */
void rtc_get_date(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *week)
{
    RTC_DateTypeDef rtc_date_struct = {0};

    HAL_RTC_GetDate(&hrtc, &rtc_date_struct, RTC_FORMAT_BIN);

    *year = rtc_date_struct.Year;
    *month = rtc_date_struct.Month;
    *date = rtc_date_struct.Date;
    *week = rtc_date_struct.WeekDay;
}

/**
* @brief 	Converts year, month, day, hour, minute and second into seconds
* @note 	Enter the Gregorian calendar date to get the week (start time is: March 1, AD 0, enter any date later, you can get the correct week)
* Using 	The Kim Larson calculation formula, the principle is explained in this post:
*              https://www.cnblogs.com/fengbohello/p/3264300.html
* @param 	syear	: year
* @param 	smon	: Month
* @param 	sday	: Date
* @retval 	0, Sunday; 1 to 6: Monday to Saturday
*/
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week = 0;

    if (month < 3)
    {
        month += 12;
        --year;
    }

    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}

/**
 * @brief       Get the current time
 * @note      	This function does not return the time directly; the time data is stored in the calendar structure
 * @param       None
 * @retval      None
 */
void hal_rtc_get_tim(void)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  calendar.hour = stimestructureget.Hours;                                   	/* Hour */
  calendar.min = stimestructureget.Minutes;                                 	/* Minute */
  calendar.sec = stimestructureget.Seconds;                                 	/* Second */
  calendar.year = 2000 + sdatestructureget.Year;								/* Year */
  calendar.month = sdatestructureget.Month;										/* Month */
  calendar.date = sdatestructureget.Date;										/* Date */
  calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);	/* Week */
}

/* USER CODE END 1 */
