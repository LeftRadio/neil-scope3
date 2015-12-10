/**
  ******************************************************************************
  * @file	 	Quick_Menu.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Quick Menu sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systick.h"
#include "Settings.h"
#include "eeprom_WR_RD.h"
#include "User_Interface.h"
#include "Quick_Menu.h"
#include "gInterface_MENU.h"
#include "EPM570.h"
#include "Synchronization.h"
#include "Processing_and_output.h"
#include "Analog.h"
#include "Measurments.h"
#include "Quick_Menu_buttons.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef QuickMenu = {
		{ 11, 19, LeftLineBtn + btnWidth, UpLineBtn + 1},
		ENABLE,
		qMenu_ClipObj,
		qMnuButtonsMAX,
		qMnuButtonsMAX,
		qMnuButtonsMAX,
		M_CLEAR,
		DOWN,
		UP,
		{
			&Color_Buttons, &Color_Grid, &btnAdvancedMenu, &AutoPowerOFF,
			&BeepEN, &btnLIGHT_INFO, &btnShowFFT, &Interpolation
		},
		(void*)0,
};


char *message;
char AutoPowerOFF_btnTxt[13] = "Power OFF -";

/* Extern variables ----------------------------------------------------------*/
extern btnINFO btnSWEEP_MODE;

/* Functions -----------------------------------------------------------------*/

/**
  * @brief  Change_Color_Buttons
  * @param  None
  * @retval None
  */
void Change_Color_Buttons(void)
{
	int8_t i = 0;
	uint16_t Color;
	uint16_t tColor_A, tColor_B;

	if(ButtonsCode == RIGHT) i = 1;
	else if(ButtonsCode == LEFT) i = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)M256_Colors);
		return;
	}

	indxColorButtons += i;
	if (indxColorButtons > 250) indxColorButtons = 127;
	else if(indxColorButtons > 127) indxColorButtons = 0;

	Change_COLOR_Mn_Position(indxColorButtons, &Color);

	tColor_A = gInterfaceMenu.Buttons[1]->Color;
	tColor_B = gInterfaceMenu.Buttons[2]->Color;

	mSet_AllButtons_Color(Color);

	gInterfaceMenu.Buttons[1]->Color = gInterfaceMenu.Buttons[8]->Color = tColor_A;
	gInterfaceMenu.Buttons[2]->Color = gInterfaceMenu.Buttons[9]->Color = tColor_B;

	ReDraw_Menu(&gInterfaceMenu, (void*)0);
	ReDraw_Menu(&QuickMenu, &Color_Buttons);
}


/**
  * @brief  Change_Color_Grid
  * @param  None
  * @retval None
  */
void Change_Color_Grid(void)
{
	int8_t i = 0;

	if(ButtonsCode == RIGHT) i = 1;
	else if(ButtonsCode == LEFT) i = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)grayScalle);
		return;
	}

	indxColorGrid += i;
	Change_COLOR_Mn_Position(indxColorGrid, &activeAreaGrid.Color);

	LCD_DrawGrid(&activeAreaGrid, DRAW); // перерисовываем сетку в области осциллограмм
	Draw_Cursor_CH(&INFO_A, INFO_A.Color);
	Draw_Cursor_CH(&INFO_B, INFO_B.Color);

	if(Get_State_COLOR_Mn() == DRAW) Draw_COLOR_Mn(128);
	else Init_COLOR_Mn(128, (uint16_t*)grayScalle);
}


/**
  * @brief  GoAdvancedMenu
  * @param  None
  * @retval None
  */
void GoAdvancedMenu(void)
{
	if(ButtonsCode == OK)
	{
		LCD_DrawButton(btn, NO_activeButton);

		SetActiveMenu(&AdvancedMenu);	// делаем активным быстрое меню
		Draw_Menu(&AdvancedMenu);
	}
}


/**
  * @brief  Auto_Power_OFF
  * @param  None
  * @retval None
  */
void Auto_Power_OFF(void)
{
	char tstr[30] = {"Auto Power OFF"};
	char result[3] = {0, 0, 0};

	if(ButtonsCode == LEFT)
	{
		if(AutoOff_Timer.State == ENABLE)
		{
			AutoOff_Timer.Work_Minutes -= 10;

			if(AutoOff_Timer.Work_Minutes <= 0)
			{
				AutoOff_Timer.State = DISABLE;
				AutoOff_Timer.Work_Minutes = 0;

				memcpy(&tstr[14], " DISABLE", 8);
				result[0] = '-';
			}
			else
			{
				memcpy(&tstr[14], " ENABLE    min", 14);
				sprintf (result, "%d", AutoOff_Timer.Work_Minutes);
				memcpy(&tstr[22], result, 2);
			}
		}
		else return;
	}
	else if(ButtonsCode == RIGHT)
	{
		AutoOff_Timer.State = ENABLE;
		AutoOff_Timer.Work_Minutes += 10;
		if(AutoOff_Timer.Work_Minutes > 90) AutoOff_Timer.Work_Minutes = 90;

		memcpy(&tstr[14], " ENABLE    min", 14);
		sprintf (result, "%d", AutoOff_Timer.Work_Minutes);
		memcpy(&tstr[22], result, 2);
	}
	else return;

	memcpy(&AutoPowerOFF_btnTxt[10], result, 2);
	AutoPowerOFF.Text = AutoPowerOFF_btnTxt;

	/* Show active message */
	Show_Message(tstr);
}


/**
  * @brief  Beep_ON_OFF
  * @param  None
  * @retval None
  */
