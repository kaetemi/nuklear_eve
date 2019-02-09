/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*
* Abstract: ILI9488 driver source
*/

#include "FT_Platform.h"
#if defined(FT900_PLATFORM) && defined(ENABLE_KD2401_HVGA_PORTRAIT)

void write_data(unsigned char w)
{
	unsigned char i;
	ILI9488_CS_LOW;
	ILI9488_CLK_LOW;
	ILI9488_MOSI_HIGH;
	usleep(1);
	ILI9488_CLK_HIGH;

	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		usleep(1);

		if (w & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		ILI9488_CLK_HIGH;
		usleep(1);
		w = w << 1;
	}
	ILI9488_CS_HIGH;
}

void write_command(unsigned char y)
{
	unsigned char i;
	ILI9488_CS_LOW;
	ILI9488_CLK_LOW;
	ILI9488_MOSI_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;

	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		usleep(1);

		if (y & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		ILI9488_CLK_HIGH;
		usleep(1);
		y = y << 1;
	}
	ILI9488_CS_HIGH;
}

ft_void_t KD2401_Bootup()
{

	/* TODO - use the method exposed by EVE HAL library and panlbsp for setting the
	 * eve and eflash into default states - mainly the CS to be high */
	/* ILI9488 driver - configure pins for bit bang */
	gpio_function(27, pad_gpio27);
	gpio_function(28, pad_gpio28); //cs0 for eve
	gpio_function(29, pad_gpio29);
	gpio_function(30, pad_gpio30);
	gpio_function(33, pad_gpio33); //ss1 for KD2401
	gpio_function(35, pad_gpio35); //ss3 for eflash
	gpio_function(1, pad_gpio1); //power down of FT81x

	gpio_dir(27, pad_dir_output); //gpios for ili9488 - spim clock
	gpio_dir(28, pad_dir_output); //cs0
	gpio_dir(29, pad_dir_output); //gpios for ili9488 - spim mosi
	gpio_dir(30, pad_dir_input); //gpios for ili9488 - spim miso
	gpio_dir(33, pad_dir_output); //gpios for ili9488 - cs1#
	gpio_dir(35, pad_dir_output); //gpios for eflash - ss3#
	gpio_dir(1, pad_dir_output); //power down

#if 1
	/* Set the default state of the GPIO */
	gpio_write(28, 1);
	gpio_write(33, 1);
	gpio_write(35, 1);
	gpio_write(1, 1);
	gpio_write(28, 1);
	gpio_write(1, 1);
	gpio_write(33, 1);
	gpio_write(35, 1);
#endif

	//display driver bring up
	{
		/***************************************/
		write_command(0xC0);
		write_data(0x17);
		write_data(0x17);

		write_command(0xC1);
		write_data(0x44);

		write_command(0xC5);
		write_data(0x00);
		write_data(0x3A); //
		write_data(0x80);

		write_command(0x36);
		write_data(0x48);

		write_command(0x3A); //Interface Mode Control
		write_data(0x60);

		write_command(0xB1); //Frame rate 70HZ
		write_data(0xA0);

		write_command(0xB4);
		write_data(0x02);

		write_command(0xB7);
		write_data(0xC6);

		write_command(0xE9);
		write_data(0x00);

		write_command(0XF7);
		write_data(0xA9);
		write_data(0x51);
		write_data(0x2C);
		write_data(0x82);

		write_command(0xE0);
		write_data(0x01);
		write_data(0x13);
		write_data(0x1E);
		write_data(0x00);
		write_data(0x0D);
		write_data(0x03);
		write_data(0x3D);
		write_data(0x55);
		write_data(0x4F);
		write_data(0x06);
		write_data(0x10);
		write_data(0x0B);
		write_data(0x2C);
		write_data(0x32);
		write_data(0x0F);

		write_command(0xE1);
		write_data(0x08);
		write_data(0x10);
		write_data(0x15);
		write_data(0x03);
		write_data(0x0E);
		write_data(0x03);
		write_data(0x32);
		write_data(0x34);
		write_data(0x44);
		write_data(0x07);
		write_data(0x10);
		write_data(0x0E);
		write_data(0x23);
		write_data(0x2E);
		write_data(0x0F);

		/**********set rgb interface mode******************/
		write_command(0xB6);
		write_data(0x30); //set rgb
		write_data(0x02); //GS,SS
		write_data(0x3B);

		write_command(0XB0); //Interface Mode Control
		write_data(0x00);
		/**************************************************/
		write_command(0x2A); //Frame rate control
		write_data(0x00);
		write_data(0x00);
		write_data(0x01);
		write_data(0x3F);

		write_command(0x2B); //Display function control
		write_data(0x00);
		write_data(0x00);
		write_data(0x01);
		write_data(0xDF);

		write_command(0x21);

		write_command(0x11);
		delayms(120);
		write_command(0x29); //display on
		write_command(0x2c);

#if 0
		write_command(0x11);
		delayms(150);
		write_command(0x29);
		delayms(50);
#endif
	}
}

#endif

/* end of file */
