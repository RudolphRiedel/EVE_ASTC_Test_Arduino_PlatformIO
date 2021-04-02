/*
@file    tft.c / tft.cpp
@brief   TFT handling functions for EVE ASTC test-project
@version 1.0
@date    2021-04-02
@author  Rudolph Riedel

@section History

1.0
- initial release

@section DESCRIPTION

This is a test-program to visualize the different levels of ASTC compression for BT81x graphics controllers.
The image used is this one: https://pixabay.com/vectors/rainbow-colors-diodes-electronic-26389/

Note, the results regarding compression artefacts depend on the image that is used.

You need to copy the provided "flash.bin" file to the external FLASH on your BT81x display first.

From the flash.map file:
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_5x5_KHR.raw   : 4096    : 524288
rainbow-26389_1280_1284x640_COMPRESSED_RGBA_ASTC_6x5_KHR.raw   : 528384  : 438272
rainbow-26389_1280_1284x642_COMPRESSED_RGBA_ASTC_6x6_KHR.raw   : 966656  : 366400
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_8x5_KHR.raw   : 1333056 : 327680
rainbow-26389_1280_1280x642_COMPRESSED_RGBA_ASTC_8x6_KHR.raw   : 1660736 : 273920
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_8x8_KHR.raw   : 1934656 : 204800
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_10x5_KHR.raw  : 2139456 : 262144
rainbow-26389_1280_1280x642_COMPRESSED_RGBA_ASTC_10x6_KHR.raw  : 2401600 : 219136
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_10x8_KHR.raw  : 2620736 : 163840
rainbow-26389_1280_1280x640_COMPRESSED_RGBA_ASTC_10x10_KHR.raw : 2784576 : 131072
rainbow-26389_1280_1284x640_COMPRESSED_RGBA_ASTC_12x10_KHR.raw : 2915648 : 109568
rainbow-26389_1280_1284x648_COMPRESSED_RGBA_ASTC_12x12_KHR.raw : 3025216 : 92480

The number in the bottom right is the number of the image displayed.
So "0" is ASTC_5x5 and "11" is ASTC 12x12.

While these are sorted by the number of the EVE display-mode,
these are not sorted by compression ratio.

ASTC		EVE-Mode	Bits per pixel
4×4			37808		8,00
5×4			37809		6,40
5×5			37810		5,12
6×5			37811		4,27
6×6			37812		3,56
8×5			37813		3,20
8×6			37814		2,67
8×8			37815		2,00
10×5		37816		2,56
10×6		37817		2,13
10×8		37818		1,60
10×10		37819		1,28
12×10		37820		1,07
12×12		37821		0,89
 */

#include "EVE.h"

#define WHITE	0xffffffUL
#define BLACK	0x000000UL

uint8_t tft_active = 0;
uint16_t num_profile_a, num_profile_b;

void touch_calibrate(void)
{
/* send pre-recorded touch calibration values, depending on the display the code is compiled for */

#if defined (EVE_PAF90)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x00000159);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x0001019c);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff93625);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x00010157);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00000000);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0x0000c101);
#endif

#if defined (EVE_RiTFT43)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000062cd);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0xfffffe45);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xfff45e0a);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x000001a3);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x00005b33);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFFbb870);
#endif

#if defined (EVE_EVE3_43G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x0000a1ff);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x00000680);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xffe54cc2);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0xffffff53);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0000912c);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xfffe628d);
#endif

#if defined (EVE_EVE3_50G)
	EVE_memWrite32(REG_TOUCH_TRANSFORM_A, 0x000109E4);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_B, 0x000007A6);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_C, 0xFFEC1EBA);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_D, 0x0000072C);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_E, 0x0001096A);
	EVE_memWrite32(REG_TOUCH_TRANSFORM_F, 0xFFF469CF);
#endif

/* activate this if you are using a module for the first time or if you need to re-calibrate it */
/* write down the numbers on the screen and either place them in one of the pre-defined blocks above or make a new block */
#if 0
	/* calibrate touch and displays values to screen */
	EVE_cmd_dl(CMD_DLSTART);
	EVE_cmd_dl(DL_CLEAR_RGB | BLACK);
	EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
	EVE_cmd_text((EVE_HSIZE/2), 50, 26, EVE_OPT_CENTER, "Please tap on the dot.");
	EVE_cmd_calibrate();
	EVE_cmd_dl(DL_DISPLAY);
	EVE_cmd_dl(CMD_SWAP);
	EVE_cmd_execute();

	uint32_t touch_a, touch_b, touch_c, touch_d, touch_e, touch_f;

	touch_a = EVE_memRead32(REG_TOUCH_TRANSFORM_A);
	touch_b = EVE_memRead32(REG_TOUCH_TRANSFORM_B);
	touch_c = EVE_memRead32(REG_TOUCH_TRANSFORM_C);
	touch_d = EVE_memRead32(REG_TOUCH_TRANSFORM_D);
	touch_e = EVE_memRead32(REG_TOUCH_TRANSFORM_E);
	touch_f = EVE_memRead32(REG_TOUCH_TRANSFORM_F);

	EVE_cmd_dl(CMD_DLSTART);
	EVE_cmd_dl(DL_CLEAR_RGB | BLACK);
	EVE_cmd_dl(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG);
	EVE_cmd_dl(TAG(0));

	EVE_cmd_text(5, 15, 26, 0, "TOUCH_TRANSFORM_A:");
	EVE_cmd_text(5, 30, 26, 0, "TOUCH_TRANSFORM_B:");
	EVE_cmd_text(5, 45, 26, 0, "TOUCH_TRANSFORM_C:");
	EVE_cmd_text(5, 60, 26, 0, "TOUCH_TRANSFORM_D:");
	EVE_cmd_text(5, 75, 26, 0, "TOUCH_TRANSFORM_E:");
	EVE_cmd_text(5, 90, 26, 0, "TOUCH_TRANSFORM_F:");

	EVE_cmd_setbase(16L);
	EVE_cmd_number(310, 15, 26, EVE_OPT_RIGHTX|8, touch_a);
	EVE_cmd_number(310, 30, 26, EVE_OPT_RIGHTX|8, touch_b);
	EVE_cmd_number(310, 45, 26, EVE_OPT_RIGHTX|8, touch_c);
	EVE_cmd_number(310, 60, 26, EVE_OPT_RIGHTX|8, touch_d);
	EVE_cmd_number(310, 75, 26, EVE_OPT_RIGHTX|8, touch_e);
	EVE_cmd_number(310, 90, 26, EVE_OPT_RIGHTX|8, touch_f);

	EVE_cmd_dl(DL_DISPLAY);	/* instruct the graphics processor to show the list */
	EVE_cmd_dl(CMD_SWAP); /* make this list active */
	EVE_cmd_execute();

	while(1);
