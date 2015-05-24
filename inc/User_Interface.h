/*************************************************************************************
*
Description :  NeilScope3 User Interface header
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#ifndef __USER_INTERFACE_H
#define __USER_INTERFACE_H 

/* Includes ------------------------------------------------------------------*/	   
#include "Trig_Menu.h"
#include "TimeScale_Menu.h"
#include "colors 5-6-5.h"

/* Exported typedef -----------------------------------------------------------*/
typedef enum { M_SKIP = (uint8_t)0, M_CLEAR = (uint8_t)1, M_NTH = (uint8_t)2 } Menu_MaxIndexState_Typedef;
typedef struct
{
	uint16_t Coord[4];
	const FunctionalState Clip;
	const uint8_t ClipObjIndx;
	uint8_t StartButton;
	const uint8_t MaxButton;
	uint8_t Indx;
	Menu_MaxIndexState_Typedef MaxIndexState;
	uint8_t ChangeIndButton_UP;
	uint8_t ChangeIndButton_DOWN;
	btnINFO *Buttons[16];
	void (*MenuCallBack)(DrawState NewDrawState);

} Menu_Struct_TypeDef;

typedef struct
{
	uint16_t Coord[4];
	char *Text;
	uint16_t Color;
	uint8_t TimeShow;
	uint32_t TimeOFF;
	Boolean Visible;
} Message_TypeDef;

typedef enum {SAVE = (uint8_t)0, RESTORE = (uint8_t)1} SavedState_TypeDef;

/* Exported define -----------------------------------------------------------*/
#define NO_Border			0
#define NO_activeButton		0
#define activeButton		1

#define btn_ForeColor				DarkAuqa
#define btn_CH_A_ForeColor			DarkOrange2
#define btn_CH_B_ForeColor			DarkAuqa
#define btn_activeForeColor			DarkOrange
#define btn_FontColor				White

/* ID for clip objects */
#define ColorMN_ClipObj			1
#define trgMenu_ClipObj			2
#define trgINFO_ClipObj			3
#define Digit_ClipObj			4
#define qMenu_ClipObj			5
#define TsMNU_ClipObj			6
#define AutoCorr_ClipObj		7
#define ChannelsMenu_ClipObj	8
#define ActiveArea_ClipObj		9
#define Message_ClipObj			10
#define ShowFreq_ClipObj		11

#define MenuMax					7

/* Exported variables --------------------------------------------------------*/
extern Menu_Struct_TypeDef gInterfaceMenu, QuickMenu, ChannelA_Menu, ChannelB_Menu, DigitTrigMenu, TrigMenu, MeasMenu;
extern Menu_Struct_TypeDef *pAll_Menu[7];
extern Menu_Struct_TypeDef *pMenu;

extern GridType activeAreaGrid;
extern Message_TypeDef gMessage;

extern uint8_t indxColorA, indxColorB;
extern uint8_t indxTextColorA, indxTextColorB;
extern uint8_t indxColorButtons;
extern uint8_t indxColorGrid;

extern uint16_t leftLimit, rightLimit, upperLimit, lowerLimit;
extern uint16_t centerX, centerY;

extern uint16_t globalBackColor;
extern uint16_t Active_BackColor;
extern uint16_t Active_BorderColor;

extern const char sweepMODE_text[4][10];

/* Exported function --------------------------------------------------------*/
void Draw_Logo(void);
void Draw_Interface(void);
void Save_ReDraw_Auto_Meas(SavedState_TypeDef State);

void UI_ShowFPS(uint8_t FPS_counter);
void UI_SamplingSetTimeout(uint16_t TextColor);
void UI_SamplingClearTimeout(void);

void Draw_CH_Cursors(void);
void Draw_Cursor_CH(uint16_t color);                // отрисовка курсора канала А/B
void DrawTrig_PosX(DrawState NewState, TrigCursorINFO *TrigCursor);                   // отрисовка указателя курсора позиции срабатывания триггера
void Draw_Cursor_Trig(DrawState NewState, uint16_t ClearColor, uint16_t TextColor);   // отрисовка курсора триггера
void Draw_Cursor_Trig_Line(DrawState NewState, TrigCursorINFO *TrigCursor);   // отрисовка линии курсора триггера
void UpdateAllCursors(void);
void Draw_Cursor_Meas_Line(uint16_t cursor, DrawState NewState);    // отрисовка линии курсора автоизмерений
void btnTIME_SCALE_trigX_Update(DrawState NewState);
void Draw_btnTIME_SCALE(uint8_t active);
void Draw_Batt(uint8_t charge_level, uint8_t upd);
uint16_t Verify_Grid_Match(uint16_t X, uint16_t Y);

void Clear_Message(void);
void Show_Message(char *Text);

void Init_COLOR_Mn(uint8_t Leight, uint16_t *ColorMassive);
void Draw_COLOR_Mn(uint8_t Leight);
DrawState Get_State_COLOR_Mn(void);
void Clear_COLOR_Mn(void);
void Change_COLOR_Mn_Position(uint16_t Position, uint16_t *OutColor);

void FrequencyMeas_Draw(Boolean NewStatus);
void FrequencyMeas_SaveRestore_State(uint8_t Save, Boolean *SaveRestoreStatus);

void BackLightPowerState_UpdateButton(void);

void Beep_Start(void);
void Beep_Stop(void);

void ConvertToString(uint32_t Num, char* Str, uint8_t NumSymbol);

void mSet_AllButtons_Color(uint16_t NewColor);
void mSet_Button_Color(Menu_Struct_TypeDef *btnMenu, uint16_t NewColor);

void setActiveButton(btnINFO *Btn);

void Draw_Menu(Menu_Struct_TypeDef *Menu);
void ReDraw_Menu(Menu_Struct_TypeDef *ReDrawMenu, btnINFO *Button);
void Clear_Menu(Menu_Struct_TypeDef *Menu);
void Change_Menu_Indx(void);
void SetActiveMenu(Menu_Struct_TypeDef *NewActiveMenu);

void UI_BackLightPowerState_UpdateButton(void);
void UI_LoadPreferenceUpdate(void);

/* Смена активной области вывода по горизонтали ---------------------------------*/
void Change_horizontal_size(uint16_t NEW_rightLimit);




#endif /* __USER_INTERFACE_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

