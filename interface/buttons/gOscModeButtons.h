/*************************************************************************************
*
Description :  NeilScope3 User Interface buttons structures
Version     :  1.0.0
Date        :  7.12.2011
Author      :  Left Radio
Comments    :
*
**************************************************************************************/

#define btnUpper_line  225
#define btnLower_line  1
#define btnHight       16

#define btnBackColor
#define CHbtnBackColor

//uint8_t btnTIME_SCALE_position = 0;


// Information for button < MENU >
btnINFO btnMENU =
{
	1,	                	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    38,		            	// ������ ������
	btnHight,           	// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"MENU",		   			// �����
	&qMenu
};


// Information for button < VDIV_A >
btnINFO btnVDIV_A =
{
	41,	                	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    73,		            	// ������
	btnHight,           	// ������ ������
	btn_CH_A_ForeColor,
    btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"50 V/Div",		   		// �����
	&change_Div_A
};



// Information for button < VDIV_B >
btnINFO btnVDIV_B =
{
	116,	            	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    73,		            	// ������
	btnHight,           	// ������ ������
	btn_CH_B_ForeColor,	        // ����
    btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"50 V/Div",   	   		// �����
	&change_Div_B
};



// Information for button < SWEEP >
btnINFO btnSWEEP =
{
	191,	            	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    75,		            	// ������
	btnHight,           	// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"250ns/Div",		   	// �����
	&change_Sweep
};



// Information for button < TRIG >
btnINFO btnSWEEP_MODE =
{
	268,	            	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    40,		            	// ������
	btnHight,           	// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"NONE",		   			// �����
	&change_Sweep_Mode
};



// Information for button < SWEEP_MODE >
btnINFO btnTRIG =
{
	310,	            	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    40,		           		// ������
	btnHight,           	// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"TRIG",   				// �����
	&change_Trigg_Mode
};



// Information for button < MEASURMENTS >
btnINFO btnMEASURMENTS =
{
	352,	             	// ����� ������� ������
	btnLower_line,	    	// ������ ������� ������
    45,		          		// ������
	btnHight,         		// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"MEAS",	   			    // �����
	Hide_Show_Meas
};


// Information for button < RUN/HOLD >
btnINFO btnRUN_HOLD =
{
	1,	                 	// ����� ������� ������
	btnUpper_line,      	// ������ ������� ������
    40,		           		// ������
	btnHight,           	// ������ ������
	btn_ForeColor,			// ���� ����
	btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"RUN",   			  	// �����
	&RUN_HOLD
};

// Information for button < CHA_AC_DC >
btnINFO btnCHA_AC_DC =
{
	43,	                	// ����� ������� ������
	btnUpper_line,      	// ������ ������� ������
	30,		            	// ������
	btnHight,           	// ������ ������
	btn_CH_A_ForeColor,		// ����
    btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"AC",  					// �����
	&ON_OFF_CHANNEL_A
};

// Information for button < CHB_AC_DC >
btnINFO btnCHB_AC_DC =
{
	75,	                	// ����� ������� ������
	btnUpper_line,      	// ������ ������� ������
    30,		            	// ������
	btnHight,           	// ������ ������
	btn_CH_B_ForeColor,	    // ����
    btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"AC",		   			// �����
	&ON_OFF_CHANNEL_B
};


// Information for < TIME SCALE >
btnINFO btnTIME_SCALE =
{
	110,	            	// ����� �������
	btnUpper_line - 1,     	// ������ �������
    190,		        	// ������
	btnHight - 1,          	// ������ ������
	LightBlack,			   	// ����
    btn_activeForeColor,	// ���� ���� ����� ������ �������
	btn_FontColor,			// ���� ������ ����� ������ �������
	"",   		    		// �����
	&change_TIME_SCALE
};





