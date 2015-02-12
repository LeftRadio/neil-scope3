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
#include "systick.h"
#include "TimeScale_Menu.h"
#include "User_Interface.h"
#include "Trig_Menu.h"
#include "Settings.h"
#include "Processing_and_output.h"
#include "EPM570.h"


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
	char Text[40] = {' ', ' '};
	uint16_t len;
	uint8_t tmpY = (TriggShowInfo.Status == ENABLE)? (upperLimit - 34) : (upperLimit - 20);
	uint32_t points = Get_numPoints();
	uint16_t X0 = leftLimit + 1;

	ConvertToString(points, &Text[2], 6);
	strcat(Text, " samples | ");
	len = strlen(Text);
	ConvertToString(gOSC_MODE.WindowPosition, &Text[len], 3);
	strcat(Text, " win pos");

	LCD_ClearArea(X0, tmpY, centerX + 25, tmpY + 14, Active_BackColor);
	LCD_SetTextColor(LighGreen);					// установить цвет текстa
	LCD_PutStrig(leftLimit + 5, tmpY, 0, Text);
}


/*******************************************************************************
* Function Name  : Draw_Time_Scale_Menu
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_TimeScale_Menu(DrawState NewDrawState)
{
	uint8_t tmpY = (TriggShowInfo.Status == ENABLE)? (upperLimit - 35) : (upperLimit - 20);
	uint16_t tmpX = centerX + 25;

	if(NewDrawState == DRAW)
	{
		Set_New_ClipObject(leftLimit - 1, tmpY, tmpX, upperLimit - 6, IN_OBJECT, TsMNU_ClipObj);
		Update_Oscillogram();	//обновлем осциллограмы
		Update_TimeScale_Menu_Text();
	}
	else
	{
		LCD_ClearArea(leftLimit + 1, tmpY, tmpX, upperLimit - 6, Active_BackColor);
		Clear_ClipObject(TsMNU_ClipObj);			// убираем ограничения для отрисовки
		LCD_DrawGrid(&activeAreaGrid, DRAW);		// перерисовываем сетку в области осциллограм
		Update_Oscillogram();						//обновлем осциллограммы
		//Trigg_Position_X();
	}
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
	uint32_t numPoints = (gOSC_MODE.WindowWidh);

	if(ButtonsCode == OK)
	{
		Draw_TimeScale_Menu(CLEAR);
		pMNU = Change_Menu_Indx;	// изменяем указатель на функцию основного меню
		Draw_btnTIME_SCALE(activeButton);
		return;
	}
	else if(((ButtonsCode == UP) || (ButtonsCode == DOWN)) && (gOSC_MODE.State != STOP))
	{
		/* Clear trig X cursor if needed */
		btnTIME_SCALE_trigX_Update(CLEAR);
		DrawTrig_PosX(CLEAR, &trigPosX_cursor);

		if(ButtonsCode == UP)
		{
			if(++nI > 329) nI = 329;
		}
		else if(--nI < 1) nI = 1;

		/* Calc new number points */
		gOSC_MODE.oscNumPoints = numPoints * nI;

		/* If active first window position */
		if(nI - 1 <= gOSC_MODE.WindowPosition)
		{
			trigPosX_cursor.Position = 150;
			trigPosX_cursor.WindowPosition = gOSC_MODE.WindowPosition = nI - 1;
		}
		else
		{
			if(gOSC_MODE.oscNumPoints < trigPosX_cursor.Position)
			{
				trigPosX_cursor.Position = gOSC_MODE.oscNumPoints - 10;
				trigPosX_cursor.WindowPosition = (numPoints / gOSC_MODE.WindowPosition) - 1;
			}
		}

		Set_numPoints(gOSC_MODE.oscNumPoints);

		/* Draw trig X cursor if needed */
		btnTIME_SCALE_trigX_Update(DRAW);
		DrawTrig_PosX(DRAW, &trigPosX_cursor);

	}
	else if((ButtonsCode == LEFT) || (ButtonsCode == RIGHT))
	{
		/* Clear trig X cursor if needed */
		btnTIME_SCALE_trigX_Update(CLEAR);
		DrawTrig_PosX(CLEAR, &trigPosX_cursor);

		if(ButtonsCode == LEFT)
		{
			gOSC_MODE.WindowPosition--;
			if(gOSC_MODE.WindowPosition <= 0) gOSC_MODE.WindowPosition = 0;
		}
		else if(ButtonsCode == RIGHT)
		{
			if(Get_numPoints() > (numPoints * (gOSC_MODE.WindowPosition + 1)))
			{
				gOSC_MODE.WindowPosition++;
				if(gOSC_MODE.WindowPosition >= 255) gOSC_MODE.WindowPosition = 255;
			}
		}

		/* Draw trig X cursor if needed */
		btnTIME_SCALE_trigX_Update(DRAW);
		DrawTrig_PosX(DRAW, &trigPosX_cursor);

		/* If Stop state read data and update oscillograms */
		if(gOSC_MODE.State == STOP)
		{
			Read_SRAM();
			Update_Oscillogram();	//обновлем осциллограммы
		}
	}

	Draw_btnTIME_SCALE(activeButton);
	Update_TimeScale_Menu_Text();
}



