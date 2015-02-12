/*************************************************************************************
*
Description :  NeilScope3 Settings header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#ifndef __SETTINGS_H
#define __SETTINGS_H 

/* Includes ------------------------------------------------------------------*/	   
#include "main.h"
#include "stm32f10x.h"

/* Exported typedef -----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define btn_POWER	  0

/* Exported variables --------------------------------------------------------*/
extern const uint32_t sweep_coff[];

extern uint16_t globalBackColor;		// Цвет фона
extern uint16_t Active_BackColor;		// Цвет фона активной области
extern uint16_t Active_BorderColor;		// Цвет бордюра активной области

/* настройки отрисовки активной области, сетки и т.д. -----------------------------*/
extern uint16_t centerX, centerY; //координаты цетра активной области
extern uint16_t leftLimit, rightLimit, upperLimit, lowerLimit;	// границы вывода
extern uint16_t radius_Active_Area; // радиус скругления активной области

///* Коэффициент количества точек для записи/чтения,
//   нужен для разных режимов, в режиме макс./мин.,
//   количество точек должно быть в два раза больше */
//extern uint8_t oscNumPoints_Ratio;

//extern uint8_t InterpolationMODE;		// режим интерполяции
extern uint8_t err_I2C_flag;

extern FunctionalState BeepState;


/* Exported function --------------------------------------------------------*/
void LoadPreference(void);							// чтение сохранненых настроек
void SavePreference(void);							// сохраннение настроек

#endif /* __SETTINGS_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

