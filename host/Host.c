/**
  ******************************************************************************
  * @file         Host.c
  * @author      Left Radio
  * @version     1.5.6
  * @date
  * @brief        NeilScope3 Host sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "defines.h"
#include "Host.h"
#include "main.h"
#include "init.h"
#include "systick.h"
#include "Settings.h"
#include "User_Interface.h"
#include "Analog.h"
#include "AutoCorrectCH.h"
#include "Processing_and_output.h"
#include "EPM570.h"
#include "EPM570_Registers.h"
#include "EPM570_GPIO.h"
#include "Synchronization.h"
#include "Sweep.h"
#include "gInterface_MENU.h"
#include "IQueue.h"
#include "i2c_gpio.h"
#include "HostCommands.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Error type and codes defines */
#define ERROR_RESPOND                    0x7F
#define ErrorCRC                         0x01
#define ErrorData                        0x02
#define ErrorBusy                        0x03

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Boolean RLE_CodeSend = FALSE;
FunctionalState HostMode = DISABLE;
volatile HostRequest_TypeDef gHostRequest = { DISABLE, NO_Request, 0, 0 };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Switch_To_HostMode(void);
static int8_t Decoding_Command(uint8_t cmd_index, uint8_t* data);
static __inline void Host_RequestProcessing(IQueue_TypeDef *IQueue);
static void host_delay(volatile uint32_t delay_cnt);

/* Private Functions ---------------------------------------------------------*/

/**
  * @brief  Recive_Host_Data
  * @param  IQueueIndex - actived queue work index
  * @retval None
  */
void Recive_Host_Data(uint8_t IQueueIndex)
{
      int8_t status;
    IQueue_TypeDef *IQueue = Host_GetIQueue(IQueueIndex);

    /* Decoding and run command request */
    status = Decoding_Command(IQueue->CMD_Index, &IQueue->Data[2]);

    /* Return message code */
    if (status == 0) {
        IQueue->Data[0] = IQueue->Data[0] + 0x40;
    }
    else {
        IQueue->Data[0] = ERROR_RESPOND;
    }

      /* Processing request */
    if(gHostRequest.State == ENABLE) {
        Host_RequestProcessing(IQueue);
    }
    else {
        Transmit_To_Host(IQueue->Data[0], &IQueue->Data[2], IQueue->Data[1]);
    }
}

/**
  * @brief  Host_Send_Byte
  * @param
  * @retval None
  */
static __inline void Host_Send_Byte(uint8_t byte)
{
    USART1->DR = byte;
    while((USART1->SR & USART_FLAG_TXE) == (uint16_t)RESET);
}

/**
  * @brief  send string(UTF-8) to host
  * @param  string pointer
  * @retval None
  */
void host_send_str(const char *str) {
    /* */
    while(*str != 0) {
        Host_Send_Byte((uint8_t)(*str));
        str++;
    }
    /* */
    Host_Send_Byte( 0x0D );
    Host_Send_Byte( 0x0A );
}

/**
  * @brief  Transmit_To_Host
  * @param  *pData - pointer to transmited data
  *         DataLen - num bytes to transmit
  * @retval None
  */
void Transmit_To_Host(uint8_t Respond_CMD, uint8_t *pData, uint8_t DataLen) {
    uint8_t header[3] = {0x5B, Respond_CMD, DataLen};
    uint8_t Control_CRC = 0;
    uint16_t i;
    /* Send header */
    for(i = 0; i < 3; i++) {
        Host_Send_Byte(header[i]);
        Control_CRC = CRC8(header[i], Control_CRC);
    }
    /* Send data */
    for(i = 0; i < DataLen; i++) {
        Host_Send_Byte(*(pData+i));
        Control_CRC = CRC8(*(pData+i), Control_CRC);
    }
    /* Send CRC byte */
    Host_Send_Byte( CRC8(0, Control_CRC) );
    /* Stop if single-shot mode  */
    if((gSyncState.Mode == Sync_SINGL) && (gOSC_MODE.State == RUN) && (EPM570_SRAM_GetWriteState() == COMPLETE)) {
        gOSC_MODE.State = STOP;
    }
}


/**
  * @brief  Transmit_DataBuf_To_Host
  * @param  None
  * @retval None
  */
