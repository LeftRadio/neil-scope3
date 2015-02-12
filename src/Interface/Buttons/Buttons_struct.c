/*************************************************************************************
*
Description :  NeilScope3 User Interface buttons structures
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#define gInterfaceButtonsMAX  9

#define btnUpper_line  225
#define btnLower_line  1
#define btnHight       16

#define btnBackColor		
#define CHbtnBackColor		

//uint8_t btnTIME_SCALE_position = 0;




// Information for button < MENU >
btnINFO btnMENU =
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


// Information for button < VDIV_A >
btnINFO btnVDIV_A =
{
	41,	                	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    73,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_CH_A_ForeColor,
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"50 V/Div",		   		// Текст
	&change_Div_A
};



// Information for button < VDIV_B >
btnINFO btnVDIV_B =
{
	116,	            	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    73,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_CH_B_ForeColor,	        // Цвет
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"50 V/Div",   	   		// Текст
	&change_Div_B
};



// Information for button < SWEEP >
btnINFO btnSWEEP =
{
	191,	            	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    75,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"250ns/Div",		   	// Текст
	&change_Sweep
};



// Information for button < TRIG >
btnINFO btnSWEEP_MODE =
{
	268,	            	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    40,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"NONE",		   			// Текст
	&change_Sweep_Mode
};



// Information for button < SWEEP_MODE >
btnINFO btnTRIG =
{
	310,	            	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    40,		           		// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"TRIG",   				// Текст
	&change_Trigg_Mode
};



// Information for button < MEASURMENTS >
btnINFO btnMEASURMENTS =
{
	352,	             	// Левая граница кнопки
	btnLower_line,	    	// Нижняя граница кнопки
    45,		          		// Ширина
	btnHight,         		// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"MEAS",	   			    // Текст
	Hide_Show_Meas
};


// Information for button < RUN/HOLD >
btnINFO btnRUN_HOLD =
{
	1,	                 	// Левая граница кнопки
	btnUpper_line,      	// Нижняя граница кнопки
    40,		           		// Ширина
	btnHight,           	// Высота кнопки
	btn_ForeColor,			// Цвет фона
	btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"RUN",   			  	// Текст
	&RUN_HOLD
};

// Information for button < CHA_AC_DC >
btnINFO btnCHA_AC_DC =
{
	43,	                	// Левая граница кнопки
	btnUpper_line,      	// Нижняя граница кнопки
	30,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_CH_A_ForeColor,		// Цвет
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"AC",  					// Текст
	&ON_OFF_CHANNEL_A
};

// Information for button < CHB_AC_DC >
btnINFO btnCHB_AC_DC =
{
	75,	                	// Левая граница кнопки
	btnUpper_line,      	// Нижняя граница кнопки
    30,		            	// Ширина
	btnHight,           	// Высота кнопки
	btn_CH_B_ForeColor,	    // Цвет
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"AC",		   			// Текст
	&ON_OFF_CHANNEL_B
};


// Information for < TIME SCALE >
btnINFO btnTIME_SCALE =
{
	110,	            	// Левая граница
	btnUpper_line - 1,     	// Нижняя граница
    190,		        	// Ширина
	btnHight - 1,          	// Высота кнопки
	LightBlack,			   	// Цвет
    btn_activeForeColor,	// Цвет фона когда кнопка активна
	btn_FontColor,			// Цвет шрифта когда кнопка активна
	"",   		    		// Текст
	&change_TIME_SCALE
};





