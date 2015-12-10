/**
  ******************************************************************************
  * @file	 	Synchronization.c
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "EPM570.h"
#include "Processing_and_output.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Sync_StateUpdate(void);
static void Sync_StateReset(void);
static uint32_t Sync_Calc_WriteNumPoints(uint32_t NumPoints, uint8_t PreTiggCoeff, uint8_t PostTrigCoeff);
static uint32_t Sync_Calc_ReadNumPoints(uint32_t WinRegVal, uint8_t PostTrigCoeff);
static void Sync_SetTriggCursor_X(uint32_t NewPosition);
static void Sync_delay_pre_trigger(void);

/* Private variables ---------------------------------------------------------*/
const Foops_TypeDef syncfoops = {

		Sync_StateUpdate,
		Sync_StateReset,
		Sync_Calc_WriteNumPoints,
		Sync_Calc_ReadNumPoints,
		Sync_SetTriggCursor_X,
		Sync_delay_pre_trigger
};

/* Global syncrhonization state var */
volatile SyncGlobalState_TypeDef gSyncState = {

		CHANNEL_A,		// channel sourse - А, B, LA
		Sync_NONE,		// mode
		Sync_Rise,		// type
		1,				// pre-trigger time in us
		1,				// X trigger cursor

		&syncfoops,
};

/* Extern function -----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Synchronization_StateUpdate
  * @param  None
  * @retval None
  */
static void Sync_StateUpdate(void)
{
	EPM570_Update_SynchronizationMode(gSyncState.Sourse, gSyncState.Mode, gSyncState.Type);
}


/**
  * @brief  Sync_StateReset
  * @param  None
  * @retval None
  */
static void Sync_StateReset(void)
{
	EPM570_Update_SynchronizationMode(CHANNEL_A, Sync_NONE, Sync_Rise);
}


/**
  * @brief  Synchronization_Calc_NumPoints
  * @param
  * @retval None
  */
static uint32_t Sync_Calc_WriteNumPoints(uint32_t NumPoints, uint8_t PreTiggCoeff, uint8_t PostTrigCoeff)
{
	uint32_t regVal = NumPoints;
	gSyncState.preTriggerTime_us = 2;

	/* If synchronization is ON in any mode */
	if(gSyncState.Mode != Sync_NONE)
	{
		/* Calc pre-trigger time
		 *    preTrigger samples num = SyncCursor_X.position;
		 *    preTrigger_time_ns = (preTrigger samples num) * (decimation coeff) * 10нс
		 *    preTrigger_time_us = preTrigger_time_ns / 1000
		 */
		gSyncState.preTriggerTime_us = (gSyncState.Cursor_X * EPM570_Get_Decimation() * PreTiggCoeff) / 100;
		if(gSyncState.preTriggerTime_us < 2) gSyncState.preTriggerTime_us = 2;

		/* Calc post trigger num points */
		regVal = (NumPoints - (gSyncState.Cursor_X / (*SweepScale))) / PostTrigCoeff;
	}
	else
	{
		regVal += 10;
	}

	return regVal;
}


/**
  * @brief  Synchronization_Calc_ReadNumPoints
  * @param
  * @retval None
  */
uint32_t Sync_Calc_ReadNumPoints(uint32_t WinRegVal, uint8_t PostTrigCoeff)
{
	/* If synchronization is ON in any mode */
	if(gSyncState.Mode != Sync_NONE)
	{
		WinRegVal += gSyncState.Cursor_X / ((*SweepScale) * PostTrigCoeff);
	}
	else
	{
		WinRegVal -= 10;
	}

	return WinRegVal;
}


/**
  * @brief  Synchronization_SetTriggCursor_X
  * @param  None
  * @retval None
  */
void Sync_SetTriggCursor_X(uint32_t NewPosition)
{
	uint32_t max_X = (gOSC_MODE.oscNumPoints * (*SweepScale)) - 1;

	/* Save new position */
	gSyncState.Cursor_X = NewPosition;

	/* Clip cursor position */
	if(gSyncState.Cursor_X < 1) gSyncState.Cursor_X = 1;
	else if(gSyncState.Cursor_X > max_X) gSyncState.Cursor_X = max_X;

	/* Update num points */
	EPM570_Set_numPoints(gOSC_MODE.oscNumPoints);
}


/**
  * @brief  Synchronization_delay_pre_trigger
  * @param  None
  * @retval None
  */
void Sync_delay_pre_trigger(void)
{
	uint32_t preTrig_us = gSyncState.preTriggerTime_us;
	uint16_t i = 0;

	while(preTrig_us > 1000)
	{
		preTrig_us -= 1000;
		i++;
	}
	if(preTrig_us < 2) preTrig_us = 2;

	if(i > 0) delay_ms(i);
	delay_us(preTrig_us);
}






/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
