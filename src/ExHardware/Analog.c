/*************************************************************************************
*
Description :  NeilScope3 Analog control
Version     :  1.0.0
Date        :  09.05.2011
Author      :  Left Radio                          
Comments:  
*
**************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "Analog.h"
#include "EPM570_Registers.h"
#include "systick.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct 
{
	const float     Gain_Coeff;			   // коэффициент передачи аналогового тракта
	const uint8_t	CODE_74HC4051;         // код для записи в мультиплексор, старший бит отвечает за реле
	const char      *V_DIV_Text;		   // текст 
} DIV_INFO;

/* Private define ------------------------------------------------------------*/
#define AD8129_CH_A_ON     GPIOA->BSRR = GPIO_BSRR_BS11
#define AD8129_CH_B_ON     GPIOB->BSRR = GPIO_BSRR_BS0
#define AD8129_CH_A_OFF    GPIOA->BSRR = GPIO_BSRR_BR11
#define AD8129_CH_B_OFF    GPIOB->BSRR = GPIO_BSRR_BR0



/* Private macro -------------------------------------------------------------*/
/* макросы переключения реле, если (state == 0x80) то переключение в нижнее по схеме положение */
#define Relay_A(state)     ( (state) == 0x80 ? (GPIOC->BSRR = GPIO_BSRR_BR10) : (GPIOC->BSRR = GPIO_BSRR_BS10) )
#define Relay_B(state)     ( (state) == 0x80 ? (GPIOC->BSRR = GPIO_BSRR_BR11) : (GPIOC->BSRR = GPIO_BSRR_BS11) )



/* Private variables ---------------------------------------------------------*/
static const DIV_INFO  Div_Coeff_massive[12]	= {
	{10.0, 0x03, "10mV/Div"},  {5.0, 0x00, "20mV/Div"},    {2.0, 0x02, "50mV/Div"},
	{1.0, 0x01, "0.1 V/Div"},  {0.5, 0x06, "0.2 V/Div"},   {0.2, 0x04, "0.5 V/Div"},
	{0.1, 0x83, "1.0 V/Div"},  {0.05, 0x80, "2.0 V/Div"},  {0.02, 0x82, "5.0 V/Div"},
	{0.01, 0x81, "10 V/Div"},  {0.005, 0x86, "20 V/Div"},  {0.002, 0x84, "50 V/Div"}
};

static uint8_t gAnalogDivider_A_index = 0, gAnalogDivider_B_index = 0;
static FunctionalState AutoDividerState[2] = { DISABLE, DISABLE };

/* Private function prototypes -----------------------------------------------*/
static __inline int8_t Signal_Limit_Exceeded (int8_t *Data);

/* Private functions ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*******************************************************************************
* Function Name  : Control_AD8129
* Description    : Включаем или выключаем AD8921
* Input          : new_STATE - ENABLE/DISABLE
*                  Channel - ChannelA / ChannelB / Both Channels
* Output         : None
* Return         : None
*******************************************************************************/
void Control_AD8129(uint8_t new_STATE)
{
	/* если выключить ОУ */
	if(new_STATE == DISABLE)
	{
	    AD8129_CH_A_OFF;
		AD8129_CH_B_OFF;
		
		return;								  
	}
	
	/* если включить ОУ, то проверяем какие каналы сейчас включены */
	if(INFO_A.Mode.EN == RUN) AD8129_CH_A_ON;	   // если канал А запущен то включаем AD8129 канала А
	if(INFO_B.Mode.EN == RUN) AD8129_CH_B_ON;	   // если канал В запущен то включаем AD8129 канала В
}



