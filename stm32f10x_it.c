/*************************************************************************************
*
Description :  NeilScope3 Main Interrupt Service Routines
Version     :  1.0.0
Date        :  25.12.2012
Author      :  Left Radio                          
Comments:   :  This file provides template for all exceptions handler and 
*              peripherals interrupt service routine.
**************************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "defines.h"
#include "main.h"
#include "Host.h"
#include "EPM570.h"
#include "Settings.h"
#include "User_Interface.h"
#include "init.h"
#include "systick.h"
#include "IQueue.h"
#include "ReceiveStateMachine.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t ON_OFF_counter = 0;
uint16_t beep_cnt = 1;
int8_t ADC_VbattPrecent = 0;
FlagStatus show_ADC_flag = RESET;

/* переменные для кнопок */
FlagStatus ButtonEnable = SET;
ButtonsCode_TypeDef ButtonsCode;            		// кнопоки пользователя
ButtonsPush_TypeDef ButtonPush = B_RESET;         	// флаг нажатия на кнопку
uint8_t speed_up_cnt = 0;

FlagStatus MessageEvent = RESET;
extern btnINFO btnRUN_HOLD;

Boolean USB_CP2102_Connect_Event = FALSE;

__IO int hostPackedRecived = 0;

/* Extern function -----------------------------------------------------------*/
extern void Error_message(char* message_text);
extern uint8_t EPM570_Registers_GetOperateStatus(void);


/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */

	LCD_SetTextColor(0x87f0);
	LCD_PutStrig(leftLimit + 5, upperLimit - 24, 0, "HardFault!");
	delay_ms(1000);
	GPIOC->BRR = GPIO_Pin_15;

	while (1)
	{
	}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


/**
  * @brief  This function handles RTC_IRQHandler .
  * @param  None
  * @retval : None
  */
void RTC_IRQHandler(void)
{
	uint32_t Curent_RTC_Counter;
	int32_t tTime, wTime;

	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		NVIC_ClearPendingIRQ(RTC_IRQn);
		RTC_ClearITPendingBit(RTC_IT_SEC);
		RTC_WaitForLastTask();

		if( HostMode == DISABLE ) {

			/* If counter is equal to 86399: one day was elapsed */
			if(RTC_GetCounter() == 86399)
			{
				/* Wait until last write operation on RTC registers has finished */
				RTC_WaitForLastTask();
				/* Reset counter value */
				RTC_SetCounter(0x0);
				/* Wait until last write operation on RTC registers has finished */
				RTC_WaitForLastTask();
			}


			Curent_RTC_Counter = RTC_GetCounter();

			// Set message event
			if(gMessage.Visible == TRUE)
			{
				tTime = Curent_RTC_Counter - gMessage.TimeOFF;
				if(tTime > 0)
				{
					MessageEvent = SET;
				}
			}

			// Verify auto power OFF
			if((AutoOff_Timer.State == ENABLE) && (HostMode != ENABLE))
			{
				tTime = Curent_RTC_Counter - AutoOff_Timer.ResetTime;
				wTime = AutoOff_Timer.Work_Minutes * 60;

				if(tTime >= wTime)
				{
					__disable_irq();
					SavePreference();
					Beep_Start();

					GPIOC->BRR = GPIO_Pin_15;
				}
			}

			// batt update
			if(show_ADC_flag != SET) {
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			}
		}
//		else {

//			if (hostPackedRecived == 0) {
//				Host_IQueue_ClearAll();
//			}
//			else {
//				hostPackedRecived = 0;
//			}
//		}
	}
}


/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_UP_IRQHandler(void)
{
	TIM1->SR &= ~TIM_SR_UIF;

	if(--TimeoutCnt < 1)
	{
		SRAM_TimeoutState = SET;
		TIM1->CR1 &= ~TIM_CR1_CEN;
	}
}


/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	static ButtonsPush_TypeDef prvsButtonPush = B_RESET;
	static uint8_t BtnPushRetentionCnt = 0;		//BtnResetRetentionCnt = 0;
	
	TIM2->SR &= ~TIM_SR_UIF;  // очищаем флаг прерывания
	
#ifndef __POWER_BUTTON_OFF__
	/* чтение кнопки питания */
	if(ON_OFF_button != 0)
	{
		/* если нажата и счетчик досчитал, то выключаем осциллограф */
		if(ON_OFF_counter++ > 5)
		{
			SavePreference();
			Beep_Start();
			Show_Message("Please release 'ON/OFF' button");

			while(ON_OFF_button != 0);
			delay_ms(300);
			GPIOC->BRR = GPIO_Pin_15;
		}
	}
	else ON_OFF_counter = 0;   // иначе сбрасываем счетчик
