﻿/*************************************************************************************
*
Description :  NeilScope3 Quick Menu buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define qMnuButtonsMAX  7

#define btnHight 		16
#define btnWidth 		100
#define btnSW			2
#define LeftLineBtn 	15
#define LowerBtn 		22
#define UpLineBtn		((LowerBtn - 4) + ((btnHight + btnSW) * (qMnuButtonsMAX + 1)))

void Change_Color_Buttons(void);
void Change_Color_Grid(void);
void GoAdvancedMenu(void);
void Auto_Power_OFF(void);
void Beep_ON_OFF(void);
void BackLight(void);
void ShowFFT_Freq(void);
void Change_Interpolation(void);


/* кнопка  ---------------------------------------------------------- */
btnINFO Color_Buttons =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 7,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Buttons Color",						// Текст
	Change_Color_Buttons					// указатель на функцию обработчик
};

/* кнопка  ---------------------------------------------------------- */
btnINFO Color_Grid =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 6,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Grid Color",							// Текст
	Change_Color_Grid						// указатель на функцию обработчик
};

/* ------------------------------------------- */
btnINFO btnAdvancedMenu =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 5,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Advansed...",								// Текст
	GoAdvancedMenu 							// указатель на функцию обработчик
};

/* кнопка сохранения ---------------------------------------------------------- */
btnINFO AutoPowerOFF =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 4,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Power OFF -",							// Текст
	Auto_Power_OFF							// указатель на функцию обработчик
};

/* кнопка бипера ---------------------------------------------------------- */
btnINFO BeepEN =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 3,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Beep OFF",								// Текст
	Beep_ON_OFF								// указатель на функцию обработчик
};

/* кнопка изменения подсведки -------------------------------------------------- */
btnINFO btnLIGHT_INFO =
{
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 2,		// Нижняя граница кнопки
	btnWidth,								// Ширина
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"BL MIN",								// Текст
	BackLight 								// указатель на функцию обработчик
};

/*  ------------------------ */
btnINFO btnShowFFT =
{
	LeftLineBtn,						// Левая граница кнопки
	LowerBtn + (btnHight + btnSW),		// Нижняя граница кнопки
	btnWidth,							// Ширина
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"Show Freq",						// Текст
	ShowFFT_Freq						// указатель на функцию обработчик
};

/* кнопка переключения интерполяции ------------------------------------------- */
btnINFO Interpolation =
{
	LeftLineBtn,						// Левая граница кнопки
	LowerBtn,							// Нижняя граница кнопки
	btnWidth,							// Ширина кнопки
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"i NONE",							// Текст
	Change_Interpolation 				// указатель на функцию обработчик
};

