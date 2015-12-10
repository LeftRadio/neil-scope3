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
#include "EPM570_Registers.h"
#include "User_Interface.h"
#include "Settings.h"
#include "Digit_Trig_Menu.h"
#include "Trig_Menu.h"
#include "Processing_and_output.h"
#include "Digit_Trig_Menu_buttons.h"
#include "Measurments.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	volatile EPM570_HWRegisterSingle_TypeDef* CND_DAT;
	volatile EPM570_HWRegisterSingle_TypeDef* CND_MSK;
	volatile EPM570_HWRegisterSingle_TypeDef* DIFF_DAT;
	volatile EPM570_HWRegisterSingle_TypeDef* DIFF_MSK;
} DigitTrig_pRegs;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Draw_Digit_Trig_Menu_Callback(uint8_t Draw);

/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef DigitTrigMenu = {
		{ LeftLineBtn - 50, LowerBtn - 1, 398, 220 },
		ENABLE,
		Digit_ClipObj,
		0,
		DigitTrigButtonsMAX,
		0,
		M_SKIP,
		RIGHT,
		LEFT,
		{
			&btnCondDigit_0, &btnCondDigit_1, &btnCondDigit_2, &btnCondDigit_3,
			&btnCondDigit_4, &btnCondDigit_5, &btnCondDigit_6, &btnCondDigit_7,

			&btnDiffDigit_0, &btnDiffDigit_1, &btnDiffDigit_2, &btnDiffDigit_3,
			&btnDiffDigit_4, &btnDiffDigit_5, &btnDiffDigit_6, &btnDiffDigit_7
		},
		Draw_Digit_Trig_Menu_Callback,
};

DigitTrig_pRegs pRegisters = {

		&EPM570_Register_LA_CND_DAT,
		&EPM570_Register_LA_CND_MSK,
		&EPM570_Register_LA_DIFF_DAT,
		&EPM570_Register_LA_DIFF_MSK
};

/* Exported variables --------------------------------------------------------*/
extern btnINFO btnSync_Sourse;

/* Functions -----------------------------------------------------------------*/

/**
  * @brief  Digit_Trig_Menu_UpdateButtons
  * @param
  * @retval None
  */
static void Digit_Trig_Menu_UpdateButtons(uint8_t startIndex, uint8_t endIndex)
{
	uint8_t i;
	uint8_t index_bit_mask = 0;
	uint8_t bit_val = 0;

	for(i = startIndex; i <= endIndex; i++)
	{
		if(i > 7)
		{
			index_bit_mask = 0x80 >> (i - 8);
			bit_val = pRegisters.DIFF_MSK->data & index_bit_mask;

			if(bit_val != 0)
			{
				bit_val = pRegisters.DIFF_DAT->data & index_bit_mask;

				if(bit_val != 0)
				{
					DigitTrigMenu.Buttons[i]->Text = "}";
				}
				else
				{
					DigitTrigMenu.Buttons[i]->Text = "{";
				}

				DigitTrigMenu.Buttons[i]->Color = btn_ForeColor;
				DigitTrigMenu.Buttons[i]->Active_Color = Color_ChangeBrightness(btn_ForeColor, +4);
			}
			else
			{
				DigitTrigMenu.Buttons[i]->Text = "X";
				DigitTrigMenu.Buttons[i]->Color = gInterfaceMenu.Buttons[0]->Color;
				DigitTrigMenu.Buttons[i]->Active_Color = Color_ChangeBrightness(gInterfaceMenu.Buttons[0]->Color, -2);
			}
		}
		else
		{
			index_bit_mask = 0x80 >> i;
			bit_val = pRegisters.CND_MSK->data & index_bit_mask;

			if(bit_val != 0)
			{
				bit_val = pRegisters.CND_DAT->data & index_bit_mask;

				if(bit_val != 0)
				{
					DigitTrigMenu.Buttons[i]->Text = "1";
				}
				else
				{
					DigitTrigMenu.Buttons[i]->Text = "0";
				}

				DigitTrigMenu.Buttons[i]->Color = btn_ForeColor;
				DigitTrigMenu.Buttons[i]->Active_Color = Color_ChangeBrightness(btn_ForeColor, +4);
			}
			else
			{
				DigitTrigMenu.Buttons[i]->Text = "X";
				DigitTrigMenu.Buttons[i]->Color = gInterfaceMenu.Buttons[0]->Color;
				DigitTrigMenu.Buttons[i]->Active_Color = Color_ChangeBrightness(gInterfaceMenu.Buttons[0]->Color, -2);
			}
		}
	}
}


