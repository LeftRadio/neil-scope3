/*************************************************************************************
*
Description :  NeilScope3 Quick Menu buttons description
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define _ADVANCED_BUTTONS_MAX_		  	6

#define btnHight 						16
#define btnWidth 						110
#define btnSW							2
#define advLeftLineBtn 					120
#define LowerBtn 						22
#define UpLineBtn						((LowerBtn - 4) + ((btnHight + btnSW) * (_ADVANCED_BUTTONS_MAX_ + 1)))


void AdvancedMenu_Closed(void);
void Perform_Erase_EEPROM(void);
void Save_pref(void);
void Select_I2C_ADDR(void);
void Select_I2C_GPIO(void);
void Select_HostMode(void);

/* ---------------------------------------------------------- */
btnINFO btnClose =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 6,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Close",								// Текст
	AdvancedMenu_Closed 					// указатель на функцию обработчик
};

/* ---------------------------------------------------------- */
btnINFO btnEraseEeprom =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 5,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Erase EEprom",							// Текст
	Perform_Erase_EEPROM 					// указатель на функцию обработчик
};

/* ---------------------------------------------------------- */
btnINFO btnSave =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 4,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"Save all sett",						// Текст
	Save_pref 								// указатель на функцию обработчик
};


/* ---------------------------------------------------------- */
btnINFO btnEmpty =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 3,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	" ----- ",								// Текст
	(void*)0 								// указатель на функцию обработчик
};


/* ---------------------------------------------------------- */
btnINFO btnI2C_ADDR =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 2,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	"0x00",									// Текст
	Select_I2C_ADDR 						// указатель на функцию обработчик
};

/* ---------------------------------------------------------- */
#define btnI2C_Txt_Max 		3
const char btnI2C_GPIO_Texts[btnI2C_Txt_Max][12] = {
    "I2C IO OFF",
    "PCA9675",
    "MAX7320",
};

btnINFO btnI2C_GPIO =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn + (btnHight + btnSW) * 1,		// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	(char*)&btnI2C_GPIO_Texts[0],			// Текст
	Select_I2C_GPIO 						// указатель на функцию обработчик
};


/* ---------------------------------------------------------- */
const char btnHostMode_Texts[5][19] = {
    "Host OFF",
    "ESP Bridge",
    "Host CP2102",
    "Host ESP",
    "ESP Boot",
};

btnINFO btnHostMode =
{
	advLeftLineBtn,							// Левая граница кнопки
	LowerBtn,								// Нижняя граница кнопки
	btnWidth,								// Ширина кнопки
	btnHight,								// Высота кнопки
	btn_ForeColor,							// Цвет фона
	btn_activeForeColor,					// Цвет фона когда кнопка активна
	btn_FontColor,							// Цвет шрифта когда кнопка активна
	(char*)&btnHostMode_Texts[0],			// Текст
	Select_HostMode							// указатель на функцию обработчик
};


