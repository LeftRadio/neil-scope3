/*************************************************************************************
*
Description :  NeilScope3 User Interface buttons structures
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio
Comments    :
*
**************************************************************************************/



// Information for button < MENU >
btnINFO btnLMENU =
{
	1,	                	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    38,		            	// Ширина кнопки
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"MENU",		   			// Текст
	&qMenu
};


// Information for button < SWEEP >
btnINFO btnLSWEEP =
{
	41,	            		// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    75,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"100MSps",		   		// Текст
	&change_Sweep
};


// Information for button < SWEEP_MODE >
btnINFO btnLTRIG =
{
	117,	            	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    40,		           		// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"TRIG",   				// Текст
	&change_Trigg_Mode
};


// Information for button < RUN/HOLD >
btnINFO btnLSTART =
{
	158,                 	// Левая граница кнопки
	btnUpper_line,      	// Нижняя граница кнопки
    50,		           		// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"START",   			  	// Текст
	&RUN_HOLD
};


// Information for button < MEASURMENTS >
btnINFO btnLCURSOR =
{
	209,	             	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    45,		          		// Ширина
	btnHight,         		// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"CURS",	   			    // Текст
	Hide_Show_Meas
};



// Information for < TIME SCALE >
btnINFO btnLTIME_SCALE =
{
	255,	            	// Левая граница
	btnUpper_line - 1,     	// Нижняя граница
    144,		        	// Ширина
	btnHight - 1,          	// Высота кнопки
	LightBlack,			   	// Цвет
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"",   		    		// Текст
	change_TIME_SCALE
};





