/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : BRIDGETEK
Abstract: ILI9488 driver source

Revision History: 
0.1 - date 2015.05.08 - Initial Version
0.2 - data 2017.03.23 - Use SPI library instead of GPIO handle
*/

#include "Platform.h"
#ifdef ENABLE_ILI9488_HVGA_PORTRAIT

#define ILI9488_SEL 1
//only write command
int16_t ILI9488_SPI_WriteCmd(uint8_t cmd, uint8_t data)
{
    uint8_t i;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_writen(SPIM, &data, 1);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;
	spi_close(SPIM, ILI9488_SEL);

	return 0;
}

//write N number of bytes
int16_t ILI9488_SPI_WriteCmdN(uint8_t cmd, uint8_t bytecount,uint8_t *pbuff)
{
		uint8_t i,j,transbit;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;

	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_writen(SPIM, pbuff, bytecount);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return 0;
}

//read/write data with cs being toggled
uint8_t ILI9488_SPI_Read(uint8_t cmd)
{
	uint8_t i,readbyte = 0;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_readn(SPIM, &readbyte, 1);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;

	return readbyte;
}

uint8_t ILI9488_SPI_ReadN(uint8_t cmd,uint8_t numbytes,uint8_t *pbuffer)
{
	uint8_t i,readbyte = 0,j;
	/* bit banging mechanism */
	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;
	usleep(1);
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
	ILI9488_DCX_HIGH;
	spi_readn(SPIM, pbuffer, numbytes);
	spi_close(SPIM, ILI9488_SEL);
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
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
#if 0
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);
#endif
	ILI9488_DCX_HIGH;
	//NOTE: for little-endian, this is fine
	spi_readn(SPIM, &readword, 3);
	spi_close(SPIM, ILI9488_SEL);
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
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &cmd, 1);
#if 0
	//extra clock cycle for dummy
	ILI9488_CLK_LOW;
	usleep(1);
	ILI9488_CLK_HIGH;
	usleep(1);
#endif

	ILI9488_DCX_HIGH;
	spi_readn(SPIM, &readword, 4);
	spi_close(SPIM, ILI9488_SEL);
	ILI9488_CS_HIGH;
	ILI9488_DCX_LOW;
	return readword;
}