#endif
}


void TFT_init(void)
{
	if(EVE_init() != 0)
	{
		tft_active = 1;

		EVE_memWrite8(REG_PWM_DUTY, 0x30);	/* setup backlight, range is from 0 = off to 0x80 = max */
		touch_calibrate();

		EVE_init_flash();
		EVE_cmd_flashread(0, 4096, 655360); /* floral-2069810_1280_1280x800_COMPRESSED_RGBA_ASTC_5x5_KHR.raw from FLASH to RAM_G, offset and length are from the .map file */
	}
}

uint8_t image = 0;


/* check for touch events and setup vars for TFT_display() */
void TFT_touch(void)
{
	uint8_t tag;
	static uint8_t toggle_lock = 0;
	
	if(EVE_busy()) /* is EVE still processing the last display list? */
	{
		return;
	}

	tag = EVE_memRead8(REG_TOUCH_TAG); /* read the value for the first touch point */

	switch(tag)
	{
		case 0:
			toggle_lock = 0;
			break;

		case 10:
			if(toggle_lock == 0)
			{
				toggle_lock = 42;
				
				image++;
				if(image > 11)
				{
					image = 0;
				}
				
				EVE_cmd_memzero(0, 525000);
				
				switch(image)
				{
					case 0:
						EVE_cmd_flashread(0, 4096, 524288);
						break;
					case 1:
						EVE_cmd_flashread(0, 528384 , 438272);
						break;
					case 2:
						EVE_cmd_flashread(0, 966656 , 366400);
						break;
					case 3:
						EVE_cmd_flashread(0, 1333056, 327680);
						break;
					case 4:
						EVE_cmd_flashread(0, 1660736, 273920);
						break;
					case 5:
						EVE_cmd_flashread(0, 1934656, 204800);
						break;
					case 6:
						EVE_cmd_flashread(0, 2139456, 262144);
						break;
					case 7:
						EVE_cmd_flashread(0, 2401600, 219136);
						break;
					case 8:
						EVE_cmd_flashread(0, 2620736, 163840);
						break;
					case 9:
						EVE_cmd_flashread(0, 2784576, 131072);
						break;
					case 10:
						EVE_cmd_flashread(0, 2915648, 109568);
						break;
					case 11:
						EVE_cmd_flashread(0, 3025216, 92480);
						break;
				}
			}
			break;
	}
}


/*
	dynamic portion of display-handling, meant to be called every 20ms or more
*/
void TFT_display(void)
{
	if(tft_active != 0)
	{
		EVE_start_cmd_burst(); /* start writing to the cmd-fifo as one stream of bytes, only sending the address once */

		EVE_cmd_dl_burst(CMD_DLSTART); /* start the display list */
		EVE_cmd_dl_burst(DL_CLEAR_RGB | WHITE); /* set the default clear color to white */
		EVE_cmd_dl_burst(DL_CLEAR | CLR_COL | CLR_STN | CLR_TAG); /* clear the screen - this and the previous prevent artifacts between lists, Attributes are the color, stencil and tag buffers */
		EVE_cmd_dl_burst(VERTEX_FORMAT(0)); /* reduce precision for VERTEX2F to 1 pixel instead of 1/16 pixel default */
		EVE_cmd_dl_burst(DL_COLOR_RGB | WHITE);

		EVE_cmd_dl_burst(TAG(10));

		EVE_cmd_setbitmap_burst(0, EVE_COMPRESSED_RGBA_ASTC_5x5_KHR+image,  1280, 640);
		EVE_cmd_dl_burst(DL_BEGIN | EVE_BITMAPS);
		EVE_cmd_dl_burst(VERTEX2F(0, 0));
		EVE_cmd_dl_burst(DL_END);

		EVE_color_rgb_burst(BLACK);
		EVE_cmd_number_burst(770, 450, 30, 0, image);

		EVE_cmd_dl_burst(DL_DISPLAY); /* instruct the graphics processor to show the list */
		EVE_cmd_dl_burst(CMD_SWAP); /* make this list active */
		EVE_end_cmd_burst(); /* stop writing to the cmd-fifo, the cmd-FIFO will be executed automatically after this or when DMA is done */
	}
}
