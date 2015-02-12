/**
  ******************************************************************************
  * @file    Analog.h
  * @author  LeftRadio
  * @version V1.0.0
  * @date
  * @brief   NeilScope3 Analog control header
  ******************************************************************************
**/

#ifndef __ANALOG_H
#define __ANALOG_H

/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define Divider_Position_MAX           11
#define Divider_Position_MIN           0

/* Exported variables --------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
void Control_AD8129(uint8_t STATE);
char *Change_AnalogDivider(uint8_t channel, uint8_t divider_index);
uint8_t Get_AnalogDivider_Index(Channel_ID_TypeDef channel);
float Get_AnalogDivider_Gain_Coeff(uint8_t divider_index);

FunctionalState Get_AutoDivider_State(Channel_ID_TypeDef CH);
void Set_AutoDivider_State(Channel_ID_TypeDef CH, FunctionalState NewState);
FunctionalState Analog_AutodividerMain(void);

#endif /* __ANALOG_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

