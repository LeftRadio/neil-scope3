/*************************************************************************************
*
Description :  NeilScope3 User Interface
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio
Comments    :
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>                           /* Include standard types */
#include "All_Menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ---------------------------------------------------------*/
const Menu_Struct_TypeDef *AllMenu[8] = {
	&ChannelA_Menu, &ChannelB_Menu, &DigitTrigMenu,
	&gInterfaceMenu, &MeasMenu, &QuickMenu,
	&TimeScaleMenu,	&TrigMenu
};

/* Private function prototypes -----------------------------------------------*/
/* Private Functions --------------------------------------------------------*/

