/**
  ******************************************************************************
  * @file	 	EPM570.c
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.5.5
  * @date		27.04.2015
  * @brief		sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <math.h>

#include "main.h"
#include "Host.h"
#include "init.h"
#include "systick.h"
#include "Analog.h"
#include "EPM570.h"
#include "EPM570_Registers.h"
#include "EPM570_GPIO.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "AD9288.h"
#include "Processing_and_output.h"
#include "Settings.h"
#include "User_Interface.h"
#include "IQueue.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum { Standby_BothCH, Standby_CH_B, NormalOperation, AlignOperation } AD9288_State_TypeDef;

/* Private define ------------------------------------------------------------*/
/* Private varibles ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//uint32_t preTrigger_time = 1;
extern uint8_t IQueue_WorkIndex;

/* Exported variables --------------------------------------------------------*/
__IO int32_t TimeoutCnt;
__IO FlagStatus SRAM_TimeoutState = RESET;
__IO State_TypeDef wrSRAM = STOP;
__IO int8_t InterliveCorrectionCoeff = 0;
__IO uint8_t LA_Diff_MASK = 0, LA_Cond_MASK = 0;
FunctionalState RLE_State = DISABLE;

/* Private function prototypes -----------------------------------------------*/
/* Function -------------------------------------------------------------------*/

/**
  * @brief  EPM570_Signals_Init
  * @param  None
  * @retval None
  */
void EPM570_Signals_Init(void)
{
	EPM570_GPIO_Configuration();
}


/**
  * @brief  EPM570_Registers_Init
  * @param  None
  * @retval EPM_ErrorStatus
  */
EPM_ErrorStatus EPM570_Registers_Init(void)
{
	/* Reset registers control signals */
	EPM570_GPIO_ResetCntrlSignals();

	/* Write init values to registers */
	EPM570_Set_Decimation(0x00);
	EPM570_Set_numPoints(0x190);
		
	EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);
	EPM570_Registers_WriteReg(EPM570_Register_cnfPin_B.address, EPM570_Register_cnfPin.bits);
	EPM570_Registers_WriteReg(EPM570_Register_extPin0.address, EPM570_Register_extPin0.bits);
	EPM570_Registers_WriteReg(EPM570_Register_extPin_1.address, EPM570_Register_extPin_1.bits);
	EPM570_Registers_WriteReg(EPM570_Register_WriteControl.address, EPM570_Register_WriteControl.bits);
	
	/* Read and verify values */
	if(EPM570_Get_Decimation() != 0x00) return ERROR_D;
	if(EPM570_Get_numPoints() != 0x190) return ERROR_N;

	return eSUCCESS;
}



/* -----------------------------------------  Decimation, num points(samples) to write/read functions ----------------------------------------- */

/**
  * @brief  Set_Decimation
  * @param  New decimation value
  * @retval None
  */
void EPM570_Set_Decimation(uint32_t value)
{
	EPM570_Register_Decim.u32_data = value;
	EPM570_Registers_WriteMultByte(&EPM570_Register_Decim);
}


/**
  * @brief  Get_Decimation
  * @param  None
  * @retval Decimation value
  */
uint32_t EPM570_Get_Decimation(void)
{
	EPM570_Registers_ReadMultByte(&EPM570_Register_Decim);
	return EPM570_Register_Decim.u32_data;
}


/**
  * @brief  Set_numPoints
  * @param  NumPoints to write/read cycle
  * @retval None
  */
void EPM570_Set_numPoints(uint32_t NumPoints)
{   
	uint32_t regVal = NumPoints;
	uint8_t Pre_TriggNumPoints_Coeff = ActiveMode->oscNumPoints_Ratio;
	uint8_t Post_TriggNumPoints_Coeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;

	/* Calc post trigger num points and pre-trigger time*/
	regVal = gSyncState.foops->Calc_WriteNumPoints(NumPoints, Pre_TriggNumPoints_Coeff, Post_TriggNumPoints_Coeff);

	/* Write sample window register */
	EPM570_Register_WIN.u32_data = regVal;
	EPM570_Registers_WriteMultByte(&EPM570_Register_WIN);
}