void Transmit_DataBuf_To_Host(void)
{
    uint8_t ResponseDataBufHeader[8] = { 0x5B, 0x70, 0x04, 0x00, 0x00, 0x00, 0x00, 0xFF };
    uint8_t Data_A, Data_B, *pData;
    uint8_t Control_CRC = 0;
    uint32_t i;
    uint32_t FullPacked = 0, DataCnt = 0;
    uint32_t DataLen = 0;

//    __disable_irq();

    /* select Channel */
    if((gOSC_MODE.Interleave == TRUE) || (pINFO == &INFO_A))
    {
        pData = &Data_A;
        ResponseDataBufHeader[6] = CHANNEL_A;
    }
    else if (pINFO == &INFO_B)
    {
        pData = &Data_B;
        ResponseDataBufHeader[6] = CHANNEL_B;
    }
    else
    {
        if (RLE_CodeSend == TRUE) pData = &Data_B;
        else pData = &Data_A;
        ResponseDataBufHeader[6] = CHANNEL_DIGIT;
    }

    if(Get_AutoDivider_State(pINFO->Mode.ID) == ENABLE) ResponseDataBufHeader[7] = pINFO->AD_Type.Analog.Div;

    /* Roll back to start read data point */
    EPM570_SRAM_ReadState(DISABLE);
    EPM570_SRAM_Shift(0x7fffffff, SRAM_READ_DOWN);

    /* Prepare SRAM to read, read and send data to Host */
    EPM570_SRAM_ReadState(ENABLE);


    DataLen = gHostRequest.DataLen;
    while(gHostRequest.DataLen > 64000){ gHostRequest.DataLen -= 64000; FullPacked++; }
    while((FullPacked > 0) || (gHostRequest.DataLen > 0))
    {
        if(FullPacked == 0)
        {
            DataCnt = gHostRequest.DataLen;
            gHostRequest.DataLen = 0;
        }
        else
        {
            DataCnt = 64000;
            FullPacked--;
        }

        if((DataLen > 64000) && (DataCnt != 0)) host_delay(2000000);

        TIM2->DIER &= ~TIM_DIER_UIE;

        Control_CRC = 0;
        ResponseDataBufHeader[3] = (uint8_t)(DataCnt >> 10);
        ResponseDataBufHeader[4] = (uint8_t)((DataCnt & 0x03FC) >> 2);
        ResponseDataBufHeader[5] = (uint8_t)((DataCnt & 0x03) << 6);

        /* sending header */
        for(i = 0; i < 8; i++) {
            Host_Send_Byte(ResponseDataBufHeader[i]);
            Control_CRC = CRC8(ResponseDataBufHeader[i], Control_CRC);
        }

        for(i = 0; i < DataCnt; i++)
        {
            if( (gOSC_MODE.Mode == OSC_MODE) && (gOSC_MODE.Interleave == TRUE) )
            {
                EPM570_GPIO_RS(SET);
                Data_A = ~(GPIOB->IDR >> 8);

                Host_Send_Byte(*pData);
                Control_CRC = CRC8(*pData, Control_CRC);

                EPM570_GPIO_RS(RESET);
                Data_A = ~(GPIOB->IDR >> 8);
                Data_A -= InterliveCorrectionCoeff;

                i++;
            }
            else if(gOSC_MODE.Mode != LA_MODE)
            {
                EPM570_GPIO_RS(SET);
                Data_A = ~(GPIOB->IDR >> 8);

                EPM570_GPIO_RS(RESET);
                Data_B = ~(GPIOB->IDR >> 8);

                if( (ActiveMode != &IntMIN_MAX) && (gSamplesWin.Sweep != 0) )
                {
                    EPM570_GPIO_RS(SET);
                    EPM570_GPIO_RS(RESET);
                }
            }
            else
            {
                EPM570_GPIO_RS(SET);
                Data_A = ~(GPIOB->IDR >> 8);

                EPM570_GPIO_RS(RESET);
                Data_B = (GPIOB->IDR >> 8);
            }

            Host_Send_Byte(*pData);
            Control_CRC = CRC8(*pData, Control_CRC);


            if(i < 600)
            {
                pINFO->DATA[i] = (*pData) - 127;
            }
        }

        /* send CRC to Host */
        Host_Send_Byte( CRC8(0, Control_CRC) );

        TIM2->DIER |= TIM_DIER_UIE;
    }

    /* Disable read state for SRAM */
    EPM570_SRAM_ReadState(DISABLE);

//    __enable_irq();
}


/**
  * @brief  Decoding command from host
  * @param  None
  * @retval None
  */
