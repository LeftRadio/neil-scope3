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
#include "Processing_and_output.h"
#include "Analog.h"
#include "Measurments.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Change_Color_Buttons(void);
void Change_Color_Grid(void);
void Change_Color_Back(void);
void BackLight(void);
void Save_pref(void);
void Beep_ON_OFF(void);
void Perform_Erase_EEPROM(void);
void ShowFFT_Freq(void);
void Change_Interpolation(void);
void Auto_Power_OFF(void);

#include "Quick_Menu_buttons.c"


/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef QuickMenu = {
		{ 11, 19, LeftLineBtn + btnWidth, UpLineBtn + 1},
		ENABLE,
		qMenu_ClipObj,
		qMnuButtonsMAX,
		qMnuButtonsMAX,
		qMnuButtonsMAX,
		DOWN,
		UP,
		{
			&Color_Buttons, &Color_Grid, &EraseEeprom, &Save,
			&AutoPowerOFF, &BeepEN,	&btnLIGHT_INFO, &btnShowFFT, &Interpolation
		},
		(void*)0
};


char *message;
char AutoPowerOFF_btnTxt[13] = "Power OFF -";

/* Extern variables ----------------------------------------------------------*/
extern btnINFO btnSWEEP_MODE;

/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Change_Color_Buttons
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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


/*******************************************************************************
* Function Name  : Change_Color_Buttons
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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
	Draw_CH_Cursors();      // рисуем курсоры

	if(Get_State_COLOR_Mn() == DRAW) Draw_COLOR_Mn(128);
	else Init_COLOR_Mn(128, (uint16_t*)grayScalle);
}


/*******************************************************************************
* Function Name  : Perform_Erase_EEPROM
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Perform_Erase_EEPROM(void)
{
	uint8_t ErasedData[100];
	message = "SUCCESSFUL Erased";

	if(ButtonsCode != OK) return;

	memset(ErasedData, 0, 150);
	err_I2C_flag = 0;
	EEPROM_Write(ErasedData, 0, 150);

	if(err_I2C_flag != 0) message = "ERROR when Erased";

	/* Show message for 3 seconds */
	Show_Message(message);

	delay_ms(2000);
	GPIOC->BRR = GPIO_Pin_15;
}


/*******************************************************************************
* Function Name  : Save_pref
* Description    : Функция сохранения смещения, коррекции нуля, режима интерполяции
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Save_pref(void)
{
	if(ButtonsCode == OK) SavePreference();
}


/*******************************************************************************
* Function Name  : Auto_Power_OFF
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Auto_Power_OFF(void)
{
	char tstr[30] = {"Auto Power OFF"};
	char result[3] = {0, 0, 0};

	if(ButtonsCode == LEFT)
	{
		if(pnt_gOSC_MODE->OFF_Struct.State == ENABLE)
		{
			pnt_gOSC_MODE->OFF_Struct.Work_Minutes -= 10;

			if(pnt_gOSC_MODE->OFF_Struct.Work_Minutes <= 0)
			{
				pnt_gOSC_MODE->OFF_Struct.State = DISABLE;
				pnt_gOSC_MODE->OFF_Struct.Work_Minutes = 0;

				memcpy(&tstr[14], " DISABLE", 8);
				result[0] = '-';
			}
			else
			{
				memcpy(&tstr[14], " ENABLE    min", 14);
				sprintf (result, "%d", pnt_gOSC_MODE->OFF_Struct.Work_Minutes);
				memcpy(&tstr[22], result, 2);
			}
		}
		else return;
	}
	else if(ButtonsCode == RIGHT)
	{
		pnt_gOSC_MODE->OFF_Struct.State = ENABLE;
		pnt_gOSC_MODE->OFF_Struct.Work_Minutes += 10;
		if(pnt_gOSC_MODE->OFF_Struct.Work_Minutes > 90) pnt_gOSC_MODE->OFF_Struct.Work_Minutes = 90;

		memcpy(&tstr[14], " ENABLE    min", 14);
		sprintf (result, "%d", pnt_gOSC_MODE->OFF_Struct.Work_Minutes);
		memcpy(&tstr[22], result, 2);
	}
	else return;

	memcpy(&AutoPowerOFF_btnTxt[10], result, 2);
	AutoPowerOFF.Text = AutoPowerOFF_btnTxt;

	/* Show active message */
	Show_Message(tstr);
}


/*******************************************************************************
* Function Name  : Save_pref
* Description    : Функция сохранения смещения, коррекции нуля, режима интерполяции
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Beep_ON_OFF(void)
{
	if(ButtonsCode == RIGHT){ BeepState = ENABLE; btn->Text = "Beep ON"; }
	else if(ButtonsCode == LEFT){ BeepState = DISABLE; btn->Text = "Beep OFF"; }
	else return;

	/* Show message for 3 seconds */
	Show_Message(btn->Text);
}


/*******************************************************************************
* Function Name  : BackLight
* Description    : Изменение яркости подсведки
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BackLight(void)
{
   static uint8_t bckLight_Val = 1;

   if((ButtonsCode == RIGHT) && (bckLight_Val != 1)){ bckLight_Val = 1; btn->Text = "B.L. MAX"; }
   else if((ButtonsCode == LEFT) && (bckLight_Val != 0)){ bckLight_Val = 0; btn->Text = "B.L. MIN"; }
   else return;

   //LCD_DrawButton(btn, activeButton);
   EPM570_ChangeBackLight(bckLight_Val);

   Show_Message(btn->Text);
}


/*******************************************************************************
* Function Name  : ShowFFT_Freq
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ShowFFT_Freq(void)
{
	Boolean tmp;
	static char *MessageText;

	if(ButtonsCode == RIGHT) tmp = TRUE;
	else if(ButtonsCode == LEFT) tmp = FALSE;
	else return;

	if((pnt_gOSC_MODE->autoMeasurments == ON) || (ActiveMode == &FFT_MODE))
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


/*******************************************************************************
* Function Name  : Change_Interpolation
* Description    : Функция смены вида интерполяции
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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
//			if(((SwipScale > 1) || (pnt_gOSC_MODE->oscSweep < 1)))
//			{
//				if((SwipScale != &SweepScaleCoff[0]) && (SwipScale != &SweepScaleCoff[1]))
//				{
//					Show_Message("Disable for this time/div");
//					return;
//				}
//				else if(pnt_gOSC_MODE->oscSweep < 1) NewMODE++;
			if(pnt_gOSC_MODE->oscSweep < 1) NewMODE++;
//			}

			/* Save show fft freq status */
			FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
		}
		sign = +1;
	}
	else if((ButtonsCode == LEFT) && (ActiveMode != &IntNONE))
	{
		if((pnt_gOSC_MODE->oscSweep < 1)  && (ActiveMode != &IntLIN)) NewMODE--;
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
	Set_numPoints(pnt_gOSC_MODE->oscNumPoints);

	if(OldMode == 3)
	{
		Save_ReDraw_Auto_Meas(RESTORE);
		if(pnt_gOSC_MODE->autoMeasurments == OFF)
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

		if(pnt_gOSC_MODE->oscSync != Sync_NONE)
		{
			pnt_gOSC_MODE->oscSync = Sync_NONE;
			Sweep_Mode(FALSE);
			LCD_DrawButton(&btnSWEEP_MODE, NO_activeButton);
		}

		Save_ReDraw_Auto_Meas(SAVE);
		pnt_gOSC_MODE->autoMeasurments = ON;
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


