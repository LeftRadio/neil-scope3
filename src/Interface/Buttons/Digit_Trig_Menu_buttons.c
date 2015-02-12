/*************************************************************************************
*
Description :  NeilScope3 Digit Trigg Menu buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define DigitTrigButtonsMAX		15

   //316
#define btnNumHeight			18
#define btnNumWidth				18
#define btnSW					2
#define LeftLineBtn 			(395 - ((btnNumWidth + btnSW) * 8))
#define LowerBtn 				(200 - (btnNumHeight * 2))
//#define UpLineBtn				((LowerBtn - 4) + ((btnNumHeight + btnSW) * 2))


/* Digit_Condition_Num -------------------------------------------------------------------------------- */

/* кнопка --------------------------------------- */
btnINFO btnCondDigit_0 = {
	LeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),		// Нижняя граница кнопки
	btnNumWidth,							// Ширина кнопки
	btnNumHeight,							// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"0",									// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_1 = {
	LeftLineBtn + (btnNumWidth + btnSW),	// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),		// Нижняя граница кнопки
	btnNumWidth,							// Ширина кнопки
	btnNumHeight,							// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"0",									// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
}; 


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_2 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 2),				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_3 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 3),				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_4 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 4),				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_5 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 5) ,				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu					// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_6 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 6),				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnCondDigit_7 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 7),				// Левая граница кнопки
	LowerBtn + (btnNumHeight + btnSW),						// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"0",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};




/* Digit_Difference_Num -------------------------------------------------------------------------------- */

/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_0 = {
	LeftLineBtn,											// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_1 = {
	LeftLineBtn + btnNumWidth + btnSW,						// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_2 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 2),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_3 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 3),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_4 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 4),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_5 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 5),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_6 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 6),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};


/* кнопка --------------------------------------- */
btnINFO btnDiffDigit_7 = {
	LeftLineBtn + ((btnNumWidth + btnSW) * 7),				// Левая граница кнопки
	LowerBtn,												// Нижняя граница кнопки
	btnNumWidth,											// Ширина кнопки
	btnNumHeight,											// Высота кнопки
	btn_ForeColor,											// Цвет фона
	btn_activeForeColor,									// Цвет фона когда кнопка активна
	btn_FontColor,											// Цвет шрифта когда кнопка активна
	"-",													// Текст
	Change_Digit_Trig_Menu									// указатель на функцию обработки нажатия
};