/**
  * @brief  Get_numPoints
  * @param  None
  * @retval NumPoints to write/read cycle
  */
uint32_t EPM570_Get_numPoints(void)
{
	uint8_t Post_TriggNumPoints_Coeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;

	/* Calc num points and return */
	return gSyncState.foops->Calc_ReadNumPoints(
			EPM570_Registers_ReadMultByte(&EPM570_Register_WIN),
			Post_TriggNumPoints_Coeff );
}



/* -----------------------------------------  Write/Read SRAM memory support functions ----------------------------------------- */

/**
  * @brief  EPM570_AD9288_SetState
  * @param  NewState
  * @retval None
  */
static void EPM570_Set_AD9288_State(AD9288_State_TypeDef NewState)
{
	switch(NewState)
	{
	case Standby_BothCH :
		EPM570_Register_extPin0.bit.S1 = 0;
		EPM570_Register_extPin0.bit.S2 = 0;
		break;
	case Standby_CH_B :
		EPM570_Register_extPin0.bit.S1 = 0;
		EPM570_Register_extPin0.bit.S2 = 1;
		break;
	case NormalOperation:
		EPM570_Register_extPin0.bit.S1 = 1;
		EPM570_Register_extPin0.bit.S2 = 0;
		break;
	case AlignOperation :
		EPM570_Register_extPin0.bit.S1 = 1;
		EPM570_Register_extPin0.bit.S2 = 1;
		break;

	default : break;
	}

	EPM570_Registers_WriteReg(EPM570_Register_extPin0.address, EPM570_Register_extPin0.bits);
}


/**
  * @brief  SRAM_TimeoutCounterSetState
  * @param  NewState
  * @retval None
  */
static void EPM570_SRAM_TimeoutCounterSetState(FunctionalState NewState, uint32_t count)
{
	SRAM_TimeoutState = RESET;
	TimeoutCnt = count;

	TIM1->CNT = 0;
	if(NewState == ENABLE) TIM1->CR1 |= TIM_CR1_CEN;
	else TIM1->CR1 &= ~TIM_CR1_CEN;


}


/**
  * @brief  SRAM_SetWriteState
  * @param  NewState
  * @retval None
  */
void EPM570_SRAM_SetWriteState(State_TypeDef NewState)
{
	wrSRAM = NewState;
}

/**
  * @brief  SRAM_GetWriteState
  * @param  None
  * @retval state
  */
State_TypeDef EPM570_SRAM_GetWriteState(void)
{
	return wrSRAM;
}


/**
  * @brief  Write_SRAM
  * @param  None
  * @retval state
  */
