/**
  ******************************************************************************
  * @file	 	Processing_and_output.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Processing and output sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include <math.h>                             /* math library */
#include "main.h"
#include "User_Interface.h"
#include "gInterface_MENU.h"
#include "EPM570.h"
#include "Settings.h"
#include "Analog.h"
#include "Measurments.h"
#include "Processing_and_output.h"
#include "Trig_Menu.h"
#include "init.h"
#include "Host.h"
#include "Sweep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Processing_Analog_CH(gChannel_MODE CH_Mode);
static void Processing_Digit_CH(gChannel_MODE CH_Mode);

static __inline void Data_Reprocessing(void);

static __inline void DrawClear_iLine(uint16_t X0pos, uint16_t Y0pos, uint16_t Y1pos, DrawState NewDrawState);
static __inline void DrawClear_MAXMIN_Line(uint16_t X0pos, uint16_t Y0pos, uint16_t Y1pos, DrawState NewDrawState);

/* функции очистки, используются только при переключении
   режима отображения/интерполяции, при отрисовке не исползуются */
static void Clear_OLD_Data_NONE(uint16_t X0, uint16_t X1);
static void Clear_OLD_Data_Lines(uint16_t X0, uint16_t X1);
static void Clear_OLD_Data_MIN_MAX(uint16_t X0, uint16_t X1);
static void Clear_OLD_Data_FFT(uint16_t X0, uint16_t X1);


/* Private variables ---------------------------------------------------------*/
CH_INFO_TypeDef *pINFO;

/* Analog Channel A */
CH_INFO_TypeDef INFO_A =
{
		DarkOrange2 + (10 << 11) + (20 << 5),	// Цвет канала
		155,
		{ CHANNEL_A, ON, RUN_AC },				// ID, state, indx(STOP; RUN_AC; RUN_DC)
		{0},             						// данные канала
		{0},            						// данные для вывода и очистки на экран
		{DISABLE},  			   				// данные для очистки экрана

		/* A/D Type */
		{
				/* analog type */
				{
						Divider_Position_MAX,	// положение аналогового делителя
						{80}, 	   				// массив значений ШИМ для каждого положения аналогового делителя
						&TIM3->CCR4,
				},
		},

		Processing_Analog_CH					// указатель на функцию вывода
};

/* Analog Channel B */
CH_INFO_TypeDef INFO_B =
{
		DarkAuqa + (10 << 11) + (10 << 5) + 10,	// Цвет канала
		65,                     				// координата на экране указателя канала
		{ CHANNEL_B, ON, RUN_AC },				// ID, state, indx(STOP; RUN_AC; RUN_DC)
		{0},              					// данные канала
		{0},            						// данные для вывода и очистки на экран
		{DISABLE},  			   				// данные для очистки экрана

		/* A/D Type */
		{
				/* analog type */
				{
						Divider_Position_MAX,	// положение аналогового делителя
						{80}, 	   				// массив значений ШИМ для каждого положения аналогового делителя
						&TIM3->CCR3,
				},
		},

		Processing_Analog_CH					// указатель на функцию вывода
};

/* Digital Channel A (8 bit widht) */
CH_INFO_TypeDef DINFO_A =
{
		DarkAuqa + (10 << 11) + (10 << 5) + 10,	// Цвет канала
		65,
		{ CHANNEL_A, ON, RUN },					// ID, state, indx(STOP; RUN_AC; RUN_DC)
		{0},              						// данные канала
		{0},            						// данные для вывода и очистки на экран
		{DISABLE},  			   				// данные для очистки экрана

		/* A/D Type */
		{
				/* digital type */
				{

				},
		},

		Processing_Digit_CH						// указатель на функцию вывода
};


uint8_t Shift = 5;

