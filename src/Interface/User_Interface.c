/*************************************************************************************
*
Description :  NeilScope3 User Interface
Version     :  1.0.0
Date        :  25.12.2012
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <math.h>                             /* math library */

#include "main.h"
#include "systick.h"
#include "Settings.h"
#include "Trig_Menu.h"
#include "User_Interface.h"
#include "Measurments.h"
#include "Processing_and_output.h"
#include "EPM570.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct /* color mn sctruct */
{
	uint16_t X;
	uint16_t Y;
	uint8_t Leight;
	uint8_t Height;
	uint8_t ColorCoeff;
	DrawState State;
} COLOR_Mn_TypeDef;


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef *pAll_Menu[7] = { &gInterfaceMenu, &QuickMenu, &ChannelA_Menu, &ChannelB_Menu, &DigitTrigMenu, &TrigMenu, &MeasMenu };
Menu_Struct_TypeDef *pMenu = &gInterfaceMenu;
uint8_t tmp_MenuIndex;

char chMessage[50] = {0};
Message_TypeDef gMessage = {{24, 206, 24, 220}, &chMessage[0], 0xd6bd, 1, 1, FALSE};

btnINFO *btn = 0;	            // Обьявляем указатель, тип данных btnINFO
btnINFO *saved_btn = 0;         // Обьявляем указатель, тип данных btnINFO

GridType activeAreaGrid = { 205, 120, 389, 200, LightBlack2, ENABLE };
FlagStatus TrigTimeoutState = RESET;

volatile COLOR_Mn_TypeDef ColorMn = { 20, 204, 128,	14, 1, CLEAR };
uint16_t *p256ColorMassive;

uint16_t globalBackColor = LightBlack;		// Цвет фона
uint16_t Active_BackColor = Black;			// Цвет фона активной области
//uint16_t GridColor = Black;					// Цвет фона активной области
uint16_t Active_BorderColor = White;		// Цвет бордюра активной области

uint8_t indxColorA = 14, indxColorB = 68;
uint8_t indxColorButtons = 27;
uint8_t indxColorGrid = 10;

uint8_t indxTextColorA = 127, indxTextColorB = 127;

/* настройки отрисовки активной области, сетки и т.д. */
uint16_t centerX = 200, centerY = 120; 				// координаты цетра активной области
uint16_t leftLimit = 10, rightLimit = 399;			// левая/правая границы вывода
uint16_t upperLimit = 226, lowerLimit = 13;			// вверхняя/нижняя границы вывода


/* Extern variables ---------------------------------------------------------*/
extern btnINFO btnTIME_SCALE;
extern uint16_t beep_cnt;


/* Private function prototypes -----------------------------------------------*/
void Draw_CH_Cursors(void);
void Draw_Batt(uint8_t charge_level, uint8_t upd);
static __inline char int_to_char(uint8_t val);

/* Private Functions --------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Draw_Logo
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_Logo(void)
{
	LCD_FillScreen(Black);	// очищаем экран выбранным цветом 
	LCD_SetTextColor(Red);	// установить цвет текста
	LCD_SetFont(&arialUnicodeMS_16ptFontInfo); // установить шрифт	16
	LCD_PutStrig(30, 120, 1, "NeilScope 3");
	
	LCD_SetTextColor(White);	// установить цвет текста
	LCD_SetFont(&timesNewRoman12ptFontInfo); // установить шрифт	10
	LCD_PutStrig(150, 110, 1, "rev 2.9; Left Radio");
	LCD_PutStrig(150, 95, 1, "full open Hw/Fw project");
	
	delay_ms(1000); delay_ms(1000);
}


/*******************************************************************************
* Function Name  : Draw_Interface
* Description    : Draw All interface
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_Interface(void)
{
	Draw_Logo();								// Нарисовать заставку
	LCD_FillScreen(globalBackColor);			// очищаем экран выбранным цветом
	LCD_SetGraphicsColor(Active_BackColor);		// цвет графики

	/* Рисуем закрашенный прямоугольник, основная область вывода осциллограмм */
	LCD_DrawFillRect(leftLimit, lowerLimit+5, rightLimit, upperLimit - 5, DRAW, Active_BorderColor);
	LCD_DrawGrid(&activeAreaGrid, DRAW); // перерисовываем сетку в области осциллограмм

	LCD_SetFont(&lucidaConsole10ptFontInfo);	// установить шрифт
	
	SetActiveMenu(&gInterfaceMenu);				// делаем активным основное меню
	Draw_Menu(&gInterfaceMenu);

	Draw_btnTIME_SCALE(0);
	Draw_CH_Cursors();      // рисуем курсоры
	Draw_Batt(100, 0);      // рисуем картинку аккумулятора
	
	LCD_SetTextColor(LightGreen);		// установить цвет текста
	LCD_PutStrig(320, 223, 1, "FPS:");	// print FPS label

	if(pnt_gOSC_MODE->autoMeasurments == ON) Draw_Menu(&MeasMenu);

	if(gShowFFTFreq == TRUE)
	{
		FrequencyMeas_SaveRestore_State(1, &gShowFFTFreq);
		FrequencyMeas_Draw(TRUE);
	}

	Draw_Trigg_Info(TriggShowInfo.Status);
}


/*******************************************************************************
* Function Name  : Change_horizontal_size
* Description    : Смена активной области вывода по горизонтали
* Input          : uint16_t NEW_rightLimit
* Return         : None
*******************************************************************************/
void Change_horizontal_size(uint16_t NEW_rightLimit)
{
	/* Очищаем старые осциллограммы */
	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_A, leftLimit, rightLimit - 1);
	Clear_OLD_DataCH_ON_SCREEN(CHANNEL_B, leftLimit, rightLimit - 1);

	/* Oчищаем сетку в области осциллограмм, указатели триггера, указатели области автоизмерений */
	LCD_DrawGrid(&activeAreaGrid, CLEAR);  
	Draw_Cursor_Trig_Line(CLEAR_ALL, (void*)0);
	Draw_Cursor_Meas_Line(measCursor1, CLEAR_ALL);
	Draw_Cursor_Meas_Line(measCursor2, CLEAR_ALL);
	
	/* переопределяем правую границу области вывода осциллограмм и вычисляем центр этой области по оси Х */
	rightLimit = NEW_rightLimit;
	activeAreaGrid.CenterX = (rightLimit + 1 + leftLimit)/2;   
	activeAreaGrid.Width = rightLimit - leftLimit; 
	
	/* Вычисляем новое смещение относительно сетки */    
	Shift = activeAreaGrid.CenterX - (((uint16_t)(activeAreaGrid.CenterX/25))*25);  
		
	LCD_DrawGrid(&activeAreaGrid, DRAW);	// перерисовываем сетку в области осциллограмм
	
	/* если включена синхронизация обновляем позицию триггера по Х */
	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{
		Trigg_Position_X();
	}
}


