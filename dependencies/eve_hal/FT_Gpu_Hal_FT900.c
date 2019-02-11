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
*/

#include "FT_Platform.h"
#if defined(FT900_PLATFORM)

// platform specific
// Ft_Gpu_HostCommand
// Ft_Gpu_HostCommand_Ext3
// Ft_Gpu_Hal_Powercycle
// Ft_Gpu_Hal_SetSPI
// Ft_Gpu_CurrentFrequency

// ft900 only
// Ft_Gpu_Panl70_GOODIXGPIO

ft_void_t Ft_Gpu_HostCommand(EVE_HalContext *phost, ft_uint8_t cmd)
{
	ft_uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd;
	hcmd[1] = 0;
	hcmd[2] = 0;
	hcmd[3] = 0;

	spi_open(SPIM, phost->Parameters.SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->Parameters.SpiCsPin);
}

//This API sends a 3byte command to the phost
ft_void_t Ft_Gpu_HostCommand_Ext3(EVE_HalContext *phost, ft_uint32_t cmd)
{
	ft_uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd & 0xff;
	hcmd[1] = (cmd >> 8) & 0xff;
	hcmd[2] = (cmd >> 16) & 0xff;
	hcmd[3] = 0;
	spi_open(SPIM, phost->Parameters.SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->Parameters.SpiCsPin);
}

/* Toggle PD_N pin of FT800 board for a power cycle*/
ft_void_t Ft_Gpu_Hal_Powercycle(EVE_HalContext *phost, ft_bool_t up)
{
	if (up)
	{
		gpio_write(phost->Parameters.PowerDownPin, 0);
		EVE_sleep(20);
		gpio_write(phost->Parameters.PowerDownPin, 1);
		EVE_sleep(20);
	}
	else
	{
		gpio_write(phost->Parameters.PowerDownPin, 1);
		EVE_sleep(20);
		gpio_write(phost->Parameters.PowerDownPin, 0);
		EVE_sleep(20);
	}
}

ft_int16_t Ft_Gpu_Hal_SetSPI(EVE_HalContext *phost, FT_GPU_SPI_NUMCHANNELS_T numchnls, FT_GPU_SPI_NUMDUMMYBYTES numdummy)
{
	ft_uint8_t writebyte = 0;

	if ((numchnls > FT_GPU_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
	{
		return -1; //error
	}

	//swicth EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= FT_SPI_TWO_DUMMY_BYTE;
	Ft_Gpu_Hal_Wr8(phost, REG_SPI_WIDTH, writebyte);
	//FT81x swicthed to dual/quad mode, now update global HAL context
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
	return 0;
}

ft_uint32_t Ft_Gpu_CurrentFrequency(EVE_HalContext *phost)
{
	ft_uint32_t t0, t1;
	ft_uint32_t addr = REG_CLOCK;
	ft_uint8_t spidata[4];
	ft_int32_t r = 15625;

	t0 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t0 read */

	__asm__(
	    "   move.l  $r0,%0"
	    "\n\t"
	    "   mul.l   $r0,$r0,100"
	    "\n\t"
	    "1:"
	    "\n\t"
	    "   sub.l   $r0,$r0,3"
	    "\n\t" /* Subtract the loop time = 4 cycles */
	    "   cmp.l   $r0,0"
	    "\n\t" /* Check that the counter is equal to 0 */
	    "   jmpc    gt, 1b"
	    "\n\t"
	    /* Outputs */
	    :
	    /* Inputs */
	    : "r"(r)
	    /* Using */
	    : "$r0"

	);

	//usleep(15625);
	//EVE_sleep(15625);

	t1 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

ft_void_t Ft_Gpu_Panl70_GOODIXGPIO(EVE_HalContext *phost)
{
#if defined(PANL70) || defined(PANL70PLUS)
	gpio_function(GOODIXGPIO, pad_gpio33);
	gpio_dir(GOODIXGPIO, pad_dir_output);
	gpio_write(GOODIXGPIO, 0);
	EVE_sleep(1);
	Ft_Gpu_Hal_Wr8(phost, REG_CPURESET, 0);
	EVE_sleep(100);
	gpio_dir(GOODIXGPIO, pad_dir_input);
#endif
}
#endif