State_TypeDef EPM570_SRAM_Write(void)
{   
	const uint32_t TimeoutTime_ms = 1000;
	static uint16_t tColor = White, tFlag = 0;
	SyncMode_TypeDef  tSyncMode = gSyncState.Mode;

	/* Set write start cycle flag */
	wrSRAM = RUN;

	/* Interlive mode or not */
	Analog_InteliveMode(gOSC_MODE.Interleave);

	/* Sync mode update */
	gSyncState.foops->StateUpdate();

	/* Calculate needed timeout counter in ms */
//	TimeoutTime_ms = 200 + ((EPM570_Get_numPoints() * EPM570_Get_Decimation() * 10 * ActiveMode->oscNumPoints_Ratio) / (1000000 * (*SweepScale)) );
//	if(TimeoutTime_ms > 1000) TimeoutTime_ms = 1000;
//	TimeoutTime_ms = 1000;

	/* Enable Oscillator KXO97 */
	EPM570_Register_extPin_1.bit.OSC_EN = 1;
	EPM570_Registers_WriteReg(EPM570_Register_extPin_1.address, EPM570_Register_extPin_1.bits);

	/* Enable OA, ADC */
	if(pINFO != &DINFO_A)
	{
		Control_AD8129(ENABLE);
		EPM570_Set_AD9288_State(AlignOperation);
	}

	/* Stabilization delay */
	delay_ms(1);

	/* start write in CPLD */
	EPM570_Register_WriteControl.bit.Start_Write = 1;
	EPM570_Registers_WriteReg(EPM570_Register_WriteControl.address, EPM570_Register_WriteControl.bits);

	/* delay for write pre-trigger data */
	gSyncState.foops->PreTrigger_Delay();

	/* Enable trigger event in CPLD */
	EPM570_Register_WriteControl.bit.Trigger_EN = 1;
	EPM570_Registers_WriteReg(EPM570_Register_WriteControl.address, EPM570_Register_WriteControl.bits);

	/* Start timeout timer */
	EPM570_SRAM_TimeoutCounterSetState(ENABLE, TimeoutTime_ms);

	/* Wait write end, or interrupt write cycle */
	while((EPM570_Write_Ready == 0) && (wrSRAM != STOP))  //
	{
		if((HostMode == ENABLE) && (Host_IQueue_Get_CommandsCount() > 1))
		{
			break;
		}

		if(SRAM_TimeoutState == SET)
		{
			if(gSyncState.Mode == Sync_AUTO)
			{
				gSyncState.Mode = Sync_NONE;
				gSyncState.foops->StateUpdate();
				tColor = Yellow;
				break;
			}
			else tColor = White;

			if(tFlag == 0) UI_SamplingSetTimeout(tColor);
			else UI_SamplingSetTimeout(LightBlack);
			tFlag ^= 1;

			/* Restart timeout timer */
			EPM570_SRAM_TimeoutCounterSetState(ENABLE, TimeoutTime_ms);
		}
	}

	/* Disable timeout timer */
	EPM570_SRAM_TimeoutCounterSetState(DISABLE, 0);

	/* Fiirst clear Start_Write and disable trigger for correct end write cycle */
	EPM570_Register_WriteControl.bits = 0x00;
	EPM570_Registers_WriteReg(EPM570_Register_WriteControl.address, EPM570_Register_WriteControl.bits);

	/* Disable OA and ADC */
	Control_AD8129(DISABLE);
	EPM570_Set_AD9288_State(Standby_BothCH);

	/* Disable oscillator KXO97, only after write to Write_Control register */
	EPM570_Register_extPin_1.bit.OSC_EN = 0;
	EPM570_Registers_WriteReg(EPM570_Register_extPin_1.address, EPM570_Register_extPin_1.bits);

	/* Update synchronization state */
	gSyncState.Mode = tSyncMode;

	if(wrSRAM != STOP)
	{
		if(SRAM_TimeoutState != SET)
		{
			UI_SamplingClearTimeout();
			wrSRAM = COMPLETE;
		}
		else if(gSyncState.Mode == Sync_AUTO)
		{
			wrSRAM = COMPLETE;
		}
	}

	return wrSRAM;
}


/**
  * @brief  SRAM_ReadDirection
  * @param  UP or DOWN direction flag
  * @retval None
  */
void EPM570_SRAM_ReadDirection(uint8_t UP_DOWN)
{
	if(UP_DOWN == SRAM_READ_UP) EPM570_Register_cnfPin.bit.Read_SRAM_UP = 1;
	else EPM570_Register_cnfPin.bit.Read_SRAM_UP = 0;

	EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);
}


/**
  * @brief  EPM570_SRAM_ReadState
  * @param  NewState
  * @retval None
  */
void EPM570_SRAM_ReadState(FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		/* Read Counter Enable */
		EPM570_Register_cnfPin.bit.ReadCounterEN = 1;
		EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);

		/* Switch in EPM570 SRAM data bus to MCU bus */
		EPM570_Registers_ReadReg(SRAM_DATA);

		/* Read enable */
		EPM570_GPIO_RD(SET);
	}
	else
	{
		/* Read Counter Disable */
		EPM570_Register_cnfPin.bit.ReadCounterEN = 0;
		EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);

		/* Read disable */
		EPM570_GPIO_RD(RESET);
	}
}


/**
  * @brief  EPM570_SRAM_Shift
  * @param
  * @retval None
  */