/*******************************************************************************
* Function Name  : Save_ReDraw_Auto_Meas
* Description    : 
* Input          : uint8_t draw
* Return         : None
*******************************************************************************/
void Save_ReDraw_Auto_Meas(SavedState_TypeDef State)
{
	static uint8_t tmpAutoMeas = OFF;
	static MeasMode_TypeDef *tmMode = (void*)0;

	if(State == RESTORE)
	{
		if(tmMode != (void*)0) mModeActive = tmMode;
		else if((ActiveMode != &FFT_MODE) && (mModeActive == (MeasMode_TypeDef*)&MeasurmentsMode[2]))
		{
			mModeActive = (MeasMode_TypeDef*)&MeasurmentsMode[0];
		}

		if(tmpAutoMeas == ON)
		{
			pnt_gOSC_MODE->autoMeasurments = ON;
			Draw_Menu(&MeasMenu);
		}
		else
		{
			pnt_gOSC_MODE->autoMeasurments = OFF;
		}
	}
	else if(State == SAVE)
	{
		tmpAutoMeas = pnt_gOSC_MODE->autoMeasurments;
		tmMode = mModeActive;

		if(pnt_gOSC_MODE->autoMeasurments == ON)
		{
			pnt_gOSC_MODE->autoMeasurments = OFF;
			Clear_Menu(&MeasMenu);
		}
	}
}


/******************************************************************************
* Function Name  : LCD_DrawSIN
* Description    : Draw "SIN"				/	Нарисовать синус
* Input          : 	X0, Y0, X1, Y1, periods, Color
* Return         : None
*******************************************************************************/
static void LCD_Draw_SIN(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, float periods, uint16_t Color)
{
	uint16_t widht = (X0 > X1)? X0-X1 : X1-X0;
	uint16_t height = (Y0 > Y1)? Y0-Y1 : Y1-Y0;
	float OnePointInPeriod = (2 * M_PI * periods) / ((float)widht);
	float PointInPeriod;
	uint16_t tmpX = (X0 > X1)? X1 : X0;
	uint16_t tmpY = (Y0 > Y1)? Y1 : Y0;

	tmpY = tmpY + (height/2);

	while((tmpX - X0) < widht)
	{
		PointInPeriod = (tmpX - X0) * OnePointInPeriod;
		DrawPixel(tmpX, (uint16_t)((sinf(PointInPeriod) * (height/2)) + tmpY), Color);
		tmpX++;
	}
}


/*******************************************************************************
* Function Name  : btnTIME_SCALE_trigX_Update
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void btnTIME_SCALE_trigX_Update(DrawState NewState)
{
	static uint16_t Old_Position = 0, Old_WindowPosition = 0;
	float win_widht = (float)btnTIME_SCALE.Width * ((float)gOSC_MODE.WindowWidh / (float)gOSC_MODE.oscNumPoints);
	float win_pos = ((float)trigPosX_cursor.WindowPosition * win_widht);
	uint16_t tX = 0;

	if(NewState == CLEAR)
	{
		tX = Old_Position;
		if(tX < win_pos + 1) tX = win_pos + 1;
		else if(tX > win_pos + win_widht - 1) tX = win_pos + win_widht - 1;
		tX += btnTIME_SCALE.Left;

		if(Old_WindowPosition == trigPosX_cursor.WindowPosition) LCD_SetGraphicsColor(DarkRed);
		else LCD_SetGraphicsColor(btnTIME_SCALE.Color);
		LCD_DrawLine(tX, btnTIME_SCALE.Lower + 1, tX, (btnTIME_SCALE.Lower + btnTIME_SCALE.Height) - 1);
	}

	LCD_Draw_SIN(btnTIME_SCALE.Left + 1, btnTIME_SCALE.Lower + (btnTIME_SCALE.Height/4) + 1, (btnTIME_SCALE.Left + btnTIME_SCALE.Width) - 1,
						btnTIME_SCALE.Lower + ((btnTIME_SCALE.Height * 3)/4) + 2, 10, LightGreen);

	if((gOSC_MODE.oscSync != Sync_NONE) && (NewState == DRAW))
	{
		tX = (uint16_t)(((float)trigPosX_cursor.Position * (float)btnTIME_SCALE.Width) / (float)gOSC_MODE.oscNumPoints);
		if(tX < (trigPosX_cursor.WindowPosition * win_widht) + 1) tX = (trigPosX_cursor.WindowPosition * win_widht) + 1;
		else if(tX > (win_widht + (trigPosX_cursor.WindowPosition * win_widht)) - 1) tX = (win_widht + (trigPosX_cursor.WindowPosition * win_widht)) - 1;
		tX += btnTIME_SCALE.Left;

		LCD_SetGraphicsColor(White);
		LCD_DrawLine(tX, btnTIME_SCALE.Lower + 1, tX, (btnTIME_SCALE.Lower + btnTIME_SCALE.Height) - 1);
	}

	Old_Position = tX - btnTIME_SCALE.Left;;
	Old_WindowPosition = trigPosX_cursor.WindowPosition;
}


/*******************************************************************************
* Function Name  : Draw_btnTIME_SCALE
* Description    : Draw button TIME_SCALE
* Input          : active
* Return         : None
*******************************************************************************/
void Draw_btnTIME_SCALE(uint8_t active)
{
//	float win_widht = (pnt_gOSC_MODE->oscSync == Sync_NONE)? (190 * 512) / (float)Get_numPoints() : (190 * 512) / (float)(Get_numPoints() * 2);
	if((*SweepScale) > 1) return;

	float win_widht = (190.0 * (float)pnt_gOSC_MODE->WindowWidh) / (float)gOSC_MODE.oscNumPoints;
	float win_pos = ((float)gOSC_MODE.WindowPosition * win_widht) + 1;
	uint16_t brdColor = (pMNU == TimeScale_Menu)? Orange1 : LightGreen;
	uint16_t fillColor = DarkRed; //(Get_numPoints() <= pnt_gOSC_MODE->WindowWidh)? LightBlack2 : DarkRed;
	
	LCD_SetGraphicsColor(btnTIME_SCALE.Color);
	LCD_DrawFillRect(btnTIME_SCALE.Left, btnTIME_SCALE.Lower, btnTIME_SCALE.Left + btnTIME_SCALE.Width,
			btnTIME_SCALE.Lower + btnTIME_SCALE.Height, active, brdColor);

	if(win_widht <= 2) win_widht = 2;
	LCD_SetGraphicsColor(fillColor);
	LCD_DrawFillRect(btnTIME_SCALE.Left + win_pos, btnTIME_SCALE.Lower + 1,
			btnTIME_SCALE.Left + win_pos + win_widht - 2, (btnTIME_SCALE.Lower + btnTIME_SCALE.Height) - 1, 0, 0);

	btnTIME_SCALE_trigX_Update(DRAW);

	if(active == 0)
	{
		LCD_SetGraphicsColor(Active_BackColor);
		LCD_DrawLine(btnTIME_SCALE.Left, btnTIME_SCALE.Lower + btnTIME_SCALE.Height, btnTIME_SCALE.Left + btnTIME_SCALE.Width, btnTIME_SCALE.Lower + btnTIME_SCALE.Height);
		LCD_DrawLine(btnTIME_SCALE.Left + btnTIME_SCALE.Width, btnTIME_SCALE.Lower, btnTIME_SCALE.Left + btnTIME_SCALE.Width, btnTIME_SCALE.Lower + btnTIME_SCALE.Height);
	}
}