/* переменные режимов интерполяции/отображения/FFT */
InterpolationMode_TypeDef IntNONE = { InterpolationNONE, Clear_OLD_Data_NONE, 1, -1, "int NONE", 0};
InterpolationMode_TypeDef IntLIN  = { InterpolationLINEAR, Clear_OLD_Data_Lines, 1, -1, "int LIN", 1};
InterpolationMode_TypeDef IntMIN_MAX = { MIN_MAX_MODEfunc, Clear_OLD_Data_MIN_MAX, 2, 1, " MIN_MAX", 2};
InterpolationMode_TypeDef FFT_MODE = { FFT_MODEfunc, Clear_OLD_Data_FFT, 1, 0, " FFT", 3};
const InterpolationMode_TypeDef *InterpModes[4] = { &IntNONE, &IntLIN, &IntMIN_MAX, &FFT_MODE };
InterpolationMode_TypeDef *ActiveMode = (InterpolationMode_TypeDef*)&IntNONE;
volatile uint8_t Interpolate_AciveMode_Index = 0;


/* Functions ----------------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : setCondition
 * Description    : приостановка/запуск каналов
 * Input          : None
 * Return         : None
 *******************************************************************************/
void Inerlive_Cmd(FunctionalState NewState)
{
	if(NewState == ENABLE)
	{
		gOSC_MODE.Interleave = TRUE;
	}
	else
	{
		gOSC_MODE.Interleave = FALSE;
	}
}


/*******************************************************************************
* Function Name  : change_CH_INFO
* Description    : переключение указателя каналов
* Input          : None
* Return         : None
*******************************************************************************/
void Set_CH_TypeINFO(Channel_ID_TypeDef NewChannelType)
{
   switch (NewChannelType)
   {
   case CHANNEL_A: pINFO = &INFO_A;
   	   break;
   case CHANNEL_B: pINFO = &INFO_B;
   	   break;
   case CHANNEL_DIGIT: pINFO = &DINFO_A;
      break;

   default: return;
   }
}


/*******************************************************************************
* Function Name  : Update_Oscillogram
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Update_Oscillogram(void)
{
	uint8_t tmpOscModeState = gOSC_MODE.State;
	CH_INFO_TypeDef *tmpInfo = pINFO;

	gOSC_MODE.State = STOP;
	if(ActiveMode == &FFT_MODE)
	{
		Clear_OLD_DataCH_ON_SCREEN(CHANNEL_A, leftLimit, rightLimit - 2);
		Clear_OLD_DataCH_ON_SCREEN(CHANNEL_B, leftLimit, rightLimit - 2);
	}
	INFO_A.Procesing(INFO_A.Mode);
	INFO_B.Procesing(INFO_B.Mode);
	
	gOSC_MODE.State = tmpOscModeState;
	pINFO = tmpInfo;
}


/*******************************************************************************
* Function Name  : changeInterpolation
* Description    : функция переключения указателя на нужную функцию отображения сигнала, интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
void changeInterpolation(InterpolationMode_TypeDef *NewMode)
{
	ActiveMode = NewMode;
	Interpolate_AciveMode_Index = ActiveMode->Indx;
}


/*******************************************************************************
* Function Name  : Data_Reprocessing
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static __inline void Data_Reprocessing_Interpolate(int cnt, int8_t fPoint_Indx, int8_t sPoint_Indx, uint8_t ScaleCoeff, uint8_t NumPoints)
{
	double intPoint;
	uint8_t i;

	if(pINFO->DATA[fPoint_Indx] == pINFO->DATA[sPoint_Indx])
	{
		for(i = 1; i < NumPoints; i++) pINFO->DATA[(cnt - NumPoints) + i] = pINFO->DATA[cnt];
	}
	else
	{
		intPoint = fabs((pINFO->DATA[fPoint_Indx] + 127) - (pINFO->DATA[sPoint_Indx] + 127)) / (double)ScaleCoeff;

		if(pINFO->DATA[fPoint_Indx] > pINFO->DATA[sPoint_Indx])
		{
			for(i = 1; i < NumPoints; i++)	pINFO->DATA[(cnt - NumPoints) + i] = pINFO->DATA[sPoint_Indx] + (int8_t)round(intPoint * i);
		}
		else
		{
			for(i = 1; i < NumPoints; i++)	pINFO->DATA[cnt - i] = pINFO->DATA[fPoint_Indx] + (int8_t)round(intPoint * (double)i);
		}
	}
}


/*******************************************************************************
* Function Name  : Data_Reprocessing
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
static __inline void Data_Reprocessing(void)
{	  	
	uint16_t StopPoint = (rightLimit - leftLimit) - 1;
	int cnt, tmp;

	/* Check */
	if((ActiveMode == &IntMIN_MAX) || (gOSC_MODE.State == STOP)) return;

	/* Interpolate */
	for(cnt = StopPoint; cnt >= (*SweepScale); cnt = cnt - (*SweepScale))
	{
		tmp = (cnt/(*SweepScale));

		pINFO->DATA[cnt] = pINFO->DATA[tmp];
		
		if((*SweepScale) == 2) pINFO->DATA[cnt - 1] = (pINFO->DATA[tmp] + pINFO->DATA[tmp - 1]) / 2;
		else
		{
			Data_Reprocessing_Interpolate(cnt, tmp, tmp - 1, (*SweepScale), (*SweepScale));
		}
	}

	/* if (NumPoint % SwipScale != 0) interpolate remain */
	if((cnt > 0) && (cnt < (*SweepScale)))
	{
		while(cnt != 0){ pINFO->DATA[cnt] = pINFO->DATA[cnt + 1]; cnt--; }
	}
}


