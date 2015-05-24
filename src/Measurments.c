/**
  ******************************************************************************
  * @file	 	Measurments.c
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Measurments sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include <math.h>                           /* */
#include "main.h"
#include "User_Interface.h"
#include "Measurments.h"
#include "Processing_and_output.h"
#include "Sweep.h"
#include "Settings.h"
#include "Analog.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	float Val;
	const uint16_t Y_pos;
	char Text[10];
} MeasurmentParam_Typedef;

/* Private define ------------------------------------------------------------*/
#define PointToPoint_Time_nS		10
#define average						50

/* Private macro -------------------------------------------------------------*/
#define  measCursor_1		(measCursor1 - leftLimit - 1)
#define  measCursor_2		(measCursor2 - leftLimit - 1)
#define _MAX(x,y)  			((x) > (y) ? (x) : (y))
#define _MIN(x,y)  			((x) < (y) ? (x) : (y))


/* Private variables ---------------------------------------------------------*/
const MeasMode_TypeDef MeasurmentsMode[3] = {
		{ All_Measurments_Menu_1, 0 },	{ All_Measurments_Menu_2, 1 },	{ All_Measurments_Menu_3, 2 }
};
MeasMode_TypeDef *mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[0];

MeasurmentParam_Typedef measurments_A[5] = {
		{0, mFRTH_YPOS + CHA_SW_Ypos, {0}}, {0, mTHRD_YPOS + CHA_SW_Ypos, {0}}, {0, mSECND_YPOS + CHA_SW_Ypos, {0}},
		{0, mFRST_YPOS + CHA_SW_Ypos, {0}}, {0, mBASE_YPOS + CHA_SW_Ypos, {0}}
};

MeasurmentParam_Typedef measurments_B[5] = {
		{0, mFRTH_YPOS, {0}}, {0, mTHRD_YPOS, {0}}, {0, mSECND_YPOS, {0}},
		{0, mFRST_YPOS, {0}}, {0, mBASE_YPOS, {0}}
};

MeasurmentParam_Typedef OLD_measurments_A[5];
MeasurmentParam_Typedef OLD_measurments_B[5];

static uint8_t averageCnt_A = 0, averageCnt_B = 0;

float OldFreq_A = -1, OldFreq_B = -1;
Boolean gShowFFTFreq = FALSE, gShowFFTFreq_DrawState = FALSE;

uint16_t measCursor1 = 50, measCursor2 = 250;
int8_t gMAX = 0, gMIN = 255;


/* Private function prototypes -----------------------------------------------*/
static __inline void Vp_p_Meas(float *MeasParam);
static __inline void deltaT_Meas(float *MeasParam);
static __inline void Vdc_Meas(float *MeasParam);
static __inline void Vrms_Meas(float *MeasParam);
static __inline void dutyCycle_Meas(float *MeasParam);

static __inline void Vmax_Meas(float *MeasParam);
static __inline void Vmin_Meas(float *MeasParam);
static __inline void FFT_Meas(float *MeasParam);
static __inline void F_Meas_Calc(float *Var, uint8_t harm_pt);
static __inline void Vcrs1_Meas(float *MeasParam);
static __inline void Vcrs2_Meas(float *MeasParam);

static void Compute_and_Print_All_Values_1(MeasurmentParam_Typedef *Measurments);
static void Compute_and_Print_All_Values_2(MeasurmentParam_Typedef *Measurments);
static void Compute_and_Print_All_Values_3(MeasurmentParam_Typedef *Measurments);
static void Print_Meas_Text(uint16_t Y_Pos, char *text, uint16_t Color);

void ConvertFloatToString(char *ValText, float value, uint8_t Length);

/* Exported function prototypes ----------------------------------------------*/
void (*MeasurmentsFunc)(void) = &All_Measurments_Menu_1;



