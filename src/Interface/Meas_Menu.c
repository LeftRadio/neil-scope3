/*************************************************************************************
*
Description :  NeilScope3 Measurment menu sourse
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <math.h>

#include "main.h"
#include "Meas_Menu.h"
#include "User_Interface.h"
#include "Measurments.h"
#include "Processing_and_output.h"
#include "Settings.h"
#include "MeasButtons.c"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t numMeasMenu = Menu_1;

/* Private function prototypes -----------------------------------------------*/
static void ChangeMeasCursors(uint16_t *MeasCursor);
static void DrawMeasModeText(uint8_t Ypos, uint16_t Color);
static void MeasMenuCallBack(DrawState NewDrawState);

/* Exported variables --------------------------------------------------------*/
Menu_Struct_TypeDef MeasMenu = {
		{ LeftLineBtn, LowerBtn, (LeftLineBtn + btnWidht), UpLineBtn },
		DISABLE,
		0,
		MeasButtonsMAX,
		MeasButtonsMAX,
		MeasButtonsMAX,
		M_NTH,
		DOWN,
		UP,
		{ &btnMeasCursor1, &btnMeasCursor2, &btnMeasMode },
		&MeasMenuCallBack,
};


/* Functions ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : MeasMenuCallBack
* Description    : спецфичные действия для меню при отрисовке и очистке
* Input          : NewDrawState
* Return         : None
*******************************************************************************/
static void MeasMenuCallBack(DrawState NewDrawState)
{
	/* очищаем */
	LCD_ClearArea(LeftLineMeas, lowerLimit + 6, 398, upperLimit - 6, Active_BackColor);

	if(NewDrawState == DRAW)
	{
		if(rightLimit != 304) Change_horizontal_size(304);		// Изменяем размер области осциллограм
		Init_Meas_Values();
		LCD_SetGraphicsColor(Active_BorderColor);
		LCD_DrawLine(leftLimit, lowerLimit + 5, leftLimit, upperLimit - 5);

		LCD_SetTextColor(gInterfaceMenu.Buttons[1]->Active_FontColor);		// установить цвет текста
		DrawMeasModeText(CHA_SW_Ypos, INFO_A.Color);
		LCD_SetTextColor(gInterfaceMenu.Buttons[2]->Active_FontColor);		// установить цвет текста
		DrawMeasModeText(0, INFO_B.Color);
	}
	else if(NewDrawState == CLEAR)
	{
		/* очищаем */
		Draw_Cursor_Meas_Line(measCursor1, CLEAR);
		Draw_Cursor_Meas_Line(measCursor2, CLEAR);
		Change_horizontal_size(399);					// изменяем размер области вывода осциллограмм
	}
}


/*******************************************************************************
* Function Name  : ChangeMeasCursor_1
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void ChangeMeasCursor_1(void)
{
	ChangeMeasCursors(&measCursor1);
}


/*******************************************************************************
* Function Name  : ChangeMeasCursor_1
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void ChangeMeasCursor_2(void)
{
	ChangeMeasCursors(&measCursor2);
}


/*******************************************************************************
* Function Name  : ChangeMeasCursor
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static void ChangeMeasCursors(uint16_t *MeasCursor)
{
	uint16_t tLeftLim = (ActiveMode == &FFT_MODE)? leftLimit + 20 : leftLimit + 2;
	uint16_t tRightLim = (ActiveMode == &FFT_MODE)? leftLimit + 20 + 256 : rightLimit - 2;
	int8_t X_Chg;

	if((ButtonsCode == RIGHT) && (*MeasCursor < tRightLim))
	{
		Draw_Cursor_Meas_Line(*MeasCursor, CLEAR);
		if(speed_up_cnt++ >= 10) X_Chg = 5;
		else X_Chg = 1;
	}
	else if((ButtonsCode == LEFT) && (*MeasCursor > tLeftLim))  // push button Right
	{
		Draw_Cursor_Meas_Line(*MeasCursor, CLEAR);
		if(speed_up_cnt++ >= 10) X_Chg = -5;
		else X_Chg = -1;
	}
	else return;

	(*MeasCursor) = (*MeasCursor) + X_Chg;
	if(*MeasCursor < tLeftLim) *MeasCursor = tLeftLim;
	else if(*MeasCursor > tRightLim) *MeasCursor = tRightLim;

	if((*MeasCursor == measCursor1) && (*MeasCursor > (measCursor2 - 2))) *MeasCursor = measCursor2 - 2;
	else if((*MeasCursor == measCursor2) && (*MeasCursor < (measCursor1 + 2))) *MeasCursor = measCursor1 + 2;

	Draw_Cursor_Meas_Line(*MeasCursor, DRAW);
}


/*******************************************************************************
* Function Name  : ChangeMeasCursor
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void ChangeMeasMode(void)
{
	static int8_t indx = 0;

	if(ActiveMode == &FFT_MODE) return;

	if(ButtonsCode == RIGHT) indx = 1;
	else if(ButtonsCode == LEFT) indx = 0;
	else return;

	mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[indx];
	Draw_Menu(&MeasMenu);
}


/*******************************************************************************
* Function Name  : DrawMeasModeText
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static void DrawMeasModeText(uint8_t Ypos, uint16_t Color)
{
	if(mModeActive->indx == 0)
	{
		LCD_PutStrig(LeftLineMeas, mFRTH_YPOS + Ypos, 1, " Vpp:");
		LCD_PutStrig(LeftLineMeas, mTHRD_YPOS + Ypos, 1, " dT  :");
		LCD_PutStrig(LeftLineMeas, mSECND_YPOS + Ypos, 1, " Vdc:");
		LCD_PutStrig(LeftLineMeas, mFRST_YPOS + Ypos, 1, " RMS:");
		LCD_PutStrig(LeftLineMeas, mBASE_YPOS + Ypos, 1, " DC%:");
	}
	else if(mModeActive->indx == 1)
	{
		LCD_PutStrig(LeftLineMeas, mFRTH_YPOS + Ypos, 1, " V 1:");
		LCD_PutStrig(LeftLineMeas, mTHRD_YPOS + Ypos, 1, " V 2:");
		LCD_PutStrig(LeftLineMeas, mSECND_YPOS + Ypos, 1, " max:");
		LCD_PutStrig(LeftLineMeas, mFRST_YPOS + Ypos, 1, " min:");
		LCD_PutStrig(LeftLineMeas, mBASE_YPOS + Ypos, 1, "  F:");
	}
	else if(mModeActive->indx == 2)
	{
		LCD_PutStrig(LeftLineMeas, mFRTH_YPOS + Ypos, 1, " F C1:");
		LCD_PutStrig(LeftLineMeas, mTHRD_YPOS + Ypos, 1, " F C2:");

		LCD_PutStrig(LeftLineMeas, mBASE_YPOS + Ypos, 1, " F H1:");
	}
}