int8_t Decoding_Command(uint8_t cmd_index, uint8_t* data)
{
  if ( (cmd_index == 255) && (cmd_index >= HOST_CMD_CNT) ){
      return -1;
  }

  /* Run respect command, return state */
  return Host_Commands[cmd_index].Handler(data);
}


/**
  * @brief  Host_RequestProcessing
  * @param  None
  * @retval None
  */
static __inline void Host_RequestProcessing(IQueue_TypeDef *IQueue)
{
    if(gHostRequest.Request == Data_Request) {
        /* LA mode */
        if(pINFO == &DINFO_A) {
            EPM570_SRAM_Write();

            RLE_CodeSend = FALSE;
            Transmit_DataBuf_To_Host();

            if(EPM570_Get_LA_RLE_State() == ENABLE)
            {
                gHostRequest.DataLen = 256000;
                RLE_CodeSend = TRUE;
                Transmit_DataBuf_To_Host();
            }
        }
        /* Oscilloscope mode */
        else {
            /* if channel A actived */
            if(INFO_A.Mode.EN == RUN) {
                /* if request for channel A start write to SRAM
                 * if request for channel B write not necessary, data for ch B already in SRAM */
                if(pINFO == &INFO_A) EPM570_SRAM_Write();
            }
            /* else if only channel B actived also start write to SRAM */
            else if(INFO_B.Mode.EN == RUN) {
                EPM570_SRAM_Write();
            }

            Transmit_DataBuf_To_Host();
            Analog_AutodividerMain();
        }
    }
    else if(gHostRequest.Request == Calibrate_Request) {
        Auto_CorrectZ_CH(gHostRequest.UserData);
    }
    else if(gHostRequest.Request == Bootoader_Request) {
        /* Respont to Host */
        Transmit_To_Host(IQueue->Data[0], &IQueue->Data[2], IQueue->Data[1]);
        /* Starting bootloader */
        Start_Bootloader();
    }
    else if (gHostRequest.Request == Disconnect) {
        /* Respont to Host */
        Transmit_To_Host(IQueue->Data[0], &IQueue->Data[2], IQueue->Data[1]);
        /* Reset (default start in automode) */
        Host_IQueue_ClearAll();
        /* Small delay and reset */
        delay_ms(100);
        NVIC_SystemReset();
    }

    /* Reset/Clear request */
    Host_RequestReset();
}


/**
  * @brief  Host_RequestReset
  * @param  None
  * @retval None
  */
void Host_RequestReset(void)
{
    gHostRequest.State = DISABLE;
    gHostRequest.Request = NO_Request;
    gHostRequest.UserData = 0;
    gHostRequest.DataLen = 0;
}


/**
  * @brief  Get_CorrectionTerminate
  * @param  None
  * @retval Host terminate command state
  */
FlagStatus Host_GetTerminateCmd(void)
{
    if(Host_IQueue_GetWorkIQueue() > 0) {
        Host_RequestReset();
        return SET;        // TERMINATE_CMD
    }
    else {
        return RESET;
    }
}


/**
  * @brief  host_delay
  * @param  delay cnt value
  * @retval None
  */
static void host_delay(volatile uint32_t delay_cnt)
{
    while(delay_cnt > 0){ delay_cnt--; }
}


/**
  * @brief  CRC8_Buff
  * @param  pointer to buff data and num bytes to calcul CRC
  * @retval CRC value for buff
  */
uint8_t CRC8_Buff(uint8_t *pBuff, uint16_t NumBytes)
{
   uint16_t i;
   uint8_t *pbuff = pBuff;
   uint8_t crcBuff = 0x00;

   for(i = 0; i < NumBytes; i++)
   {
       crcBuff = CRC8(*pbuff, crcBuff);
       pbuff++;
   }

   return (crcBuff);
}


/**
  * @brief  CRC8
  * @param  byte and prev CRC
  * @retval CRC
  */
uint8_t CRC8(uint8_t Byte, uint8_t crc)
{
   uint8_t i;

   for ( i = 0; i < 8 ; i++)
   {
      if (crc & 0x80)
      {
         crc <<= 1;
         if (Byte & 0x80) crc |= 0x01;
         else crc &= 0xFE;
         crc ^= 0x85;
      }
      else
      {
        crc <<= 1;
        if (Byte & 0x80) crc |= 0x01;
        else crc &= 0xFE;
      }

      Byte <<= 1;
   }
   return (crc);
}



