/*************************************************************************************
*
Description :  NeilScope3 Channels Menu buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define ChannelButtonsMAX	5

#define btnHight 		16
#define btnWidth 		85
#define btnSW			2
#define LeftLineBtnA 	36
#define LeftLineBtnB 	36 + 75
#define LowerBtn 		22
#define UpLineBtn		((LowerBtn - 4) + ((btnHight + btnSW) * (ChannelButtonsMAX + 1)))


/* кнопка цвета текста канала А  -------------------------------------------- */
btnINFO IntrlCorrect_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 6,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Intrl Corr",							// Текст
	Intrlive_Correct
};


/* кнопка цвета текста канала А  -------------------------------------------- */
btnINFO TextColor_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 5,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Text Color",							// Текст
	Change_TextColor_CH_A
};


/* кнопка цвета канала А на экране ------------------------------------------ */
btnINFO Color_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 4,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Color CH A",							// Текст
	Change_COLOR_CH_A
};

/* кнопка автокоррекции нуля канала А ----------------------------------------------- */
btnINFO Auto_Correction_Zero_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 3,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Auto Corr",							// Текст
	Auto_CorrectZ_CH_A 						// указатель на функцию обработчик
};

/* кнопка коррекции нуля канала А ----------------------------------------------- */
btnINFO Correction_Zero_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 2,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Correction",							// Текст
	Correct_AnalogZero_CH_A 				// указатель на функцию обработчик
};

/* кнопка коррекции нуля канала А ----------------------------------------------- */
btnINFO AutoDiv_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn + btnHight + btnSW,			// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Auto Div",								// Текст
	AutoDivider_A 							// указатель на функцию обработчик
};

/* кнопка смещения канала А на экране ------------------------------------------ */
btnINFO Offset_A =
{
	LeftLineBtnA,							// Левая граница кнопки
	LowerBtn,								// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Offset A",								// Текст
	OFFSET_CH_A 
};





/* кнопка цвета текста канала B  -------------------------------------------- */
btnINFO TextColor_B =
{
	LeftLineBtnB,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 5,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Text Color",							// Текст
	Change_TextColor_CH_B
};


/* кнопка цвета канала B на экране ------------------------------------------ */
btnINFO Color_B =
{
	LeftLineBtnB,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 4,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Color CH B",							// Текст
	Change_COLOR_CH_B
};

/* кнопка автокоррекции нуля канала B ----------------------------------------------- */
btnINFO Auto_Correction_Zero_B =
{
	LeftLineBtnB,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 3,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Auto Corr",							// Текст
	Auto_CorrectZ_CH_B 						// указатель на функцию обработчик
};

/* кнопка коррекции нуля канала В ----------------------------------------------- */
btnINFO Correction_Zero_B =
{
	LeftLineBtnB,						// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 2,	// Нижняя граница кнопки
	btnWidth,							// Ширина кнопки
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"Correction",						// Текст
	Correct_AnalogZero_CH_B				// указатель на функцию обработчик
};

/* кнопка  ------------------------------------------ */
btnINFO AutoDiv_B =
{
	LeftLineBtnB,						// Левая граница кнопки
	LowerBtn + btnHight + btnSW,		// Нижняя граница кнопки
	btnWidth,							// Ширина кнопки
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"Auto Div",							// Текст
	AutoDivider_B
};

/* кнопка смещения канала В на экране ------------------------------------------ */
btnINFO Offset_B =
{
	LeftLineBtnB,						// Левая граница кнопки
	LowerBtn,							// Нижняя граница кнопки
	btnWidth,							// Ширина кнопки
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"Offset B",							// Текст
	OFFSET_CH_B 
};





//const btnINFO *Chennel_A_MenuButtons[ChannelMnuIndxMAX+1] =		 // массив указателей на кнопки меню
//{
//	&TextColor_A, &Color_A, &Auto_Correction_Zero_A, &Correction_Zero_A, &Offset_A
//};
//
//const btnINFO *Chennel_B_MenuButtons[ChannelMnuIndxMAX+1] =		 // массив указателей на кнопки меню
//{
//	&TextColor_B, &Color_B, &Auto_Correction_Zero_B, &Correction_Zero_B, &Offset_B
//};

