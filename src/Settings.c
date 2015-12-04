/**
  ******************************************************************************
  * @file	 	Settings.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Settings sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "Settings.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "Synchronization.h"
#include "eeprom_WR_RD.h"
#include "gInterface_MENU.h"
#include "Analog.h"
#include "User_Interface.h"
#include "Quick_Menu.h"
#include "systick.h"
#include "Measurments.h"
#include "Sweep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PARAM_NUM						36

/* Private macro -------------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#include "SettingsParams.h"

/* Private function prototypes -----------------------------------------------*/
uint8_t Save_Pref_CheckSum(void);
uint8_t CheckSum(uint8_t *pData, uint8_t numValues);

/* Private functions ---------------------------------------------------------*/

static void Settings_Message(char* msg, uint16_t color)
{
	static char old_msg[50] = "";

	LCD_SetFont(&timesNewRoman12ptFontInfo);
	LCD_PutColorStrig(20, 20, 0, old_msg, Black);
	LCD_PutColorStrig(20, 20, 0, msg, color);

	memset(old_msg, 0, 50);
	memcpy(old_msg, msg, strlen(msg));
}

/**
  * @brief  Settings_U8_CS
  * @param  None
  * @retval None
  */
uint8_t Settings_U8_CS(uint8_t pData, uint8_t numValues)
{
	return (pData & 0xF0) | ((pData ^ 0xFF) & 0x0F);
}


/**
  * @brief  SavePreference
  * @param  None
  * @retval None
  */
void Settings_EraseParam(const SettingsParam_TypeDef* param)
{
	uint8_t ErasedData[param->num_bytes + 1];

	memset(ErasedData, 0xFF, param->num_bytes);
	EEPROM_Write(ErasedData, param->start_addr, param->num_bytes);
}

/**
  * @brief  SavePreference
  * @param  None
  * @retval None
  */
uint8_t ErasePreference(void)
{
	uint8_t i;

	for(i = 0; i < PARAM_NUM; i++) Settings_EraseParam(SettingsParams[i]);
	return err_I2C_flag;
}



/**
  * @brief  Settings_SaveParam
  * @param  None
  * @retval None
  */
uint8_t Settings_SaveParam(const SettingsParam_TypeDef* param)
{
	uint8_t CS_byte = Settings_U8_CS(*(param->u8_data), param->num_bytes);

	EEPROM_Write((uint8_t*)param->u8_data, param->start_addr, param->num_bytes);
	EEPROM_Write(&CS_byte, param->start_addr + param->num_bytes, 1);

	return err_I2C_flag;
}

/**
  * @brief  SavePreference
  * @param  None
  * @retval None
  */
uint8_t SavePreference(void)
{
	uint16_t i;
	err_I2C_flag = 0;

	/* сохраняем вид интерполяции */
	Interpolate_AciveMode_Index = ActiveMode->Indx;

	/* значения времени автоотключения */
	if(AutoOff_Timer.State != ENABLE) AutoOff_Timer.Work_Minutes = 255;

	/* состояниe автоделителя */
	if((Get_AutoDivider_State(CHANNEL_A) == ENABLE) && (Get_AutoDivider_State(CHANNEL_B) == ENABLE)) param_auto_div = BOTH_CHANNEL;
	else if(Get_AutoDivider_State(CHANNEL_A) == ENABLE) param_auto_div = CHANNEL_A;
	else if(Get_AutoDivider_State(CHANNEL_B) == ENABLE) param_auto_div = CHANNEL_B;
	else param_auto_div = 255;


	/* Save all params */
	for(i = 0; i < PARAM_NUM; i++) Settings_SaveParam(SettingsParams[i]);
	return err_I2C_flag;
}



/**
  * @brief  Settings_SaveParam
  * @param  None
  * @retval None
  */
static uint8_t Settings_LoadParam(const SettingsParam_TypeDef* param)
{
	uint8_t CS_ReadByte, CS_ControlByte;
	uint8_t p_data[param->num_bytes + 1];

	/* First read and verify CRC byte */
	EEPROM_Read(p_data, param->start_addr, param->num_bytes);
	EEPROM_Read(&CS_ReadByte, param->start_addr + param->num_bytes, 1);

	/* if CRC is OK copy readed data to param */
	CS_ControlByte = Settings_U8_CS(p_data[0], param->num_bytes);
	if(CS_ReadByte == CS_ControlByte)
	{
		memcpy((uint8_t*)param->u8_data, p_data, param->num_bytes);
		return 0;
	}
	else
	{
		Settings_EraseParam(param);
		return 255;
	}
}