/*******************************************************************************
* Function Name  : Draw_Batt
* Description    : Draw battarey ico
* Input          : charge_level, upd
* Return         : None
*******************************************************************************/
void Draw_Batt(uint8_t charge_level, uint8_t upd)
{
	uint16_t i, j;//, X = 369, Y = upperLimit - 3;
	uint8_t BattWidth = 27, BattHeight = 10;
	uint16_t tmpColor = LCD_GetGraphicsColor();
	uint16_t ScaleColor, tClolor;
	uint8_t  RedColorScale;

	if(charge_level >= 100) charge_level = 25;
	else charge_level = charge_level / 4;

	if(upd == 0)
	{
		LCD_SetGraphicsColor(White);
		LCD_DrawRect(399 - BattWidth - 4, upperLimit + 1, 399 - BattWidth, upperLimit + 6);
		LCD_DrawRect(399 - BattWidth, upperLimit - 3, 399, upperLimit + BattHeight);
	}
	
	RedColorScale = (uint8_t)(31/charge_level);
	for(i = 0; i < charge_level; i++)
	{		
		for(j = 0; j <= BattHeight + 1; j++)
		{
			tClolor = M256_Colors[150 + j];
			ScaleColor = (tClolor - ((((i/2) * RedColorScale)<<5) + (i * RedColorScale))) + ((i * RedColorScale)<<11);
			DrawPixel(399 - (charge_level - i), j + upperLimit - 2, ScaleColor);
		}
	}
	
	LCD_SetGraphicsColor(globalBackColor);
	LCD_DrawLine(399 - BattWidth, upperLimit + 2, 399 - BattWidth, upperLimit + 6);
	
	for(i = 399 - (BattWidth - 1); i < 398 - charge_level; i++)
	{
		LCD_DrawLine(i, upperLimit - 2, i, upperLimit + 10);      	 
	}
	
	LCD_SetGraphicsColor(tmpColor);
}


/*******************************************************************************
* Function Name  : ShowFPS
* Description    : отрисовка показаний количества кадров за сек.
* Input          : uint8_t FPS_counter
* Return         : None
*******************************************************************************/
void UI_ShowFPS(uint8_t FPS_counter)
{   
    static char FPS_TEXT[3] = {' ', ' ', 0};
    
    /* установить цвет фона, шрифт */
    LCD_SetBackColor(globalBackColor);
    LCD_SetFont(&lucidaConsole10ptFontInfo);
        
    /* очищаем старые показания */
    LCD_PutColorStrig(349, 223, 0, &FPS_TEXT[0], globalBackColor);
	
    /* verify limit */
	if(FPS_counter >= 100) FPS_counter = 99;
	ConvertToString(FPS_counter, &FPS_TEXT[0], 2);

	/* выводим новые показания */
    LCD_PutColorStrig(349, 223, 0, &FPS_TEXT[0], White);
    
    /* сбрасываем флаг */
    show_FPS_flag = 0;
}


/*******************************************************************************
* Function Name  : IndicateTrigTimeout
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void UI_SamplingSetTimeout(uint16_t TextColor)
{   
    static char txt_OLD = 'T';
    char *txtTimeout = "T";

	LCD_SetBackColor(Active_BackColor);			// set text color to globalBackColor
	LCD_SetFont(&lucidaConsole10ptFontInfo);	// set font

	/* выводим */
	if(pnt_gOSC_MODE->oscSync == Sync_NONE)
	{
		LCD_SetTextColor(Blue);
		LCD_SetGraphicsColor(Gray);
		txtTimeout = "S";
	}
	else
	{
		/* установить цвет текста */
		LCD_SetTextColor(TextColor);
		LCD_SetGraphicsColor(Red);
	}

	if(txt_OLD != *txtTimeout) UI_SamplingClearTimeout();

	LCD_DrawFillRect(304, 225, 314, 235, 0, 0);
	LCD_PutStrig(306, 223, 1, txtTimeout);

	txt_OLD = *txtTimeout;
	TrigTimeoutState = SET;
}


/*******************************************************************************
* Function Name  : IndicateTrigTimeout
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void UI_SamplingClearTimeout(void)
{
	TrigTimeoutState = RESET;
	LCD_SetGraphicsColor(globalBackColor);		 // set clear color to globalBackColor
	LCD_DrawFillRect(304, 225, 314, 235, 0, 0);	 // clear
}

/* ----------------------------------------------- CURSORS ----------------------------------------------- */

/*******************************************************************************
* Function Name  : Draw_CH_Cursors
* Description    : отрисовка курсоров растеризированными треугольниками
* Input          : uint16_t color
* Return         : None
*******************************************************************************/
void Draw_CH_Cursors(void)
{
	Set_CH_TypeINFO(CHANNEL_A); 
	Draw_Cursor_CH(pINFO->Color);
	Set_CH_TypeINFO(CHANNEL_B);
	Draw_Cursor_CH(pINFO->Color);    
}


