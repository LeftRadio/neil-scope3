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
#include "EPM570_Registers.h"
#include "Synchronization.h"
#include "Processing_and_output.h"
#include "Measurments.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const char Trigg_Sourse_Sync_Text[3][10] = { "Sync A", "Sync B", "Digital" };

/* init varible of trigger info */
TriggShowInfo_TypeDef  TriggShowInfo = {
	{
			"RISE", "FALL", "IN WIN", "OUT WIN",
			"COND", "DIFF", "C & D", "C | D"
	},

	15,
	206,
	"",
	DISABLE
};

TrigCursorINFO Height_Y_cursor = { 120, 0, LightGray3, "1", TRUE };		// "верхний" курсор триггера по Y
TrigCursorINFO Low_Y_cursor    = { 120, 0, LightGray3, "2", TRUE };		// "нижний" курсор триггера по Y
TrigCursorINFO trigPosX_cursor = { 205, 0, LightRed2, "T", TRUE };		//  курсор триггера по X
TrigCursorINFO *pntTrigCursor = &Height_Y_cursor;

/* buttons vars */
#include "Trig_Menu_buttons.c"

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
		M_CLEAR,
		DOWN,
		UP,
		{
			&btnTrigg_ShowInfo,	&btnTrigg_Position_X, &btnLow_Level_W,
			&btnHeight_Level_W, &btnTrigg_Type_sync, &btnSync_Sourse
		},
		TrigMenuCallBack,
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

	if(gSyncState.Mode != Sync_NONE)
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

		trigPosX_cursor.WindowPosition = gSamplesWin.WindowPosition;
		trigPosX_cursor.Position = trigPosX_cursor.Position + sign;	// изменяем позицию курсорa

		btnTIME_SCALE_trigX_Update(DRAW);
		DrawTrig_PosX(DRAW, &trigPosX_cursor);
		Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
	}

	/* Set new position for trig X, update numPoints*/
	gSyncState.foops->SetTrigg_X(trigPosX_cursor.Position - leftLimit);
}


/*******************************************************************************
* Function Name  : change_H_CursorLevel
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Change_H_CursorLevel(void)
{
	if((gSyncState.Mode != Sync_NONE) && (gSyncState.Sourse != CHANNEL_DIGIT))
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
	if((gSyncState.Mode != Sync_NONE) && (gSyncState.Sourse != CHANNEL_DIGIT))
	{
		if((gSyncState.Type == Sync_IN_WIN) || (gSyncState.Type == Sync_OUT_WIN))
		{
			btn->Active_Color = Orange;
			Change_Sync_CursorLevel(&Low_Y_cursor);
		}
	}
	else btn->Active_Color = Gray;
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
	/* Select new cursor and clear old */
	SetActiveTrigCursor(Cursor);
	Draw_Cursor_Trig(CLEAR, globalBackColor, globalBackColor);

	/* Change cursor position */
	Cursor->Position += Diff;

	/* Clip cursor position */
	if((Cursor->Position < lowerLimit + 10)) Cursor->Position = lowerLimit + 10;
	else if(Cursor->Position > upperLimit - 10) Cursor->Position = upperLimit - 10;

	/* Update trigger and channels cursors, trigger info on LCD */
	Draw_Cursor_Trig(DRAW, globalBackColor, Red);
	Draw_Cursor_CH(&INFO_A, INFO_A.Color);
	Draw_Cursor_CH(&INFO_B, INFO_B.Color);
	Update_triggInfo_OnScreen(ReDRAW);

	/* Update sync mode */
	gSyncState.foops->StateUpdate();
}


/*******************************************************************************
* Function Name  : changeTriggModeSync
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Trigg_SyncType(void)
{
	/* Select new sync type */
	if((ButtonsCode == RIGHT) && (gSyncState.Type < Sync_LA_State_OR_Different)) gSyncState.Type++;
	else if((ButtonsCode == LEFT) && (gSyncState.Type > 0)) gSyncState.Type--;
	else return;

	/* Update text and cursors */
	btn->Text = TriggShowInfo.triggType_Name[gSyncState.Type];

	if(gSyncState.Type <= Sync_OUT_WIN)
	{
		Draw_Cursor_Trig(CLEAR, globalBackColor, globalBackColor);

		if((gSyncState.Type == Sync_IN_WIN) || (gSyncState.Type == Sync_OUT_WIN))
		{
			Low_Y_cursor.Position = Height_Y_cursor.Position - 30;
			if(Low_Y_cursor.Position < (lowerLimit + 5)) Low_Y_cursor.Position = lowerLimit + 5;
			pntTrigCursor = &Height_Y_cursor;
		}

		/* Update trigger cursor */
		Draw_Cursor_Trig(DRAW, LightGray4, Red);
	}

	/* Update trigger info */
	Update_triggInfo_OnScreen(ReDRAW);

	/* Update EPM570 sync registers */
	gSyncState.foops->StateUpdate();
}


