/*************************************************************************************
*
Description :  NeilScope3 Time Scale Menu sourse
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio
Comments:
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "main.h"
#include "init.h"
#include "TimeScale_Menu.h"
#include "User_Interface.h"
#include "Trig_Menu.h"
#include "Settings.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "Synchronization.h"
#include "Sweep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO int32_t nI = 1;

/* Private function prototypes -----------------------------------------------*/
/* Functions ----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Draw_Time_Scale_Menu
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static void Update_TimeScale_Menu_Text(void)
{
	static char Text[40] = {' ', ' '};
	uint8_t tmpY = (TriggShowInfo.Status == ENABLE)? (upperLimit - 34) : (upperLimit - 20);

	LCD_PutColorStrig(leftLimit + 5, tmpY, 1, Text, globalBackColor);
	memset(&Text[2], 0, 38);

	ConvertToString(gOSC_MODE.oscNumPoints, &Text[2], 6);
	strcat(Text, " samples | ");
	ConvertToString(gSamplesWin.WindowPosition, &Text[strlen(Text)], 3);
	strcat(Text, " win pos");

	LCD_PutColorStrig(leftLimit + 5, tmpY, 0, Text, LighGreen);
}


/*******************************************************************************
* Function Name  : Draw_Time_Scale_Menu
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_TimeScale_Menu(DrawState NewDrawState)
{
	uint8_t tmpY = (TriggShowInfo.Status == ENABLE)? (TriggShowInfo.Y0 - 14) : (TriggShowInfo.Y0);
	uint16_t tmpX = centerX + 25;

	if(NewDrawState == DRAW)
	{
		Set_New_ClipObject(leftLimit - 1, tmpY, tmpX, tmpY + 15, IN_OBJECT, TsMNU_ClipObj);
		Update_TimeScale_Menu_Text();
	}
	else
	{
		LCD_ClearArea(leftLimit + 1, tmpY, tmpX, tmpY + 14, Active_BackColor);
		Clear_ClipObject(TsMNU_ClipObj);
		LCD_DrawGrid(&activeAreaGrid, DRAW);
	}

	Update_Oscillogram();
}


/*******************************************************************************
* Function Name  : Draw_Time_Scale_Menu
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Reset_TimeScale_Menu(void)
{
	nI = 1;
}


/*******************************************************************************
* Function Name  : TimeScale_Menu
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void TimeScale_Menu(void)
{
	uint16_t max_win_num;
	static int16_t old_WindowPosition = -1;
	static uint16_t old_WindowsNum = 0xFFFF;
	int16_t new_WindowPosition = gSamplesWin.WindowPosition;
	uint16_t new_WindowsNum = gSamplesWin.WindowsNum;

	if(ButtonsCode == OK)
	{
		Draw_TimeScale_Menu(CLEAR);
		pMNU = Change_Menu_Indx;	// изменяем указатель на функцию основного меню
		Draw_btnTIME_SCALE(activeButton);
		return;
	}
	else if(ButtonsCode != NO_Push)
	{
		if(((ButtonsCode == UP) || (ButtonsCode == DOWN)) && (gOSC_MODE.State != STOP))
		{
			if(ButtonsCode == UP) new_WindowsNum++;
			else new_WindowsNum--;
		}
		else if((ButtonsCode == LEFT) || (ButtonsCode == RIGHT))
		{
			if(ButtonsCode == LEFT) new_WindowPosition--;
			else new_WindowPosition++;
		}

		max_win_num = 128000 / (gSamplesWin.WindowWidh * (*SweepScale));

		if( new_WindowsNum > max_win_num){ new_WindowsNum = max_win_num; }
		else if(new_WindowsNum < 1){ new_WindowsNum = 1; }

		if(new_WindowPosition < 0){ new_WindowPosition = 0; }
		else if(new_WindowPosition >= new_WindowsNum){ new_WindowPosition = new_WindowsNum - 1; }


		if((old_WindowPosition != new_WindowPosition) || (old_WindowsNum != new_WindowsNum))
		{
			/* Clear trig X cursor if needed */
			btnTIME_SCALE_trigX_Update(CLEAR);
			DrawTrig_PosX(CLEAR, &trigPosX_cursor);

			/* Aplly new position for active window and new num windows */
			gSamplesWin.WindowPosition = new_WindowPosition;
			gSamplesWin.WindowsNum = new_WindowsNum;

			/* Calc new number points */
			gOSC_MODE.oscNumPoints = (gSamplesWin.WindowWidh * gSamplesWin.WindowsNum) / (*SweepScale);

			/* If number points now < trigger X position */
			if((gOSC_MODE.oscNumPoints * (*SweepScale)) < trigPosX_cursor.Position)
			{
				trigPosX_cursor.Position = gOSC_MODE.oscNumPoints - (gSamplesWin.WindowWidh / 2);
				trigPosX_cursor.WindowPosition = gSamplesWin.WindowPosition;
			}

			/* If Stop state read data and update oscillograms */
			if(gOSC_MODE.State == STOP)
			{
				EPM570_SRAM_Read();
				Update_Oscillogram();	//обновлем осциллограммы
			}

			/* Set new position for trig X */
			gSyncState.foops->SetTrigg_X(trigPosX_cursor.Position - leftLimit);

			/* Update trig X cursor if needed */
			DrawTrig_PosX(DRAW, &trigPosX_cursor);

			/* Draw timeScale button */
			Draw_btnTIME_SCALE(activeButton);
			Update_TimeScale_Menu_Text();

			old_WindowPosition = gSamplesWin.WindowPosition;
			old_WindowsNum = gSamplesWin.WindowsNum;
		}
	}
}



