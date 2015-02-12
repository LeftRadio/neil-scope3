/**
  ******************************************************************************
  * @file	 	RTC.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 RTC sourse
  ******************************************************************************
**/


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "RTC.h"

/* Private variables--------------------------------------------------------- */
Time_s s_TimeStructVar;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : RTC_User_Init
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_User_Init(void)
{
  /*Enables the clock to Backup and power interface peripherals    */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);

  NVIC_EnableIRQ(RTC_IRQn);
  NVIC_SetPriority(RTC_IRQn, 10);

  /* RTC Configuration*/
  RTC_Configuration();
}


/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
	/*Enables the clock to Backup and power interface peripherals    */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);

	/*Allow access to Backup Registers*/
	PWR_BackupAccessCmd(ENABLE);

	/* Backup Domain Reset */
	BKP_DeInit();
	BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_DONE);

	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);

	/* RTC Enabled */
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForLastTask();

	/*Wait for RTC registers synchronisation */
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();

	/* Setting RTC Interrupts-Seconds interrupt enabled */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1 sec */
	RTC_SetPrescaler(62250); /* RTC period  */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}


/*******************************************************************************
* Function Name  : Calculate_Time
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Calculate_Time(void)
{
	uint32_t TimeVar;

	TimeVar = RTC_GetCounter();
//	TimeVar = TimeVar % 86400;

	s_TimeStructVar.Hours = (uint8_t)(TimeVar / 3600);
	s_TimeStructVar.Minutes = (uint8_t)((TimeVar % 3600) / 60);
	s_TimeStructVar.Seconds = (uint8_t)((TimeVar % 3600) % 60);
}


