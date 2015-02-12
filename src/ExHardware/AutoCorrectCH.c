/*************************************************************************************
*
Description :  NeilScope3 Auto Correction sourse
Version     :  1.0.0
Date        :  25.12.2012
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>                           /* Include standard types */
#include <stdio.h>

#include "main.h"
#include "systick.h"
#include "HX8352_Library.h"
#include "User_Interface.h"
#include "gInterface_MENU.h"
#include "colors 5-6-5.h"
#include "Trig_Menu.h"
#include "Settings.h"
#include "Processing_and_output.h"
#include "Analog.h"
#include "EPM570.h"
#include "AutoCorrectCH.h"
#include "Host.h"
#include "Measurments.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Fillcoeff			((float)3.41)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t AutoCorrection_Upper_Y;
extern uint16_t CH_sw;

/* Private function prototypes -----------------------------------------------*/
static void Correction(void);
static int16_t Read_zCycle(void);
static void Update_Progress(uint16_t Val);
static void Fill_Progress(uint16_t X, uint16_t color);
static FlagStatus Get_CorrectionTerminate(void);

/* Functions ----------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : CorrectZ_CH_A
* Description    : Функция коррекции нуля канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Auto_CorrectZ_CH_A(void)
{
	Auto_CorrectZ_CH(CHANNEL_A);
}


/*******************************************************************************
* Function Name  : CorrectZ_CH_A
* Description    : Функция коррекции нуля канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Auto_CorrectZ_CH_B(void)
{
	Auto_CorrectZ_CH(CHANNEL_B);
}


/*******************************************************************************
* Function Name  : CorrectZ_CH_A
* Description    : Функция коррекции нуля канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Auto_CorrectZ_CH(Channel_ID_TypeDef Channel)
{    
	uint8_t tmp_Analog_Div;
	char *text = (Channel == CHANNEL_A)? "Channel A" : "Channel B";
	InterpolationMode_TypeDef *tActiveMode = ActiveMode;
	Channel_ID_TypeDef tAutoDivState = Get_AutoDivider_State(Channel);

	/* -------------  Host mode Calibration ------------- */
	if(HostMode == ENABLE)
	{
		INFO_A.Mode.EN = RUN; INFO_B.Mode.EN = RUN;
		Set_AutoDivider_State(Channel, DISABLE);

		Set_CH_TypeINFO(Channel);
		tmp_Analog_Div = pINFO->AD_Type.Analog.Div;
		Correction();
		pINFO->AD_Type.Analog.Div = tmp_Analog_Div;

		Change_AnalogDivider(Channel, tmp_Analog_Div);
		Set_AutoDivider_State(Channel, tAutoDivState);

		return;
	}

	/* -------------  Autonome mode Calibration ------------- */
	if(TriggShowInfo.Status == DISABLE) AutoCorrection_Upper_Y = upperLimit - 5;
	else AutoCorrection_Upper_Y = (upperLimit - 20);
	
	if(ButtonsCode != OK) return;
	INFO_A.Mode.EN = RUN; INFO_B.Mode.EN = RUN;
	Set_AutoDivider_State(Channel, DISABLE);
	changeInterpolation((InterpolationMode_TypeDef*)&IntNONE);

	NVIC_DisableIRQ(TIM2_IRQn);			// OFF buttons read

	/* Save show fft freq status */
	FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
	FrequencyMeas_Draw(FALSE);

	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_A, leftLimit, rightLimit - 2);
	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_B, leftLimit, rightLimit - 2);

	/* устанавливаем ограничения для отрисовки */
	LCD_ClearArea(rightLimit - 151, AutoCorrection_Upper_Y - 68, rightLimit - 1, AutoCorrection_Upper_Y - 6, Active_BackColor);
	LCD_DrawGrid(&activeAreaGrid, DRAW);	// перерисовываем сетку в области осциллограмм
	Set_New_ClipObject(rightLimit - 151, AutoCorrection_Upper_Y - 68, rightLimit + 1, AutoCorrection_Upper_Y + 1, IN_OBJECT, AutoCorr_ClipObj);
	
	/* рисуем прямоугольник полосы прогресса */
	LCD_SetGraphicsColor(White);
	LCD_DrawRect(rightLimit - 150, AutoCorrection_Upper_Y - 22, rightLimit-5, AutoCorrection_Upper_Y - 6);
	
	/* переключаем канал */
	Set_CH_TypeINFO(Channel);
	tmp_Analog_Div = pINFO->AD_Type.Analog.Div;

	/* выводим текст */
	LCD_SetTextColor(pINFO->Color);
	LCD_PutStrig(rightLimit - 150, AutoCorrection_Upper_Y - 37, 0, text);
	LCD_PutStrig(rightLimit - 150, AutoCorrection_Upper_Y - 52, 0, "Range:");

	LCD_SetTextColor(Red);
	LCD_PutStrig(rightLimit - 150, AutoCorrection_Upper_Y - 67, 0, "For exit push LEFT");

	Correction();
	
	pINFO->AD_Type.Analog.Div = tmp_Analog_Div;
	Change_AnalogDivider(Channel, pINFO->AD_Type.Analog.Div);
	
	/* убираем ограничения для отрисовки */
	Clear_ClipObject(AutoCorr_ClipObj);
	LCD_ClearArea(rightLimit - 151, AutoCorrection_Upper_Y - 68, rightLimit - 1, AutoCorrection_Upper_Y - 6, Active_BackColor);
	
	/* перерисовываем сетку в области осциллограмм, обновлем осциллограммы */
	LCD_DrawGrid(&activeAreaGrid, DRAW);
	Update_Oscillogram();

	/* Restore show fft freq status */
	FrequencyMeas_SaveRestore_State(0, &gShowFFTFreq);
	FrequencyMeas_Draw(gShowFFTFreq);

	changeInterpolation(tActiveMode);
	Set_AutoDivider_State(Channel, tAutoDivState);

	NVIC_EnableIRQ(TIM2_IRQn);	
}