void EPM570_SRAM_Shift(int32_t cnt, uint8_t UP_DOWN)
{
	EPM570_SRAM_ReadDirection(UP_DOWN);

	if((cnt > 0) || (gSyncState.Mode != Sync_NONE))
	{
		if(cnt == 0x7fffffff)
		{
			EPM570_Register_cnfPin_B.bit.ReadCounter_sLoad = 1;
			EPM570_Registers_WriteReg(EPM570_Register_cnfPin_B.address, EPM570_Register_cnfPin_B.bits);

			EPM570_RS_set; EPM570_RS_clr;

			EPM570_Register_cnfPin_B.bit.ReadCounter_sLoad = 0;
			EPM570_Registers_WriteReg(EPM570_Register_cnfPin_B.address, EPM570_Register_cnfPin_B.bits);
		}
		else
		{
			if(gSyncState.Mode != Sync_NONE)
			{
				if(gSamplesWin.Sweep != 0) cnt += 2;
				else cnt += 4;
			}


			EPM570_SRAM_ReadState(ENABLE);
			while(cnt-- > 0)
			{
				EPM570_RS_set; EPM570_RS_set;
				EPM570_RS_clr; EPM570_RS_clr;

				if((ActiveMode != &IntMIN_MAX) && (gSamplesWin.Sweep != 0))
				{
					EPM570_RS_set; EPM570_RS_set;
					EPM570_RS_clr; EPM570_RS_clr;
				}
			}
			EPM570_SRAM_ReadState(DISABLE);
		}
	}
}


/**
  * @brief  EPM570_Read_SRAM
  * @param  None
  * @retval None
  */
void EPM570_SRAM_Read(void)
{  
	int32_t SamplesWindow, VisibleSamples;
	int32_t sCNT, StarPointRead = 0;
	uint8_t InterliveCoeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;

	SamplesWindow = EPM570_Get_numPoints() * ActiveMode->oscNumPoints_Ratio * InterliveCoeff;
	VisibleSamples = (((rightLimit - leftLimit) - 1) * ActiveMode->oscNumPoints_Ratio) / (*SweepScale);
	StarPointRead = ((SamplesWindow - ((gSamplesWin.WindowPosition + 1) * VisibleSamples)) - 0) / InterliveCoeff;

	NVIC_DisableIRQ(TIM2_IRQn);	  		// запрет прерываний таймера опроса кнопок

	/* Reset read state */
	EPM570_SRAM_ReadState(DISABLE);

	/* Roll back and shift to read data */
	EPM570_SRAM_Shift(0x7fffffff, SRAM_READ_DOWN);
	EPM570_SRAM_Shift(StarPointRead, SRAM_READ_DOWN);

	/* Enable read state for capture data */
	EPM570_SRAM_ReadState(ENABLE);

	if(ActiveMode == &IntMIN_MAX)
	{
		EPM570_RS_set; EPM570_RS_set;
		EPM570_RS_clr; EPM570_RS_clr;
	}

	/* Read Data */
	sCNT = VisibleSamples;
	if(gOSC_MODE.Interleave == TRUE)
	{
		do
		{
			/* Read data CH A */
			EPM570_RS_set; EPM570_RS_set;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);
			sCNT--;

			EPM570_RS_clr; EPM570_RS_clr;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);
			INFO_A.DATA[sCNT] -= InterliveCorrectionCoeff;
			sCNT--;

		}
		while(sCNT > 0);
	}
	else
	{
		do
		{
			/* Read data CH A */
			EPM570_RS_set; EPM570_RS_set;
			INFO_A.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);

			/* Read data CH B */
			EPM570_RS_clr; EPM570_RS_clr;
			INFO_B.DATA[sCNT] = ~((GPIOB->IDR >> 8) - 127);

			if((ActiveMode != &IntMIN_MAX) && (gSamplesWin.Sweep != 0))
			{
				EPM570_RS_set; EPM570_RS_set;
				EPM570_RS_clr; EPM570_RS_clr;
			}
		}
		while(sCNT-- > 0);
	}

	/* Reset read state */
	EPM570_SRAM_ReadState(DISABLE);



	/* Enable interupts for read buttons */
	NVIC_EnableIRQ(TIM2_IRQn);	   // разрешение прерываний таймера опроса кнопок
}



/* -----------------------------------------  Synchronization support functions ----------------------------------------- */

/**
  * @brief  Set_Trigger
  * @param
  * @retval None
  */
