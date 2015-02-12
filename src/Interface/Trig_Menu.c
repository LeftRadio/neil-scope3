/*************************************************************************************
*
Description :  NeilScope3 Trig Menu sourse
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Settings.h"
#include "eeprom_WR_RD.h"
#include "User_Interface.h"
#include "Trig_Menu.h"
#include "EPM570.h"
#include "Processing_and_output.h"
#include "Measurments.h"
#include "Trig_Menu_buttons.c"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char Trigg_Sourse_Sync_Text[3][10] = { "Sync A", "Sync B", "Digital" };

/* init varible of trigger info */
TriggShowInfo_TypeDef  TriggShowInfo = {
	//398,
	{ "Rise", "Fall", "IN Win", "OUT Win" },
	DISABLE
};

TrigCursorINFO *pntTrigCursor;
TrigCursorINFO Height_Y_cursor = { 120, 0, LightGray3, "1", TRUE };		// "верхний" курсор триггера по Y
TrigCursorINFO Low_Y_cursor    = { 120, 0, LightGray3, "2", TRUE };		// "нижний" курсор триггера по Y
TrigCursorINFO trigPosX_cursor = { 205, 0, LightRed2, "T", TRUE };		//  курсор триггера по X


/* Private function prototypes -----------------------------------------------*/
static void TrigMenuCallBack(DrawState NewDrawState);
static void Change_Sync_CursorLevel(TrigCursorINFO *Cursor);

/* Exported variables --------------------------------------------------------*/
Menu_Struct_TypeDef TrigMenu = {
		{ LeftLineBtn - 1, 19, (LeftLineBtn + btnWidth), UpLineBtn + 1},
		ENABLE,
		trgMenu_ClipObj,
		TrigButtonsMAX,
		TrigButtonsMAX,
		TrigButtonsMAX,
		DOWN,
		UP,
		{
			&btnTrigg_ShowInfo,	&btnTrigg_Position_X, &btnLow_Level_W,
			&btnHeight_Level_W, &btnTrigg_Mode_sync, &btnSync_Sourse
		},
		TrigMenuCallBack
};


/* Functions -----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : TrigMenuCallBack
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void TrigMenuCallBack(DrawState NewDrawState)
{
	if(NewDrawState == DRAW) Save_ReDraw_Auto_Meas(SAVE);
	else Save_ReDraw_Auto_Meas(RESTORE);
}


/*******************************************************************************
* Function Name  : SetActiveTrigCursor
* Description    : Функция установки "активного" курсора для триггера 
* Input          : *pTrigcursor
* Return         : None
*******************************************************************************/
void SetActiveTrigCursor(TrigCursorINFO *pTrigcursor)
{
   pntTrigCursor = pTrigcursor;
}


/*******************************************************************************
* Function Name  : Hide_Show_triggInfo
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Hide_Show_triggInfo(void)
{  
	/* if push "OK" in this menu Show or Hide trigger param info */
	if(ButtonsCode != OK) return;

	if(TriggShowInfo.Status == DISABLE)	TriggShowInfo.Status = ENABLE;	// if show info are disabled, enable it
	else TriggShowInfo.Status = DISABLE;								// else show info are enable, disabled it

	Draw_Trigg_Info(TriggShowInfo.Status);
}


/*******************************************************************************
* Function Name  : TriggPositionX
* Description    : изменение позиций курсора триггера по Х
* Input          : None
* Return         : None
*******************************************************************************/
void Trigg_Position_X(void)
{
	int8_t sign = 0;

	btnTIME_SCALE_trigX_Update(CLEAR);
	DrawTrig_PosX(CLEAR, &trigPosX_cursor);

	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{

		if(ButtonsCode == RIGHT)				// else if sync is ON and push RIGHT
		{
			if(speed_up_cnt++ >= 10) sign = 5;
			else sign = 1;
		}
		else if(ButtonsCode == LEFT)				// else if sync is ON and push LEFT
		{
			if(speed_up_cnt++ >= 10) sign = -5;
			else sign = -1;
		}

		trigPosX_cursor.WindowPosition = pnt_gOSC_MODE->WindowPosition;
		trigPosX_cursor.Position = trigPosX_cursor.Position + sign;	// изменяем позицию курсорa

		btnTIME_SCALE_trigX_Update(DRAW);
		DrawTrig_PosX(DRAW, &trigPosX_cursor);

		Set_numPoints(pnt_gOSC_MODE->oscNumPoints);	   /* обновляем количество точек */
		Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
	}
}


