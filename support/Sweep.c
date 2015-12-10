/**
  ******************************************************************************
  * @file	 	Sweep.c
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Sweep.h"
#include "User_Interface.h"
#include "Synchronization.h"
#include "EPM570.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint32_t sweep_coff[] = {
	1, 2, 4, 8, 20, 40, 80, 200, 400, 800, 2000, 4000,
	8000, 20000, 40000, 80000, 200000, 400000, 800000, 2000000, 4000000
};

const uint8_t IntrlSweepScaleCoff[4] = { 1, 1, 2, 5 };
const uint8_t SweepScaleCoff[4] = { 1, 2, 5, 10 };
uint8_t SweepIndex = 0, ScaleIndex = 0;
uint8_t *SweepScale = (uint8_t*)&SweepScaleCoff[0];

extern btnINFO btnSWEEP;

/* Extern function -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Function_Name
  * @param  None
  * @retval None
  */
void Sweep_UpdateState(void)
{
	/* Select sweep scale and scale index */
	if(gOSC_MODE.Interleave == TRUE) SweepScale = (uint8_t*)&IntrlSweepScaleCoff[ScaleIndex];
	else SweepScale = (uint8_t*)&SweepScaleCoff[ScaleIndex];

	/* Set new number points and sweep values */
	gOSC_MODE.oscNumPoints = (gSamplesWin.WindowWidh * gSamplesWin.WindowsNum) / (*SweepScale);
	gSamplesWin.Sweep = sweep_coff[SweepIndex] - 1;

	/*  */
	EPM570_Set_Decimation(gSamplesWin.Sweep);
	gSyncState.foops->SetTrigg_X(trigPosX_cursor.Position - leftLimit);
}








/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