/*******************************************************************************
* Function Name  : changeSyncSourse
* Description    : Смена канала для синхронизации
* Input          : None
* Return         : None
*******************************************************************************/
void Trigg_SyncSourse(void)
{
	if((gSyncState.Sourse == CHANNEL_DIGIT) && (ButtonsCode == OK))
	{
		LCD_DrawButton((btnINFO*)&btnSync_Sourse, NO_activeButton);
		SetActiveMenu(&DigitTrigMenu);
		Draw_Menu(&DigitTrigMenu);
		return;
	}
	else if((ButtonsCode == LEFT) && (gSyncState.Sourse > CHANNEL_A)) gSyncState.Sourse--;
	else if((ButtonsCode == RIGHT) && (gSyncState.Sourse < CHANNEL_DIGIT)) gSyncState.Sourse++;
	else return;


	Draw_Cursor_Trig(CLEAR, globalBackColor, globalBackColor);

	if( ((gSyncState.Sourse == CHANNEL_A) || (gSyncState.Sourse == CHANNEL_B)) && (gSyncState.Mode != Sync_NONE) )
	{
		if(gSyncState.Sourse == CHANNEL_A)
		{
			Height_Y_cursor.Position = INFO_A.Position;
			Low_Y_cursor.Position = INFO_A.Position - 30;
		}
		else
		{
			Height_Y_cursor.Position = INFO_B.Position;
			Low_Y_cursor.Position = INFO_B.Position - 30;
		}

		gSyncState.Type = Sync_Rise;
		Draw_Cursor_Trig(DRAW, LightGray4, Red);
	}
	else
	{
		Height_Y_cursor.Visible = FALSE;
		Low_Y_cursor.Visible = FALSE;

		gSyncState.Type = Sync_LA_State;
	}

	/* Update texts, button, cursors and trigger info */
	btnSync_Sourse.Text = (char*)&Trigg_Sourse_Sync_Text[gSyncState.Sourse][0];
	btnTrigg_Type_sync.Text = TriggShowInfo.triggType_Name[gSyncState.Type];
	LCD_DrawButton(&btnTrigg_Type_sync, NO_activeButton);
	Update_triggInfo_OnScreen(ReDRAW);

	/* redraw channel cursors */
	Draw_Cursor_CH(&INFO_A, INFO_A.Color);
	Draw_Cursor_CH(&INFO_B, INFO_B.Color);

	/* Update EPM570 sync registers */
	gSyncState.foops->StateUpdate();
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
		if(gSyncState.Sourse != CHANNEL_DIGIT)
		{
			if((gSyncState.Type == Sync_IN_WIN) || (gSyncState.Type == Sync_OUT_WIN))
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
		}

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

		if(gSyncState.Sourse != CHANNEL_DIGIT)
		{
			/* update trigger Y positions if push UP or DOWN */
			SetActiveTrigCursor(pntTrigCursor);
			Change_Sync_CursorLevel(pntTrigCursor);
			Update_triggInfo_OnScreen(ReDRAW);		/* обновляем инфо триггера */
		}
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
	uint16_t X0 = leftLimit + 1;
	uint16_t Y0 = upperLimit - 20;
	uint16_t X1 = (gShowFFTFreq == TRUE)? rightLimit - 62 : rightLimit - 1;
	uint16_t Y1 = upperLimit - 6;

	LCD_ClearArea(X0, Y0, X1, Y1, Active_BackColor);

	if(NewState == ENABLE)
	{
		/* Set new object to clip and update trigg info */
		Set_New_ClipObject(X0, Y0, X1, Y1, IN_OBJECT, trgINFO_ClipObj);
		Update_triggInfo_OnScreen(DRAW);
	}
	else if(NewState == DISABLE)
	{
		/* Clear, update grid */
		Clear_ClipObject(trgINFO_ClipObj);
		LCD_DrawGrid(&activeAreaGrid, DRAW);
	}

	/* Update oscillograms */
	Update_Oscillogram();
}