#endif

	/* If EPM registers not busy then read buttons register */
	if(EPM570_Registers_GetOperateStatus() == 0) ButtonsCode = EPM570_Read_Keys();
	
	/* If any button is pressed */
	if(ButtonsCode != NO_Push)
	{
		if(prvsButtonPush == B_RESET)
		{
			prvsButtonPush = ButtonPush = SHORT_SET;
		}
		else
		{
			if(BtnPushRetentionCnt++ > 2) ButtonPush = SHORT_SET;
			else ButtonPush = B_RESET;
		}

		if(EPM570_SRAM_GetWriteState() != COMPLETE)
		{
			if(btn == &btnRUN_HOLD)
			{
				if((ButtonsCode != DOWN) && (ButtonsCode != OK)) EPM570_SRAM_SetWriteState(STOP);
			}
			else EPM570_SRAM_SetWriteState(STOP);
		}

		/* Reset auto off timer */
		if(AutoOff_Timer.Vbatt != 1)
		{
			AutoOff_Timer.ResetTime = RTC_GetCounter();
		}
	}
	else
	{
		prvsButtonPush = ButtonPush = B_RESET;
		BtnPushRetentionCnt = 0;
		speed_up_cnt = 0;
	}
}


/**
  * @brief  This function handles TIM4 interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM4->SR &= ~TIM_SR_UIF;

		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) != RESET)	GPIOC->BRR = GPIO_Pin_13;
		else GPIOC->BSRR = GPIO_Pin_13;

		//TIM_SetAutoreload(TIM4, 24000 + (beep_cnt * 200));	// изменение частоты звука

		if(beep_cnt++ >= 25)	// длительность звучания
		{
			GPIOC->BRR = GPIO_Pin_13;
			Beep_Stop();
			beep_cnt = 1;
		}
	}
}


/**
  * @brief  This function handles I2C ERROR interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_ER_IRQHandler(void)
{
	/* Check on I2C2 SMBALERT flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_SMBALERT))	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_SMBALERT);
	}
	/* Check on I2C2 Time out flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_TIMEOUT);
	}
	/* Check on I2C2 Arbitration Lost flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_ARLO))	{
		I2C_ClearITPendingBit(I2C1, I2C_IT_ARLO);
	}	
	/* Check on I2C2 PEC error flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_PECERR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_PECERR);
	} 
	/* Check on I2C2 Overrun/Underrun error flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_OVR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_OVR);
	} 
	/* Check on I2C2 Acknowledge failure error flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_AF)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
	}
	/* Check on I2C2 Bus error flag and clear it */
	if (I2C_GetITStatus(I2C1, I2C_IT_BERR)) {
		I2C_ClearITPendingBit(I2C1, I2C_IT_BERR);
	}

//	Error_message("I2C Bus ERROR");
//	LCD_PutStrig(20, 160, 0, "Push button OK for continue");
//	while(EPM570_Read_Keys() != OK);
//
//	LCD_SetTextColor(0x0000);
//	LCD_PutStrig(20, 160, 0, "Push button OK for continue");
//	LCD_DrawGrid(&activeAreaGrid, DRAW); // перерисовываем сетку в области осциллограмм
}


/**
  * @brief  This function handles ADC1_2 interrupt request.
  * @param  None
  * @retval None
  */
void ADC1_2_IRQHandler(void)
{
	static uint16_t ADC_Sum = 0;
	static uint8_t i = 0;
	float temp = 0;

	ADC1->SR &= ~ADC_SR_EOC;	// очищаем флаг прерывания
	ADC_Sum += ADC1->DR;
	i++;
	if(i >= 5)
	{
		temp = (((float)ADC_Sum * 3.3 * 2.5) / 20480) - 3.45;
		ADC_VbattPrecent = (uint8_t)((temp * 100) / (4.10 - 3.45));
		ADC_Sum = i = 0;
		show_ADC_flag = SET;
	}

	show_FPS_flag = SET;
}


/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	static uint8_t even_byte = 0;
	uint8_t data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		/* Read one byte from the receive data register */
		data = (uint16_t)(USART1->DR & (uint16_t)0x01FF);

		if(gOSC_MODE.HostCommunicate == Host_ESP_Mode) {
			if(!even_byte) {
				even_byte = 1;
			}
			else {
				even_byte = 0;
				return;
			}
		}

		/* push recived data to recive state machine */
		ReceivedStateMachine_Event(data);
	}
}


