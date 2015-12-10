/**
  ******************************************************************************
  * @file	 	gInterface_MENU.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 gInterface Menu sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "Settings.h"
#include "gInterface_MENU.h"
#include "User_Interface.h"
#include "Analog.h"
#include "EPM570.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "Processing_and_output.h"
#include "Measurments.h"
#include "systick.h"

#include "gOscModeButtons.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define gInterfaceButtonsMAX			10		// (all buttons - 1)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

Menu_Struct_TypeDef gInterfaceMenu = {
		{ 0, 0, 0, 0 },		// Coordinates, not atemp
		DISABLE,
		0,
		0,
		gInterfaceButtonsMAX,
		0,
		M_SKIP,
		RIGHT,
		LEFT,
		{
			&btnMENU, &btnVDIV_A, &btnVDIV_B, &btnSWEEP,
			&btnSWEEP_MODE, &btnTRIG, &btnMEASURMENTS, &btnRUN_HOLD,
			&btnCHA_AC_DC, &btnCHB_AC_DC, &btnTIME_SCALE
		},
		(void*)0,
};

char *gmessage;

/* Exported variables --------------------------------------------------------*/
/* Extern function -----------------------------------------------------------*/
extern void Init_Meas_Values(void);

/* Private function prototypes -----------------------------------------------*/
static void Auto_Sweep(void);
static void Change_Analog_Div(Channel_ID_TypeDef Channel);


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Menu
* Description    : Функция вызова глобального или быстрого меню
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void qMenu(void)
{
	if(ButtonsCode == UP)
	{
		LCD_DrawButton(btn, NO_activeButton);

		QuickMenu.Coord[0] -= 2;
		SetActiveMenu(&QuickMenu);	// делаем активным быстрое меню
		Draw_Menu(&QuickMenu);
		QuickMenu.Coord[0] += 2;
	}
}


/*******************************************************************************
* Function Name  : change_Div_A
* Description    : Функция изменения делителя канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_Div_A(void)
{
	Change_Analog_Div(CHANNEL_A);
}


/*******************************************************************************
* Function Name  : change_Div_B
* Description    : Функция изменения делителя канала В
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_Div_B(void)
{
	Change_Analog_Div(CHANNEL_B);
}


/*******************************************************************************
* Function Name  : Change_Analog_Div
* Description    : Функция изменения развертки
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Change_Analog_Div(Channel_ID_TypeDef Channel)
{
	Menu_Struct_TypeDef *tMenu = (Channel == CHANNEL_A)? &ChannelA_Menu : &ChannelB_Menu;

	/* Change pointer pINFO to input <Channel> */
	Set_CH_TypeINFO(Channel);
	
	/* проверка нажатий кнопок вверх/вниз и изменение делителя */
	if(ButtonsCode != OK)
	{
		if(Get_AutoDivider_State(Channel) == DISABLE)
		{
			if((ButtonsCode == UP) && (pINFO->AD_Type.Analog.Div < Divider_Position_MAX)) pINFO->AD_Type.Analog.Div++;
			else if((ButtonsCode == DOWN) && (pINFO->AD_Type.Analog.Div > Divider_Position_MIN)) pINFO->AD_Type.Analog.Div--;
			else return;
		}
		else if((ButtonsCode == UP) || (ButtonsCode == DOWN)) Show_Message("Auto ON, manual set not available");
		else return;
	}
	else
	{
		LCD_DrawButton(btn, NO_activeButton);
		SetActiveMenu(tMenu);					// Set active menu corresponding to input <Channel>
		Draw_Menu(tMenu);
		return;
	}

	*(pINFO->AD_Type.Analog.corrZ) = pINFO->AD_Type.Analog.Zero_PWM_values[pINFO->AD_Type.Analog.Div];	  /* установка ШИМ в соответсвии с диапазоном делителя */

	/* смена делителя и изменение надписи на кнопке на соответсвующую делителю */
	btn->Text = Change_AnalogDivider(Channel, pINFO->AD_Type.Analog.Div);
	Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
}


