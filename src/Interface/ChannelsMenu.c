/*************************************************************************************
*
Description :  NeilScope3 Quick Menu sourse
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Settings.h"
#include "User_Interface.h"
#include "ChannelsMenu.h"
#include "AutoCorrectCH.h"
#include "Trig_Menu.h"
#include "EPM570.h"
#include "Processing_and_output.h"
#include "ChannelsMenuButtons.c"
#include "colors 5-6-5.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t CH_sw = 0;
uint8_t COLOR_Mn_Upper_Y;
extern uint16_t *p256ColorMassive;

/* Exported variables --------------------------------------------------------*/
Menu_Struct_TypeDef ChannelA_Menu = {
		{ 35, 19, 120, ((LowerBtn - 3) + ((btnHight + btnSW) * (6 + 1))) + 1 },
		ENABLE,
		ChannelsMenu_ClipObj,
		6,
		6,
		6,
		DOWN,
		UP,
		{ &IntrlCorrect_A, &TextColor_A, &Color_A, &Auto_Correction_Zero_A, &Correction_Zero_A, &AutoDiv_A, &Offset_A },
		(void*)0
};

Menu_Struct_TypeDef ChannelB_Menu = {
		{ 110, 19, 195, UpLineBtn + 1 },
		ENABLE,
		ChannelsMenu_ClipObj,
		ChannelButtonsMAX,
		ChannelButtonsMAX,
		ChannelButtonsMAX,
		DOWN,
		UP,
		{ &TextColor_B, &Color_B, &Auto_Correction_Zero_B, &Correction_Zero_B, &AutoDiv_B, &Offset_B },
		(void*)0
};


/* Private function prototypes -----------------------------------------------*/
static void OFFSET_CH(int8_t sign);
static void AutoDivider(Channel_ID_TypeDef Channel, FunctionalState NewState);
static void Correct_Analog_Zero(CH_INFO_TypeDef *Channel);

static void Change_COLOR_CH(int8_t sign, uint8_t CHANNEL, uint8_t *pColorIndx);


/* Functions ----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : CorrectZ_CH_A
* Description    : Функция коррекции нуля канала А
* Input          : None
* Return         : None
*******************************************************************************/
void Correct_AnalogZero_CH_A(void)
{
	Correct_Analog_Zero(&INFO_A);
}


/*******************************************************************************
* Function Name  : CorrectZ_CH_B
* Description    : Функция коррекции нуля канала В
* Input          : None
* Return         : None
*******************************************************************************/
void Correct_AnalogZero_CH_B(void)
{ 
	Correct_Analog_Zero(&INFO_B);
}


/*******************************************************************************
* Function Name  : CorrectZ
* Description    : Функция смещения
* Input          : None
* Return         : None
*******************************************************************************/
static void Correct_Analog_Zero(CH_INFO_TypeDef *Channel)
{
	if(ActiveMode == &FFT_MODE) return;

	if((ButtonsCode == RIGHT) && (*(Channel->AD_Type.Analog.corrZ) < 1023)) *Channel->AD_Type.Analog.corrZ += 1;
	else if((ButtonsCode == LEFT) && (*(Channel->AD_Type.Analog.corrZ) > 0)) *Channel->AD_Type.Analog.corrZ -= 1;
	else return;

	Channel->AD_Type.Analog.Zero_PWM_values[Channel->AD_Type.Analog.Div] = *(Channel->AD_Type.Analog.corrZ);
}


/*******************************************************************************
* Function Name  : OFFSET_CH_A
* Description    : Функция смещения канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OFFSET_CH_A(void)
{
	int8_t sign;
	
	if(ActiveMode == &FFT_MODE) return;

	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else return;

	Set_CH_TypeINFO(CHANNEL_A);
	OFFSET_CH(sign);
}
 

/*******************************************************************************
* Function Name  : OFFSET_CH_B
* Description    : Функция смещения канала B
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void OFFSET_CH_B(void)
{
	int8_t sign;
	
	if(ActiveMode == &FFT_MODE) return;

	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else return;

	Set_CH_TypeINFO(CHANNEL_B);
	OFFSET_CH(sign);  
}


/*******************************************************************************
* Function Name  : OFFSET_CH
* Description    : Функция смещения каналов А/В
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
void OFFSET_CH(int8_t sign)
{
	uint8_t speed = (speed_up_cnt++ >= 10)? 5 : 1;
	int16_t diff = speed * sign;

	Draw_Cursor_CH(globalBackColor); // очищаем старый курсор

	/* изменяем позицию курсорa */
	if(pINFO->Position < lowerLimit + 12) pINFO->Position = lowerLimit + 12;
	else if (pINFO->Position > upperLimit - 12) pINFO->Position = upperLimit - 12;
	else pINFO->Position = pINFO->Position + diff;

	if((gOSC_MODE.SyncSourse == pINFO->Mode.ID) && (pnt_gOSC_MODE->oscSync != Sync_NONE))
	{
		if((pnt_gOSC_MODE->AnalogSyncType == Sync_Rise) || (pnt_gOSC_MODE->AnalogSyncType == Sync_Fall))
		{
			Sync_ChangeLevel(&Height_Y_cursor, diff);
		}
		else if((pnt_gOSC_MODE->AnalogSyncType == Sync_IN_WIN) || (pnt_gOSC_MODE->AnalogSyncType == Sync_OUT_WIN))
		{
			Sync_ChangeLevel(&Height_Y_cursor, diff);
			Sync_ChangeLevel(&Low_Y_cursor, diff);
		}
	}