static void EPM570_Set_Trigger(Channel_ID_TypeDef Sourse, SyncAType_TypeDef Type)
{    
	int16_t *pCursor;
	uint8_t Height_Pos = Height_Y_cursor.Position;
	uint8_t Low_Pos = Low_Y_cursor.Position;

	
	/* if sync is a analog sourse */
	if(Sourse != CHANNEL_DIGIT)
	{
		EPM570_Register_cnfPin.bit.OSC_LA_TRIGG = 0;

		/* Select current sync sourse mode */
		if(Sourse == CHANNEL_A)
		{
			pCursor = (int16_t*)&INFO_A.Position;
			EPM570_Register_cnfPin.bit.ADC_SyncSourse_RLE_Sel = 0;
		}
		else if(Sourse == CHANNEL_B)
		{ 
			pCursor = (int16_t*)&INFO_B.Position;
			EPM570_Register_cnfPin.bit.ADC_SyncSourse_RLE_Sel = 1;
		}
		else return;


		/* If autonome mode */
		if(HostMode == DISABLE)
		{
			Height_Pos = (uint8_t)(~(Height_Y_cursor.Position - (*pCursor)) + 128);
			Low_Pos = (uint8_t)(~(Low_Y_cursor.Position - (*pCursor)) + 128);
		}

		/* Reset win flag */
		EPM570_Register_cnfPin.bit.ADC_SyncWIN = 0;

		/* select current sync type */
		switch(Type)
		{
			case Sync_Rise:
			{
				EPM570_Register_TRG_UP.data = 255;
				EPM570_Register_TRG_DWN.data = Height_Pos;
			}
			break;
			case Sync_Fall:
			{
				EPM570_Register_TRG_UP.data = Height_Pos;
				EPM570_Register_TRG_DWN.data = 0;
			}
			break;
			case Sync_IN_WIN:
			case Sync_OUT_WIN:
			{
				EPM570_Register_TRG_UP.data = Low_Pos;
				EPM570_Register_TRG_DWN.data = Height_Pos;

				/* Set flag if sync is IN_WIN */
				if(Type == Sync_IN_WIN) EPM570_Register_cnfPin.bit.ADC_SyncWIN = 1;
			}
			break;

			default:
				return;
		}

		/* Write trigg levels registers */
		EPM570_Registers_WriteReg(EPM570_Register_TRG_UP.address, EPM570_Register_TRG_UP.data);
		EPM570_Registers_WriteReg(EPM570_Register_TRG_DWN.address, EPM570_Register_TRG_DWN.data);
	}

	/* Sync digital LA sourse */
	else if (Sourse == CHANNEL_DIGIT)
	{
		EPM570_Register_cnfPin.bit.OSC_LA_TRIGG = 1;

		/* select current sync type */
		switch(Type)
		{
			case Sync_LA_State:
			{
				/* LA_MUX_SEL[1..0] = 2`b10 */
				EPM570_Register_cnfPin.bit.ADC_SyncWIN = 1;			// LA_MUX_SEL[1]
				EPM570_Register_cnfPin.bit.AND_OR_LA_TRIGG = 0;		// LA_MUX_SEL[0]
			}
			break;
			case Sync_LA_Different:
			{
				/* LA_MUX_SEL[1..0] = 2`b11 */
				EPM570_Register_cnfPin.bit.ADC_SyncWIN = 1;			// LA_MUX_SEL[1]
				EPM570_Register_cnfPin.bit.AND_OR_LA_TRIGG = 1;		// LA_MUX_SEL[0]
			}
			break;
			case Sync_LA_State_AND_Different:
			{
				/* LA_MUX_SEL[1..0] = 2`b01 */
				EPM570_Register_cnfPin.bit.ADC_SyncWIN = 0;			// LA_MUX_SEL[1]
				EPM570_Register_cnfPin.bit.AND_OR_LA_TRIGG = 1;		// LA_MUX_SEL[0]
			}
			break;
			case Sync_LA_State_OR_Different:
			{
				/* LA_MUX_SEL[1..0] = 2`b00 */
				EPM570_Register_cnfPin.bit.ADC_SyncWIN = 0;			// LA_MUX_SEL[1]
				EPM570_Register_cnfPin.bit.AND_OR_LA_TRIGG = 0;		// LA_MUX_SEL[0]
			}
			break;

			default:
				return;
		}

		EPM570_Registers_WriteReg(EPM570_Register_LA_CND_MSK.address, EPM570_Register_LA_CND_MSK.data);
		EPM570_Registers_WriteReg(EPM570_Register_LA_DIFF_MSK.address, EPM570_Register_LA_DIFF_MSK.data);

		EPM570_Registers_WriteReg(EPM570_Register_LA_CND_DAT.address, EPM570_Register_LA_CND_DAT.data);
		EPM570_Registers_WriteReg(EPM570_Register_LA_DIFF_DAT.address, EPM570_Register_LA_DIFF_DAT.data);
	}
	else return;   // else ERROR, return


	/* Write config register */
	EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);
}


