
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
*
*/

#include "EVE_Platform.h"
#if (defined(FT9XX_PLATFORM)) \
    && (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))

//only write command
int16_t ILI9488_SPI_WriteCmd(uint8_t cmd, uint8_t data)
{
	uint8_t i;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	ILI9488_DCX_HIGH;
	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		if (i == 7)
		{
			ILI9488_DCX_HIGH;
		}
		if (data & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		data <<= 1;
	}

	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return 0;
}

//write N number of bytes
int16_t ILI9488_SPI_WriteCmdN(uint8_t cmd, uint8_t bytecount, uint8_t *pbuff)
{
	uint8_t i, j, transbit;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	ILI9488_DCX_HIGH;
	for (j = 0; j < bytecount; j++)
	{
		transbit = 0x80;
		for (i = 0; i < 8; i++)
		{
			ILI9488_CLK_LOW;
			if (*pbuff & transbit)
			{
				ILI9488_MOSI_HIGH;
			}
			else
			{
				ILI9488_MOSI_LOW;
			}
			usleep(1);
			ILI9488_CLK_HIGH;
			usleep(1);
			transbit >>= 1;
		}
		pbuff++;
	}

	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return 0;
}

//read/write data with cs being toggled
uint8_t ILI9488_SPI_Read(uint8_t cmd)
{
	uint8_t i, readbyte = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	ILI9488_DCX_HIGH;
	for (i = 0; i < 8; i++)
	{
		if (i == 7)
		{
			ILI9488_DCX_HIGH;
		}
		ILI9488_CLK_LOW;
		usleep(1);

		ILI9488_CLK_HIGH;
		usleep(1);

		readbyte <<= 1;

		if (ILI9488_MISO_LOWHIGH == true)
		{
			readbyte |= 0x1;
		}
	}

	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return readbyte;
}

uint8_t ILI9488_SPI_ReadN(uint8_t cmd, uint8_t numbytes, uint8_t *pbuffer)
{
	uint8_t i, readbyte = 0, j;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;
		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	ILI9488_DCX_HIGH;

	for (j = 0; j < numbytes + 1; j++)
	{
		readbyte = 0;
		for (i = 0; i < 8; i++)
		{
			if (i == 7)
			{
				ILI9488_DCX_HIGH;
			}
			ILI9488_CLK_LOW;
			usleep(1);

			ILI9488_CLK_HIGH;
			usleep(1);

			readbyte <<= 1;

			if (ILI9488_MISO_LOWHIGH == true)
			{
				readbyte |= 0x1;
			}
		}
		*pbuffer++ = readbyte;
	}

	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return readbyte;
}

//read 24 bits
uint32_t ILI9488_SPI_ReadRDDID(uint8_t cmd)
{
	uint8_t i;
	uint32_t readword = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);

	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;

		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);

	for (i = 0; i < 24; i++)
	{
		ILI9488_CLK_LOW;
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);

		if (ILI9488_MISO_LOWHIGH)
		{
			readword |= 0x1;
		}
		readword <<= 1;
	}

	ILI9488_CS_HIGH;

	return readword;
}
//read 32 bits
uint32_t ILI9488_SPI_ReadRDDST(uint8_t cmd)
{
	uint8_t i;
	uint32_t readword = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);

	for (i = 0; i < 8; i++)
	{
		ILI9488_CLK_LOW;

		if (cmd & 0x80)
		{
			ILI9488_MOSI_HIGH;
		}
		else
		{
			ILI9488_MOSI_LOW;
		}
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);
		cmd <<= 1;
	}
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);

	ILI9488_DCX_HIGH;
	for (i = 0; i < 32; i++)
	{
		ILI9488_CLK_LOW;
		usleep(1);
		ILI9488_CLK_HIGH;
		usleep(1);

		readword <<= 1;
		if (ILI9488_MISO_LOWHIGH)
		{
			readword |= 0x1;
		}
	}

	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;
	return readword;
}