/*******************************************************************************
* Function Name  : Processing_Analog_CH
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Processing_Analog_CH(gChannel_MODE CH_Mode)
{
	uint8_t tmp = NO_activeButton;

	/* If channel is STOP(OFF) return */
	if(CH_Mode.EN == STOP) return;

	/* Set pointer pINFO to the appropriate channel ID, if it no sets before */
	if(pINFO->Mode.ID != CH_Mode.ID) Set_CH_TypeINFO(CH_Mode.ID);

	NVIC_DisableIRQ(ADC1_2_IRQn);

	/* If global state is RUN */
	if(gOSC_MODE.State == RUN)
	{

		if( !(INFO_A.Mode.EN == RUN && pINFO == &INFO_B) )
		{
			if(EPM570_SRAM_Write() != COMPLETE) return;
			EPM570_SRAM_Read();
		}

		/* Проверяем "зашкал" и переключаем делитель при необходимости */
		if(Analog_AutodividerMain() == ENABLE)
		{
			Clear_OLD_DataCH_ON_SCREEN(pINFO->Mode.ID, leftLimit, rightLimit - 2);
			gInterfaceMenu.Buttons[pINFO->Mode.ID + 1]->Text = Change_AnalogDivider(pINFO->Mode.ID, pINFO->AD_Type.Analog.Div);

			if((pMenu == &gInterfaceMenu) && (pMenu->Indx == pINFO->Mode.ID + 1)) tmp = activeButton;
			LCD_DrawButton(gInterfaceMenu.Buttons[pINFO->Mode.ID + 1], tmp);
		}
	}

	if(HostMode == DISABLE)
	{
		/* Если коэффициент цифровой лупы больше 1, то репроцессинг данных */
		if((*SweepScale) > 1) Data_Reprocessing();

		/* вызов отрисовки соответсвующей интерполяции */
		ActiveMode->InterpolationMODEfunc(0, rightLimit - leftLimit, (int16_t)pINFO->Position);

		/* автоизмерения */
		if(gOSC_MODE.autoMeasurments == ON) mModeActive->MeasurmentsFunc();
		else if(gShowFFTFreq == TRUE) Calc_Show_Freq();
	}

	NVIC_EnableIRQ(ADC1_2_IRQn);
}


