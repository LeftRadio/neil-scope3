/**
  ******************************************************************************
  * @file	 	SettingsParams.h
  * @author  	Neil Lab :: Left Radio
  * @version 	v1.0.0
  * @date
  * @brief		header
  ******************************************************************************
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETTINGS_PARAM_H
#define __SETTINGS_PARAM_H

/* Includes ------------------------------------------------------------------*/
#include  "main.h"

/* Exported define -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
const SettingsParam_TypeDef Param_ADC_Zero_A = { (uint8_t*)(&INFO_A.AD_Type.Analog.Zero_PWM_values[0]), 0x00, 24 };
const SettingsParam_TypeDef Param_ADC_Zero_B = { (uint8_t*)(&INFO_B.AD_Type.Analog.Zero_PWM_values[0]), 0x19, 24 };

const SettingsParam_TypeDef Param_A_Position = { &INFO_A.Position, 0x32, 1 };
const SettingsParam_TypeDef Param_B_Position = { &INFO_B.Position, 0x34, 1 };

const SettingsParam_TypeDef Param_ActiveMode = { (uint8_t*)&Interpolate_AciveMode_Index, 0x36, 1 };

const SettingsParam_TypeDef Param_ADC_IN_Div_A = { &INFO_A.AD_Type.Analog.Div, 0x38, 1 };
const SettingsParam_TypeDef Param_ADC_IN_Div_B = { &INFO_B.AD_Type.Analog.Div, 0x3A, 1 };

const SettingsParam_TypeDef Param_SweepIndex = { &SweepIndex, 0x3C, 1 };
const SettingsParam_TypeDef Param_ScaleIndex = { &ScaleIndex, 0x3E, 1 };
const SettingsParam_TypeDef Param_WindowsNum = { (uint8_t*)&gSamplesWin.WindowsNum, 0x40, 2 };
const SettingsParam_TypeDef Param_WindowPosition = { (uint8_t*)&gSamplesWin.WindowPosition, 0x43, 2 };

const SettingsParam_TypeDef Param_AutoOff_Timer = { (uint8_t*)&AutoOff_Timer.Work_Minutes, 0x46, 1 };

extern Channel_ID_TypeDef param_auto_div;
const SettingsParam_TypeDef Param_AutoDivider_State = { (uint8_t*)&param_auto_div, 0x48, 1 };

const SettingsParam_TypeDef Param_SyncState_Mode = { (uint8_t*)&gSyncState.Mode, 0x4A, 1 };
const SettingsParam_TypeDef Param_SyncState_Sourse = { (uint8_t*)&gSyncState.Sourse, 0x4C, 1 };
const SettingsParam_TypeDef Param_SyncState_Type = { (uint8_t*)&gSyncState.Type, 0x4E, 1 };

const SettingsParam_TypeDef Param_Height_Y_cursor = { (uint8_t*)&Height_Y_cursor.Position, 0x50, 1 };
const SettingsParam_TypeDef Param_Low_Y_cursor = { (uint8_t*)&Low_Y_cursor.Position, 0x52, 1 };
const SettingsParam_TypeDef Param_trigPosX_cursor = { (uint8_t*)&trigPosX_cursor.Position, 0x54, 3 };
const SettingsParam_TypeDef Param_trigPosX_WinPosition = { (uint8_t*)&trigPosX_cursor.WindowPosition, 0x58, 2 };
const SettingsParam_TypeDef Param_TriggShowInfo = { (uint8_t*)&TriggShowInfo.Status, 0x5B, 1 };

const SettingsParam_TypeDef Param_indxColorA = { &indxColorA, 0x5D, 1 };
const SettingsParam_TypeDef Param_indxColorB = { &indxColorB, 0x5F, 1 };
const SettingsParam_TypeDef Param_indxColorButtons = { &indxColorButtons, 0x61, 1 };
const SettingsParam_TypeDef Param_indxTextColorA = { &indxTextColorA, 0x63, 1 };
const SettingsParam_TypeDef Param_indxTextColorB = { &indxTextColorB, 0x65, 1 };
const SettingsParam_TypeDef Param_indxColorGrid = { &indxColorGrid, 0x67, 1 };

const SettingsParam_TypeDef Param_PowerSave = { (uint8_t*)&gOSC_MODE.PowerSave, 0x69, 1 };
const SettingsParam_TypeDef Param_BL = { (uint8_t*)&gOSC_MODE.BackLight, 0x6B, 1 };
const SettingsParam_TypeDef Param_BeepState = { (uint8_t*)&gOSC_MODE.BeepState, 0x6D, 1 };

const SettingsParam_TypeDef Param_ADC_Input_AC_DC_A = { (uint8_t*)&INFO_A.Mode.AC_DC, 0x6F, 1 };
const SettingsParam_TypeDef Param_ADC__Input_AC_DC_B = { (uint8_t*)&INFO_B.Mode.AC_DC, 0x71, 1 };
const SettingsParam_TypeDef Param_ShowFFTFreq = { (uint8_t*)&gShowFFTFreq, 0x73, 1 };

const SettingsParam_TypeDef Param_InterliveCorrectionCoeff = { (uint8_t*)&InterliveCorrectionCoeff, 0x75, 1 };
const SettingsParam_TypeDef Param_oscNumPoints = { (uint8_t*)&gOSC_MODE.oscNumPoints , 0x77, 3 };

const SettingsParam_TypeDef Param_I2C_GPIO = { (uint8_t*)&gOSC_MODE.i2c_gpio_chip_index, 0x7B, 1 };
const SettingsParam_TypeDef Param_HostCommunicate = { (uint8_t*)&gOSC_MODE.HostCommunicate, 0x7D, 1 };


const SettingsParam_TypeDef* SettingsParams[PARAM_NUM] = {

		&Param_ADC_Zero_A,
		&Param_ADC_Zero_B,
		&Param_A_Position,
		&Param_B_Position,
		&Param_ActiveMode,
		&Param_ADC_IN_Div_A,
		&Param_ADC_IN_Div_B,
		&Param_SweepIndex,
		&Param_ScaleIndex,
		&Param_WindowsNum,
		&Param_WindowPosition,
		&Param_AutoOff_Timer,
		&Param_AutoDivider_State,
		&Param_SyncState_Mode,
		&Param_SyncState_Sourse,
		&Param_SyncState_Type,
		&Param_Height_Y_cursor,
		&Param_Low_Y_cursor,
		&Param_trigPosX_cursor,
		&Param_trigPosX_WinPosition,
		&Param_TriggShowInfo,
		&Param_indxColorA,
		&Param_indxColorB,
		&Param_indxColorButtons,
		&Param_indxTextColorA,
		&Param_indxTextColorB,
		&Param_indxColorGrid,
		&Param_PowerSave,
		&Param_BL,
		&Param_BeepState,
		&Param_ADC_Input_AC_DC_A,
		&Param_ADC__Input_AC_DC_B,
		&Param_ShowFFTFreq,
		&Param_InterliveCorrectionCoeff,
		&Param_oscNumPoints,
		&Param_I2C_GPIO,
		&Param_HostCommunicate,
};


/* Exported function ---------------------------------------------------------*/






#endif /* __SETTINGS_PARAM_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