/*******************************************************************************
* Function Name  : Update_triggInfo
* Description    : Отрисовка и/или обновление информации курсоров триггера на экране
* Input          : None
* Return         : None
*******************************************************************************/
void Update_triggInfo_OnScreen(DrawState NewDrawState)
{
	uint8_t sLen;
	float tmp;

	if(TriggShowInfo.Status == ENABLE)
	{

		if(NewDrawState == ReDRAW)
		{
			/* Clear Print values string */
			LCD_PutColorStrig(TriggShowInfo.X0, TriggShowInfo.Y0, 0, TriggShowInfo.Text, Active_BackColor);
		}

		/* if sync is a analog */
		if(gSyncState.Sourse != CHANNEL_DIGIT)
		{
			/* change pINFO for needed channel */
			if(gSyncState.Sourse == CHANNEL_A) pINFO = &INFO_A;
			else pINFO = &INFO_B;

			/* prepare H_Level value text to print */
			strcpy(&TriggShowInfo.Text[0], "  H: ");
			sLen = strlen(TriggShowInfo.Text);

			/* compute value position Height trigger level in mV or V */
			tmp = (float)(Height_Y_cursor.Position) - (float)(pINFO->Position);
			cnvrtToVolts(tmp, &TriggShowInfo.Text[sLen]);

			/* add to string spaces and trigger L_level print Name*/
			strcat(TriggShowInfo.Text, "   ");
			strcat(TriggShowInfo.Text, "L: ");

			if((gSyncState.Type == Sync_IN_WIN) || (gSyncState.Type == Sync_OUT_WIN))
			{
				/* prepare L_Level value text to print */
				sLen = strlen(TriggShowInfo.Text);

				/* compute value position Low trigger level in mV or V */
				tmp = (float)(Low_Y_cursor.Position) - (float)(pINFO->Position);
				cnvrtToVolts(tmp, &TriggShowInfo.Text[sLen]);
			}
			else strcat(TriggShowInfo.Text, "----  ");

			/* prepare PosX value text to print */
			strcat(TriggShowInfo.Text, "  ");
			strcat(TriggShowInfo.Text, "X: ");
			sLen = strlen(TriggShowInfo.Text);

			/* compute value position X trigger in nS, uS, mS, or Sec. */
			tmp = ((float)(trigPosX_cursor.Position) - (float)leftLimit);
			cnvrtToTime(tmp, &TriggShowInfo.Text[sLen]);
		}
		else
		{
			/* prepare Condition value text to print */
			strcpy(&TriggShowInfo.Text[0], "CM:");
			sprintf(&TriggShowInfo.Text[strlen(TriggShowInfo.Text)], "0x%02X", EPM570_Register_LA_CND_MSK.data);
			strcat(TriggShowInfo.Text, " CD:");
			sprintf(&TriggShowInfo.Text[strlen(TriggShowInfo.Text)], "0x%02X", EPM570_Register_LA_CND_DAT.data);

			/* add to string spaces and Different print Name*/
			strcat(TriggShowInfo.Text, "  DM:");
			sprintf(&TriggShowInfo.Text[strlen(TriggShowInfo.Text)], "0x%02X", EPM570_Register_LA_DIFF_MSK.data);
			strcat(TriggShowInfo.Text, " DD:");
			sprintf(&TriggShowInfo.Text[strlen(TriggShowInfo.Text)], "0x%02X", EPM570_Register_LA_DIFF_DAT.data);
		}

		/* prepare Sync type Name text to print */
		sLen = strlen(TriggShowInfo.Text);
		strcat(TriggShowInfo.Text, " <");
		strcat(TriggShowInfo.Text, TriggShowInfo.triggType_Name[gSyncState.Type]);
		strcat(TriggShowInfo.Text, ">");

		/* Print values string */
		LCD_PutColorStrig(TriggShowInfo.X0, TriggShowInfo.Y0, 0, TriggShowInfo.Text, LighGreen);
	}
}




