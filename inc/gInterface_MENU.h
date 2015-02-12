/**
  ******************************************************************************
  * @file	 	gInterface_MENU.h
  * @author  	Left Radio
  * @version 	1.5.6
  * @date
  * @brief		NeilScope3 gInterface Menu header
  ******************************************************************************
**/

#ifndef __G_INTERFACE_H
#define __G_INTERFACE_H 

/* Includes ------------------------------------------------------------------*/	   
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const char sweep_text[][10];
extern const char sweep_Scale_text[][10];
extern const char div_text[6][10];

//

extern uint8_t enChangeSyncMODE;  // флаг изменения режима синхронизации

/* Private function --------------------------------------------------------*/
/* Exported function --------------------------------------------------------*/
void qMenu(void);
void change_Div_A(void);
void change_Div_B(void);
void change_Sweep(void);
void change_Sweep_Mode(void);
void Sweep_Mode(Boolean Init);
void change_Trigg_Mode(void);
void Hide_Show_Meas(void);
void RUN_HOLD(void);
void ON_OFF_CHANNEL_A(void);
void ON_OFF_CHANNEL_B(void);
void change_TIME_SCALE(void);
void setCondition(uint8_t RUN_HOLD);    /* приостановка/запуск каналов */


#endif /* __G_INTERFACE_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
