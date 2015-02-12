/*************************************************************************************
*
Description :  NeilScope3 Digit Trig Menu sourse
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "EPM570.h"
#include "User_Interface.h"
#include "Settings.h"
#include "Digit_Trig_Menu.h"
#include "Trig_Menu.h"
#include "Processing_and_output.h"
#include "Digit_Trig_Menu_buttons.c"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef DigitTrigMenu = {
		{ LeftLineBtn - 77, LowerBtn, 399, 206 },
		ENABLE,
		Digit_ClipObj,
		DigitTrigButtonsMAX,
		DigitTrigButtonsMAX,
		DigitTrigButtonsMAX,
		RIGHT,
		LEFT,
		{
			&btnCondDigit_0, &btnCondDigit_1, &btnCondDigit_2, &btnCondDigit_3,
			&btnCondDigit_4, &btnCondDigit_5, &btnCondDigit_6, &btnCondDigit_7,

			&btnDiffDigit_0, &btnDiffDigit_1, &btnDiffDigit_2, &btnDiffDigit_3,
			&btnDiffDigit_4, &btnDiffDigit_5, &btnDiffDigit_6, &btnDiffDigit_7
		},
		(void*)0
};

uint8_t *pnt_State_Sync_Var = &ConditionState_Sync_Var;
static uint8_t tmp_Digit_Num_Indx = 0;

/* Exported variables --------------------------------------------------------*/
extern btnINFO btnSync_Sourse;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

///*******************************************************************************
//* Function Name  : Draw_Digit_Trig_Menu
//* Description    : Функция включения/выключения меню
//* Input          : None
//* Return         : None
//*******************************************************************************/
//void Draw_Digit_Trig_Menu(uint8_t Draw)
//{
//	uint8_t i;
//	static FunctionalState trigShowStatus;
//
//	trigShowStatus = TriggShowInfo.Status;	// сохраним состояние инфо триггера
//
//	if(Draw == CLEAR) //если очистка
//	{
//		pMNU = Change_Trig_Menu_indx;				// изменяем указатель на функцию основного меню
//
//		setActiveButton(&btnSync_Sourse);
//		LCD_DrawButton(btn, activeButton);
//
//		LCD_SetGraphicsColor(Active_BackColor);
//		LCD_DrawFillRect(LeftLineBtn - 77, LowerBtn - 1, rightLimit - 1, upperLimit - 24, 0, 0); // Oчищаем область быстрого меню
//
//		Clear_ClipObject(Digit_ClipObj);				// убираем ограничения для отрисовки
//		setActiveButton(&btnSync_Sourse);		// восстанавливаем сохраненную активную кнопку
//		LCD_DrawGrid(&activeAreaGrid, DRAW);	// перерисовываем сетку в области осциллограмм
//		Update_Oscillogram();					// обновлем осциллограммы
//
//
//		if(trigShowStatus == DISABLE) Hide_Show_triggInfo();
//
//		return; //выход
//	}
//
//	/* устанавливаем ограничения для отрисовки */
//    Set_New_ClipObject(LeftLineBtn - 77, LowerBtn - 1, rightLimit + 1, upperLimit + 1, IN_OBJECT, Digit_ClipObj);
//	Update_Oscillogram();	// обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения
//
//	/* отрисовываем меню */
//	LCD_SetFont(&lucidaConsole10ptFontInfo);	// изменяем шрифт
//	for(i = 1; i < 16; i++ ) LCD_DrawButton((btnINFO*)btnDigit_Num[i], NO_activeButton);
//	setActiveButton((btnINFO*)btnDigit_Num[0]);
//	btn->Color = Orange;
//	LCD_DrawButton(btn, NO_activeButton);
//
//	if(TriggShowInfo.Status != ENABLE) Hide_Show_triggInfo();
//
//    LCD_SetFont(&timesNewRoman12ptFontInfo);	/* изменяем шрифт на timesNewRoman12 */
//	LCD_SetTextColor(White); //
//	LCD_PutStrig(LeftLineBtn - 76, LowerBtn + btnNumHeight + btnSW, 0, "Condition");
//	LCD_SetTextColor(LightGray3);
//	LCD_PutStrig(LeftLineBtn - 76, LowerBtn, 0, "Different");
//}