/*******************************************************************************
* Function Name  : Draw_Cursor_CH
* Description    : отрисовка курсора канала А/B
* Input          : uint16_t color
* Return         : None
*******************************************************************************/
void Draw_Cursor_CH(uint16_t color)
{ 
	LCD_DrawFillTriangle(0, pINFO->Position - 3, 0, pINFO->Position + 5, leftLimit, pINFO->Position + 1, color);  
}


/*******************************************************************************
* Function Name  : Draw_Cursor_Trig
* Description    : указатели триггера
* Input          :
* Return         : None
*******************************************************************************/
void Draw_Cursor_Trig(DrawState NewState, uint16_t ClearColor, uint16_t TextColor)
{
	TrigCursorINFO *tTrigCursor = (pntTrigCursor == &Height_Y_cursor)? &Low_Y_cursor : &Height_Y_cursor;
	uint8_t tmpNewState = NewState;
	uint16_t textColor1, textColor2;

	LCD_SetFont(&lucidaConsole_9pt_Bold_FontInfo);
	LCD_SetTextColor(globalBackColor);

	if(pnt_gOSC_MODE->AnalogSyncType > 1)
	{
		if(NewState == DRAW)
		{
			pntTrigCursor->Color = White;
			tTrigCursor->Color = LightGray4;
		}
		else
		{
			pntTrigCursor->Color = ClearColor;
			tTrigCursor->Color = ClearColor;
		}

		textColor1 = TextColor;
		textColor2 = TextColor;
	}
	else
	{
		if(NewState == DRAW) pntTrigCursor->Color = White;
		else pntTrigCursor->Color = ClearColor;

		tTrigCursor->Color = globalBackColor;
		tmpNewState = CLEAR;
		textColor1 = TextColor;
		textColor2 = globalBackColor;
	}


	/* Не активный курсор */
	LCD_DrawFillTriangle(0, tTrigCursor->Position - 3, 0, tTrigCursor->Position + 5,
			leftLimit, tTrigCursor->Position + 1, tTrigCursor->Color);

	Draw_Cursor_Trig_Line(tmpNewState, tTrigCursor);
	LCD_SetTextColor(textColor2);
	LCD_PutStrig(0, tTrigCursor->Position - 2, 1, tTrigCursor->Name);			/* очищаем старые показания */

	/* Активный курсор */
	LCD_DrawFillTriangle(0, pntTrigCursor->Position - 3, 0, pntTrigCursor->Position + 5,
			leftLimit, pntTrigCursor->Position + 1, pntTrigCursor->Color);

	Draw_Cursor_Trig_Line(NewState, pntTrigCursor);
	LCD_SetTextColor(textColor1);
	LCD_PutStrig(0, pntTrigCursor->Position - 2, 1, pntTrigCursor->Name);		/* очищаем старые показания */

	LCD_SetFont(&lucidaConsole10ptFontInfo);
}


/*******************************************************************************
* Function Name  : Draw_Cursor_Trig_Line
* Description    : отрисовка линии курсора триггера
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_Cursor_Trig_Line(DrawState NewState, TrigCursorINFO *TrigCursor)
{   
	uint16_t cnt_, clrColor;
	uint16_t tmpLineX0 = leftLimit + 2, tmpLineY0 = Height_Y_cursor.Position;
	uint16_t tmpLineX1 = rightLimit - leftLimit - 2, tmpLineY1 = Height_Y_cursor.Position;
	uint16_t tmp;
	
	if(NewState == CLEAR)  // если очистка с перерисовкой сетки в точках совпадения
	{ 
		/* Очищаем старую линию */
		for (cnt_ = 0; cnt_ < rightLimit - leftLimit - 2; cnt_++)
		{
			tmp = cnt_ + leftLimit + 2;
			if(Verify_Clip_Point(tmp, TrigCursor->Position) != SET)
	  		{
				clrColor = Verify_Grid_Match(tmp, TrigCursor->Position);
				DrawPixel(tmp, TrigCursor->Position, clrColor);
			}
		}           
	}	
	else if(NewState == CLEAR_ALL)  // если полная очистка
	{
		/* Очищаем старую линию */
		LCD_SetGraphicsColor(Active_BackColor);
		if(Verify_Clip_Line(&tmpLineX0, &tmpLineY0, &tmpLineX1, &tmpLineY1) != SET)
		{
			LCD_DrawLine(tmpLineX0, tmpLineY0, tmpLineX1, tmpLineY1);
		}

		tmpLineX0 = leftLimit + 2; tmpLineY0 = Low_Y_cursor.Position;
		tmpLineX1 = rightLimit - leftLimit - 2; tmpLineY1 = Low_Y_cursor.Position;
		if(Verify_Clip_Line(&tmpLineX0, &tmpLineY0, &tmpLineX1, &tmpLineY1) != SET)
		{
			LCD_DrawLine(tmpLineX0, tmpLineY0, tmpLineX1, tmpLineY1);
		}
	}
	else if (TrigCursor != (void*)0)   // иначе отрисовка
	{
		LCD_SetGraphicsColor(TrigCursor->Color);	// цвет серый
		
		for(cnt_ = leftLimit + 2; cnt_ <= rightLimit - 7; cnt_ += 10)
		{         
			if(Verify_Clip_Point(cnt_, TrigCursor->Position) != SET)
	  		{
				LCD_DrawLine(cnt_, TrigCursor->Position, cnt_ + 6, TrigCursor->Position);
			}
		}
	}          
}


