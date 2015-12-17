/**
  ******************************************************************************
  * @file	 	Advanced_Menu.c
  * @author  	Left Radio
  * @version 	1.0.0
  * @date
  * @brief		NeilScope3 Advanced Menu sourse
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systick.h"
#include "User_Interface.h"
#include "Settings.h"
#include "eeprom_WR_RD.h"
#include "i2c_gpio.h"
#include "init.h"

#include "Advanced_Menu.h"
#include "Advanced_Menu_buttons.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void AdvancedMenu_DrawCallback(DrawState state);

/* Private variables ---------------------------------------------------------*/
Menu_Struct_TypeDef AdvancedMenu = {
		{ advLeftLineBtn-2, 19, advLeftLineBtn + btnWidth, UpLineBtn + 1},
		ENABLE,
		Auto_ClipObj,
		_ADVANCED_BUTTONS_MAX_,
		_ADVANCED_BUTTONS_MAX_,
		_ADVANCED_BUTTONS_MAX_,
		M_SKIP,
		DOWN,
		UP,
		{
				&btnBootloader, &btnEraseEeprom, &btnSave,
				&btnEmpty, &btnI2C_ADDR, &btnI2C_GPIO,
				&btnHostMode, &btnClose
		},
		AdvancedMenu_DrawCallback,
};

uint8_t I2C_ADDR = 0x00;
char I2C_ADDR_Text[5] = "0x00";

/* Extern variables ----------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
  * @brief  AdvancedMenu_DrawCallback
  * @param  None
  * @retval None
  */
static void Update_I2C_ADDR(void)
{
	NS_I2C_GPIO_TypeDef* gpios = ns_i2c_gpios[gOSC_MODE.i2c_gpio_chip_index];

	if (gpios != (void*)0) {
		I2C_ADDR = gpios->i2c_address;
		sprintf (I2C_ADDR_Text, "%#02x", I2C_ADDR);
		btnI2C_ADDR.Text = I2C_ADDR_Text;
	}
	else {
		I2C_ADDR = 0x00;
		btnI2C_ADDR.Text = "0x00";
	}
}

/**
  * @brief  AdvancedMenu_DrawCallback
  * @param  None
  * @retval None
  */
static void Host_Communicate(void)
{
	int8_t state;
	uint16_t color;

	state = Host_Comunication_Configuration(
			(NS_Host_Communicate_TypeDef*)&gOSC_MODE.HostCommunicate
	);

	color = gMessage.Color;
	if (!state) {
		gMessage.Color = LighGreen;
		Show_Message("Successful");
	}
	else {
		gMessage.Color = Red;
		Show_Message("Error!");
	}
	gMessage.Color = color;
}

/**
  * @brief  AdvancedMenu_DrawCallback
  * @param  None
  * @retval None
  */
static void AdvancedMenu_DrawCallback(DrawState state)
{
	if(state == DRAW) {

		/* I2C GPIO button text */
		btnI2C_GPIO.Text = (char*)btnI2C_GPIO_Texts[gOSC_MODE.i2c_gpio_chip_index];

//		for (i = 0; i < btnI2C_Txt_Max; i++) {
//			if(gpios[i] == ns_i2c_gpios[gOSC_MODE.i2c_gpio_chip_index]) {
//				I2C_GPIO_Index = i;
//				btnI2C_GPIO.Text = (char*)btnI2C_GPIO_Texts[I2C_GPIO_Index];
//				break;
//			}
//		}

		/* I2C ADDR button text */
		Update_I2C_ADDR();

		/* Host mode button text */
		btnHostMode.Text = (char*)btnHostMode_Texts[gOSC_MODE.HostCommunicate];
	}
}


/**
  * @brief  Jump_To_Boot
  * @param  None
  * @retval None
  */
void Jump_To_Boot(void)
{
	if(ButtonsCode != OK) return;

	Start_Bootloader();
}

