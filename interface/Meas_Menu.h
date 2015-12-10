/*************************************************************************************
*
Description :  NeilScope3 Measurment menu header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#ifndef __MEAS_MENU_H
#define __MEAS_MENU_H


/* Includes ------------------------------------------------------------------*/
//#include "HX8352_Library.h"

/* Exported typedef ----------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#define Menu_1  2
#define Menu_2  3

/* Exported variables --------------------------------------------------------*/
extern uint8_t tmpAutoMeas, numMeasMenu;

/* Private function prototypes -----------------------------------------------*/

/* Exported function ---------------------------------------------------------*/
extern void ChangeMeasCursor_1(void);
extern void ChangeMeasCursor_2(void);
extern void ChangeMeasMode(void);

#endif /* __MEAS_MENU_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/











