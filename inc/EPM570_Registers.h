/*************************************************************************************
*
Description :  NeilScope3 EPM570 Registers header
Version     :  1.0.0
Date        :  25.04.2012
Author      :  Left Radio                          
Comments    :  
*
**************************************************************************************/

#ifndef __EPM570_REGISTERS_H
#define __EPM570_REGISTERS_H 

/* Includes ------------------------------------------------------------------*/	   
#include "stm32f10x_gpio.h"

/* Exported typedef -----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

/** Управляющие сигналы EPM570 и битовая маска **/
#define EPM570_portMask   				(uint16_t)0x00FF

#define EPM570_WR_set     				GPIOB->BSRR=GPIO_BSRR_BS1
#define EPM570_WR_clr     				GPIOB->BSRR=GPIO_BSRR_BR1
#define EPM570_RD_set     				GPIOC->BSRR=GPIO_BSRR_BS7
#define EPM570_RD_clr     				GPIOC->BSRR=GPIO_BSRR_BR7
#define EPM570_RS_set     				GPIOC->BSRR=GPIO_BSRR_BS6
#define EPM570_RS_clr     				GPIOC->BSRR=GPIO_BSRR_BR6

#define GPIO_Pin_7_0      				(uint16_t)0x00FF
#define GPIO_Pin_8_15     				(uint16_t)0xFF00

#define EPM570_Write_Ready              (GPIOC->IDR & GPIO_Pin_5)

/** Режимы порта - вход/выход **/
#define IN                            	GPIO_Mode_IPU
#define OUT     					  	GPIO_Mode_Out_PP


#define	SetEPM570Reg(addres)          	GPIOB->ODR = (GPIOB->IDR & EPM570_portMask) | (addres << 8) 
//#define ResetPortOUT()				  	GPIOB->ODR = (GPIOB->IDR & EPM570_portMask) | 0xFF00

/** Адреса регистров EPM570 **/
#define Decim_Low        				(uint8_t)0
#define Decim_High0      				(uint8_t)1
#define Decim_High1      				(uint8_t)2
#define Trigger_UP       				(uint8_t)3
#define Trigger_Down     				(uint8_t)4
#define WIN_DATA_Low     				(uint8_t)5
#define WIN_DATA_High0   				(uint8_t)6
#define WIN_DATA_High1   				(uint8_t)7
#define cnfPin           				(uint8_t)8
#define IN_KEY           				(uint8_t)9
#define Del              				(uint8_t)10
#define extPin_reg_0     				(uint8_t)11
#define extPin_reg_1     				(uint8_t)12
#define Write_Control    				(uint8_t)13
#define SRAM_DATA        				(uint8_t)15
#define ExtPin_B        				(uint8_t)16


/** внутренние сигналы ПЛИС, регистр cnfPin[7:0] **/
#define Sync_channel_sel       			0x01
#define Sync_ON                			0x02
#define Sync_IN_OUT_WIN        			0x04
#define sClear_Decim_Counter   			0x08
#define Read_SRAM_UP           			0x10 
#define OSC_LA                 			0x20
#define AND_OR_LA_TRIGG        			0x40
#define LA_OR_OSC_TRIGG        			0x80


/** внутренние сигналы ПЛИС, регистр Write_Control[1:0] **/
#define WriteSRAM_Start                 (EPM570_Write_Control_DATA | 0x01)
#define WriteSRAM_Stop                  (EPM570_Write_Control_DATA & ~0x01)
#define Enable_Trigger					(EPM570_Write_Control_DATA | 0x02)
#define Disable_Trigger					(EPM570_Write_Control_DATA & ~0x02)
#define Enable_Interleave				(ExtPin_B_Data | 0x04)
#define Disable_Interleave				(ExtPin_B_Data & ~0x04)

#define Enable_sResetReadCounter		(ExtPin_B_Data | 0x01)
#define Disable_sResetReadCounter		(ExtPin_B_Data & ~0x01)

/** Внешние сигналы с ПЛИС, регистр extPin_reg_0 **/
#define S1                     			(uint8_t)(0x01)
#define S2                     			(uint8_t)(0x02)
#define Open_IN_A              			(uint8_t)(0x04)
#define Open_IN_B              			(uint8_t)(0x08)
#define Closed_IN_A            			(uint8_t)(~(uint8_t)(Open_IN_A))
#define Closed_IN_B            			(uint8_t)(~(uint8_t)(Open_IN_B))

#define ADC_Standby_BothCH	   			(EPM570_extPin_0_DATA & (~(S1|S2)))
#define ADC_Standby_CH_B	   			((EPM570_extPin_0_DATA | S2) & ~S1)
#define ADC_NormalOperation    			((EPM570_extPin_0_DATA | S1) & ~S2)
#define ADC_AlignOperation 				(EPM570_extPin_0_DATA | (S1 | S2))


/** Внешние сигналы с ПЛИС, регистр extPin_reg_1 **/
#define A0                     			0x01
#define A1                     			0x02
#define A2                     			0x04
#define B0                     			0x08
#define B1                     			0x10
#define B2                     			0x20
#define OSC_Enable             			(EPM570_extPin_1_DATA | 0x40)
#define OSC_Disable            			(EPM570_extPin_1_DATA & ~0x40)
#define BackLight_MAX          			(EPM570_extPin_1_DATA | 0x80)
#define BackLight_MIN          			(EPM570_extPin_1_DATA & ~0x80)


/* Exported variables --------------------------------------------------------*/
extern volatile uint8_t EPM570_cnfPin_DATA;            // данные регистра ПЛИС cnfPin
extern volatile uint8_t EPM570_extPin_0_DATA;		   // данные регистра ПЛИС extPin_0
extern volatile uint8_t EPM570_extPin_1_DATA;		   // данные регистра ПЛИС extPin_1
extern volatile uint8_t EPM570_Write_Control_DATA;	   // данные регистра ПЛИС Write_Control

extern volatile uint8_t EPM570_actual_Register;        // хранение адреса "активного" регистра ПЛИС
extern volatile uint8_t EPM570_Register_Process;       // флаг операций над регистром ПЛИС
extern volatile uint8_t ExtPin_B_Data;

/* Exported function --------------------------------------------------------*/
extern void ResetEPM570_cntrlSignals(void);							/** Сброс в 0 управляющих сигналов **/
extern void Set_EPM570_Port(GPIOMode_TypeDef GPIO_Mode);			/** Настройк порта вход/выход **/
extern void Write_Reg_Address(uint8_t address);						/** Пишем адрес регистра ПЛИС **/
extern void EPM570_Write_Register(uint8_t address, uint8_t data);	/** Запись регистра EPM570 по адресу **/
extern uint8_t EPM570_Read_Register(uint8_t address);				/** Чтение регистра EPM570 по адресу **/


#endif /* __EPM570_REGISTERS_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/