/*******************************************************************************
* Function Name  : Draw_Digit_Trig_Menu
* Description    : Функция включения/выключения меню
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_Digit_Trig_Menu_Callback(uint8_t Draw)
{
	if(Draw == CLEAR)
	{
		/* Restore show fft freq status */
		FrequencyMeas_SaveRestore_State(0, &gShowFFTFreq);
		FrequencyMeas_Draw(gShowFFTFreq);

		Draw_Trigg_Info(TriggShowInfo.Status);
	}
	else
	{
		/* Save show fft freq status */
		FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
		FrequencyMeas_Draw(FALSE);

		LCD_SetFont(&timesNewRoman12ptFontInfo);	/* изменяем шрифт на timesNewRoman12 */
		LCD_SetTextColor(LightGray3); //
		LCD_PutStrig(DigitTrigMenu.Coord[0] + 1, DigitTrigMenu.Coord[1] + btnNumHeight + btnSW, 1, "CND");
		LCD_PutStrig(DigitTrigMenu.Coord[0] + 1, DigitTrigMenu.Coord[1], 1, "DIFF");

		LCD_SetFont(&lucidaConsole10ptFontInfo);
		Digit_Trig_Menu_UpdateButtons(0, 15);

		Draw_Trigg_Info(ENABLE);
	}
}



/**
 * @brief  Change_Digit_Trig_Menu
 * @param  None
 * @retval None
 */
static void Change_Digit_Trig_Menu(void)
{
	uint8_t bit_val = 0, index_bit_mask;

	if(ButtonsCode == OK)
	{
		setActiveButton(TrigMenu.Buttons[TrigMenu.Indx]);
		LCD_DrawButton(btn, activeButton);

		Clear_Menu(&DigitTrigMenu);
		SetActiveMenu(&TrigMenu);
		return;
	}

	else if(ButtonsCode == UP)
	{
		/* DIFFERENT */
		if(DigitTrigMenu.Indx > 7)
		{
			index_bit_mask = 0x80 >> (DigitTrigMenu.Indx - 8);
			bit_val = pRegisters.DIFF_MSK->data & index_bit_mask;

			/* if bit is in ignored state */
			if(bit_val == 0)
			{
				pRegisters.DIFF_DAT->data &= ~index_bit_mask;
				pRegisters.DIFF_MSK->data |= index_bit_mask;
			}
			/* else if bit is not ignory state */
			else
			{
				pRegisters.DIFF_DAT->data |= index_bit_mask;
			}
		}

		/* CONDITION */
		else
		{
			index_bit_mask = 0x80 >> DigitTrigMenu.Indx;
			bit_val = pRegisters.CND_MSK->data & index_bit_mask;

			/* if bit is in ignored state */
			if(bit_val == 0)
			{
				pRegisters.CND_DAT->data &= ~index_bit_mask;
				pRegisters.CND_MSK->data |= index_bit_mask;
			}
			/* else if bit is not ignory state */
			else
			{
				pRegisters.CND_DAT->data |= index_bit_mask;

			}
		}
	}
	else if(ButtonsCode == DOWN)
	{
		/* DIFFERENT */
		if(DigitTrigMenu.Indx > 7)
		{
			index_bit_mask = 0x80 >> (DigitTrigMenu.Indx - 8);
			bit_val = pRegisters.DIFF_MSK->data & index_bit_mask;

			/* if bit is not ignory state */
			if(bit_val != 0)
			{
				bit_val = pRegisters.DIFF_DAT->data & index_bit_mask;

				if(bit_val != 0)
				{
					pRegisters.DIFF_DAT->data &= ~index_bit_mask;
				}
				else
				{
					pRegisters.DIFF_MSK->data &= ~index_bit_mask;
				}
			}
		}
		/* CONDITION */
		else
		{
			index_bit_mask = 0x80 >> DigitTrigMenu.Indx;
			bit_val = pRegisters.CND_MSK->data & index_bit_mask;

			/* if bit is not ignory state */
			if(bit_val != 0)
			{
				bit_val = pRegisters.CND_DAT->data & index_bit_mask;

				if(bit_val != 0)
				{
					pRegisters.CND_DAT->data &= ~index_bit_mask;
				}
				else
				{
					pRegisters.CND_MSK->data &= ~index_bit_mask;
				}
			}
		}
	}
	else return;

	/* verify index */
	if(DigitTrigMenu.Indx > 250) DigitTrigMenu.Indx = 15;
	else if(DigitTrigMenu.Indx > 15) DigitTrigMenu.Indx = 0;

	Digit_Trig_Menu_UpdateButtons(DigitTrigMenu.Indx, DigitTrigMenu.Indx);
	Update_triggInfo_OnScreen(ReDRAW);
}