/**
  * @brief  AdvancedMenu_Closed
  * @param  None
  * @retval None
  */
void AdvancedMenu_Closed(void)
{
	if (ButtonsCode == OK) {

		Clear_Menu(pMenu);
		SetActiveMenu(&QuickMenu);
		setActiveButton(pMenu->Buttons[pMenu->Indx]);
		LCD_DrawButton(btn, activeButton);
	}
}

/**
  * @brief  Perform_Erase_EEPROM
  * @param  None
  * @retval None
  */
void Perform_Erase_EEPROM(void)
{
	if(ButtonsCode != OK) return;

	if(ErasePreference() != 0) Show_Message("Error erase!");
	else Show_Message("Successful erased, reboot...");

	/* delay before reboot */
	delay_ms(2000);
	NVIC_SystemReset();
}

/**
  * @brief  Save_pref
  * @param  None
  * @retval None
  */
void Save_pref(void)
{
	if(ButtonsCode != OK) return;

	if(SavePreference() != 0) {
		LCD_SetTextColor(0xF800);
		Show_Message("Preference save error!");
	}
	else {
		LCD_SetTextColor(LightGray2);
		Show_Message("Preference save successful");
	}
}


/**
  * @brief  Select_I2C_ADDR
  * @param  None
  * @retval None
  */
void Select_I2C_ADDR(void)
{
	if ( (ButtonsCode == OK) || (ButtonsCode == RIGHT) ) {
		I2C_ADDR++;
	}
	else if (ButtonsCode == LEFT) {
		I2C_ADDR--;
	}
	else {
		return;
	}

	NS_I2C_GPIO_TypeDef* gpios = ns_i2c_gpios[gOSC_MODE.i2c_gpio_chip_index];

	if (gpios != (void*)0) {
		gpios->i2c_address = I2C_ADDR;
	}
	else {
		return;
	}

	/* I2C ADDR button text */
	Update_I2C_ADDR();
	Host_Communicate();
	LCD_DrawButton(&btnI2C_ADDR, activeButton);
}

/**
  * @brief  Select_I2C_GPIO
  * @param  None
  * @retval None
  */
void Select_I2C_GPIO(void)
{
	if ( (ButtonsCode == OK) || (ButtonsCode == RIGHT) ) {
		if (++gOSC_MODE.i2c_gpio_chip_index > btnI2C_Txt_Max-1) {
			gOSC_MODE.i2c_gpio_chip_index = 0;
		}
	}
	else if (ButtonsCode == LEFT) {
		if (--gOSC_MODE.i2c_gpio_chip_index < 0) {
			gOSC_MODE.i2c_gpio_chip_index = btnI2C_Txt_Max-1;
		}
	}
	else {
		return;
	}

	Host_Communicate();

	btnI2C_GPIO.Text = (char*)btnI2C_GPIO_Texts[gOSC_MODE.i2c_gpio_chip_index];
	LCD_DrawButton(&btnI2C_GPIO, activeButton);

	/* I2C ADDR button text */
	Update_I2C_ADDR();
	LCD_DrawButton(&btnI2C_ADDR, NO_activeButton);
}

/**
  * @brief  Select_HostMode
  * @param  None
  * @retval None
  */
void Select_HostMode(void)
{
	if ( (ButtonsCode == OK) || (ButtonsCode == RIGHT) ) {
		if (++gOSC_MODE.HostCommunicate > 4) {
			gOSC_MODE.HostCommunicate = 0;
		}
	}
	else if (ButtonsCode == LEFT) {
		if (--gOSC_MODE.HostCommunicate < 0) {
			gOSC_MODE.HostCommunicate = 4;
		}
	}
	else {
		return;
	}

	Host_Communicate();

	btnHostMode.Text = (char*)btnHostMode_Texts[gOSC_MODE.HostCommunicate];
	LCD_DrawButton(&btnHostMode, activeButton);
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