void Beep_ON_OFF(void)
{
	if(ButtonsCode == RIGHT){ gOSC_MODE.BeepState = ENABLE; btn->Text = "Beep ON"; }
	else if(ButtonsCode == LEFT){ gOSC_MODE.BeepState = DISABLE; btn->Text = "Beep OFF"; }
	else return;

	/* Show message for 3 seconds */
	Show_Message(btn->Text);
}


/**
  * @brief  BackLight
  * @param  None
  * @retval None
  */
void BackLight(void)
{
	if((ButtonsCode != RIGHT) && (ButtonsCode != LEFT)) return;

	if(gOSC_MODE.PowerSave == PWR_S_ENABLE)
	{
		if(gOSC_MODE.BackLight == BCKL_MIN)
		{
			if(ButtonsCode == RIGHT) gOSC_MODE.BackLight = BCKL_MAX;
			else return;
		}
		else if(gOSC_MODE.BackLight == BCKL_MAX)
		{
			if(ButtonsCode == RIGHT) gOSC_MODE.PowerSave = PWR_S_DISABLE;
			gOSC_MODE.BackLight = BCKL_MIN;
		}
	}
	else
	{
		if(gOSC_MODE.BackLight == BCKL_MIN)
		{
			if(ButtonsCode == LEFT) gOSC_MODE.PowerSave = PWR_S_ENABLE;
			gOSC_MODE.BackLight = BCKL_MAX;
		}
		else if(gOSC_MODE.BackLight == BCKL_MAX)
		{
			if(ButtonsCode == LEFT) gOSC_MODE.BackLight = BCKL_MIN;
			else return;
		}
	}

	EPM570_Set_BackLight(gOSC_MODE.BackLight);
	UI_BackLightPowerState_UpdateButton();
}


/**
  * @brief  ShowFFT_Freq
  * @param  None
  * @retval None
  */
void ShowFFT_Freq(void)
{
	Boolean tmp;
	static char *MessageText;

	if(ButtonsCode == RIGHT) tmp = TRUE;
	else if(ButtonsCode == LEFT) tmp = FALSE;
	else return;

	if((gOSC_MODE.autoMeasurments == ON) || (ActiveMode == &FFT_MODE))
	{
		MessageText = "FFT display disabled in this Mode";
	}
	else if(ActiveMode == &IntMIN_MAX)
	{
		MessageText = "FFT measure disabled in this Mode";
	}
	else
	{
		if(tmp == TRUE){ MessageText = "FFT measure ON"; }
		else { MessageText = "FFT measure OFF";	}

		FrequencyMeas_Draw(tmp);
	}

	Show_Message(MessageText);
}


/**
  * @brief  Change_Interpolation
  * @param  None
  * @retval None
  */
void Change_Interpolation(void)
{
	uint8_t NewMODE = ActiveMode->Indx;
	uint8_t OldMode = ActiveMode->Indx;
	int8_t sign;

	/* проверка нажатий кнопок вправо/влево, иначе выход */
	if((ButtonsCode == RIGHT) && (ActiveMode != &FFT_MODE))
	{
		if(ActiveMode == &IntLIN)
		{
//			if(((SwipScale > 1) || (gSamplesWin.Sweep < 1)))
//			{
//				if((SwipScale != &SweepScaleCoff[0]) && (SwipScale != &SweepScaleCoff[1]))
//				{
//					Show_Message("Disable for this time/div");
//					return;
//				}
//				else if(gSamplesWin.Sweep < 1) NewMODE++;
			if(gSamplesWin.Sweep < 1) NewMODE++;
//			}

			/* Save show fft freq status */
			FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
		}
		sign = +1;
	}
	else if((ButtonsCode == LEFT) && (ActiveMode != &IntNONE))
	{
		if((gSamplesWin.Sweep < 1)  && (ActiveMode != &IntLIN)) NewMODE--;
		sign = -1;
	}
	else return;

	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_A, leftLimit, rightLimit - 2);
	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_B, leftLimit, rightLimit - 2);
	NewMODE += sign;

	/* изменяем интерполяцию и текст на кнопке */
	changeInterpolation((InterpolationMode_TypeDef*)InterpModes[NewMODE]);
	btn->Text = (char*)ActiveMode->Text;

	/* обновляем регистры ПЛИС в зависимости от oscNumPoints_Ratio */
	EPM570_Set_numPoints(gOSC_MODE.oscNumPoints);

	if(OldMode == 3)
	{
		Save_ReDraw_Auto_Meas(RESTORE);
		if(gOSC_MODE.autoMeasurments == OFF)
		{
			Clear_Menu(&MeasMenu);
			LCD_DrawGrid(&activeAreaGrid, DRAW); // перерисовываем сетку в области осциллограмм
		}

		if(ActiveMode == &IntLIN)
		{
			FrequencyMeas_SaveRestore_State(0, &gShowFFTFreq);
			FrequencyMeas_Draw(gShowFFTFreq);
		}
	}
	else if(ActiveMode == &FFT_MODE)
	{
		if(gShowFFTFreq == TRUE) FrequencyMeas_Draw(FALSE);

		if(gSyncState.Mode != Sync_NONE)
		{
			Sweep_Mode(Sync_NONE, FALSE);
			LCD_DrawButton(&btnSWEEP_MODE, NO_activeButton);
		}

		Save_ReDraw_Auto_Meas(SAVE);
		gOSC_MODE.autoMeasurments = ON;
		mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[2];
		Draw_Menu(&MeasMenu);
	}
	else if(ActiveMode == &IntMIN_MAX)
	{
		if(gShowFFTFreq == TRUE) FrequencyMeas_Draw(FALSE);
	}
	else
	{
		FrequencyMeas_SaveRestore_State(0, &gShowFFTFreq);
		FrequencyMeas_Draw(gShowFFTFreq);
	}
}




/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