/**
  * @brief  EPM570_Sync
  * @param  NewMode
  * @retval None
  */
void EPM570_Update_SynchronizationMode(Channel_ID_TypeDef Sourse, SyncMode_TypeDef Mode, SyncAType_TypeDef Type)
{
	if(Mode != Sync_NONE) EPM570_Register_cnfPin.bit.GlobalSync_ON = 1;
	else EPM570_Register_cnfPin.bit.GlobalSync_ON = 0;

	/* Update sync registers */
	EPM570_Set_Trigger(Sourse, Type);
}






/* -----------------------------------------  Oscilloscope or Logic Analyzer, misc support functions ----------------------------------------- */

/**
  * @brief  EPM570_Set_AnalogDigital_Input
  * @param  NewMode
  * @retval None
  */
void EPM570_Set_AnalogDigital_DataInput(OSC_LA_Mode_Typedef NewMode)
{
	if(NewMode == LA_MODE)
	{
		EPM570_Register_cnfPin.bit.OSC_LA_DATA = 1;
		EPM570_Register_cnfPin.bit.OSC_LA_TRIGG = 1;
	}
	else
	{
		EPM570_Register_cnfPin.bit.OSC_LA_DATA = 0;
		EPM570_Register_cnfPin.bit.OSC_LA_TRIGG = 0;
	}

	EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);
}


/**
  * @brief  EPM570_Set_LA_RLE
  * @param
  * @retval None
  */
void EPM570_Set_LA_RLE_State(FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		EPM570_Register_cnfPin.bit.ADC_SyncSourse_RLE_Sel = 1;
	}
	else
	{
		EPM570_Register_cnfPin.bit.ADC_SyncSourse_RLE_Sel = 0;
	}

	EPM570_Registers_WriteReg(EPM570_Register_cnfPin.address, EPM570_Register_cnfPin.bits);
	RLE_State = NewState;
}


/**
  * @brief  EPM570_Get_LA_RLE
  * @param  None
  * @retval
  */
FunctionalState EPM570_Get_LA_RLE_State(void)
{
	return RLE_State;
}



/**
  * @brief  EPM570_Get_BackLight
  * @param  None
  * @retval
  */
BcklightState_Typedef EPM570_Get_BackLight(void)
{
//	EPM570_Register_extPin_1.bits = EPM570_Registers_ReadReg(EPM570_Register_extPin_1.address);
	return (BcklightState_Typedef)EPM570_Register_extPin_1.bit.BckLght;
}


/**
  * @brief  EPM570_ChangeBackLight
  * @param  bckLight_Val
  * @retval None
  */
void EPM570_Set_BackLight(BcklightState_Typedef bckLight_Val)
{
	EPM570_Register_extPin_1.bit.BckLght = (uint8_t)bckLight_Val;
	EPM570_Registers_WriteReg(EPM570_Register_extPin_1.address, EPM570_Register_extPin_1.bits);
}



/**
  * @brief  EPM570_Read_Keys
  * @param  None
  * @retval keys state value
  */
uint8_t EPM570_Read_Keys(void)
{
	EPM570_Register_InKeys.data = EPM570_Registers_ReadReg(EPM570_Register_InKeys.address);
	return EPM570_Register_InKeys.data;
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