/*******************************************************************************
* Function Name  : Processing_Digit_CH
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Processing_Digit_CH(gChannel_MODE CH_Mode)
{

}




/*******************************************************************************
* Function Name  : InterpolationNONE
* Description    : Очистка и вывод на экран осциллограммы при отсутсвии интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
void InterpolationNONE(uint16_t X0, uint16_t X1, int16_t Position)
{
	uint16_t cnt_;
	uint16_t clrColor;  
	
	/* устанавливаем цвет осциллограммы */
	LCD_SetGraphicsColor(pINFO->Color);
	
	for (cnt_ = X0; cnt_ < X1 - 2; cnt_++)
	{
		if(Verify_Clip_Point(cnt_ + leftLimit + 1, pINFO->visDATA[cnt_]) != SET)
		{
			/* Проверяем на совпадение координат точки очистки с сеткой, если совпало то очищаем цветом сетки */
			clrColor = Verify_Grid_Match(cnt_ + leftLimit + 1, pINFO->visDATA[cnt_]);

			/* очищаем точку */
			DrawPixel(cnt_ + leftLimit + 1, pINFO->visDATA[cnt_], clrColor);
		}

		/* проверка на выход за пределы экрана, обновляем значения двух точек для отображения */
		if((pINFO->DATA[cnt_] + Position) < lowerLimit + 6) pINFO->visDATA[cnt_] = lowerLimit + 6;
		else if(((int16_t)pINFO->DATA[cnt_] + Position) > upperLimit - 6) pINFO->visDATA[cnt_] = upperLimit - 6;
		else pINFO->visDATA[cnt_] = pINFO->DATA[cnt_] + Position;


		if(Verify_Clip_Point(cnt_ + leftLimit + 1, pINFO->visDATA[cnt_]) != SET)
		{
			/* рисуем точку */
			DrawPixel(cnt_ + leftLimit + 1, pINFO->visDATA[cnt_], pINFO->Color);	 
		}
	}  
}


/*******************************************************************************
* Function Name  : InterpolationLINEAR
* Description    : Очистка и вывод на экран осциллограммы при линейной интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
void InterpolationLINEAR(uint16_t X0, uint16_t X1, int16_t Position)
{
	uint16_t cnt_ = 0;
	uint8_t oldVisData;
	uint16_t tmpLineX0, tmpLineY0, tmpLineY1;
	int16_t tmpData;

	/* запоминаем нулевую точку для отрисовки и последующей очистки */
	oldVisData = pINFO->visDATA[X0];

	/* проверка первой линии на выход за границы области осциллограмм */
	tmpData = (pINFO->DATA[X0] + Position);
	if(tmpData > upperLimit - 6) pINFO->visDATA[X0]  = upperLimit - 6;
	else if(tmpData < lowerLimit + 6) pINFO->visDATA[X0] = lowerLimit + 6;
	else pINFO->visDATA[X0] = tmpData;


	for (cnt_ = X0; cnt_ < X1 - 1; cnt_++)
	{     
		tmpLineX0 = cnt_ + leftLimit + 1;

		tmpLineY0 = pINFO->visDATA[cnt_];
		tmpLineY1 = pINFO->visDATA[cnt_ + 1];
		
		/* очищаем линию от точки (cnt_ ) до точки (cnt_ + 1) */
//		if(gOSC_MODE.State == RUN)
//		{
			if(pINFO->OldData_PointsFlag[cnt_] == ENABLE) DrawClear_iLine(tmpLineX0, oldVisData, tmpLineY1, CLEAR);
			oldVisData = tmpLineY1;
//		}
		
		/* проверка на выход за пределы экрана, обновляем значения двух точек для отображения */
		tmpData = (pINFO->DATA[cnt_ + 1] + Position);
		if(tmpData < lowerLimit + 6) tmpLineY1 = lowerLimit + 6;
		else if(tmpData > upperLimit - 6) tmpLineY1 = upperLimit - 6;
		else tmpLineY1 = tmpData;		
		
		/* Теперь выводим новую линию */
		if(Verify_Clip_Line(&tmpLineX0, &tmpLineY0, &tmpLineX0, &tmpLineY1) != SET)
		{
			DrawClear_iLine(tmpLineX0, tmpLineY0, tmpLineY1, DRAW);
			pINFO->visDATA[cnt_] = tmpLineY0;
			pINFO->OldData_PointsFlag[cnt_] = ENABLE;
		}
		else pINFO->OldData_PointsFlag[cnt_] = DISABLE;
		
		pINFO->visDATA[cnt_+1] = tmpLineY1;
	}          
}