/*******************************************************************************
* Function Name  : change_H_CursorLevel
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Change_H_CursorLevel(void)
{
	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{
		setActiveButton(&btnHeight_Level_W);
		Change_Sync_CursorLevel(&Height_Y_cursor);
	}
	else btn->Active_Color = Gray;
}


/*******************************************************************************
* Function Name  : change_L_CursorLevel
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Change_L_CursorLevel(void)
{
	btn->Active_Color = Gray;

	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{
		if((pnt_gOSC_MODE->AnalogSyncType == Sync_IN_WIN) || (pnt_gOSC_MODE->AnalogSyncType == Sync_OUT_WIN))
		{
			btn->Active_Color = Orange;
			Change_Sync_CursorLevel(&Low_Y_cursor);
		}
	}
}


/*******************************************************************************
* Function Name  : changeCursor_Level
* Description    : Функция изменения уровня синхронизации
* Input          : None
* Return         : None
*******************************************************************************/
void Change_Sync_CursorLevel(TrigCursorINFO *Cursor)
{
	int8_t sign;

	if(ButtonsCode == RIGHT) sign = +1;
	else if(ButtonsCode == LEFT) sign = -1;
	else return;

	/* изменяем позицию курсорa */
	if(speed_up_cnt++ >= 10) sign += sign * 4;
	Sync_ChangeLevel(Cursor, sign);
}


/*******************************************************************************
* Function Name  : changeCursor_Level
* Description    : Функция изменения уровня синхронизации
* Input          : None
* Return         : None
*******************************************************************************/
void Sync_ChangeLevel(TrigCursorINFO *Cursor, int16_t Diff)
{
	SetActiveTrigCursor(Cursor);
	Draw_Cursor_Trig(CLEAR, globalBackColor, globalBackColor); // очищаем старый курсор

	Cursor->Position += Diff;

	/* проверяем позицию курсора на выход за область экрана */
	if((Cursor->Position < lowerLimit + 10)) Cursor->Position = lowerLimit + 10;
	else if(Cursor->Position > upperLimit - 10) Cursor->Position = upperLimit - 10;

	/* обновляем курсоры уровня триггера */
	Draw_Cursor_Trig(DRAW, globalBackColor, Red);

	Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);			/* обновляем регистр ПЛИС - Trigger_level_A  */
	Draw_CH_Cursors();									/* обновляем указатели каналов */
	Update_triggInfo_OnScreen(ReDRAW);					/* обновляем инфо триггера */
}


/*******************************************************************************
* Function Name  : changeTriggModeSync
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Change_Trigg_ModeSync(void)
{
   if((ButtonsCode == RIGHT) && (pnt_gOSC_MODE->AnalogSyncType < oscSyncMode_MAX)) pnt_gOSC_MODE->AnalogSyncType++;
   else if((ButtonsCode == LEFT) && (pnt_gOSC_MODE->AnalogSyncType > 0)) pnt_gOSC_MODE->AnalogSyncType--;
   else return;
      
   btn->Text = TriggShowInfo.triggType_Name[pnt_gOSC_MODE->AnalogSyncType];
   
   /* Очищаем линии указателей триггера */
//   Draw_Cursor_Trig(CLEAR, LightGray4, Red);
   Draw_Cursor_Trig(CLEAR, globalBackColor, globalBackColor);

   if(pnt_gOSC_MODE->AnalogSyncType > 1)
   {
	   Low_Y_cursor.Position = Height_Y_cursor.Position - 30;
	   if(Low_Y_cursor.Position < (lowerLimit + 5)) Low_Y_cursor.Position = lowerLimit + 5;
	   pntTrigCursor = &Height_Y_cursor;
   }

   Draw_Cursor_Trig(DRAW, LightGray4, Red);
   Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);							/* обновляем регистры синхронизации ПЛИС */
   Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
}


