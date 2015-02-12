/**
  ******************************************************************************
  * @file    	colors 5-6-5.h
  * @author  	LeftRadio
  * @version 	1.5.6
  * @date
  * @brief   	NeilScope3 Colors header
  ******************************************************************************
**/

#ifndef __COLOR565_H
#define __COLOR565_H

/* Includes ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define White       		0xffff
#define Black       		0x0000
#define LightBlack  		0x841
#define LightBlack2  		0x3186
#define Gray        		0x8c71
#define LightGray   		0xce59
#define LightGray2			0xbdf7
#define LightGray3          0x94b2
#define LightGray4          0x8410
#define LightSky    		0x7e7f
#define Red         		0xf800
#define DarkRed     		0x5000
#define LightRed    		0xfcb2
#define LightRed2           0xf904
#define LightGreen  		0x679
#define Blue        		0x1f
#define LightBlue1  		0x3bd
#define LighGreen   		0x87f0
#define Green1      		0x160
#define Orange      		0xeba0
#define Orange1     		0xfb23
#define DarkOrange  		0xa9a0
#define DarkOrange2			0x8960
#define Auqa				0x332c
#define DarkAuqa			0x1e4
#define GrayBlue    		0x41f
#define Yellow          	0xffe0

/* Exported variables --------------------------------------------------------*/
extern const uint16_t M256_Colors[256];
extern const uint16_t grayScalle[256];

/* Exported function ---------------------------------------------------------*/
uint16_t Color_ChangeBrightness(uint16_t ColorIn, int8_t BrightLevel);



#endif /* __COLOR565_H */
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