/*******************************************************************************
* Function Name  : MIN_MAX_MODEfunc
* Description    : Очистка и вывод на экран осциллограммы в режиме макс./мин.
* Input          : None
* Return         : None
*******************************************************************************/
void MIN_MAX_MODEfunc(uint16_t X0, uint16_t X1, int16_t Position)
{
	uint16_t cnt_;			// счетчик
	uint16_t point = 0;		// подсчет отрисованных точек
	int16_t tmpData;		// временная

	uint16_t tmpLineX0, tmpLineY0, tmpLineY1;

	/* Перерисовываем осциллограму */
	for (cnt_ = X0; cnt_ < ((X1 - 2)*2); cnt_ += 2)
	{     			   
		tmpLineX0 = point + leftLimit + 1;

		tmpLineY0 = pINFO->visDATA[cnt_];
		tmpLineY1 = pINFO->visDATA[cnt_+1];
		
//		if(gOSC_MODE.State == RUN)
//		{
			if(pINFO->OldData_PointsFlag[point] == ENABLE) DrawClear_MAXMIN_Line(tmpLineX0, tmpLineY0, tmpLineY1, CLEAR);
//		}

		/* проверка на выход за пределы экрана, обновляем значения двух точек для отображения */
		tmpData = (pINFO->DATA[cnt_ + 1] + Position);
		if(tmpData < lowerLimit + 6) pINFO->visDATA[cnt_ + 1] = lowerLimit + 6;
		else if(tmpData > upperLimit - 6) pINFO->visDATA[cnt_ + 1] = upperLimit - 6;
		else pINFO->visDATA[cnt_ + 1] = pINFO->DATA[cnt_ + 1] + Position;

		tmpData = (pINFO->DATA[cnt_] + Position);
		if(tmpData < lowerLimit + 6) pINFO->visDATA[cnt_] = lowerLimit + 6;
		else if(tmpData > upperLimit - 6) pINFO->visDATA[cnt_] = upperLimit - 6;
		else pINFO->visDATA[cnt_] = pINFO->DATA[cnt_] + Position;
	

		if((pINFO->visDATA[cnt_ + 1] - pINFO->visDATA[cnt_]) >= 0)
		{	
			tmpLineY0 = pINFO->visDATA[cnt_];
			tmpLineY1 = pINFO->visDATA[cnt_ + 1];

			if(Verify_Clip_Line(&tmpLineX0, &tmpLineY0, &tmpLineX0, &tmpLineY1) != SET)
			{
				DrawClear_MAXMIN_Line(tmpLineX0, tmpLineY0, tmpLineY1, DRAW);

				pINFO->visDATA[cnt_] = tmpLineY0;
				pINFO->visDATA[cnt_+1] = tmpLineY1;
				pINFO->OldData_PointsFlag[point] = ENABLE;
			}
			else pINFO->OldData_PointsFlag[point] = DISABLE;
		}
		else pINFO->OldData_PointsFlag[point] = DISABLE;
				
		point++;
	}   
}



/*******************************************************************************
* Function Name  : MIN_MAX_MODEfunc
* Description    : Очистка и вывод на экран осциллограммы в режиме макс./мин.
* Input          : None
* Return         : None
*******************************************************************************/
void FFT_MODEfunc(uint16_t X0, uint16_t X1, int16_t Position)
{
	uint16_t i, j = 0;
	volatile int16_t posY = (pINFO == &INFO_A)? centerY + 130 : 150;
	uint8_t mOut[256] = {0};

	fft_realMag_calc(pINFO->DATA, mOut);

	for(i = 0; i < 128; i++)
	{
		pINFO->DATA[(i*2) + 20] = (int8_t)(((mOut[j] ) * 100) / 255) - 127;
		pINFO->DATA[(i*2) + 21] = (int8_t)((((mOut[j] + mOut[j + 1]) / 2) * 100) / 255) - 127;
		j++;
	}

	pINFO->DATA[21] = pINFO->DATA[20] = -127;

	InterpolationLINEAR(20, 20 + 255, posY);
}