/*******************************************************************************
* Function Name  : Change_Trig_Menu_indx
* Description    : Функция смены индекса меню триггера
* Input          : None
* Return         : None
*******************************************************************************/
static void Change_Digit_Trig_Menu(void)
{
	uint16_t ColorCond = 0, ColorDiff = 0;
	static uint8_t IndxVar = 0;

	if(ButtonsCode == OK)
	{
		Clear_Menu(&DigitTrigMenu);
		SetActiveMenu(&TrigMenu);		// делаем активным меню
		return;
	}

	else if(ButtonsCode == UP)
	{
		if(DigitTrigMenu.Indx > 7)
		{
			if(DifferentState_Sync_Fall == ENABLE)
			{
				DifferentState_Sync_Fall = DISABLE;
				DifferentState_Sync_Rise = DISABLE;
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "-";
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = Orange;
				*pnt_State_Sync_Var &= ~(0x01 << (7 - IndxVar));
			}
			else
			{
				DifferentState_Sync_Rise = ENABLE;
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "}";
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = LightRed2;
				*pnt_State_Sync_Var |= (0x01 << (7 - IndxVar));
			}
		}
		else
		{
			DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "1";
			DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = LightRed2;
		 	*pnt_State_Sync_Var |= (0x01 << (7 - IndxVar));
		}
	}
	else if(ButtonsCode == DOWN)
	{
		if(DigitTrigMenu.Indx > 7)
		{
			if(DifferentState_Sync_Rise == ENABLE)
			{
				DifferentState_Sync_Rise = DISABLE;
				DifferentState_Sync_Fall = DISABLE;
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "-";
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = Orange;
				*pnt_State_Sync_Var &= ~(0x01 << (7 - IndxVar));
			}
			else
			{
				DifferentState_Sync_Fall = ENABLE;
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "{";
				DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = LightRed2;
				*pnt_State_Sync_Var |= (0x01 << (7 - IndxVar));
			}
		}
		else
		{
			DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Text = "0";
			DigitTrigMenu.Buttons[DigitTrigMenu.Indx]->Color = btn_ForeColor;
			*pnt_State_Sync_Var &= ~(0x01 << (7 - IndxVar));
		}
	}
	else return;

	/* switch varible pointer and verify index */
	if(DigitTrigMenu.Indx > 15)	DigitTrigMenu.Indx = 15;

	if(DigitTrigMenu.Indx <= 7)
	{
		pnt_State_Sync_Var = &ConditionState_Sync_Var;
		IndxVar = DigitTrigMenu.Indx;
		ColorCond = White;
		ColorDiff = LightGray3;
	}
	else if((DigitTrigMenu.Indx > 7) && (DigitTrigMenu.Indx <= 15))
	{
		pnt_State_Sync_Var = &DifferentState_Sync_Var;
		IndxVar = DigitTrigMenu.Indx - 8;
		ColorCond = LightGray3;
		ColorDiff = White;
	}

	LCD_SetFont(&timesNewRoman12ptFontInfo);	// изменяем шрифт на timesNewRoman12
	LCD_SetTextColor(ColorCond); //
	LCD_PutStrig(LeftLineBtn - 76, LowerBtn + btnNumHeight + btnSW, 1, "Condition");
	LCD_SetTextColor(ColorDiff);
	LCD_PutStrig(LeftLineBtn - 76, LowerBtn, 1, "Different");

	Update_triggInfo_OnScreen(ReDRAW);		// обновляем инфо триггера

	tmp_Digit_Num_Indx = DigitTrigMenu.Indx;	// сохраним текущий индекс
}

