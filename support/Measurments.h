/**
  ******************************************************************************
  * @file	 	Measurments.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Measurments header
  ******************************************************************************
**/

#ifndef __MEASURMENTS_H
#define __MEASURMENTS_H

/* Includes ------------------------------------------------------------------*/
#include "fft.h"

/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
	void (*MeasurmentsFunc)(void);
	const uint8_t indx;
} MeasMode_TypeDef;

/* Exported define -----------------------------------------------------------*/
#define CHA_SW_Ypos			75

#define LeftLineMeas		305
#define YPOS_DIST			14

#define mFRTH_YPOS			mTHRD_YPOS + YPOS_DIST
#define mTHRD_YPOS			mSECND_YPOS + YPOS_DIST
#define mSECND_YPOS			mFRST_YPOS + YPOS_DIST
#define mFRST_YPOS			mBASE_YPOS + YPOS_DIST
#define mBASE_YPOS			75

#define MEAS_PARAM_MAX		5


/* Exported variables --------------------------------------------------------*/
extern uint16_t measCursor1, measCursor2, Ypos;
extern uint8_t fft_Mag[1 << NS_P];

extern const MeasMode_TypeDef MeasurmentsMode[3];
extern MeasMode_TypeDef *mModeActive;

extern Boolean gShowFFTFreq, gShowFFTFreq_Init;

/* Private function prototypes -----------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
void All_Measurments_Menu_1(void);
void All_Measurments_Menu_2(void);
void All_Measurments_Menu_3(void);

void Init_Meas_Values(void);

void F_Meas(float *MeasParam, uint8_t Freq);

void Reset_Calc_Show_Freq(void);
void Calc_Show_Freq(void);

float cnvrtToVolts(float Value, char *result);
float cnvrtToTime(float Value, char *result);
float cnvrtToFrequency(double Value, char *result);



#endif /* __MEASURMENTS_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