//	Set_Trigger(pnt_gOSC_MODE->AnalogSyncType);		/* обновляем регистр ПЛИС - Trigger_level_A  */
	Draw_CH_Cursors();	/* обновляем указатели по новой позиции */
}


/*******************************************************************************
* Function Name  : AutoDivider_A
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void AutoDivider_A(void)
{
	if(ButtonsCode == RIGHT) AutoDivider(CHANNEL_A, ENABLE);
	else if(ButtonsCode == LEFT) AutoDivider(CHANNEL_A, DISABLE);
}


/*******************************************************************************
* Function Name  : AutoDivider_B
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void AutoDivider_B(void)
{
	if(ButtonsCode == RIGHT) AutoDivider(CHANNEL_B, ENABLE);
	else if(ButtonsCode == LEFT) AutoDivider(CHANNEL_B, DISABLE);
}


/*******************************************************************************
* Function Name  : AutoDivider
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void AutoDivider(Channel_ID_TypeDef Channel, FunctionalState NewState)
{
	static char *message;

	if(NewState == ENABLE) message = "Auto analog divider ON";
	else message = "Auto analog divider OFF";

	Set_AutoDivider_State(Channel, NewState);
	Show_Message(message);
}



/*----------------------------------------------- Colors --------------------------------------------------*/

/*******************************************************************************
* Function Name  : COLOR_CH_A
* Description    : Функция изменения цвета канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_COLOR_CH_A(void)
{
	int8_t sign;

	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)M256_Colors);
		return;
	}

	Change_COLOR_CH(sign, CHANNEL_A, &indxColorA);
}


/*******************************************************************************
* Function Name  : COLOR_CH_B
* Description    : Функция изменения цвета канала B
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_TextColor_CH_A(void)
{
	int8_t sign;

	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)grayScalle);
		return;
	}

	Change_COLOR_CH(sign, CHANNEL_A, &indxTextColorA);
}


/*******************************************************************************
* Function Name  : COLOR_CH_B
* Description    : Функция изменения цвета канала B
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_COLOR_CH_B(void)
{
	int8_t sign;

	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)M256_Colors);
		return;
	}

	Change_COLOR_CH(sign, CHANNEL_B, &indxColorB);
}


/*******************************************************************************
* Function Name  : COLOR_CH_B
* Description    : Функция изменения цвета канала B
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_TextColor_CH_B(void)
{
	int8_t sign;
	
	if(ButtonsCode == RIGHT) sign = 1;
	else if(ButtonsCode == LEFT) sign = -1;
	else
	{
		Init_COLOR_Mn(128, (uint16_t*)grayScalle);
		return;
	}

	Change_COLOR_CH(sign, CHANNEL_B, &indxTextColorB);
}



/*******************************************************************************
* Function Name  : Change_COLOR_CH
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Change_COLOR_CH(int8_t sign, uint8_t CHANNEL, uint8_t *pColorIndx)
{
	uint16_t Color, tColor;

	btnINFO *btnVDIV = (CHANNEL == CHANNEL_A)? (btnINFO*)gInterfaceMenu.Buttons[1] : (btnINFO*)gInterfaceMenu.Buttons[2];
	btnINFO *btn_AC_DC	= (CHANNEL == CHANNEL_A)? (btnINFO*)gInterfaceMenu.Buttons[8] : (btnINFO*)gInterfaceMenu.Buttons[9];

	Set_CH_TypeINFO(CHANNEL);

	*pColorIndx += sign;
    Change_COLOR_Mn_Position(*pColorIndx, &Color);

    if(p256ColorMassive == (uint16_t*)grayScalle)
    {
    	btnVDIV->Active_FontColor = Color;
    	btn_AC_DC->Active_FontColor = Color;
    }
    else
    {
    	tColor = Color_ChangeBrightness(Color, -24);
    	pINFO->Color = Color_ChangeBrightness(Color, 2);
    	btnVDIV->Color = tColor;
    	btn_AC_DC->Color = tColor;

    	Draw_Cursor_CH(pINFO->Color);
    }
	
    LCD_DrawButton(btnVDIV, NO_activeButton);
	LCD_DrawButton(btn_AC_DC, NO_activeButton);
}



/*******************************************************************************
* Function Name  : Intrlive_Correct
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Intrlive_Correct(void)
{
	if((InterliveCorrectionCoeff < 126) && (ButtonsCode == RIGHT)) InterliveCorrectionCoeff++;
	else if((InterliveCorrectionCoeff > -126) && (ButtonsCode == LEFT)) InterliveCorrectionCoeff--;
	else return;
}