/**
  * @brief  LoadPreference
  * @param  None
  * @retval None
  */
void LoadPreference(void)
{
	uint16_t i, j = 0;
	uint8_t load_success = 0;
	char m_txt[50] = "param ";
	uint16_t m_color;

	Settings_Message("For skip load settings push 'DOWN'", Red);

	/* Init data massive */
	for(i = 0; i < 800; i ++)
	{
		INFO_A.DATA[i] = INFO_B.DATA[i] = 0;
		INFO_A.visDATA[i] = INFO_B.visDATA[i] = 120;
	}

	delay_ms(1000);

	if(EPM570_Read_Keys() == DOWN)
	{
		Settings_Message("Load Preference SKIPED", White);
		delay_ms(1000);
	}
	else
	{
		/* Load all params */
		for(i = 0; i < PARAM_NUM; i++)
		{
			load_success = Settings_LoadParam(SettingsParams[i]);

			memset(&m_txt[6], 0, 49 - 6);
			ConvertToString(i, &m_txt[6], 3);
			j = strlen(m_txt);

			if(load_success != 0)
			{
				strcat(&m_txt[j], " failed, erased");
				m_color = Red;
			}
			else
			{
				strcat(&m_txt[j], " load");
				m_color = LighGreen;
			}

			Settings_Message(m_txt, m_color);
			if(load_success != 0) delay_ms(200);
		}


		/* ------------------------------------- Update States, Modes, Texts and other ------------------------------------- */

		/* вид интерполяции */
		ActiveMode = (InterpolationMode_TypeDef*)InterpModes[Interpolate_AciveMode_Index];	//
		if(ActiveMode == &FFT_MODE)
		{
			gOSC_MODE.autoMeasurments = ON;
			mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[2];
		}

		/* чтение значения времени автоотключения */
		if( (AutoOff_Timer.Work_Minutes != 0) && (AutoOff_Timer.Work_Minutes != (int8_t)-1) )
		{
			AutoOff_Timer.State = ENABLE;
			AutoOff_Timer.ResetTime = RTC_GetCounter();

			sprintf (&AutoPowerOFF_btnTxt[10], "%d", AutoOff_Timer.Work_Minutes);
			AutoPowerOFF.Text = AutoPowerOFF_btnTxt;
		}

		/* Samples num points */
		if(gOSC_MODE.oscNumPoints == 0) gOSC_MODE.oscNumPoints = 388;

		/* Update sync */
		gInterfaceMenu.Buttons[4]->Text = (char*)&sweepMODE_text[gSyncState.Mode];
		TrigMenu.Buttons[4]->Text = TriggShowInfo.triggType_Name[gSyncState.Type];
		TrigMenu.Buttons[5]->Text = (char*)&Trigg_Sourse_Sync_Text[gSyncState.Sourse][0];

		gSyncState.foops->SetTrigg_X(trigPosX_cursor.Position - leftLimit);
		gSyncState.foops->StateUpdate();

		/* AC/DC inputs state */
		setActiveButton(gInterfaceMenu.Buttons[8]);
		ON_OFF_Channels(&INFO_A, TRUE);
		setActiveButton(gInterfaceMenu.Buttons[9]);
		ON_OFF_Channels(&INFO_B, TRUE);

		/* Autodivider for analog inputs */
		Set_AutoDivider_State(param_auto_div, ENABLE);

		/* PWM correction zero for analog inputs */
		*(INFO_A.AD_Type.Analog.corrZ) = INFO_A.AD_Type.Analog.Zero_PWM_values[INFO_A.AD_Type.Analog.Div];
		*(INFO_B.AD_Type.Analog.corrZ) = INFO_B.AD_Type.Analog.Zero_PWM_values[INFO_B.AD_Type.Analog.Div];

		/*  */
		gInterfaceMenu.Buttons[1]->Text = Change_AnalogDivider(CHANNEL_A, INFO_A.AD_Type.Analog.Div);		// btnVDIV_A
		gInterfaceMenu.Buttons[2]->Text = Change_AnalogDivider(CHANNEL_B, INFO_B.AD_Type.Analog.Div);		// btnVDIV_B

		/* Interpolation */
		changeInterpolation(ActiveMode);
		Interpolation.Text = (char*)ActiveMode->Text;

		/* Update sweep state, scale coeffs */
		Sweep_UpdateState();

		/* Update UI elements */
		UI_LoadPreferenceUpdate();

		/*Set LCD backlight*/
		EPM570_Set_BackLight(gOSC_MODE.BackLight);
	}
}