/*******************************************************************************
* Function Name  : DrawTrig_PosX
* Description    : отрисовка указателя курсора позиции срабатывания триггера
* Input          : None
* Return         : None
*******************************************************************************/
void DrawTrig_PosX(DrawState NewState, TrigCursorINFO *TrigCursor)
{
	uint16_t cnt_, clrColor;
	uint32_t StartWindowX = pnt_gOSC_MODE->WindowPosition * 389;
	uint32_t WindowX0 = StartWindowX + leftLimit + 10;
	uint32_t WindowX1 = StartWindowX + rightLimit - 10;
	uint16_t X;

	if(pnt_gOSC_MODE->oscSync == Sync_NONE) NewState = CLEAR;

	if(trigPosX_cursor.WindowPosition == pnt_gOSC_MODE->WindowPosition)
	{
		if(TrigCursor->Position < WindowX0) TrigCursor->Position = WindowX0;
		else if(TrigCursor->Position > WindowX1) TrigCursor->Position = WindowX1;

		X = (TrigCursor->Position - (StartWindowX));
	}
	else return;

	if(NewState == CLEAR)  // если очистка
	{ 
		/* Очищаем указатель */
		LCD_SetGraphicsColor(Active_BackColor);      
		LCD_DrawLine(X - 3, upperLimit - 4, X + 4, upperLimit - 4);
		LCD_DrawLine(X - 3, lowerLimit + 4, X + 4, lowerLimit + 4);

		/* Очищаем старую линию */
		for (cnt_ = lowerLimit + 6; cnt_ < upperLimit - 6; cnt_++)
		{
			if(Verify_Clip_Point(X, cnt_) != SET)
			{
				clrColor = Verify_Grid_Match(X, cnt_);	// проверяем на совпадение с сеткой
				DrawPixel(X, cnt_, clrColor);			// устанавливаем цвет точки и выводим точку
			}
		}
		trigPosX_cursor.Drawed = FALSE;
	}	
	else if((NewState == DRAW) )  // иначе отрисовка
	{
		/* рисуем указатель */
		LCD_SetGraphicsColor(Orange);
		LCD_DrawLine(X - 3, upperLimit - 4, X + 4, upperLimit - 4);
		LCD_DrawLine(X - 3, lowerLimit + 4, X + 4, lowerLimit + 4);

		/* рисуем линию*/
		for (cnt_ = lowerLimit + 6; cnt_ < upperLimit - 6; cnt_++)
		{
			if(Verify_Clip_Point(X, cnt_) != SET)
			{
				DrawPixel(X, cnt_, trigPosX_cursor.Color);
			}
		}
		trigPosX_cursor.Drawed = TRUE;
	}
}


/*******************************************************************************
* Function Name  : Draw_Cursor_Meas_Line
* Description    : отрисовка линии курсора автоизмерений
* Input          : None
* Return         : None
*******************************************************************************/
void Draw_Cursor_Meas_Line(uint16_t cursor, DrawState NewState)
{   
	uint16_t cnt_, clrColor;
	uint16_t tmpLineX0 = cursor, tmpLineY0 = lowerLimit + 6;
	uint16_t tmpLineX1 = cursor, tmpLineY1 = upperLimit - lowerLimit - 16;

	if(NewState == CLEAR) // если очистка
	{
		/* устанавливаем цвет графики цветом бордюра области вывода осциллограмм */
		LCD_SetGraphicsColor(Active_BorderColor);	
		
		/* очищаем указатели */
		LCD_DrawLine(leftLimit, upperLimit - 5, rightLimit, upperLimit - 5);
		LCD_DrawLine(cursor - 2, lowerLimit + 5, cursor + 3, lowerLimit + 5);
		
		LCD_SetGraphicsColor(Active_BackColor);
		LCD_DrawLine(cursor - 2, lowerLimit + 4, cursor + 3, lowerLimit + 4);
		LCD_DrawLine(leftLimit, upperLimit - 4, rightLimit, upperLimit - 4);

		/* Очищаем старую линию */
		for (cnt_ = lowerLimit + 6; cnt_ < upperLimit - 9; cnt_++)
		{
			clrColor = Verify_Grid_Match(cursor, cnt_);
						
			if(Verify_Clip_Point(cursor, cnt_) != SET)
			{
				/* устанавливаем цвет точки и выводим точку */
				DrawPixel(cursor, cnt_, clrColor);
			}
		}           
	}
	else if(NewState == CLEAR_ALL)  // если полная очистка
	{
		LCD_SetGraphicsColor(Active_BackColor);
		if(Verify_Clip_Line(&tmpLineX0, &tmpLineY0, &tmpLineX1, &tmpLineY1) != SET)
		{
			LCD_DrawLine(tmpLineX0, tmpLineY0, tmpLineX1, tmpLineY1);
		}
	}	
	else // иначе отрисовка
	{
		/* устанавливаем цвет графики цветом указателей курсоров автоизмерений */
		LCD_SetGraphicsColor(Red);	
		
		/* рисуем указатели */
		LCD_DrawLine(cursor - 2, upperLimit - 5, cursor + 3, upperLimit - 5);
		LCD_DrawLine(cursor - 2, lowerLimit + 5, cursor + 3, lowerLimit + 5);
		LCD_DrawLine(cursor - 2, upperLimit - 4, cursor + 3, upperLimit - 4);
		LCD_DrawLine(cursor - 2, lowerLimit + 4, cursor + 3, lowerLimit + 4);
		
		/* устанавливаем цвет графики цветом сетки */
		LCD_SetGraphicsColor(Gray);	// серый
		
		/* рисуем штрих пунктирную линию*/
		for(cnt_ = lowerLimit + 6; cnt_ < upperLimit - 10; cnt_ += 10)
		{
			if(Verify_Clip_Point(cursor, cnt_) != SET)
			{
				LCD_DrawLine(cursor, cnt_, cursor, cnt_ + 4);
			}
		}
	}          
}


/*******************************************************************************
* Function Name  : UpdateAllCursors
* Description    : 
* Input          : None
* Return         : None
*******************************************************************************/
void UpdateAllCursors(void)
{
	if(pnt_gOSC_MODE->oscSync != Sync_NONE)
	{
		Draw_Cursor_Trig_Line(DRAW, &Height_Y_cursor);

		if((pnt_gOSC_MODE->AnalogSyncType == Sync_IN_WIN) || (pnt_gOSC_MODE->AnalogSyncType == Sync_OUT_WIN))
		{
			Draw_Cursor_Trig_Line(DRAW, &Low_Y_cursor);
		}

		DrawTrig_PosX(DRAW, &trigPosX_cursor);
	}
  
   if(pnt_gOSC_MODE->autoMeasurments == ON)
   {
      Draw_Cursor_Meas_Line(measCursor1, DRAW);
      Draw_Cursor_Meas_Line(measCursor2, DRAW);
   }
}