/*******************************************************************************
* Function Name  : DrawClear_iLine
* Description    : отрисовка или очистка одной линии при линейной интерполяции
* Input          : X0pos, Y0pos, X1pos, Y1pos, clear
* Return         : None
*******************************************************************************/
static __inline void DrawClear_iLine(uint16_t X0pos, uint16_t Y0pos, uint16_t Y1pos, DrawState NewDrawState)
{ 	
	uint16_t clrColor;    
	uint16_t i, j;
	uint16_t tmpY;
	
	if(Y1pos < Y0pos){ tmpY = Y0pos; Y0pos = Y1pos; Y1pos = tmpY; }   

	if(Y1pos - Y0pos == 0) j = 1;
	else j = Y1pos - Y0pos;

	Set_LCD_CS();
	LCD_SetCursor(X0pos,Y0pos);        // Установка курсора в нужную позицию
	
	if(NewDrawState == CLEAR)
	{
		for (i = 0; i < j; i++)
		{
			/* Проверяем на совпадение координат очистки с сеткой, если совпало то очищаем цветом сетки */
			clrColor = Verify_Grid_Match(X0pos, Y0pos);
					
			LCD_PutPixel(clrColor);           // очистить точку на установленной позиции
			Y0pos++;
		}
	}
	else	// if(NewDrawState == DRAW)
	{
		for (i = 0; i < j; i++)	LCD_PutPixel(pINFO->Color);		// Нарисовать точку на установленной позиции
	}

	Reset_LCD_CS();
}


/*******************************************************************************
* Function Name  : DrawClear_iLine
* Description    : отрисовка или очистка одной линии при линейной интерполяции
* Input          : X0pos, Y0pos, X1pos, Y1pos, clear
* Return         : None
*******************************************************************************/
static __inline void DrawClear_MAXMIN_Line(uint16_t X0pos, uint16_t Y0pos, uint16_t Y1pos, DrawState NewDrawState)
{
	uint16_t tmL = 0, tmpY;
	uint16_t clrColor;			// цвет очистки
	int16_t Length;				// длинна линни для отрисовки/очистки

	if(Y1pos < Y0pos){ tmpY = Y0pos; Y0pos = Y1pos; Y1pos = tmpY; }

	Set_LCD_CS();	//  установить низкий уровень CS LCD
					// используем "низкоуровневый" доступ к ЖК для макс. скорости

	LCD_SetCursor(X0pos, Y0pos);

	Length = Y1pos - Y0pos;
	if((Length <= 0) || (Length > 190)) Length = 1;

	if(NewDrawState == CLEAR)
	{
		while(tmL++ != Length)	// очищаем линию от минимума до максимума
		{
			/* Проверяем на совпадение координат очистки с сеткой, если совпало то очищаем цветом сетки */
			clrColor = Verify_Grid_Match(X0pos, Y0pos + tmL - 1);
			LCD_PutPixel(clrColor);
		}
	}
	else	// if(NewDrawState == DRAW)
	{
		while(tmL++ != Length) LCD_PutPixel(pINFO->Color);		// Нарисовать точку на установленной позиции
	}

	Reset_LCD_CS();
}




/* ФУНКЦИИ ОЧИСТКИ ПРИ СМЕНЕ РЕЖИМА **************************************************************************/