void ILI9488_Bootup()
{

#if (defined(FT900_PLATFORM) || defined(FT93X_PLATFORM))

	/* ILI9488 driver - configure pins for bit bang */
	gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
	gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
	gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
	gpio_function(GPIO_SPIM_MISO, pad_spim_miso);
	gpio_function(GPIO_ILI9488_DCX, pad_ili9488_dcx);
	gpio_function(GPIO_ILI9488_CS1, pad_ili9488_cs1);

	gpio_function(GPIO_PWD, pad_pwd);

	gpio_dir(GPIO_ILI9488_DCX,pad_dir_output);//gpios for ili9488 - dcx
	gpio_dir(GPIO_ILI9488_CS1,pad_dir_output);//gpios for ili9488 - cs1#

	gpio_dir(GPIO_PWD,pad_dir_output);//gpios for ili9488 - pwd#

	gpio_write(GPIO_ILI9488_DCX, 1);
	gpio_write(GPIO_SPIM_SS0, 1);
	gpio_write(GPIO_ILI9488_CS1, 1);
	gpio_write(GPIO_PWD,1);

        /* Enable the SPI Master device... */

	sys_enable(sys_device_spi_master);
    spi_init(SPIM, spi_dir_master, spi_mode_0, 64);
	spi_option(SPIM,spi_option_fifo_size,16);
	spi_option(SPIM,spi_option_fifo,1);
    spi_option(SPIM, spi_option_bus_width, 1);
#endif
	//display driver bring up
	{

       /* Temp buffer to construct the parameters */
       uint8_t arraytemp[16];

       ILI9488_SPI_WriteCmd(ILI9488_CMD_SOFTWARE_RESET,0);
       usleep(120);

       //colomn address set - 0 to 319
       arraytemp[0] = 0x00;
       arraytemp[1] = 0x00;
       arraytemp[2] = 0x01;
       arraytemp[3] = 0x3f;
       ILI9488_SPI_WriteCmdN(ILI9488_CMD_COLOMNADDR,4,arraytemp);

	//row address set - 0 to 479
	arraytemp[0] = 0x00;
	arraytemp[1] = 0x00;
	arraytemp[2] = 0x01;
	arraytemp[3] = 0xdf;
	ILI9488_SPI_WriteCmdN(ILI9488_CMD_ROWADDR,4,arraytemp);

	//Frame rate 70HZ
	ILI9488_SPI_WriteCmd(ILI9488_CMD_FRAME_RATE_CONTROL,0xB0);

	//adjust control 3
	arraytemp[0] = 0xa9;
	arraytemp[1] = 0x51;
	arraytemp[2] = 0x2c;
	arraytemp[3] = 0x82;
	ILI9488_SPI_WriteCmdN(0XF7,4,arraytemp);

	ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_MODE_CONTROL,2);
	ILI9488_SPI_WriteCmd(ILI9488_CMD_INTERFACE_PIXEL_FORMAT,ILI9488_INTERFACE_PIXEL_FORMAT_18BIT_DPI);
	ILI9488_SPI_WriteCmd(ILI9488_CMD_IMAGEFUNCTION,0);
	ILI9488_SPI_WriteCmd(ILI9488_CMD_WRITE_CONTROL_DISPLAY,0x2c);

	ILI9488_SPI_WriteCmd(ILI9488_CMD_MADCTRL,0x48);//bgr connection and colomn address order

	arraytemp[0] = 0x30;
	arraytemp[1] = 0x02;
	arraytemp[2] = 0x3b;

       ILI9488_SPI_WriteCmdN(ILI9488_CMD_DISPLAY_FUNCTION_CONTROL,3,arraytemp);


       ILI9488_SPI_WriteCmd(ILI9488_CMD_SLEEP_OUT,0);
       usleep(120);

       ILI9488_SPI_WriteCmd(ILI9488_CMD_DISPLAYON,0);
       ILI9488_SPI_WriteCmd(0x2c,0xB0);

#if 1//enable for debugging
	printf("ILI9488 id1 %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_ID1));
	printf("ILI9488 id2 %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_ID2));
	printf("ILI9488 id3 %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_ID3));
	printf("ILI9488 id4 %x \n",ILI9488_SPI_ReadRDDST(ILI9488_CMD_READ_ID4));
	printf("ILI9488 disp identy %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_DISPLAY_IDENTIFICATION));
	printf("ILI9488 disp power mode %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_DISPLAY_POWERMODE));
	printf("ILI9488 mad ctrl %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_MADCTRL));
	printf("ILI9488 pixel format %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_PIXEL_FORMAT));
	printf("ILI9488 disp signal mode %x \n",ILI9488_SPI_Read(ILI9488_READ_DISPLAY_SIGNALMODE));
	//with dummy byte read
	ILI9488_SPI_ReadN(ILI9488_CMD_READ_ID1,1,arraytemp);
	printf("ILI9488 id1 %x %x\n",arraytemp[0],arraytemp[1]);
	ILI9488_SPI_ReadN(ILI9488_CMD_READ_DISPLAY_IDENTIFICATION,3,arraytemp);
	printf("ILI9488 id4 %x %x %x %x\n",arraytemp[0],arraytemp[1],arraytemp[2],arraytemp[3]);

	printf("ILI9488 pixel format %x \n",ILI9488_SPI_Read(ILI9488_CMD_READ_PIXEL_FORMAT));
	ILI9488_SPI_ReadN(ILI9488_CMD_READ_PIXEL_FORMAT,1,arraytemp);
	printf("ILI9488 pixel format n %x %x\n",arraytemp[0],arraytemp[1]);
#endif

    }
}

void write_data(unsigned char w) {
	spi_open(SPIM, ILI9488_SEL);
	spi_writen(SPIM, &w, 1);
	spi_close(SPIM, ILI9488_SEL);
}

void write_command (unsigned int y)
{

	ILI9488_DCX_LOW;
	ILI9488_CS_LOW;

	write_data(0x20); //high 8bit
	write_data(y>>8);
	write_data(0x00); //low 8bit
	write_data(y & 0xFF);
	ILI9488_CS_HIGH;
}
#endif