/* Private function ----------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Reset_Calc_Show_Freq
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Reset_Calc_Show_Freq(void)
{
	OldFreq_A = -1;
	OldFreq_B = -1;
}


/*******************************************************************************
* Function Name  : Calc_Show_Freq
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Calc_Show_Freq(void)
{
	float Freq_A = 0, Freq_B = 0;
	static char str_A[10] = {0}, str_B[10] = {0};
	char *str;
	const uint8_t Xsw = 55;
	uint8_t Ysw;

	if(ActiveMode != &IntMIN_MAX)
	{
		LCD_SetTextColor(StillBlue);

		if(pINFO == &INFO_A)
		{
			FFT_Meas(&Freq_A);
			if(OldFreq_A == Freq_A) return;
			else
			{
				Ysw = 20;
				str = (char*)str_A;
				LCD_PutStrig(rightLimit - Xsw, upperLimit - Ysw, 1, str);
				cnvrtToFrequency(Freq_A, str_A);
				OldFreq_A = Freq_A;
			}
		}
		else //if(pINFO == &INFO_B)
		{
			FFT_Meas(&Freq_B);
			if(OldFreq_B == Freq_B) return;
			else
			{
				Ysw = 32;
				str = (char*)str_B;
				LCD_PutStrig(rightLimit - Xsw, upperLimit - Ysw, 1, str);
				cnvrtToFrequency(Freq_B, str_B);
				OldFreq_B = Freq_B;
			}
		}

		LCD_SetTextColor(pINFO->Color);
		LCD_PutStrig(rightLimit - Xsw, upperLimit - Ysw, 1, str);
	}
}


/*******************************************************************************
* Function Name  : All_Measurments_Menu_1
* Description    : Вычисление всех автоизмерений первой группы
* Input          : None
* Return         : None
*******************************************************************************/
void All_Measurments_Menu_1(void)
{
	uint8_t *pCounter;
	MeasurmentParam_Typedef *pMeas;

	if(pINFO == &INFO_A){ pCounter = &averageCnt_A; pMeas = &measurments_A[0]; }
	else{ pCounter = &averageCnt_B; pMeas = &measurments_B[0]; }

	Vp_p_Meas(&pMeas->Val);
	deltaT_Meas(&(pMeas + 1)->Val);
	Vdc_Meas(&(pMeas + 2)->Val);
	Vrms_Meas(&(pMeas + 3)->Val);
	dutyCycle_Meas(&(pMeas + 4)->Val);

	if((*pCounter)++ >= average)
	{
		Compute_and_Print_All_Values_1(pMeas);
		(*pCounter) = 0;
	}
}


/*******************************************************************************
* Function Name  : All_Measurments_Menu_2
* Description    : Вычисление всех автоизмерений второй группы
* Input          : None
* Return         : None
*******************************************************************************/
void All_Measurments_Menu_2(void)
{   
	uint8_t *pCounter;
	MeasurmentParam_Typedef *pMeas;

	if(pINFO == &INFO_A){ pCounter = &averageCnt_A; pMeas = &measurments_A[0]; }
	else{ pCounter = &averageCnt_B; pMeas = &measurments_B[0]; }

	Vcrs1_Meas(&pMeas->Val);
	Vcrs2_Meas(&(pMeas + 1)->Val);
	Vmax_Meas(&(pMeas + 2)->Val);
	Vmin_Meas(&(pMeas + 3)->Val);
	if(ActiveMode != &IntMIN_MAX) FFT_Meas(&(pMeas + 4)->Val);

	if((*pCounter)++ >= average)
	{
		Compute_and_Print_All_Values_2(pMeas);
		(*pCounter) = 0;
	}
}


/*******************************************************************************
* Function Name  : All_Measurments_Menu_3
* Description    : Вычисление всех автоизмерений третьей группы
* Input          : None
* Return         : None
*******************************************************************************/
void All_Measurments_Menu_3(void)
{
	uint8_t *pCounter;
	MeasurmentParam_Typedef *pMeas;

	if(pINFO == &INFO_A){ pCounter = &averageCnt_A; pMeas = &measurments_A[0]; }
	else{ pCounter = &averageCnt_B; pMeas = &measurments_B[0]; }

	FFT_Meas(&(pMeas + 4)->Val);

	if(++(*pCounter) >= average)
	{
		F_Meas_Calc(&pMeas->Val, (measCursor_1 - 20)/2);
		F_Meas_Calc(&(pMeas + 1)->Val, (measCursor_2 - 20)/2);

		Compute_and_Print_All_Values_3(pMeas);
		(*pCounter) = 0;
	}
}


/*******************************************************************************
* Function Name  : Init_Meas_Values
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Init_Meas_Values(void)
{
	uint8_t i;

	for(i = 0; i < MEAS_PARAM_MAX; i++)
	{
		measurments_A[i].Val = 0;
		measurments_B[i].Val = 0;
		OLD_measurments_A[i].Val = 1;
		OLD_measurments_B[i].Val = 1;
	}
}



/* ФУНКЦИИ ВЫЧЕСЛЕНИЙ ************************************************************************************/