/*******************************************************************************
* Function Name  : change_Sweep
* Description    : Функция изменения развертки
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_Sweep(void)
{
	char tmpText[30] = "Decim:  ";

	if(ButtonsCode == DOWN)
	{
		if(ScaleIndex > 0)
		{
			ScaleIndex--;

			if(!ScaleIndex) gOSC_MODE.Interleave = FALSE;

			if(gOSC_MODE.Interleave == TRUE) SweepScale = (uint8_t*)&IntrlSweepScaleCoff[ScaleIndex];
			else SweepScale = (uint8_t*)&SweepScaleCoff[ScaleIndex];
		}
		else if(SweepIndex < 20)
		{
			SweepIndex++;
		}
	}
	else if(ButtonsCode == UP)
	{
		if(SweepIndex == ActiveMode->MinimumAllowed_SweepIndex)
		{
			Show_Message("TimeScale is minimum for this mode");
			return;
		}
		else if(SweepIndex == 0)
		{
			if(ScaleIndex < 3) ScaleIndex++;

			/* Enable interlive if only channel A is ON */
			if((ScaleIndex == 1) && (INFO_B.Mode.EN == STOP)) Inerlive_Cmd(ENABLE);
		}
		else
		{
			SweepIndex--;
		}
	}
	else if((ButtonsCode == OK) && (ButtonPush == LONG_SET))
	{
		gmessage = "Search decimation coefficient...";

		Show_Message(gmessage);
		Auto_Sweep();
		ButtonEnable = RESET;
	}
	else if(ButtonsCode == NO_Push);
	else return;  

	if(ScaleIndex > 0) btnSWEEP.Text = (char *)&sweep_Scale_text[ScaleIndex - 1];
	else btnSWEEP.Text = (char *)&sweep_text[SweepIndex];

	Sweep_UpdateState();

	Init_Meas_Values();
	Update_triggInfo_OnScreen(ReDRAW);

	sprintf(&tmpText[7], "%d", (int)(gSamplesWin.Sweep + 1));
	strcpy(&tmpText[strlen(tmpText)], "  Scale: ");
	sprintf(&tmpText[strlen(tmpText)], "%d", (*SweepScale));
	Show_Message(tmpText);
}


/*******************************************************************************
* Function Name  : Auto_Sweep
* Description    : Функция авто-развертки
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Auto_Sweep(void)
{

}


/*******************************************************************************
* Function Name  : change_Sweep_Mode
* Description    : Функция изменения режима развертки
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_Sweep_Mode(void)
{
	if(ActiveMode != &FFT_MODE)
	{
		if((ButtonsCode == UP) || (ButtonsCode == OK)) gSyncState.Mode++;
		else if(ButtonsCode == DOWN) gSyncState.Mode--;
		else return;

		if(gSyncState.Mode > Sync_SINGL) gSyncState.Mode = Sync_NONE;
		else if (gSyncState.Mode < Sync_NONE) gSyncState.Mode = Sync_SINGL;

		Sweep_Mode(gSyncState.Mode, FALSE);
		setCondition(RUN);
	}
}


/*******************************************************************************
* Function Name  : Sweep_Mode
* Description    : Функция изменения режима развертки
* Input          : Init - TRUE or FALSE
* Output         : None
* Return         : None
*******************************************************************************/
void Sweep_Mode(SyncMode_TypeDef NewSyncMode, Boolean Init)
{
	gInterfaceMenu.Buttons[4]->Text = (char*)&sweepMODE_text[NewSyncMode];

	if(gSyncState.Sourse != CHANNEL_DIGIT)
	{
		if(NewSyncMode != Sync_NONE)
		{
			SetActiveTrigCursor(&Height_Y_cursor);
			Draw_Cursor_Trig(CLEAR, LightGray4, Red);
		}
		else
		{
			/* Очищаем линии указателей уровней триггера */
			if(Init != TRUE) Draw_Cursor_Trig(CLEAR, Active_BackColor, Active_BackColor);
			Draw_Cursor_CH(&INFO_A, INFO_A.Color);
			Draw_Cursor_CH(&INFO_B, INFO_B.Color);
		}
	}

	/* Update trig X */
	gSyncState.Mode = NewSyncMode;
	Trigg_Position_X();
}


/*******************************************************************************
* Function Name  : change_Trigg_Mode
* Description    : Меню триггера
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_Trigg_Mode(void)
{
	if(ActiveMode == &FFT_MODE) return;

	if(ButtonsCode == UP)
	{
		/* Clear current active button */
		LCD_DrawButton(btn, NO_activeButton);

		/* Switch to TrigMenu */
		SetActiveMenu(&TrigMenu);
		Draw_Menu(&TrigMenu);
	}
	else if((ButtonsCode == OK) && (gSyncState.Mode != Sync_NONE))
	{ 
		/* изменяем указатель на функцию изменения уровней триггера */
		pMNU = Change_Trig_X_Y_onMainMenu;
		
		if(gSyncState.Sourse != CHANNEL_DIGIT)
		{
			pntTrigCursor = &Height_Y_cursor;

			/* обновляем курсоры уровня триггера */
			Draw_Cursor_Trig(DRAW, Active_BackColor, Red);
		}

//		/* обновляем инфо триггера */
//		Update_triggInfo_OnScreen(ReDRAW);

		btn->Active_Color = GrayBlue;
		return;
	}
	else return;	
}


