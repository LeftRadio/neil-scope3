/**
  ******************************************************************************
  * @file	 	RTC.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 RTC header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Time Structure definition */
typedef struct
{
  uint8_t Seconds;
  uint8_t Minutes;
  uint8_t Hours;
} Time_s;

extern Time_s s_TimeStructVar;

/* Exported constants --------------------------------------------------------*/
#define SECONDS_IN_DAY 86399
#define CONFIGURATION_DONE 0xAAAA
#define CONFIGURATION_RESET 0x0000
#define DEFAULT_HOURS 0
#define DEFAULT_MINUTES 0
#define DEFAULT_SECONDS 0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RTC_User_Init(void);
void RTC_Configuration(void);
void RTC_NVIC_Configuration(void);
void Calculate_Time(void);				// получение времени



#endif /* RTC__H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