/* Вычисление напряжения по координате курсора 1 */
static __inline void Vcrs1_Meas(float *MeasParam)
{
	*MeasParam += pINFO->DATA[measCursor_1];
}

/* Вычисление напряжения по координате курсора 2 */
static __inline void Vcrs2_Meas(float *MeasParam)
{
	*MeasParam += pINFO->DATA[measCursor_2];
}

/* Вычисление максимального напряжения */
static __inline void Vmax_Meas(float *MeasParam)
{
	uint16_t cnt_;

	gMAX = -127;
	for(cnt_ = measCursor_1; cnt_ < measCursor_2; cnt_++) gMAX = _MAX(pINFO->DATA[cnt_], gMAX);

	*MeasParam += gMAX;
}

/* Вычисление минимального напряжения */
static __inline void Vmin_Meas(float *MeasParam)
{
	uint16_t cnt_;

	gMIN = 127;
	for(cnt_ = measCursor_1; cnt_ < measCursor_2; cnt_++) gMIN = _MIN(pINFO->DATA[cnt_], gMIN);

	*MeasParam += gMIN;
}

/* Вычисление частоты */
static __inline void FFT_Meas(float *MeasParam)
{
	uint8_t i, harm_Mag = 0, harm_num = 0;
	float tM = 0;
	uint8_t mOut[256] = {0};
	uint8_t *pData = mOut;
	uint8_t div = 0;

	if((SweepScale!= &IntrlSweepScaleCoff[1]) && (*SweepScale <= 1))
	{
		if(ActiveMode != &FFT_MODE) fft_realMag_calc(pINFO->DATA, mOut);
		else { pData = (uint8_t*)(&pINFO->DATA[20]); div = 1; }

		for(i = 1; i < 128; i++)
		{
			if(harm_Mag < (uint8_t)(*(pData + i) + 127))
			{
				harm_Mag = (uint8_t)(*(pData + i) + 127);
				harm_num = i;
			}
		}

		F_Meas_Calc(&tM, harm_num >> div);
		(*MeasParam) += tM;
	}
	else (*MeasParam) = 0;
}

/* Получение частоты от номера точки в выходном буфере; [63] = Fs/4; [127] = Fs/2 */
static __inline void F_Meas_Calc(float *Var, uint8_t harm_pt)
{
	*Var = harm_pt * ((50000000.0 / ((float)gSamplesWin.Sweep + 1)) / 128.0);
}

/* Вычисление напряжения пик-пик */
static __inline void Vp_p_Meas(float *MeasParam)
{
   uint16_t cnt_;
   int8_t max = -127, min = 127;
   
   for(cnt_ = measCursor_1; cnt_ < measCursor_2; cnt_++)
   {
       max = _MAX(pINFO->DATA[cnt_], max);
       min = _MIN(pINFO->DATA[cnt_], min);      
   }

   gMAX = max;
   gMIN = min;

   *MeasParam += fabs(max - min);
}

/* Вычисление времени между курсорами */
static __inline void deltaT_Meas(float *MeasParam)
{   
	*MeasParam = measCursor_2 - measCursor_1;
}

/* Вычисление постоянной составляющей */
static __inline void Vdc_Meas(float *MeasParam)
{
	*MeasParam += (gMAX + gMIN)/2;
}

/* Вычисление среднее квадратического значения напряжения */
static __inline void Vrms_Meas(float *MeasParam)
{   
   uint16_t cnt_;
   uint32_t squareSum = 0;
   
   for(cnt_ = measCursor_1; cnt_ < measCursor_2; cnt_++)
   {
       squareSum += (pINFO->DATA[cnt_]) * (pINFO->DATA[cnt_]);
   }
           
   *MeasParam += sqrtf(squareSum / (measCursor_2 - measCursor_1));
}

/* Вычисление скважности */
static __inline void dutyCycle_Meas(float *MeasParam)
{   
   uint16_t cnt_, posNumPoints = 0, negNumPoints = 0;
      
   for(cnt_ = measCursor_1; cnt_ < measCursor_2; cnt_++)
   {
       if(pINFO->DATA[cnt_] > (gMAX + gMIN)/2 )posNumPoints++;
       else if(pINFO->DATA[cnt_] < (gMAX + gMIN)/2 )negNumPoints++;      
   }
   
   if(posNumPoints == 0) *MeasParam = 0;
   else if(negNumPoints == 0) *MeasParam = 100;
   else *MeasParam += ((float)posNumPoints/(float)negNumPoints) * 50.0;
}



