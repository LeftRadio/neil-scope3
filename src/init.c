/**
  ******************************************************************************
  * @file    init.c 
  * @author  LeftRadio
  * @version V1.0.0
  * @date    25.12.2012
  * @brief   Main Init Peripherals.
  ******************************************************************************
**/ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "defines.h"
#include "init.h"
#include "systick.h"
#include "main.h"
#include "init.h"
#include "Settings.h"
#include "HX8352_Library.h"
#include "colors 5-6-5.h"
#include "EPM570.h"
#include "Analog.h"
#include "User_Interface.h"
#include "RTC.h"
#include "IQueue.h"
#include "i2c_gpio.h"
#include "pca9675.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed             	100000
#define GPIO_Pin_7_0          	(uint16_t)0x00FF
#define GPIO_Pin_8_15         	(uint16_t)0xFF00

#define SYSTICK_RELOAD_VAL 		72000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t IN_HostData[CMD_MAX_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void ExHardware_Init_ERROR(void);


/* Functions ----------------------------------------------------------------*/

/**
 * @brief  Init_GPIO
 * @param  None
 * @retval None
 */
static void Init_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enabling clock for GPIO A,B,C and */
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;  
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;  
	
	//AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
#ifndef __SWD_DEBUG__
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	/* Init Relay, Beep and ON_A signal */
	GPIOC->BSRR = GPIO_Pin_15;		// "защелкиваем" сигнал кнопки питания
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;	 // RL_CA0, RL_CB0, Beep, ON_A
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* Init ON_B signal as Input PU */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Init /PDN for AD8129 */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_11;	       // PDN channel A, PORTA.11
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0;	       // PDN channel B, PORTB.0
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Init USB host active input */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8;	       // HostActive PORTA.8
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



#ifndef __SWD_DEBUG__
	/*  Init Inerlive & HC573_LE GPIO */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

/**
 * @brief  Timer1, used for ADC sampling and FPS counting
 * @param  None
 * @retval None
 */
static void Timer1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;		// разрешаем татирование модуля
	TIM1->PSC = 72 - 1;						// предделитель
	TIM1->ARR = 1000;						// значение до которого будет считать таймер
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->CR1 |=  TIM_CR1_CEN | TIM_CR1_ARPE;	// разрешаем работу и автоперезагрузку

	/* устанавливаем приоритет и разрешаем прерывания */
	NVIC_EnableIRQ(TIM1_UP_IRQn);
	NVIC_SetPriority(TIM1_UP_IRQn, 10);
}

/**
 * @brief  Timer2, used for buttons read interrupt
 * @param  None
 * @retval None
 */
static void Timer2_init(void)
{ 	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;			// разрешаем татирование модуля
	TIM2->PSC = 10000 - 1;						// устанавливаем предделитель
	TIM2->ARR = 1000;							// значение до которого будет считать таймер
	TIM2->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;		// разрешаем работу и автоперезагрузку
	TIM2->DIER = TIM_DIER_UIE;					// прерывание по обновлению
	
	/* устанавливаем приоритет и разрешаем прерывания */
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 14);	
}

/**
 * @brief  Timer3, used for generate PWM (offset analog channels)
 * @param  None
 * @retval None
 */
static void Timer3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;

	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  	/* PWM signals init , инициализация портов под выхода ШИМ */           
    GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8 | GPIO_Pin_9;	  
    GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;			  // альтернативная функция, выход
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1024;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    /* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 320;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 320;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief  Beep_Timer, used for 'beep' sounds
 * @param  None
 * @retval None
 */
static void Beep_Timer4_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;			// разрешаем татирование модуля
	TIM4->PSC = 4 - 1;							// устанавливаем предделитель
	TIM4->ARR = 24000;							// значение до которого будет считать таймер
	TIM4->CR1 = TIM_CR1_ARPE;					// разрешаем автоперезагрузку
	TIM4->DIER = TIM_DIER_UIE;					// прерывание по обновлению

	/* устанавливаем приоритет и разрешаем прерывания */
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, 12);
}

/**
 * @brief  I2C, used for EEPROM
 * @param  None
 * @retval None
 */
static void I2C_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable I2C1 Clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Initialise I2C1 GPIO port */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Initialise I2C */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x01;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

	/* Enables the specified I2C peripheral. */
	I2C_Cmd(I2C1, ENABLE);

	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

	/* Enables the error I2C interupts */
	I2C1->CR2 |= I2C_CR2_ITERREN;

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	// устанавливаем приоритет и разрешаем прерывания I2C
	NVIC_EnableIRQ(I2C1_ER_IRQn);
	NVIC_SetPriority(I2C1_ER_IRQn, 13);
}