/*******************************************************************************
* Function Name  : Change_AnalogDivider
* Description    : Изменение положения аналогового делителя 
* Input          : channel - ChannelA / ChannelB 
*                  divider_index - индекс делителя
*                  
* Output         : None
* Return         : Возвращает указатель на строку соответвующего делителя -
*                  - (char*)&Div_Coeff_massive[divider_index].V_DIV_Text[0]
*******************************************************************************/
char *Change_AnalogDivider(Channel_ID_TypeDef channel, uint8_t divider_index)
{
	uint8_t register_code = Div_Coeff_massive[divider_index].CODE_74HC4051 & 0x07;
	uint8_t relay_state   = Div_Coeff_massive[divider_index].CODE_74HC4051 & 0x80;

//	EPM570_extPin_1_DATA = EPM570_Read_Register(extPin_reg_1);

	if(channel == CHANNEL_A)
	{
		gAnalogDivider_A_index = divider_index;

		Relay_A(relay_state);
		
		/* очищаем текущее положение делителя, младшие 3 бита регистра extPin_reg_1 */	
	    EPM570_extPin_1_DATA &= (uint8_t)(~0x07);
	}
	else if(channel == CHANNEL_B)
	{
		gAnalogDivider_B_index = divider_index;

		Relay_B(relay_state);
		register_code = register_code << 3;

		/* очищаем текущее положение делителя, 3 бита (EPM570_extPin_1_DATA & ~0b00111000) регистра extPin_reg_1 */	
		EPM570_extPin_1_DATA &= (uint8_t)(~0x38);
	}
	else return 0;	

	/* пишем новое значение */
	EPM570_Write_Register(extPin_reg_1, EPM570_extPin_1_DATA | register_code);

	return (char*)&Div_Coeff_massive[divider_index].V_DIV_Text[0];
}


/*******************************************************************************
* Function Name  : Get_AnalogDivider_Index
* Description    : Получение текущего индекса делителя
* Input          : channel
* Output         : None
* Return         : Analog Divider Index
*******************************************************************************/
uint8_t Get_AnalogDivider_Index(Channel_ID_TypeDef channel)
{
	if(channel == CHANNEL_A)
	{
		return gAnalogDivider_A_index;
	}
	else if(channel == CHANNEL_B)
	{
		return gAnalogDivider_B_index;
	}
	else return 255;
}


/*******************************************************************************
* Function Name  : Get_AnalogDivider_Gain_Coeff
* Description    : Получение текущего коэффициента усилиения аналогового тракта
* Input          : divider_index - индекс делителя
*                   
* Output         : None
* Return         : Gain coefficient
*******************************************************************************/
float Get_AnalogDivider_Gain_Coeff(uint8_t divider_index)
{
	return Div_Coeff_massive[divider_index].Gain_Coeff;
}



/*******************************************************************************
* Function Name  : Get_Auto_AnalogDivider
* Description    :
* Input          : None
* Output         : None
* Return         : Gain Auto Divider State
*******************************************************************************/
FunctionalState Get_AutoDivider_State(Channel_ID_TypeDef CH)
{
	return AutoDividerState[CH];
}


/*******************************************************************************
* Function Name  : Set_AutoDivider_State
* Description    :
* Input          : New State
* Output         : None
* Return         : None
*******************************************************************************/
void Set_AutoDivider_State(Channel_ID_TypeDef CH, FunctionalState NewState)
{
	AutoDividerState[CH] = NewState;
}



/*******************************************************************************
* Function Name  : Analog_AutodividerMain
* Description    :
* Input          : New State
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState Analog_AutodividerMain(void)
{
	int8_t An_indx = Get_AnalogDivider_Index(pINFO->Mode.ID);

	/* Проверяем "зашкал" и переключаем делитель при необходимости */
	if(Get_AutoDivider_State(pINFO->Mode.ID) == ENABLE)
	{
		An_indx = An_indx + Signal_Limit_Exceeded(&pINFO->DATA[0]);
		if((An_indx != pINFO->AD_Type.Analog.Div) && (An_indx >= 0))
		{
			pINFO->AD_Type.Analog.Div = An_indx;
			Change_AnalogDivider(pINFO->Mode.ID, pINFO->AD_Type.Analog.Div);
			delay_ms(10);
			return ENABLE;
		}
	}

	return DISABLE;
}


/*******************************************************************************
* Function Name  : Clear_OLD_Data_MIN_MAX
* Description    : очистка, режим максимумов/минимумов
* Input          : None
* Return         : None
*******************************************************************************/
static __inline int8_t Signal_Limit_Exceeded (int8_t *Data)
{
	uint16_t i;
	int8_t max = -127, min = 127;
	uint8_t diff;

	for(i = 50; i < 350; i++)
	{
		if(*Data > max) max = *Data;
		else if(*Data < min) min = *Data;
		Data++;
	}

	diff = (uint8_t)(max - min);

	if( (max > 120) || (min < -120) || (diff > 240) ) return +1;
	else if(diff < 50) return -1;
	else return 0;
}
