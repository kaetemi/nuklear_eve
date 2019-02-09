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

#include "FT_Gpu_Hal_FT900.h"

/*The APIs for reading/writing transfer continuously only with small buffer system*/
ft_void_t Ft_Gpu_Hal_StartTransfer(EVE_HalContext *phost, FT_GPU_TRANSFERDIR_T rw, ft_uint32_t addr)
{
	eve_assert(!(phost->CmdFrame && (addr == REG_CMDB_WRITE)));
	if (phost->Status != FT_GPU_HAL_OPENED)
	{
		eve_printf_debug("Cannot start transfer on current host status %i\n", (int)phost->Status);
	}
	if (FT_GPU_READ == rw)
	{
		ft_uint8_t spidata[4];
		spidata[0] = (addr >> 16);
		spidata[1] = (addr >> 8);
		spidata[2] = addr & 0xff;
		spi_open(SPIM, phost->Parameters.SpiCsPin);
		spi_writen(SPIM, spidata, 3 + phost->SpiDummyBytes);
		phost->Status = FT_GPU_HAL_READING;
	}
	else
	{
		ft_uint8_t spidata[4];
		spidata[0] = (0x80 | (addr >> 16));
		spidata[1] = (addr >> 8);
		spidata[2] = addr;

		spi_open(SPIM, phost->Parameters.SpiCsPin);
		spi_writen(SPIM, spidata, 3);

		phost->Status = FT_GPU_HAL_WRITING;
	}
}

ft_uint8_t Ft_Gpu_Hal_Transfer8(EVE_HalContext *phost, ft_uint8_t value)
{
	if (phost->Status == FT_GPU_HAL_WRITING)
	{
		spi_write(SPIM, value);
		return 0;
	}
	else
	{
		spi_read(SPIM, value);
		return value;
	}
}

ft_void_t Ft_Gpu_Hal_EndTransfer(EVE_HalContext *phost)
{
	spi_close(SPIM, phost->Parameters.SpiCsPin);
	phost->Status = FT_GPU_HAL_OPENED;
}

ft_uint8_t Ft_Gpu_Hal_Rd8(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint8_t value;

	ft_uint8_t spiData[1];
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	spi_readn(SPIM, spiData, 1);
	value = spiData[0];

	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}

ft_uint16_t Ft_Gpu_Hal_Rd16(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint16_t value;
	ft_uint8_t spiData[2];
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	spi_readn(SPIM, spiData, 2);
	value = spiData[0] | (spiData[1] << 8);
	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}

ft_uint32_t Ft_Gpu_Hal_Rd32(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint32_t value;
	ft_uint8_t spiData[4];
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	spi_readn(SPIM, spiData, 4);
	value = (spiData[3] << 24) | (spiData[2] << 16) | (spiData[1] << 8) | spiData[0];

	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}