/*******************************************************************************
* Function Name  : Hide_Show_Meas
* Description    : включение отключение первой группы автоизмерений
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Hide_Show_Meas(void)
{
	static float X_TrigPositionPrecent;

	if(ButtonsCode == OK)
	{
		if(ActiveMode == &FFT_MODE) return;
		if(TriggShowInfo.Status == ENABLE) Draw_Trigg_Info(DISABLE);

		X_TrigPositionPrecent = (((rightLimit - leftLimit) - 1) * 100) / (trigPosX_cursor.Position - leftLimit);

		/* если автоизмерения были выключены то отрисовываем боковую панель */
		if(gOSC_MODE.autoMeasurments == OFF)
		{
			/* Save show fft freq status */
			FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
			FrequencyMeas_Draw(FALSE);

			gOSC_MODE.autoMeasurments = ON;
			Init_Meas_Values();
			Draw_Menu(&MeasMenu);
		}
		else		// иначе очищаем
		{
			gOSC_MODE.autoMeasurments = OFF;
			Clear_Menu(&MeasMenu);

			/* Restore show fft freq status */
			FrequencyMeas_SaveRestore_State(0, &gShowFFTFreq);
			FrequencyMeas_Draw(gShowFFTFreq);
		}

		if(TriggShowInfo.Status == ENABLE) Draw_Trigg_Info(ENABLE);

		DrawTrig_PosX(CLEAR, &trigPosX_cursor);						// очищаем старый курсор
		trigPosX_cursor.Position = (uint16_t)((((rightLimit - leftLimit) - 1) * 100) / X_TrigPositionPrecent) + leftLimit;
		DrawTrig_PosX(DRAW, &trigPosX_cursor);						// перерисовуем курсор по новой позиции
		Update_triggInfo_OnScreen(ReDRAW);							/* обновляем инфо триггера */

		gSyncState.foops->StateUpdate();
	}
	else if((ButtonsCode == UP) && (gOSC_MODE.autoMeasurments == ON))
	{
		LCD_DrawButton(btn, NO_activeButton);

		SetActiveMenu(&MeasMenu);
		setActiveButton(MeasMenu.Buttons[MeasMenu.StartButton]);
		LCD_DrawButton(btn, activeButton);
	}
}


/*******************************************************************************
* Function Name  : RUN_HOLD
* Description    : Функция приостановки/запуска каналов
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RUN_HOLD(void)
{  
	if((ButtonsCode == UP) || (ButtonsCode == DOWN) || (ButtonsCode == OK))
	{
		if(gOSC_MODE.State == RUN)
		{
			setCondition(STOP);
			Show_Message("HOLD, power save ON");
		}
		else
		{
			setCondition(RUN);
			if(gOSC_MODE.PowerSave == PWR_S_DISABLE) Show_Message("RUN, power save OFF");
			else Show_Message("RUN, power save ON");
		}
	}
}


/*******************************************************************************
* Function Name  : ON_OFF_CHANNEL_A
* Description    : Функция вкл./отк. канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ON_OFF_CHANNEL_A(void)
{
	ON_OFF_Channels(&INFO_A, FALSE);
}


/*******************************************************************************
* Function Name  : ON_OFF_CHANNEL_B
* Description    : Функция вкл./отк. канала B
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ON_OFF_CHANNEL_B(void)
{
	Channel_AC_DC_TypeDef tAC_DC = INFO_B.Mode.AC_DC;

	ON_OFF_Channels(&INFO_B, FALSE);

	if(((SweepIndex == 0) && (ScaleIndex > 0)) && (tAC_DC != INFO_B.Mode.AC_DC))
	{
		if(INFO_B.Mode.EN == STOP) gOSC_MODE.Interleave = TRUE;
		else if(gOSC_MODE.Interleave == TRUE) gOSC_MODE.Interleave = FALSE;
		else return;

		Sweep_UpdateState();
	}
}


/*******************************************************************************
* Function Name  : ON_OFF_CHANNEL_A
* Description    : Функция вкл./отк. канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ON_OFF_Channels(CH_INFO_TypeDef* Channel, Boolean init)
{
	static const char ON_OFF_TEXT[3][4] = { {"OFF"}, {"AC "}, {"DC "} };

	/* проверка нажатий кнопок и изменения индекса канала если были нажаты */
	if((ButtonsCode == UP) || (ButtonsCode == OK)) Channel->Mode.AC_DC++;
	else if(ButtonsCode == DOWN) Channel->Mode.AC_DC--;
	else if(!init) return;

	if(Channel->Mode.AC_DC > 2) Channel->Mode.AC_DC = 0;
	else if(Channel->Mode.AC_DC < 0) Channel->Mode.AC_DC = 2;

	if(Channel->Mode.AC_DC == NONE)         // если индекс равен 0
	{
		Channel->Mode.EN = STOP;
		Clear_OLD_DataCH_ON_SCREEN(Channel->Mode.ID, leftLimit, rightLimit - 2);
		Draw_Cursor_CH(Channel, globalBackColor);
	}
	else if(Channel->Mode.AC_DC > 0)	    // иначе запускаем
	{
		Channel->Mode.EN = RUN;
		/* Write to EPM570 new AC/DC channel state */
		Analog_SetInput_ACDC(Channel->Mode.ID, Channel->Mode.AC_DC);
		/* Draw channel cursor*/
		Draw_Cursor_CH(Channel, Channel->Color);
	}

	/* обновляем текст на кнопке соответсвующего канала */
	btn->Text = (char *)&ON_OFF_TEXT[Channel->Mode.AC_DC];
}


/*******************************************************************************
* Function Name  : change_TIME_SCALE
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void change_TIME_SCALE(void)
{
	if(ButtonsCode == OK)
	{
		pMNU = TimeScale_Menu;
		Draw_TimeScale_Menu(DRAW);
	}
}