/*******************************************************************************
* Function Name  : changeSyncSourse
* Description    : Смена канала для синхронизации
* Input          : None
* Return         : None
*******************************************************************************/
void Change_SyncSourse(void)
{
	if((pnt_gOSC_MODE->SyncSourse == CHANNEL_DIGIT) && (ButtonsCode == OK))
	{
		LCD_DrawButton((btnINFO*)&btnSync_Sourse, NO_activeButton);
		SetActiveMenu(&DigitTrigMenu);		// делаем активным меню
		Draw_Menu(&DigitTrigMenu);
		return;
	}
	else if((ButtonsCode == LEFT) && (pnt_gOSC_MODE->SyncSourse > CHANNEL_A)) pnt_gOSC_MODE->SyncSourse--;
	else if((ButtonsCode == RIGHT) && (pnt_gOSC_MODE->SyncSourse < CHANNEL_DIGIT)) pnt_gOSC_MODE->SyncSourse++;
	else return;
	
	Sync_Sourse_Texts();
	btn->Text = (char*)&Trigg_Sourse_Sync_Text[pnt_gOSC_MODE->SyncSourse][0];

	Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);	/* обновляем регистры синхронизации ПЛИС */
	Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */

	/* обновляем кнопку вида синхронизации */
	btnTrigg_Mode_sync.Text = TriggShowInfo.triggType_Name[pnt_gOSC_MODE->AnalogSyncType];
	LCD_DrawButton(&btnTrigg_Mode_sync, NO_activeButton);
}


/*******************************************************************************
* Function Name  : SyncSourseTexts
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Sync_Sourse_Texts(void)
{
	if(pnt_gOSC_MODE->SyncSourse < CHANNEL_DIGIT)
	{
		TriggShowInfo.triggType_Name[0] = "Rise";
		TriggShowInfo.triggType_Name[1] = "Fall";
		TriggShowInfo.triggType_Name[2] = "IN Win";
		TriggShowInfo.triggType_Name[3] = "OUT Win";
	}
	else
	{
		TriggShowInfo.triggType_Name[0] = "Cond";
		TriggShowInfo.triggType_Name[1] = "Diff";
		TriggShowInfo.triggType_Name[2] = "C or D";
		TriggShowInfo.triggType_Name[3] = "C and D";
	}
}


/*******************************************************************************
* Function Name  : changeTrig_X_Y_onMainMenu
* Description    : изменение позиций курсоров триггера из главного меню
* Input          : None
* Return         : None
*******************************************************************************/
void Change_Trig_X_Y_onMainMenu(void)
{
	if(ButtonsCode == OK)
	{
		if((pnt_gOSC_MODE->AnalogSyncType == Sync_IN_WIN) || (pnt_gOSC_MODE->AnalogSyncType == Sync_OUT_WIN))
		{
			if(pntTrigCursor == &Height_Y_cursor)
			{
				pntTrigCursor = &Low_Y_cursor;
				Draw_Cursor_Trig(DRAW, globalBackColor, Red);				/* обновляем курсоры уровня триггера */
				return;
			}
		}

		pntTrigCursor = &Height_Y_cursor;
		Draw_Cursor_Trig(CLEAR, LightGray4, Red);				/* обновляем курсоры уровня триггера */

		LCD_SetFullScreen();
		btn->Active_Color = Orange;
		LCD_DrawButton(btn, activeButton);

		pMNU = Change_Menu_Indx;       // изменяем указатель на функцию основного меню
		return;
	}
	else
	{
		if((ButtonsCode == UP) && (pntTrigCursor->Position - 1 < upperLimit - 10)) ButtonsCode = RIGHT;
		else if((ButtonsCode == DOWN) && (pntTrigCursor->Position > lowerLimit + 1)) ButtonsCode = LEFT;
		else
		{
			Trigg_Position_X();
			return;
		}

		/* update trigger Y positions if push UP or DOWN */
		SetActiveTrigCursor(pntTrigCursor);
		Change_Sync_CursorLevel(pntTrigCursor);
		Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
	}
}



/*******************************************************************************
* Function Name  : Draw_Trigg_Info
* Description    :
* Input          : DrawState Draw
* Output         : None
* Return         : None
*******************************************************************************/
void Draw_Trigg_Info(FunctionalState NewState)
{
	uint16_t X0 = leftLimit + 1, Y0 = upperLimit - 20;
	uint16_t X1 = (gShowFFTFreq == TRUE)? rightLimit - 61 : rightLimit - 1;
	uint16_t Y1 = upperLimit - 6;

	LCD_ClearArea(X0, Y0, X1, Y1, Active_BackColor);

	if(NewState == ENABLE)
	{
		/* устанавливаем ограничения для отрисовки */
		Set_New_ClipObject(X0, Y0, X1, Y1, IN_OBJECT, trgINFO_ClipObj);
		Update_Oscillogram();	// обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения
		Update_triggInfo_OnScreen(DRAW);
	}
	else if(NewState == DISABLE)
	{
		/* Clear */
		Clear_ClipObject(trgINFO_ClipObj);		/* убираем ограничения для отрисовки */
		LCD_DrawGrid(&activeAreaGrid, DRAW);	/* перерисовываем сетку в области осциллограмм */
		Update_Oscillogram();				    /* обновлем осциллограммы */
	}
}