ft_void_t Ft_Gpu_Hal_Wr8(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer8(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Wr16(EVE_HalContext *phost, ft_uint32_t addr, ft_uint16_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer16(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Wr32(EVE_HalContext *phost, ft_uint32_t addr, ft_uint32_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer32(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

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

ft_bool_t Ft_Gpu_Hal_WrCmdBuf(EVE_HalContext *phost, ft_uint8_t *buffer, ft_uint32_t count)
{
	ft_int32_t length = 0, availablefreesize = 0;
	count = (count + 3) & ~0x3;
	do
	{
		length = count;
		availablefreesize = Ft_Gpu_Hal_Rd16(phost, REG_CMDB_SPACE) & FIFO_SIZE_MASK; // max: 4095 ; last 4's multiple = 4092
		if (FT_COCMD_FAULT(availablefreesize))
			return FT_FALSE; // Co processor fault
		if (length > availablefreesize)
			length = availablefreesize;

		Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
		spi_writen(SPIM, buffer, length);
		buffer += length;

		Ft_Gpu_Hal_EndTransfer(phost);
		//printf("\n Length: %d", length);
		count -= length;
	} while (count > 0);
	//printf("\n\n\n");
	return FT_TRUE;
}

ft_bool_t Ft_Gpu_Hal_WrCmdBuf_ProgMem(EVE_HalContext *phost, ft_prog_uchar8_t *buffer, ft_uint32_t count)
{
	eve_assert(!phost->CmdFrame);
	eve_assert(!((uintptr_t)buffer & 0x3)); // must have 32-bit alignment
	ft_uint32_t FT_PROGMEM_CONST *buf32 = (ft_uint32_t FT_PROGMEM_CONST *)(void FT_PROGMEM_CONST *)(buffer);
	ft_int32_t length = 0, availablefreesize = 0;
	count = (count + 3) & ~0x3UL;
	do
	{
		length = count;
		availablefreesize = Ft_Gpu_Hal_Rd16(phost, REG_CMDB_SPACE) & FIFO_SIZE_MASK;
		if (FT_COCMD_FAULT(availablefreesize))
			return FT_FALSE; // Co processor fault
		if (length > availablefreesize)
			length = availablefreesize;
		if (length)
		{
			count -= length;
			length >>= 2;
			Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
			for (int i = 0; i < length; ++i)
				Ft_Gpu_Hal_Transfer32(phost, *(buf32++));
			Ft_Gpu_Hal_EndTransfer(phost);
		}
	} while (count > 0);
	return FT_TRUE;
}

ft_void_t Ft_Gpu_Hal_WrCmd32(EVE_HalContext *phost, ft_uint32_t cmd)
{
	Ft_Gpu_Hal_Wr32(phost, REG_CMDB_WRITE, cmd);
}

/* Toggle PD_N pin of FT800 board for a power cycle*/
ft_void_t Ft_Gpu_Hal_Powercycle(EVE_HalContext *phost, ft_bool_t up)
{
	if (up)
	{
		gpio_write(phost->Parameters.PowerDownPin, 0);
		ft_delay(20);
		gpio_write(phost->Parameters.PowerDownPin, 1);
		ft_delay(20);
	}
	else
	{
		gpio_write(phost->Parameters.PowerDownPin, 1);
		ft_delay(20);
		gpio_write(phost->Parameters.PowerDownPin, 0);
		ft_delay(20);
	}
}

ft_void_t Ft_Gpu_Hal_WrMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_uint8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	spi_writen(SPIM, buffer, length);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_WrMem_ProgMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_prog_uchar8_t *buffer, ft_uint32_t length)
{
	eve_assert(!((uintptr_t)buffer & 0x3)); // must be 32-bit aligned
	eve_assert(!(length & 0x3)); // must be 32-bit aligned
	uint32_t FT_PROGMEM_CONST *buf32 = (uint32_t FT_PROGMEM_CONST *)(void FT_PROGMEM_CONST *)buffer;
	length >>= 2;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	while (length--)
		Ft_Gpu_Hal_Transfer32(phost, *(buf32++));
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	spi_readn(SPIM, buffer, length);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Sleep(ft_uint32_t ms)
{
	delayms(ms);
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
	if (numdummy == FT_GPU_SPI_TWODUMMY)
		writebyte |= FT_SPI_TWO_DUMMY_BYTE;
	Ft_Gpu_Hal_Wr8(phost, REG_SPI_WIDTH, writebyte);
	//FT81x swicthed to dual/quad mode, now update global HAL context
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
	return 0;
}

ft_void_t getFlashTextString(char __flash__ *str, ft_uchar8_t *destArray, ft_uint16_t numOfChars)
{
	ft_uint16_t i;
	for (i = 0; i < numOfChars; i++)
		destArray[i] = str[i];
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
	//ft_delay(15625);

	t1 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

ft_void_t Ft_Gpu_Panl70_GOODIXGPIO(EVE_HalContext *phost)
{
#if defined(PANL70) || defined(PANL70PLUS)
	gpio_function(GOODIXGPIO, pad_gpio33);
	gpio_dir(GOODIXGPIO, pad_dir_output);
	gpio_write(GOODIXGPIO, 0);
	ft_delay(1);
	Ft_Gpu_Hal_Wr8(phost, REG_CPURESET, 0);
	ft_delay(100);
	gpio_dir(GOODIXGPIO, pad_dir_input);
#endif
}

ft_void_t Ft_DisplayPanel_Init()
{
#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	ILI9488_Bootup();
	eve_printf_debug("after ILI9488 bootup\n");
#endif

#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	KD2401_Bootup();
	eve_printf_debug("after KD2401 bootup\n");
#endif

	/* Reconfigure the SPI */
	// Initialize SPIM HW
	sys_enable(sys_device_spi_master);
	gpio_function(27, pad_spim_sck); /* GPIO27 to SPIM_CLK */
	gpio_function(28, pad_spim_ss0); /* GPIO28 as CS */
	gpio_function(29, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(30, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_write(28, 1);
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);
}
#endif