/**
 * @brief  ADC, used for measurment VBatt
 * @param  None
 * @retval None
 */
static void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* Enable ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channels configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_239Cycles5);
		
//	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 reset calibration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* Start ADC1 calibration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

    // пока запрещаем прерывания и устанавливаем приоритет  
    NVIC_DisableIRQ(ADC1_2_IRQn);
    NVIC_SetPriority(ADC1_2_IRQn, 15);
}

/**
 * @brief  USART, used for host comunication
 * @param  None
 * @retval None
 */
static void USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* USART resources configuration (Clock, GPIO pins and USART/DMA registers) */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART configured as follow:
	        - BaudRate = 921600 baud
	        - Word Length = 8 Bits
	        - One Stop Bit
	        - No parity
	        - Hardware flow control disabled (RTS and CTS signals)
	        - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 921600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* Enable USART1 DMA Rx request */
	USART_DMACmd(USART1, USART_DMAReq_Rx , ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);		/* Enable the USART1 */

	/* Enable USART1 RX DMA1 Channel */
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

/**
 * @brief  USART_DMA, used for host comunication, transfer data
 * @param  None
 * @retval None
 */
static void USART_DMA_DefaultConfiguration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	DMA_DeInit(DMA1_Channel5);
	DMA1_Channel5->CCR &= ~DMA_CCR5_EN; 							// Disable
	DMA1_Channel5->CPAR = (uint32_t)&USART1->DR; 					// Periph addr
	DMA1_Channel5->CMAR = (uint32_t)&(Host_GetIQueue(0)->Data[0]);	// Memory addr
	DMA1_Channel5->CCR &= ~DMA_CCR5_DIR; 							// Periph -> Memory
	DMA1_Channel5->CNDTR = (uint32_t)0x03;							// Data cnt
	DMA1_Channel5->CCR &= ~DMA_CCR1_PINC; 							// Perip addr not increment
	DMA1_Channel5->CCR |= DMA_CCR1_MINC; 							// Memory addr increment
	DMA1_Channel5->CCR &= ~DMA_CCR1_PSIZE; 							// Periph data 8 bit
	DMA1_Channel5->CCR &= ~DMA_CCR1_MSIZE; 							// Memory data 8 bit
	DMA1_Channel5->CCR &= ~DMA_CCR1_CIRC; 							// Not circular mode
	DMA1_Channel5->CCR |= DMA_Priority_VeryHigh; 					// Very High priority

	USART_Config();

	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
}

/**
 * @brief  LCD_PinsInit, init GPIO for LCD comunication
 * @param  None
 * @retval None
 */
static void LCD_PinsInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* LCD data port, PORTA.0-PORTA.7 Push-pull выход 50МГц */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_7_0;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	  
	/* LCD command signals */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	/* Инициализация внутри граф. библиотеки - Graphics Library HX8352.lib */
	set_LCD_DATAPort_Init(GPIOA, GPIO_Pin_7_0);
	
	set_RES_LCD(GPIOC, GPIO_Pin_4);		    // Сигнал сброса контроллера ЖК, PC.4
	set_RS_LCD(GPIOC, GPIO_Pin_1);		    // Сигнал RS, нужен для записи регистров, PC.1
	set_WR_LCD(GPIOC, GPIO_Pin_2);		    // Строб записи, PC.2
	set_RD_LCD(GPIOC, GPIO_Pin_12);		    // Не используется, назначаем на любой свободный вывод, например PC.12
	set_CS_LCD(GPIOC, GPIO_Pin_0);		    // Сигнал Chip Select, PC.0

#if defined (__LCD_HC573__) && !defined(__SWD_DEBUG__)
	set_HC573_LE_LCD(GPIOA, GPIO_Pin_14);
#endif
}

/**
 * @brief  EPM570_Init, init GPIO for EPM570 comunication,
 * 		   verify and write default values to EPM registers
 * @param  None
 * @retval None
 */