/*******************************************************************************
* Function Name  : Update_triggInfo
* Description    : Отрисовка и/или обновление информации курсоров триггера на экране
* Input          : None
* Return         : None
*******************************************************************************/
void Update_triggInfo_OnScreen(DrawState NewDrawState)
{
	uint16_t X0 = leftLimit + 5, Y0 = upperLimit - 20;
	static char tmpText[60];
	uint8_t sLen;
	float tmp;

	if(TriggShowInfo.Status != ENABLE) return;

	if(NewDrawState == ReDRAW)
	{
		/* Clear Print values string */
		LCD_SetTextColor(Active_BackColor);
		//LCD_SetFont(&timesNewRoman12ptFontInfo);    // установить шрифт
		LCD_PutStrig(X0, Y0, 0, tmpText);
	}

	/* if sync is a analog */
	if(pnt_gOSC_MODE->SyncSourse != CHANNEL_DIGIT)
	{
		/* change pINFO for needed channel */
		if(pnt_gOSC_MODE->SyncSourse == CHANNEL_B)	pINFO = &INFO_B;
		else if(pnt_gOSC_MODE->SyncSourse == CHANNEL_A) pINFO = &INFO_A;

		/* prepare H_Level value text to print */
		strcpy(&tmpText[0], "  H: ");
		sLen = strlen(tmpText);

		/* compute value position Height trigger level in mV or V */
		tmp = (float)(Height_Y_cursor.Position) - (float)(pINFO->Position);			
		cnvrtToVolts(tmp, &tmpText[sLen]);

		/* add to string spaces and trigger L_level print Name*/
		strcat(tmpText, "   ");
		strcat(tmpText, "L: ");
	
		if((pnt_gOSC_MODE->AnalogSyncType == Sync_IN_WIN) || (pnt_gOSC_MODE->AnalogSyncType == Sync_OUT_WIN))
		{
			/* prepare L_Level value text to print */
			sLen = strlen(tmpText);

			/* compute value position Low trigger level in mV or V */
			tmp = (float)(Low_Y_cursor.Position) - (float)(pINFO->Position);			
			cnvrtToVolts(tmp, &tmpText[sLen]);
		}
		else strcat(tmpText, "----  ");
	}
	else if(pnt_gOSC_MODE->SyncSourse == CHANNEL_DIGIT)
	{
		/* prepare Condition value text to print */
		strcpy(&tmpText[0], " Cond: ");
		sLen = strlen(tmpText);
		sprintf(&tmpText[sLen], "0x%02X", ConditionState_Sync_Var);
	 	
		/* add to string spaces and Difference print Name*/
		strcat(tmpText, "  Diff: ");
		sLen = strlen(tmpText);
		sprintf(&tmpText[sLen], "0x%02X", DifferentState_Sync_Var);
		
		if(DifferentState_Sync_Rise == ENABLE) strcat(tmpText, "}");
		else if	(DifferentState_Sync_Fall == ENABLE)strcat(tmpText, "{");
	}
	else return;

	
	/* prepare PosX value text to print */
	strcat(tmpText, "  ");
	strcat(tmpText, "X: ");
	sLen = strlen(tmpText);

	/* compute value position X trigger in nS, uS, mS, or Sec. */
	tmp = ((float)(trigPosX_cursor.Position) - (float)leftLimit);
	cnvrtToTime(tmp, &tmpText[sLen]);
	
	/* prepare Sync type Name text to print */
	sLen = strlen(tmpText);
	strcat(tmpText, " <");
	strcat(tmpText, TriggShowInfo.triggType_Name[pnt_gOSC_MODE->AnalogSyncType]);
	strcat(tmpText, ">");
	

	/* Print values string */
	LCD_SetTextColor(LighGreen);
	//LCD_SetFont(&timesNewRoman12ptFontInfo);    // установить шрифт
	LCD_PutStrig(X0, Y0, 0, tmpText);
}




