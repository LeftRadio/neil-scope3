/*************************************************************************************
*
Description :  NeilScope3 Measurments buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define MeasButtonsMAX	2

#define btnHeight 		15
#define btnWidht 		90
#define btnSW			2
#define LeftLineBtn 	306
#define LowerBtn 		22
#define UpLineBtn		((LowerBtn - 3) + ((btnHeight + btnSW) * (MeasButtonsMAX + 1)))


/* кнопка изменения верхней границы окна или уровень триггера ---------------- */
btnINFO btnMeasCursor1 =
{
	306,					// Левая граница кнопки
	56,						// Нижняя граница кнопки
	btnWidht,				// Ширина кнопки
	btnHeight,				// Высота кнопки
	btn_ForeColor,	  		// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"Cursor1",				// Текст
	ChangeMeasCursor_1
};

/* кнопка изменения нижней границы окна --------------------------------------- */
btnINFO btnMeasCursor2 =
{
	306,					// Левая граница кнопки
	39,						// Нижняя граница кнопки
	btnWidht,				// Ширина кнопки
	btnHeight,				// Высота кнопки
	btn_ForeColor,	  		// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"Cursor2",				// Текст
	ChangeMeasCursor_2
};



/* кнопка ---------------- */
btnINFO btnMeasMode =
{
	306,					// Левая граница кнопки
	22,						// Нижняя граница кнопки
	btnWidht,				// Ширина кнопки
	btnHeight,				// Высота кнопки
	btn_ForeColor,	  		// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"Mode",					// Текст
	ChangeMeasMode
};

