/*******************************************************************************
* Function Name  : Verify_Grid_Match
* Description    : Функция 
* Input          : X_Left, X_Right, Y_Up
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t Verify_Grid_Match(uint16_t X, uint16_t Y)
{
	if((Y == activeAreaGrid.CenterY) || (X == activeAreaGrid.CenterX)) return activeAreaGrid.Color;
	else if(((Y - 20)%5 == 0) && ((X - Shift)%25 == 0)) return activeAreaGrid.Color;
	else if(((Y - 20)%25 == 0)&& ((X - Shift)%5 == 0)) return activeAreaGrid.Color;
	else return Active_BackColor;
} 		




/* BUTTONS & MENU ********************************************************************************************/

/*******************************************************************************
* Function Name  : setActiveButton
* Description    : Функция выбора активной кнопки меню
* Input          : btnINFO *selectBtn
* Output         : None
* Return         : None
*******************************************************************************/
void mSet_AllButtons_Color(uint16_t NewColor)
{
	uint8_t j;
	uint16_t tColor = Color_ChangeBrightness(NewColor, -20);

	for(j = 0; j < MenuMax; j++){ mSet_Button_Color(pAll_Menu[j], tColor); }
}


/*******************************************************************************
* Function Name  : setActiveButton
* Description    : Функция выбора активной кнопки меню
* Input          : btnINFO *selectBtn
* Output         : None
* Return         : None
*******************************************************************************/
void mSet_Button_Color(Menu_Struct_TypeDef *btnMenu, uint16_t NewColor)
{
	uint8_t i;
	uint8_t MaxButtons = (btnMenu == &gInterfaceMenu)? btnMenu->MaxButton - 1 : btnMenu->MaxButton;

	for(i = 0; i <= MaxButtons; i++) btnMenu->Buttons[i]->Color = NewColor;
}


/*******************************************************************************
* Function Name  : setActiveButton
* Description    : Функция выбора активной кнопки меню
* Input          : btnINFO *selectBtn
* Output         : None
* Return         : None
*******************************************************************************/
void setActiveButton(btnINFO *selectBtn)
{
	btn = selectBtn;
}


/*******************************************************************************
* Function Name  : saveActiveButton
* Description    : Функция сохранения активной кнопки меню
* Input          : btnINFO *selectBtn
* Output         : None
* Return         : None
*******************************************************************************/
void saveActiveButton(btnINFO *selectBtn)
{
	saved_btn = selectBtn;
}


/*******************************************************************************
* Function Name  : Draw_Quick_Menu_Buttons
* Description    : Функция включения/выключения меню
* Input          : DRAW/CLEAR - нарисовать/очистить
* Return         : None
*******************************************************************************/
void Draw_Menu(Menu_Struct_TypeDef *Menu)
{
	uint8_t i;

	/* если отрисовываемое меню активно тогда сохраняем активную кнопку меню из которого переходим */
	if((Menu != &gInterfaceMenu) && (Menu == pMenu)){ saveActiveButton(btn); LCD_DrawButton(btn, NO_activeButton); }

	/* если коллбэк функция меню не равна 0, то выполняем ее */
	if(Menu->MenuCallBack != (void*)0) Menu->MenuCallBack(DRAW);

	/* если для меню нужно установить ограничение по отрисовке */
	if(Menu->Clip == ENABLE)
	{
		/* устанавливаем ограничения для отрисовки */
		Set_New_ClipObject(Menu->Coord[0], Menu->Coord[1], Menu->Coord[2], Menu->Coord[3], IN_OBJECT, Menu->ClipObjIndx);
		LCD_ClearArea(Menu->Coord[0] + 2, Menu->Coord[1] + 2, Menu->Coord[2] - 2, Menu->Coord[3] - 2, Active_BackColor);
		Update_Oscillogram();	/* обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения */
	}

	/* отрисовываем кнопки */
	for(i = 0; i <= Menu->MaxButton; i++)
	{
		LCD_DrawButton(Menu->Buttons[i], NO_activeButton);
	}

	if(Menu == pMenu)	// если отрисовываемое меню активно тогда отрисовываем активной стартовую кнопку
	{
		setActiveButton(Menu->Buttons[Menu->StartButton]);
		LCD_DrawButton(btn, activeButton);
	}
}


/*******************************************************************************
* Function Name  : ReDraw_Menu
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void ReDraw_Menu(Menu_Struct_TypeDef *ReDrawMenu, btnINFO *ActiveButton)
{
	uint8_t i;
	uint8_t MaxButtons = (ReDrawMenu == &gInterfaceMenu)? ReDrawMenu->MaxButton - 1 : ReDrawMenu->MaxButton;

	/* отрисовываем кнопки */
	for(i = 0; i <= MaxButtons; i++) if(ReDrawMenu->Buttons[i] != ActiveButton) LCD_DrawButton(ReDrawMenu->Buttons[i], NO_activeButton);
	if(ActiveButton != (void*)0){ setActiveButton(ActiveButton); LCD_DrawButton(btn, activeButton); }
}


/*******************************************************************************
* Function Name  : Draw_Quick_Menu_Buttons
* Description    : Функция включения/выключения меню
* Input          : DRAW/CLEAR - нарисовать/очистить
* Return         : None
*******************************************************************************/
void Clear_Menu(Menu_Struct_TypeDef *Menu)
{
	/* Очищаем */
	if(Menu->Clip == ENABLE)
	{
		LCD_ClearArea(Menu->Coord[0], Menu->Coord[1], Menu->Coord[2], Menu->Coord[3], Active_BackColor);
		Clear_ClipObject(Menu->ClipObjIndx);	// убираем ограничения для отрисовки
		LCD_DrawGrid(&activeAreaGrid, DRAW);	// перерисовываем сетку в области осциллограмм
		Update_Oscillogram();					// обновлем осциллограммы
	}

	if(Menu->MenuCallBack != (void*)0) Menu->MenuCallBack(CLEAR);

	if((Menu != &gInterfaceMenu) && (Menu == pMenu))
	{
		setActiveButton(saved_btn);				/* восстанавливаем сохраненную активную кнопку */
		LCD_DrawButton(btn, activeButton);
	}

}