/*******************************************************************************
* Function Name  : Clear_OLD_DataCH_ON_SCREEN
* Description    : общая функция очистки, используется при изменении интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
void Clear_OLD_DataCH_ON_SCREEN(uint8_t channel, uint16_t X0, uint16_t X1)
{
	CH_INFO_TypeDef *tmpCH = pINFO;

	Set_CH_TypeINFO(channel);
	ActiveMode->Clear_OLD_DataFunc(X0, X1);

	if(pINFO != tmpCH) pINFO = tmpCH;
}


/*******************************************************************************
* Function Name  : Clear_OLD_Data_NONE
* Description    : очистка при отсутсвии интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
static void Clear_OLD_Data_NONE(uint16_t X0, uint16_t X1)
{
	uint16_t cnt_, clrColor;
	
	/* Очищаем старую осциллограмму */
	for (cnt_ = X0; cnt_ < X1; cnt_++)
	{
	   if(Verify_Clip_Point(cnt_, pINFO->visDATA[cnt_ - leftLimit - 1]) != SET)
	   {
		   /* Проверяем на совпадение координат очистки с сеткой, если совпало то очищаем цветом сетки */
		   clrColor = Verify_Grid_Match((cnt_ - leftLimit - 1), pINFO->visDATA[cnt_ - leftLimit - 1]);
		   			  
		   /* рисуем точку */
		   DrawPixel(cnt_, pINFO->visDATA[cnt_ - leftLimit - 1], clrColor);
	   }
	   pINFO->OldData_PointsFlag[cnt_ - leftLimit - 1] = DISABLE;
	}
}


/*******************************************************************************
* Function Name  : Clear_OLD_Data_Lines
* Description    : очистка осциллограммы при линейной интерполяции
* Input          : None
* Return         : None
*******************************************************************************/
static void Clear_OLD_Data_Lines(uint16_t X0, uint16_t X1)
{
	uint16_t cnt_;
	
	/* очищаем линии */
	for (cnt_ = X0; cnt_ < X1; cnt_++)
	{   	  
		if(pINFO->OldData_PointsFlag[cnt_ - leftLimit - 1] == ENABLE)
		{
			DrawClear_iLine(cnt_, pINFO->visDATA[cnt_ - leftLimit - 1], pINFO->visDATA[cnt_+1 - leftLimit - 1], CLEAR);
		}
		pINFO->OldData_PointsFlag[cnt_ - leftLimit - 1] = DISABLE;       	  
	}  
}


/*******************************************************************************
* Function Name  : Clear_OLD_Data_MIN_MAX
* Description    : очистка, режим максимумов/минимумов
* Input          : None
* Return         : None
*******************************************************************************/
static void Clear_OLD_Data_MIN_MAX(uint16_t X0, uint16_t X1)
{
	uint16_t cnt_, point = X0 - leftLimit, tmL, Length, clrColor;
	uint16_t num = (X1 - X0) * 2;
	
	/* Очищаем старую осциллограмму */

	Set_LCD_CS();

	for (cnt_ = (point * 2); cnt_ < num; cnt_ += 2)
	{			    
		if(pINFO->OldData_PointsFlag[point] == ENABLE)
		{
			LCD_SetCursor(point + leftLimit + 1, pINFO->visDATA[cnt_]);
			
			Length = fabs(pINFO->visDATA[cnt_+1] - pINFO->visDATA[cnt_]);
			if((Length < 1) || (Length > 190)) Length = 1;
			tmL = 0;
			
			/* очищаем линию от минимума до максимума */
			while(tmL++ != Length)
			{				
				/* Проверяем на совпадение координат очистки с сеткой, если совпало то очищаем цветом сетки */
				clrColor = Verify_Grid_Match(point + leftLimit + 1, pINFO->visDATA[cnt_] + tmL - 1);
					 
				LCD_PutPixel(clrColor);				
			}
		}
		pINFO->OldData_PointsFlag[point] = DISABLE;		  
		point++;		  		 
	}
	Reset_LCD_CS();
}


/*******************************************************************************
* Function Name  : Clear_OLD_Data_FFT
* Description    : очистка, режим FFT
* Input          : None
* Return         : None
*******************************************************************************/
static void Clear_OLD_Data_FFT(uint16_t X0, uint16_t X1)
{
	Clear_OLD_Data_Lines(leftLimit + 20, leftLimit + 276);
}