/*******************************************************************************
* Function Name  : Correction
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Correction(void)
{
	uint8_t Valid_Zero = 0; //, tmpCH;
	uint8_t tValid;
	__IO int16_t _cnt, avrg_data_A = 127;
	uint16_t tX;
	char *text = "", *ptext = "";
	uint8_t Div = pINFO->AD_Type.Analog.Div;

//	Set_AutoDivider_State(pINFO->Mode.ID, DISABLE);

	for(Div = Divider_Position_MIN; Div <= Divider_Position_MAX; Div++)
	{
		pINFO->AD_Type.Analog.Div = Div;

		if(HostMode == DISABLE)
		{
			for(_cnt = 2; _cnt < 490; _cnt++)
			{
				tX = (uint16_t)(((float)_cnt - 1) / Fillcoeff);
				Fill_Progress(tX, Active_BackColor);
			}

			LCD_SetGraphicsColor(Orange);
			LCD_SetTextColor(Active_BackColor);
			LCD_PutStrig(rightLimit - 90, AutoCorrection_Upper_Y - 52, 0, text);
			LCD_PutStrig(rightLimit - 60, AutoCorrection_Upper_Y - 37, 0, ptext );

			text = Change_AnalogDivider(pINFO->Mode.ID, Div);

			LCD_SetTextColor(LighGreen);
			LCD_PutStrig(rightLimit - 90, AutoCorrection_Upper_Y - 52, 0, text);
			LCD_PutStrig(rightLimit - 60, AutoCorrection_Upper_Y - 37, 0, "WAIT...");
		}
		else Change_AnalogDivider(pINFO->Mode.ID, Div);

		for(_cnt = 2; _cnt < 490; _cnt++)
		{  
			Update_Progress(_cnt);

			*(pINFO)->AD_Type.Analog.corrZ = _cnt + 150;
			
			delay_ms(10);
			avrg_data_A = Read_zCycle();			// read data
			
			Valid_Zero = 0;
			if((avrg_data_A >= 0) && (avrg_data_A <= 1))
			{
				pINFO->AD_Type.Analog.Zero_PWM_values[Div] = *(pINFO->AD_Type.Analog.corrZ);
				Valid_Zero = 1;			  			  
				Update_Progress(489);
				break;         
			}

			if(Get_CorrectionTerminate() == SET)
			{
				INFO_A.Mode.EN = RUN; INFO_B.Mode.EN = RUN;
				return;
			}
	    }		
		
		if(HostMode == DISABLE)
		{
			/* clear old text */
			LCD_SetTextColor(Active_BackColor);
			LCD_PutStrig(rightLimit - 60, AutoCorrection_Upper_Y - 37, 0, "WAIT...");

			/* update state text */
			if(Valid_Zero == 0){ LCD_SetTextColor(Red); ptext = "Falure";  }
			else { LCD_SetTextColor(LighGreen); ptext = "OK"; }
			LCD_PutStrig(rightLimit - 60, AutoCorrection_Upper_Y - 37, 0, ptext );
			LCD_SetTextColor(Active_BackColor);
		}
		else
		{
			if(Valid_Zero == 1) tValid = 0xAA;
			else tValid = 0x7F;

			OUT_HostData[0] = pINFO->Mode.ID;				// 0 - CHA; 1 - CHB
			OUT_HostData[1] = tValid;

			delay_ms(100);
			Transmit_To_Host((uint8_t)(0x12 + 0x40), OUT_HostData, 2);	// 0xFE - CALIBRATE_ZERO_CMD
		}

		delay_ms(300);
    }

	/* sending finish calibration packed */
	if(HostMode != DISABLE)
	{
		OUT_HostData[0] = pINFO->Mode.ID;				// 0 - CHA; 1 - CHB
		OUT_HostData[1] = 0x25;
		OUT_HostData[2] = 0x52;

		Transmit_To_Host((uint8_t)(0x12 + 0x40), OUT_HostData, 3);	// 0xFE - CAL

		Beep_Start(); delay_ms(300);
		Beep_Start(); delay_ms(100);
		Beep_Start();
	}

	INFO_A.Mode.EN = RUN; INFO_B.Mode.EN = RUN;
}