/*******************************************************************************
* Function Name  : Change_Menu_Indx
* Description    : Функция смены индекса меню
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_Menu_Indx(void)
{
	//tmp_MenuIndex = pMenu->MenuMaxButton;

	/* Если были нажаты кнопки вверх или вниз то изменяем положения указателя меню */
	if(ButtonsCode == pMenu->ChangeIndButton_UP)pMenu->Indx++;       // нажата кнопка DOWN
	else if(ButtonsCode == pMenu->ChangeIndButton_DOWN)pMenu->Indx--;    // нажата кнопка UP

	/* проверка и выход если нажата кнопка вниз на самом нижнем положении указателя */
	if(pMenu->Indx == 255) pMenu->Indx = pMenu->MaxButton;
	else if(pMenu->Indx > pMenu->MaxButton)
	{
		pMenu->Indx = pMenu->StartButton;		// индекс меню на начальную позицию
		if(pMenu != &gInterfaceMenu)
		{
			if(pMenu == &MeasMenu)
			{
				LCD_DrawButton(btn, NO_activeButton);		// перерисовываем текущую кнопку как неактивную
				setActiveButton(gInterfaceMenu.Buttons[6]);	// сделать активной кнопку по индексу
				LCD_DrawButton(btn, activeButton);
			}
			else if(pMenu == &DigitTrigMenu) return;
			else Clear_Menu(pMenu);	// Очищаем меню

			SetActiveMenu(&gInterfaceMenu);		// делаем активным основное меню
			return;
		}
	}

	/* если положения указателя изменилось, то */
	if(tmp_MenuIndex != pMenu->Indx)
	{
		if(ColorMn.State == DRAW) Clear_COLOR_Mn();

		if(btn == &btnTIME_SCALE) Draw_btnTIME_SCALE(NO_activeButton);
		else LCD_DrawButton(btn, NO_activeButton);			// перерисовываем текущую кнопку как неактивную
		setActiveButton(pMenu->Buttons[pMenu->Indx]);	// сделать активной кнопку по индексу
		tmp_MenuIndex = pMenu->Indx;					// сохраним текущий индекс
	}

	if(BeepState != DISABLE) Beep_Start();
	btn->btnEvent_func();								// вызвать соответсвующую функцию обработчик
	if(btn == &btnTIME_SCALE) Draw_btnTIME_SCALE(activeButton);
	else LCD_DrawButton(btn, activeButton);					// и перерисовать кнопку
}


/*******************************************************************************
* Function Name  : Change_Menu_Indx
* Description    : Функция смены индекса меню
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetActiveMenu(Menu_Struct_TypeDef *NewActiveMenu)
{
	pMenu = NewActiveMenu;
}




/* ----------------------------------------------- COLORS ----------------------------------------------- */

/*******************************************************************************
* Function Name  : Init_COLOR_Mn
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_COLOR_Mn(uint8_t Leight, uint16_t *ColorMassive)
{
	if(ColorMn.State == DRAW) return;
	else if(gMessage.Visible == TRUE) Clear_Message();

	if(TriggShowInfo.Status != DISABLE) ColorMn.Y = 189;
	else ColorMn.Y = 203;

	ColorMn.Leight = Leight;
	ColorMn.State = DRAW;
	ColorMn.ColorCoeff = (uint8_t)(256/Leight);
	p256ColorMassive = ColorMassive;

	/* устанавливаем ограничения для отрисовки */
	Set_New_ClipObject(ColorMn.X - 1, ColorMn.Y, ColorMn.X + ColorMn.Leight, upperLimit - 6, IN_OBJECT, ColorMN_ClipObj);

	/* обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения */
	Update_Oscillogram();

	Draw_COLOR_Mn(Leight);
}


/*******************************************************************************
* Function Name  : Get_State_COLOR_Mn
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Draw_COLOR_Mn(uint8_t Leight)
{
	uint8_t i;

	for(i = 0; i < Leight; i++) 
	{
		LCD_SetGraphicsColor(*(p256ColorMassive + (i * ColorMn.ColorCoeff)));
		LCD_DrawLine(ColorMn.X + i, ColorMn.Y, ColorMn.X + i, ColorMn.Y + ColorMn.Height);
	}

	ColorMn.State = DRAW;
}


/*******************************************************************************
* Function Name  : Get_State_COLOR_Mn
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
DrawState Get_State_COLOR_Mn(void)
{
	return ColorMn.State;
}


/*******************************************************************************
* Function Name  : Clear_COLOR_Mn
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Clear_COLOR_Mn(void)
{
	if(ColorMn.State != DRAW) return;
	
	ColorMn.State = CLEAR;
	Clear_ClipObject(ColorMN_ClipObj);				/* убираем ограничения для отрисовки */
	LCD_SetGraphicsColor(Active_BackColor);
	LCD_DrawFillRect(ColorMn.X - 1, ColorMn.Y - 1, ColorMn.X + ColorMn.Leight, ColorMn.Y + ColorMn.Height, 0, 0);

	LCD_DrawGrid(&activeAreaGrid, DRAW);	// перерисовываем сетку в области осциллограмм					
	Update_Oscillogram();					/* обновлем осциллограммы */
}


/*******************************************************************************
* Function Name  : Change_COLOR_Mn_Position
* Description    : Функция изменения цвета канала А
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Change_COLOR_Mn_Position(uint16_t Position, uint16_t *OutColor)
{
	static uint16_t Old_Position = 0;
	uint16_t ColorIndx = ColorMn.ColorCoeff * Position;

	if((ColorMn.State != DRAW) || (Position >= ColorMn.Leight)) return;
	
	/* очищаем старую позицию */	
	LCD_SetGraphicsColor(*(p256ColorMassive + ColorIndx));
	LCD_DrawLine(ColorMn.X + Old_Position, ColorMn.Y, ColorMn.X + Old_Position, ColorMn.Y + ColorMn.Height);

	*OutColor = *(p256ColorMassive + ColorIndx);
	
	/* рисуем по новой позиции */	
	LCD_SetGraphicsColor(Black);
	LCD_DrawLine(ColorMn.X + Position, ColorMn.Y, ColorMn.X + Position, ColorMn.Y + ColorMn.Height);
		
	Old_Position = Position;
}