/**
  * @brief  Message succsessful recived callback from USART state machine
  * @param  None
  * @retval None
  */
void ReceivedStateMachine_CompleteCallBack(uint8_t command_index, uint8_t* command_data)
{
	Host_SetIQueue(command_data+1, command_index);
}


/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{
//	static uint8_t stg = 0;
//	static uint8_t WorkCell_Index = 0;
//	static __IO IQueue_TypeDef *IQueue = 0;

//	uint8_t esp_mltp = (gOSC_MODE.HostCommunicate == Host_ESP_Mode)? 2 : 1;
//	uint8_t i;


	if ((DMA1->ISR & DMA1_IT_TC5) != (uint32_t)RESET)
	{
		/* Clear DMA1 Channel 1 Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA1->IFCR = DMA1_IT_GL5;
//
//		/* Disable the selected USART by clearing the UE bit in the CR1 register */
//		USART_DMACmd(USART1, USART_DMAReq_Rx , DISABLE);
////		USART_Cmd(USART1, DISABLE);
//		DMA1_Channel5->CCR  &= ~DMA_CCR5_EN;
//
//		/* Get IQueue pointer */
//		IQueue = Host_GetIQueue(WorkCell_Index);
//
//
//		/* If start message and first byte recive is 0x5B */
//		if((stg == 0) && (IQueue->Data[0] == 0x5B))
//		{
//			stg = 1;
//			DMA1_Channel5->CNDTR = (IQueue->Data[2*esp_mltp] + 1) * esp_mltp;  	// New recive data count
//			DMA1_Channel5->CMAR = (uint32_t)&IQueue->Data[3*esp_mltp]; 			// New memory addr
//		}
//		else
//		{
//			if(stg == 1)
//			{
//				/* If "Terminate_LastCommad" recived */
//				if(IQueue->Data[1*esp_mltp] == 0x50) IQueue_CommandStatus = FALSE;
//				else
//				{
//					/* Save host request to current cell */
//					IQueue->IsEmpty = FALSE;
//					IQueue->CMD_Length = IQueue->Data[2*esp_mltp] + 4;
//
//					if (gOSC_MODE.HostCommunicate == Host_ESP_Mode) {
//						for(i = 1; i < IQueue->CMD_Length; i++) {
//							IQueue->Data[i] = IQueue->Data[i*2];
//						}
//					}
//
//					hostPackedRecived++;
//
//					/* Search next cell to write for the next command */
//					WorkCell_Index = Host_IQueue_GetEmptyIndex();
//					IQueue = Host_GetIQueue(WorkCell_Index);
//				}
//			}
//
//			/* If was an attempt to init bootloader on 115200 then 0x5B saved in 3th byte */
//			if((IQueue->Data[1*esp_mltp] == 0x80) && (IQueue->Data[2*esp_mltp] == 0x5B))
//			{
//				IQueue->Data[0] = 0x5B;
//				DMA1_Channel5->CNDTR = 2*esp_mltp;							// Set to default data length
//				DMA1_Channel5->CMAR = (uint32_t)&IQueue->Data[1*esp_mltp];	// Memory addr
//			}
//			else	/* Normal connecting */
//			{
//				if(stg == 0) {
//
//					for(i = 0; i < 3; i++) {
//						USART1->DR = IQueue->Data[i];
//						while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
//					}
//
//					/* Search next cell to write for the next command */
//					Host_IQueue_Clear(WorkCell_Index);
//					if(++WorkCell_Index >= IQUEUE_SIZE) WorkCell_Index = 0;
//					IQueue = Host_GetIQueue(WorkCell_Index);
//
//				}
//				DMA1_Channel5->CNDTR = 3*esp_mltp;							// Set to default data length
//				DMA1_Channel5->CMAR = (uint32_t)&IQueue->Data[0];	// Memory addr
//			}
//
//			stg = 0;
//		}

		/* Run DMA channel */
//		DMA1_Channel5->CCR  |=  DMA_CCR5_EN;
//		USART_DMACmd(USART1, USART_DMAReq_Rx , ENABLE);
//		USART_Cmd(USART1, ENABLE);
	}
}


/**
  * @brief  This function handles EXTI9_5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line8);
//		NVIC_DisableIRQ(EXTI9_5_IRQn);

		USB_CP2102_Connect_Event = TRUE;
	}
}



/***********************END OF FILE*******************************/