static void EPM570_Init(void)
{
	EPM_ErrorStatus regInit;
	uint16_t Xstr;

	/* Инициализация выводов МК под связь с EPM570 */
	EPM570_Signals_Init();

	/* изменяем шрифт на timesNewRoman12 и цвет на красный */
	LCD_SetFont(&timesNewRoman12ptFontInfo);

	/* EPM570 Registers init*/
	regInit = EPM570_Registers_Init();
	if(regInit != eSUCCESS)
	{
		if(regInit == ERROR_D) LCD_PutColorStrig(20, 170, 0, "ERROR write/read EPM570 Decimatin register...", Red);
		else if(regInit == ERROR_N) LCD_PutColorStrig(20, 170, 0, "ERROR write/read EPM570 NumSamples register...", Red);
		ExHardware_Init_ERROR();
	}

	Xstr = LCD_PutColorStrig(20, 170, 0, "SUCCESSFUL ", LightGreen);
	LCD_PutColorStrig(Xstr, 170, 0, "write/read EPM570 registers...", White);

	delay_ms(1);
	EPM570_Set_numPoints(20); 

	LCD_SetFont(&timesNewRoman12ptFontInfo);

	if(EPM570_SRAM_Write() == STOP)
	{
		LCD_PutColorStrig(20, 150, 0, "ERROR write cycle SRAM...", Red);
		ExHardware_Init_ERROR();
	}   

	EPM570_SRAM_Read();
	LCD_SetFont(&timesNewRoman12ptFontInfo);
	Xstr = LCD_PutColorStrig(20, 150, 0, "SUCCESSFUL ", LightGreen);
	LCD_PutColorStrig(Xstr, 150, 0, "write/read cycle SRAM...", White);
}

/**
 * @brief  ExHardware_Init_ERROR
 * @param  None
 * @retval None
 */
static void ExHardware_Init_ERROR(void)
{
	while(1)
	{
		Beep_Start();
		EPM570_Set_BackLight(0);
		delay_ms(1000);
		EPM570_Set_BackLight(1);
		delay_ms(1000);
	}
}

/**
 * @brief  Global_Init
 * @param  None
 * @retval None
 */
void Global_Init(void)
{
	Init_GPIO();
	RTC_User_Init();
	delay_init();

	ADC_Configuration();			// ADC configuration
	Timer1_init();
	Timer2_init();
	Timer3_init();
	Beep_Timer4_Init();
	I2C_Configuration();			// I2C configuration

	Host_IQueue_Initialization();
	USART_DMA_DefaultConfiguration();

//	if(ON_OFF_button == 0) GPIOC->BRR = GPIO_Pin_15;
}

/**
 * @brief  External_Peripheral_Init
 * @param  None
 * @retval None
 */
void External_Peripheral_Init(void)
{  
	/* LCD pins & controller Init */
	LCD_PinsInit();    
	LCD_Init();  
	
	/* Set bit color (DataWidth, NumberDepthBits) */
#ifdef __LCD_16_BIT__
	LCD_Set_DataWidth_ColorBitDepth(8, 16);		// 16 bit color
#elif defined(__LCD_18_BIT__)
	LCD_Set_DataWidth_ColorBitDepth(8, 18);		// 18 bit color
#elif defined(__LCD_HC573__)
	LCD_Set_DataWidth_ColorBitDepth(16, 18);	// HC573
#endif

	/* Set "Album" mode LCD, clear */
	LCD_Change_Direction(0);
	LCD_FillScreen(Black);
	LCD_SetBackColor(Black);
	
	/* Set Font and print verification peripheral message */
	LCD_SetFont(&timesNewRoman12ptFontInfo);
	LCD_PutColorStrig(75, 200, 0, "VERIFICATION PERIPHERAL", LightGray3);
	delay_ms(10);
	
	/* Инициализируем порты под связь с EPM570 и инициализируем регистры ПЛИС */
	EPM570_Init();

	/* Инициализируем аналоговые делители в начальное положение, 
	   Divider_Position_MAX задефайнен в Analog.h */
	Change_AnalogDivider(CHANNEL_A, Divider_Position_MAX);	
	Change_AnalogDivider(CHANNEL_B, Divider_Position_MAX);
	
	Beep_Start();
	delay_ms(500);

}

/**
 * @brief  Start_Bootloader
 * @param  None
 * @retval None
 */
void Start_Bootloader(void)
{
	uint16_t X = 0;
	const char *str = "Start Bootloader...";

	FLASH_Unlock();
	FLASH_ErasePage(0x08020000-0x200);
	FLASH_ProgramWord(0x08020000-0x200, 0xA5FDDF5A);
	FLASH_Lock();

	LCD_ClearArea(0, 0, 399, 239, LightBlack);
	LCD_SetFont(&arialUnicodeMS_16ptFontInfo);
	LCD_Strig_PointsWidht(&X, (char*)str);
	LCD_PutColorStrig((400 - X) >> 1, 112, 1, (char*)str, LightBlue1);

	delay_ms(100);

	/* Reset and jump to bootloader */
	NVIC_SystemReset();
}





/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