/* ----------------------------------------------- Show FFT ----------------------------------------------- */
/*******************************************************************************
* Function Name  : FrequencyMeas_Draw
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void FrequencyMeas_Draw(Boolean NewState)
{
	uint16_t X0 = rightLimit - 60, Y0 = upperLimit - 33;
	uint16_t X1 = rightLimit - 1, Y1 = upperLimit - 6;

	if(pnt_gOSC_MODE->autoMeasurments != OFF) return;

	if((NewState == TRUE) && (ActiveMode != &IntMIN_MAX))
	{
		/* устанавливаем ограничения для отрисовки */
		if(Set_New_ClipObject(X0, Y0, X1, Y1, IN_OBJECT, ShowFreq_ClipObj) != ERROR)
		{
			Update_Oscillogram();	// обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения
			Reset_Calc_Show_Freq();
			LCD_SetGraphicsColor(LightBlack2);
			LCD_DrawFillRect(X0, Y0, X1, Y1, 0, 0);
		}
	}
	else
	{
		/* Clear */
		if(Clear_ClipObject(ShowFreq_ClipObj) != ERROR)
		{
			LCD_ClearArea(X0, Y0, X1, Y1, Active_BackColor);
			LCD_DrawGrid(&activeAreaGrid, DRAW);	/* перерисовываем сетку в области осциллограмм */
			Update_Oscillogram();				    /* обновлем осциллограммы */
		}
	}

	gShowFFTFreq = NewState;
}


/*******************************************************************************
* Function Name  : FrequencyMeas_SaveRestore_State
* Description    :
* Input          :
* Return         : None
*******************************************************************************/
void FrequencyMeas_SaveRestore_State(uint8_t Save, Boolean *SaveRestoreStatus)
{
	static Boolean SavedStatus;
	static uint8_t SavedFlag = 0;

	if((Save == 1) && (SavedFlag == 0))
	{
		SavedStatus = *SaveRestoreStatus;
		SavedFlag = 1;
	}
	else if((Save == 0) && (SavedFlag == 1))
	{
		*SaveRestoreStatus = SavedStatus;
		SavedFlag = 0;
	}
}




/*******************************************************************************
* Function Name  : Beep_Start
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Beep_Start(void)
{
	beep_cnt = 1;
	TIM_SetAutoreload(TIM4, 20000);		// для изменения частоты звука изменить значение 20000
	TIM_Cmd(TIM4, ENABLE);				/* TIM4 enable counter */
}


/*******************************************************************************
* Function Name  : Beep_Stop
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void Beep_Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);		/* TIM4 disable counter */
}


/*******************************************************************************
* Function Name  : ShowMessage
* Description    :
* Input          : *Text
* Return         : None
*******************************************************************************/
void Clear_Message(void)
{
	/* Clear */
	LCD_ClearArea(leftLimit + 1, gMessage.Coord[1], gMessage.Coord[2] + 5, gMessage.Coord[3], Active_BackColor);
	Clear_ClipObject(Message_ClipObj);		/* убираем ограничения для отрисовки */
	LCD_DrawGrid(&activeAreaGrid, DRAW);	/* перерисовываем сетку в области осциллограмм */
	Update_Oscillogram();				    /* обновлем осциллограммы */

	memset(&chMessage[0], 0, 50);
	gMessage.TimeOFF = 0;
	gMessage.Visible = FALSE;
}


/*******************************************************************************
* Function Name  : ShowMessage
* Description    :
* Input          : *Text
* Return         : None
*******************************************************************************/
void Show_Message(char *Text)
{
	/*  */
	if(TriggShowInfo.Status == DISABLE){ gMessage.Coord[1] = 206; gMessage.Coord[3] = 220; }
	else{ gMessage.Coord[1] = 186; gMessage.Coord[3] = 200; }

	/* Clear if another message now displayed */
	if(gMessage.Visible == TRUE) Clear_Message();

	/* Reset X1 coordinate */
	gMessage.Coord[2] = gMessage.Coord[0];
	memcpy(&chMessage[0], Text, strlen(Text));

	/* Width message text */
	LCD_Strig_PointsWidht((uint16_t*)&gMessage.Coord[2], gMessage.Text);

	/* устанавливаем ограничения для отрисовки */
	Set_New_ClipObject(leftLimit + 1, gMessage.Coord[1], gMessage.Coord[2], gMessage.Coord[3], IN_OBJECT, Message_ClipObj);
	Update_Oscillogram();	// обновлем осциллограммы, для того что бы они "обрезались" по размерам отсечения

	/* Print message */
	LCD_ClearArea(leftLimit + 1, gMessage.Coord[1], gMessage.Coord[2] + 5, gMessage.Coord[3], Active_BackColor);
	LCD_PutColorStrig(gMessage.Coord[0], gMessage.Coord[1], 1, gMessage.Text, gMessage.Color);

	gMessage.TimeOFF = RTC_GetCounter();
	gMessage.TimeOFF += gMessage.TimeShow;
	gMessage.Visible = TRUE;
}



///*******************************************************************************
//* Function Name  : Update_DHT_Sensor_OnScreen
//* Description    :
//* Input          : None
//* Return         : None
//*******************************************************************************/
//void ConvertToString(uint32_t Num, char* Str, uint8_t NumSymbol)
//{
//	uint8_t A[6] = { 0 };
//	int8_t i;
//
//	while(Num >= 100000){ Num = Num - 100000; A[5]++; }
//	while(Num >= 10000){ Num = Num - 10000; A[4]++; }
//	while(Num >= 1000){ Num = Num - 1000; A[3]++; }
//	while(Num >= 100){ Num = Num - 100;	A[2]++; }
//	while(Num >= 10){ Num = Num - 10; A[1]++; }
//	while(Num >= 1){ Num = Num - 1; A[0]++; }
//
//	for(i = NumSymbol - 1; i >= 0; i--)
//	{
//		(*Str) = A[i] + 48;
//		Str++;
//	}
//}

/*******************************************************************************
* Function Name  : ConvertToString
* Description    :
* Input          : None
* Return         : None
*******************************************************************************/
void ConvertToString(uint32_t Num, char* Str, uint8_t NumSymbol)
{
	int32_t i, j, Rate;
	uint8_t A[NumSymbol];
	uint32_t NumPow = 1;

	for(i = 0; i < NumSymbol+1; i++) NumPow *= 10;
	if(Num > NumPow - 1) Num = NumPow - 1;

	for(i = NumSymbol - 1; i >= 0; i--)
	{
		A[i] = 0; Rate = 1;

		for(j = 0; j < i; j++) Rate *= 10;
		while(Num >= Rate){ Num = Num - Rate; A[i]++; }

		(*Str) = int_to_char(A[i]);
		Str++;
	}
}


/*******************************************************************************
* Function Name  : int_to_char
* Description    :
* Input          :
* Return         :
*******************************************************************************/
static __inline char int_to_char(uint8_t val)
{
	return (48 + val);
}
