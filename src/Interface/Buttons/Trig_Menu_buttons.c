/*************************************************************************************
*
Description :  NeilScope3 Trigg Menu buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define TrigButtonsMAX	5

#define btnHight 		16
#define btnWidth 		80
#define btnSW			2
#define LeftLineBtn 	291
#define LowerBtn 		22
#define UpLineBtn		((LowerBtn - 4) + ((btnHight + btnSW) * (TrigButtonsMAX + 1)))


/* кнопка  -------------------------------------- */
btnINFO btnTrigg_ShowInfo = {
	LeftLineBtn,						// Левая граница кнопки
	LowerBtn + (btnHight + btnSW)*5,	// Нижняя граница кнопки
	btnWidth,							// Ширина кнопки
	btnHight,							// Высота кнопки
	btn_ForeColor,						// Цвет фона
	btn_activeForeColor,				// Цвет фона когда кнопка активна
	btn_FontColor,						// Цвет шрифта когда кнопка активна
	"show Info",						// Текст
	Hide_Show_triggInfo					// указатель на функцию обработки нажатия
};


/* кнопка  -------------------------------------- */
btnINFO btnTrigg_Position_X = {
	LeftLineBtn,					// Левая граница кнопки
	22 + (btnHight + btnSW)*4,		// Нижняя граница кнопки
	btnWidth,						// Ширина кнопки
	btnHight,						// Высота кнопки
	btn_ForeColor,					// Цвет фона
	btn_activeForeColor,			// Цвет фона когда кнопка активна
	btn_FontColor,					// Цвет шрифта когда кнопка активна
	"Pos X",						// Текст
	Trigg_Position_X				// указатель на функцию обработки нажатия
};

/* кнопка изменения нижней границы окна --------------------------------------- */
btnINFO btnLow_Level_W = {
	LeftLineBtn,					// Левая граница кнопки
	22 + (btnHight + btnSW)*3,		// Нижняя граница кнопки
    btnWidth,						// Ширина кнопки
	btnHight,						// Высота кнопки
	btn_ForeColor,					// Цвет фона
	btn_activeForeColor,			// Цвет фона когда кнопка активна
	btn_FontColor,					// Цвет шрифта когда кнопка активна
	"L  Level",						// Текст
	Change_L_CursorLevel 
};

/* кнопка изменения верхней границы окна или уровень триггера ---------------- */
btnINFO btnHeight_Level_W =	{
	LeftLineBtn,					// Левая граница кнопки
	22 + (btnHight + btnSW)*2,		// Нижняя граница кнопки
  	btnWidth,						// Ширина кнопки
	btnHight,						// Высота кнопки
	btn_ForeColor,					// Цвет фона
	btn_activeForeColor,			// Цвет фона когда кнопка активна
	btn_FontColor,					// Цвет шрифта когда кнопка активна
	"H  Level",						// Текст
	Change_H_CursorLevel 
};

/* кнопка выбора фронт/спад ------------------------------------------------- */
btnINFO btnTrigg_Mode_sync =
{
	LeftLineBtn,	     			// Левая граница кнопки
	22 + (btnHight + btnSW),		// Нижняя граница кнопки
    btnWidth,						// Ширина кнопки
	btnHight,           			// Высота кнопки
	btn_ForeColor,	  				// Цвет фона
	btn_activeForeColor,	 		// Цвет фона когда кнопка активна
	btn_FontColor,		     		// Цвет шрифта когда кнопка активна
	"Rise", 	  	     			// Текст
	Change_Trigg_ModeSync 
};

/* кнопка выбора канала для синхронизации -------------------------------------- */
btnINFO btnSync_Sourse = 
{
	LeftLineBtn,					// Левая граница кнопки
	22,								// Нижняя граница кнопки
	btnWidth,						// Ширина кнопки
	btnHight,						// Высота кнопки
	btn_ForeColor,					// Цвет фона
	btn_activeForeColor,			// Цвет фона когда кнопка активна
	btn_FontColor,					// Цвет шрифта когда кнопка активна
	"Sync A",						// Текст
	Change_SyncSourse 
};











