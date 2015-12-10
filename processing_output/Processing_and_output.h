/**
  ******************************************************************************
  * @file	 	Processing_and_output.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 Processing and output header
  ******************************************************************************
**/

#ifndef __PROCESSING_AND_OUT_H
#define __PROCESSING_AND_OUT_H 

/* Includes ------------------------------------------------------------------*/	   
/* Exported typedef -----------------------------------------------------------*/
typedef struct
{
	void (*InterpolationMODEfunc)(uint16_t X0, uint16_t X1, int16_t Position);
	void (*Clear_OLD_DataFunc)(uint16_t X0, uint16_t X1);
	uint8_t oscNumPoints_Ratio;
	int8_t MinimumAllowed_SweepIndex;
	const char *Text;
	const uint8_t Indx;
} InterpolationMode_TypeDef;


/* Exported define -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint16_t X0_Restrict, X1_Restrict, Y_Restrict;
extern uint8_t Shift;

extern InterpolationMode_TypeDef *ActiveMode;
extern InterpolationMode_TypeDef IntNONE, IntLIN, IntMIN_MAX, FFT_MODE;
extern const InterpolationMode_TypeDef *InterpModes[4];
extern volatile uint8_t Interpolate_AciveMode_Index;

/* Exported function --------------------------------------------------------*/
void Update_Oscillogram(void);
void changeInterpolation(InterpolationMode_TypeDef *NewMode);
void InterpolationNONE(uint16_t X0, uint16_t X1, int16_t Position);
void InterpolationLINEAR(uint16_t X0, uint16_t X1, int16_t Position);
void MIN_MAX_MODEfunc(uint16_t X0, uint16_t X1, int16_t Position);
void FFT_MODEfunc(uint16_t X0, uint16_t X1, int16_t Position);

void Clear_OLD_DataCH_ON_SCREEN(uint8_t channel, uint16_t X0, uint16_t X1);
void Clear_OLD_Line(uint16_t X0pos, uint16_t Y0pos, uint16_t X1pos, uint16_t Y1pos, uint8_t clear);
void Set_Oscillogram_Restrictions(uint16_t X_Left, uint16_t X_Right, uint16_t Y_Up);
void Set_CH_TypeINFO(Channel_ID_TypeDef NewChannelType);

void Inerlive_Cmd(FunctionalState NewState);


#endif /* __PROCESSING_AND_OUT_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