void EVE_ILI9488_bootup()
{
	/* ILI9488 driver - configure pins for bit bang */
	gpio_function(27, pad_gpio27);
	gpio_function(28, pad_gpio28);
	gpio_function(29, pad_gpio29);
	gpio_function(30, pad_gpio30);
	gpio_function(34, pad_gpio34);
	gpio_function(33, pad_gpio33);

	gpio_function(43, pad_gpio43);

	gpio_dir(27, pad_dir_output); //gpios for ili9488 - spim clock
	gpio_dir(28, pad_dir_output);
	gpio_dir(29, pad_dir_output); //gpios for ili9488 - spim mosi
	gpio_dir(30, pad_dir_input); //gpios for ili9488 - spim miso
	gpio_dir(34, pad_dir_output); //gpios for ili9488 - dcx
	gpio_dir(33, pad_dir_output); //gpios for ili9488 - cs1#

	gpio_dir(43, pad_dir_output); //gpios for ili9488 - cs1#

	gpio_write(34, 1);
	gpio_write(28, 1);
	gpio_write(33, 1);
	/*gpio_write(34, 0);
	gpio_write(29, 0);
	gpio_write(27, 0);*/
	gpio_write(43, 1);
	gpio_write(34, 1);
	gpio_write(28, 1);
	gpio_write(43, 1);
	gpio_write(33, 1);
	gpio_write(33, 1);

	//display driver bring up
	{

		/* Temp buffer to construct the parameters */
		uint8_t arraytemp[16];

		ILI9488_SPI_WriteCmd(ILI9488_CMD_SOFTWARE_RESET, 0);
		usleep(120);

		//colomn address set - 0 to 319
		arraytemp[0] = 0x00;
		arraytemp[1] = 0x00;
		arraytemp[2] = 0x01;
		arraytemp[3] = 0x3f;
		ILI9488_SPI_WriteCmdN(ILI9488_CMD_COLOMNADDR, 4, arraytemp);

		//row address set - 0 to 479
		arraytemp[0] = 0x00;
		arraytemp[1] = 0x00;
		arraytemp[2] = 0x01;
		arraytemp[3] = 0xdf;
		ILI9488_SPI_WriteCmdN(ILI9488_CMD_ROWADDR, 4, arraytemp);

		//Frame rate 70HZ
		ILI9488_SPI_WriteCmd(ILI9488_CMD_FRAME_RATE_CONTROL, 0xB0);

		//adjust control 3
		arraytemp[0] = 0xa9;
		arraytemp[1] = 0x51;
		arraytemp[2] = 0x2c;
		arraytemp[3] = 0x82;
		ILI9488_SPI_WriteCmdN(0XF7, 4, arraytemp);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_MODE_CONTROL, 2);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_PIXEL_FORMAT, ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_IMAGEFUNCTION, 0);
		ILI9488_SPI_WriteCmd(ILI9488_CMD_WRITE_CONTROL_DISPLAY, 0x2c);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_MADCTRL, 0x48); //bgr connection and colomn address order

		arraytemp[0] = 0x30;
		arraytemp[1] = 0x02;
		arraytemp[2] = 0x3b;

		ILI9488_SPI_WriteCmdN(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL, 3, arraytemp);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_SLEEP_OUT, 0);
		usleep(120);

		ILI9488_SPI_WriteCmd(ILI9488_CMD_DISPLAYON, 0);
		ILI9488_SPI_WriteCmd(0x2c, 0xB0);

#if 0 //enable for debugging
		eve_printf_debug("ILI9488 id1 %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_ID1));
		eve_printf_debug("ILI9488 id2 %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_ID2));
		eve_printf_debug("ILI9488 id3 %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_ID3));
		eve_printf_debug("ILI9488 id4 %x\n", ILI9488_SPI_ReadRDDST(ILI9488_CMD_READ_ID4));
		eve_printf_debug("ILI9488 disp identy %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_DISPLAY_IDENTIFICATION));
		eve_printf_debug("ILI9488 disp power mode %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_DISPLAY_POWERMODE));
		eve_printf_debug("ILI9488 mad ctrl %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_MADCTRL));
		eve_printf_debug("ILI9488 pixel format %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_PIXEL_FORMAT));
		eve_printf_debug("ILI9488 disp signal mode %x\n", ILI9488_SPI_Read(ILI9488_READ_DISPLAY_SIGNALMODE));
		//with dummy byte read
		ILI9488_SPI_ReadN(ILI9488_CMD_READ_ID1, 1, arraytemp);
		eve_printf_debug("ILI9488 id1 %x %x\n", arraytemp[0], arraytemp[1]);
		ILI9488_SPI_ReadN(ILI9488_CMD_READ_DISPLAY_IDENTIFICATION, 3, arraytemp);
		eve_printf_debug("ILI9488 id4 %x %x %x %x\n", arraytemp[0], arraytemp[1], arraytemp[2], arraytemp[3]);

		eve_printf_debug("ILI9488 pixel format %x\n", ILI9488_SPI_Read(ILI9488_CMD_READ_PIXEL_FORMAT));
		ILI9488_SPI_ReadN(ILI9488_CMD_READ_PIXEL_FORMAT, 1, arraytemp);
		eve_printf_debug("ILI9488 pixel format n %x %x\n", arraytemp[0], arraytemp[1]);
#endif
	}
}

#endif

/* end of file */
