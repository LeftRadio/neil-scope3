// 
//  Font data for Microsoft Sans Serif 10pt
// 

#include "HX8352_Library.h"

//
//  Font data for Lucida Console 9pt
//

// Character bitmaps for Lucida Console 9pt
const uint8_t lucidaConsole_9ptBitmaps[] =
{
	// @0 '0' (7 pixels wide)
	0x3C, //   ####
	0xFF, // ########
	0xC3, // ##    ##
	0x81, // #      #
	0xC3, // ##    ##
	0xFF, // ########
	0x3C, //   ####

	// @7 '1' (6 pixels wide)
	0b10000010, // #     #
	0b10000010, // #     #
	0b11111111, // ########
	0b11111111, // ########
	0b10000000, // #
	0b10000000, // #

	// @13 '2' (6 pixels wide)
	0b11000001, // ##     #
	0b11100001, // ###    #
	0b10110001, // # ##   #
	0b10011001, // #  ##  #
	0b10001111, // #   ####
	0b10001111, // #   ####

	// @19 '3' (5 pixels wide)
	0x81, // #      #
	0x89, // #   #  #
	0x89, // #   #  #
	0xFF, // ########
	0x77, //  ### ###

	// @24 '4' (7 pixels wide)
	0x30, //   ##
	0x38, //   ###
	0x2C, //   # ##
	0x26, //   #  ##
	0xFF, // ########
	0xFF, // ########
	0x20, //   #

	// @31 '5' (5 pixels wide)
	0x8F, // #   ####
	0x8F, // #   ####
	0x89, // #   #  #
	0xF9, // #####  #
	0x71, //  ###   #

	// @36 '6' (6 pixels wide)
	0x7C, //  #####
	0xFE, // #######
	0x9B, // #  ## ##
	0x89, // #   #  #
	0xF9, // #####  #
	0x71, //  ###   #

	// @42 '7' (6 pixels wide)
	0xC1, // ##     #
	0xF1, // ####   #
	0x39, //   ###  #
	0x0D, //     ## #
	0x07, //      ###
	0x03, //       ##

	// @48 '8' (6 pixels wide)
	0x76, //  ### ##
	0xFF, // ########
	0x89, // #   #  #
	0x99, // #  ##  #
	0xFF, // ########
	0x66, //  ##  ##

	// @54 '9' (6 pixels wide)
	0x8E, // #   ###
	0x9F, // #  #####
	0x91, // #  #   #
	0xD1, // ## #   #
	0x7F, //  #######
	0x3E, //   #####
};

// Character descriptors for Lucida Console 9pt
// { [Char width in bits], [Offset into lucidaConsole_9ptCharBitmaps in bytes] }
const FONT_CHAR_INFO lucidaConsole_9ptDescriptors[] =
{
	{7, 0}, 		// 0
	{6, 7}, 		// 1
	{6, 13}, 		// 2
	{5, 19}, 		// 3
	{7, 24}, 		// 4
	{5, 31}, 		// 5
	{6, 36}, 		// 6
	{6, 42}, 		// 7
	{6, 48}, 		// 8
	{6, 54}, 		// 9
};

// Font information for Lucida Console 9pt
const FONT_INFO lucidaConsole_9pt_Bold_FontInfo =
{
	8, //  Character height
	1,  //	 Character height in bytes
	'0', //  Start character
	'9', //  End character
	2,   //  Width, in pixels, of space character
	lucidaConsole_9ptDescriptors, //  Character descriptor array
	lucidaConsole_9ptBitmaps, //  Character bitmap array
};