/*******************************************************************************
* Function Name  : Fill_Progress
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Update_Progress(uint16_t Val)
{
	uint16_t tX;
	static uint16_t OLD_tX = 0xFFFF;

	if(HostMode == DISABLE)
	{
		tX = (uint16_t)(((float)Val - 1) / Fillcoeff);
		Fill_Progress(tX, Auqa);
	}
	else
	{
		tX = ((Val - 2) * 100) / 487;

		if(tX != OLD_tX)
		{
			OUT_HostData[0] = pINFO->Mode.ID;				// 0 - CHA; 1 - CHB
			OUT_HostData[1] = pINFO->AD_Type.Analog.Div;	// operating divider
			OUT_HostData[2] = tX;							// progress (0 - 100)

			Transmit_To_Host((uint8_t)(0x12 + 0x40), OUT_HostData, 3);	// 0x12 - CALIBRATE_ZERO_CMD

			OLD_tX = tX;
		}
	}
}


/*******************************************************************************
* Function Name  : Fill_Progress
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Fill_Progress(uint16_t X, uint16_t color)
{
	uint16_t tmL = 0, clrColor;

	Set_LCD_CS();	//  установить низкий уровень CS LCD
					// используем "низкий" доступ к ЖК для макс. скорости	  	  
	
	LCD_SetCursor(rightLimit - 149 + X, AutoCorrection_Upper_Y - 21);
	
	tmL = 0;
		
	while(tmL++ != 15) // 
	{				   	
		/* Проверяем на совпадение координат очистки с сеткой,
	  	если совпало то очищаем цветом сетки */
		clrColor = Verify_Grid_Match(rightLimit - 149 + X, AutoCorrection_Upper_Y - 21 + tmL);
		
		if(clrColor == Active_BackColor) clrColor = color;
	
		LCD_PutPixel(clrColor);
	}			
	Reset_LCD_CS();
}


/*******************************************************************************
* Function Name  : Read_zCycle
* Description    : 
* Input          : None
* Output         : None
* Return         : uint16_t sum
*******************************************************************************/
int16_t Read_zCycle(void)
{
	uint8_t _cnt;
	int32_t sum = 0;
	
	/* если канал В, то выключим канал А */
	if(pINFO == &INFO_B) INFO_A.Mode.EN = STOP;
	pnt_gOSC_MODE->State = RUN;			 			// Установим флаг состояния осциллографа
	pINFO->Procesing(pINFO->Mode);					// запустим цикл записи в SRAM и получаем данные по нужному каналу
	INFO_A.Mode.EN = RUN;							// восстанавливаем состояние канала А
	
	/* усредняем 256 значений */
	for(_cnt = 0; _cnt < 255; _cnt++) sum += pINFO->DATA[_cnt];

	return (int16_t)(sum/256);		// возвращаем усредненное значение
}



/*******************************************************************************
* Function Name  : Get_CorrectionTerminate
* Description    :
* Input          : None
* Output         : None
* Return         : uint16_t sum
*******************************************************************************/
FlagStatus Get_CorrectionTerminate(void)
{
	if(HostMode == DISABLE)
	{
		if(EPM570_Read_Keys() == LEFT) return SET;
		else return RESET;
	}
	else return Host_GetTerminateCmd();
}