/*******************************************************************************
* Function Name  : Print_All_Values_1
* Description    : Вывод всех значений автоизмерений подгруппы 1
* Input          : None
* Return         : None
* Output		 : None
*******************************************************************************/
static void Compute_and_Print_All_Values_1(MeasurmentParam_Typedef *Measurments)
{
	uint8_t i;
	MeasurmentParam_Typedef *OLD_measurments = (Measurments == measurments_A)? OLD_measurments_A : OLD_measurments_B;

	/* очищаем старые значения и выводим новые*/
	for(i = 0; i < MEAS_PARAM_MAX; i++)
	{
		if(Measurments->Val != OLD_measurments->Val)
		{
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, Active_BackColor);		// очищаем
			if(i == 1) cnvrtToTime(Measurments->Val, Measurments->Text);		//measurments.deltaT = 0;
			else if(i == 4) sprintf(Measurments->Text, "%d", (uint8_t)(Measurments->Val / average));
			else cnvrtToVolts(Measurments->Val / average, Measurments->Text);
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, pINFO->Color);	// выводим новую строку

			OLD_measurments->Val = Measurments->Val;
		}
		Measurments->Val = 0;
		Measurments++;
		OLD_measurments++;
	}
}


/*******************************************************************************
* Function Name  : Print_All_Values_2
* Description    : Вывод всех значений автоизмерений подгруппы 2
* Input          : None
* Return         : None
* Output		 : None
*******************************************************************************/
static void Compute_and_Print_All_Values_2(MeasurmentParam_Typedef *Measurments)
{
	uint8_t i;
	MeasurmentParam_Typedef *OLD_measurments = (Measurments == measurments_A)? OLD_measurments_A : OLD_measurments_B;

	/* очищаем старые значения и выводим новые*/
	for(i = 0; i < MEAS_PARAM_MAX; i++)
	{
		if((Measurments->Val != OLD_measurments->Val) || (i == 4))
		{
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, Active_BackColor);		// очищаем
			if(i == 4) cnvrtToFrequency(Measurments->Val / average, Measurments->Text);
			else cnvrtToVolts(Measurments->Val / average, Measurments->Text);
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, pINFO->Color);	// выводим новую строку

			OLD_measurments->Val = Measurments->Val;
		}
		Measurments->Val = 0;
		Measurments++;
		OLD_measurments++;
	}
}


/*******************************************************************************
* Function Name  : Print_All_Values_3
* Description    : Вывод всех значений автоизмерений подгруппы 3
* Input          : None
* Return         : None
* Output		 : None
*******************************************************************************/
static void Compute_and_Print_All_Values_3(MeasurmentParam_Typedef *Measurments)
{
	uint8_t i;
	MeasurmentParam_Typedef *OLD_measurments = (Measurments == measurments_A)? OLD_measurments_A : OLD_measurments_B;

	/* очищаем старые значения и выводим новые*/
	for(i = 0; i < 2; i++)
	{
		if((Measurments->Val != OLD_measurments->Val))
		{
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, Active_BackColor);	// очищаем
			cnvrtToFrequency(Measurments->Val, Measurments->Text);
			Print_Meas_Text(Measurments->Y_pos, Measurments->Text, pINFO->Color);		// выводим новую строку
			OLD_measurments->Val = Measurments->Val;
		}
		Measurments->Val = 0;
		Measurments++;
		OLD_measurments++;
	}

	// Максимальная по амплитуде гармоика по FFT
	Measurments += 2;
	OLD_measurments += 2;
	Measurments->Val /= average;
	Print_Meas_Text(Measurments->Y_pos, Measurments->Text, Active_BackColor);	// очищаем
	cnvrtToFrequency(Measurments->Val, Measurments->Text);
	Print_Meas_Text(Measurments->Y_pos, Measurments->Text, pINFO->Color);		// выводим новую строку
	OLD_measurments->Val = Measurments->Val;
	Measurments->Val = 0;
}


