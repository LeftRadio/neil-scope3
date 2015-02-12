/*************************************************************************************
*
Description :  NeilScope3 Trig Menu header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

#ifndef __TRIG_MENU_H
#define __TRIG_MENU_H 


/* Includes ------------------------------------------------------------------*/
#include "HX8352_Library.h"

/* Exported typedef ----------------------------------------------------------*/
typedef struct 	  /* тип данных курсоров триггера */
{
	int32_t Position;
	int16_t WindowPosition;
	uint16_t Color;
	char* Name;
	Boolean Visible;
	Boolean Drawed;
} TrigCursorINFO;


typedef struct   /* тип данных параметров триггера для вывода на экран */
{
	char *triggType_Name[4];
	FunctionalState Status;
} TriggShowInfo_TypeDef;


/* Exported define -----------------------------------------------------------*/
#define trgHeigth  0
#define trgLow     1

/* Exported variables --------------------------------------------------------*/
extern TrigCursorINFO *pntTrigCursor;
extern TrigCursorINFO *TempTrigCursor;
extern TrigCursorINFO Height_Y_cursor, Low_Y_cursor, trigPosX_cursor;	 // курсоры триггера
extern TriggShowInfo_TypeDef  TriggShowInfo;

extern const char Trigg_Sourse_Sync_Text[3][10];

/* Private function prototypes -----------------------------------------------*/
void Change_SyncSourse(void);
void Sync_Sourse_Texts(void);
void Change_Trigg_ModeSync(void);
void Change_H_CursorLevel(void);
void Change_L_CursorLevel(void);
void Sync_ChangeLevel(TrigCursorINFO *Cursor, int16_t Diff);
void Hide_Show_triggInfo(void);
void Draw_Trigg_Info(FunctionalState NewState);

/* Exported function ---------------------------------------------------------*/
extern void SetActiveTrigCursor(TrigCursorINFO *pTrigcursor);	// Функция установки "активного" курсора для триггера
extern void Draw_Trig_Menu_Buttons(uint8_t Draw);				// Функция включения/выключения меню триггера
extern void Change_Trig_Menu_indx(void);						// Функция смены индекса меню триггера
extern void Change_Trig_X_Y_onMainMenu(void);					// Функция изменения уровней триггера из основного меню
extern void Trigg_Position_X(void);
extern void Update_triggInfo_OnScreen(DrawState NewDrawState);


#endif /* __TRIG_MENU_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/




