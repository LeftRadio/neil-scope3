/*************************************************************************************
*
Description :  NeilScope3 Quick Menu header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#ifndef __CHENNELS_MENU_H
#define __CHENNELS_MENU_H 

/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function --------------------------------------------------------*/
void Draw_Channels_Menu(void);
void Change_Channel_Menu_indx(void);

void Correct_AnalogZero_CH_A(void);
void Correct_AnalogZero_CH_B(void);

void OFFSET_CH_A(void);
void OFFSET_CH_B(void);

void AutoDivider_A(void);
void AutoDivider_B(void);

void Change_COLOR_CH_A(void);
void Change_COLOR_CH_B(void);

void Change_TextColor_CH_A(void);
void Change_TextColor_CH_B(void);

void Intrlive_Correct(void);

#endif /* __CHENNELS_MENU_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