/*******************************************************************************
* Function Name  : Clear_Old_Meas_Text
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
static void Print_Meas_Text(uint16_t Y_Pos, char *text, uint16_t Color)
{
	LCD_SetArea(LeftLineMeas + 40, 75, 398, upperLimit - 6);
	LCD_SetTextColor(Color);	// установить цвет текста
	LCD_PutStrig(LeftLineMeas + 40, Y_Pos, 0, text);
}



/* CONVERT VALUES ***********************************************************************************************/

/*******************************************************************************
* Function Name  : cnvrtToVolts
* Description    : Перевод значений в Вольты
* Input          : float Value - value for convert
* Return         :
* Output		 : *result
*******************************************************************************/
float cnvrtToVolts(float Value, char *resultString)
{
	char* dim_text;
	float val;

	val = Get_AnalogDivider_Gain_Coeff(pINFO->AD_Type.Analog.Div);
	val = Value / val;
	val = val*4;

	if((val < -9999) || (val > 9999)){ val = val / 1000; dim_text = "V "; }
	else if((val < -999) || (val > 999)){ val = val / 1000;	dim_text = "V "; }
	else if((val < -99) || (val > 99)) dim_text = "mV ";
	else dim_text = "mV ";

	/* если требуется возврат текстового значения параметра то */
	if(resultString != (void*)0)
	{
		memset(resultString, 0, 10);
		ConvertFloatToString(resultString, val, 3);
		strcat(resultString, dim_text);
	}
	return val;
}


/*******************************************************************************
* Function Name  : cnvrtToTime
* Description    : Перевод значения растояния между курсорами в время
* Input          : float Value - value for convert
* Return         :
* Output		 : *result
*******************************************************************************/
float cnvrtToTime(float Value, char *resultString)
{ 
	char* dim_text;
	uint8_t InterliveCoeff = (gOSC_MODE.Interleave == TRUE)? 2 : 1;
	float val = (Value * (gSamplesWin.Sweep + 1) * (PointToPoint_Time_nS / ((*SweepScale) * InterliveCoeff)));

	if((val < -999999) || (val > 999999)){ val = val / 1000000;	dim_text = "mS "; }
	else if((val < -9999) || (val > 9999)){	val = val / 1000; dim_text = "uS "; }
	else if((val < -999) || (val > 999)){ val = val / 1000;	dim_text = "uS "; }
	else dim_text = "nS ";

	/* если требуется возврат текстового значения параметра то */
	if(resultString != (void*)0)
	{
		memset(resultString, 0, 10);
		ConvertFloatToString(resultString, val, 4);
		strcat(resultString, dim_text);
	}
	return val;
}


/*******************************************************************************
* Function Name  : cnvrtToTime
* Description    : Перевод значения растояния между курсорами в эквивалентную частоту
* Input          : float Value - value for convert
* Return         :
* Output		 : *result
*******************************************************************************/
float cnvrtToFrequency(double Value, char *resultString)
{
	char* dim_text;
	double val = Value;

	if(val < 1000){	dim_text = " H"; }
	else if(val < 1000000){	val = val / 1000; dim_text = "kH"; }
	else{ val = val / 1000000; dim_text = "MH"; }

	/* если требуется возврат текстового значения параметра то */
	if(resultString != (void*)0)
	{
		memset(resultString, 0, 10);
		ConvertFloatToString(resultString, val, 4);
		strcat(resultString, dim_text);
	}
	return val;
}


/*******************************************************************************
* Function Name  :
* Description    :
* Input          : value - value for convert
* Return         : None
* Output		 : *string
*******************************************************************************/
void ConvertFloatToString(char *ValText, float Value, uint8_t Length)
{
	int d1, d2;     // integer & fractional parts
	float f2;     	// fractional part
	uint8_t i = Length;

	/* --- Convert to string, fixed 5 digit with float point position --- */
	if(Value < 0) Value *= -1;

	if(Value > 1e4){ Value = 99999; i = 0; }
	else if(Value < 1) i = Length - 1;
	else
	{
		f2 = Value;
		while(f2 >= 1)
		{
			f2 /= 10;
			i--;
		}
	}

	d1 = Value;		// Get the integer part

	if(i != 0)
	{
		f2 = Value - d1;     					// Get fractional part
		d2 = trunc(f2 * pow10f(i));		// Turn into integer

		ConvertToString(d1, ValText, Length - i);
		ValText[strlen(ValText)] = '.';
		ConvertToString(d2, &ValText[strlen(ValText)], i);
	}
	else ConvertToString((uint32_t)Value, ValText, Length);

	ValText[strlen(ValText)] = 0;
}

