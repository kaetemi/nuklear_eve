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

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(FT900_PLATFORM)

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	g_HalPlatform.TotalChannels = 1;

	// Initialize SPIM HW
	sys_enable(sys_device_spi_master);

	gpio_function(27, pad_spim_sck); /* GPIO27 to SPIM_CLK */
	gpio_function(28, pad_spim_ss0); /* GPIO28 as CS */
	gpio_function(29, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(30, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_dir(27, pad_dir_output);
	gpio_dir(28, pad_dir_output);
	gpio_dir(29, pad_dir_output);
	gpio_dir(30, pad_dir_input);
#if (defined(ENABLE_SPI_QUAD))
	/* Initialize IO2 and IO3 pad/pin for quad settings */
	gpio_function(31, pad_spim_io2); /* GPIO31 to IO2 */
	gpio_function(32, pad_spim_io3); /* GPIO32 to IO3 */
	gpio_dir(31, pad_dir_output);
	gpio_dir(32, pad_dir_output);
#endif

#if defined(PANL70) || defined(PANL70PLUS)
	gpio_function(GOODIXGPIO, pad_gpio33);
	gpio_dir(GOODIXGPIO, pad_dir_output);
	gpio_write(GOODIXGPIO, 1);
#endif

	gpio_write(28, 1);

#ifdef EVDEMO
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);
#else
	spi_init(SPIM, spi_dir_master, spi_mode_0, 16); //SPISysInit(SPIM);
#endif
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	spi_uninit(SPIM);
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	parameters->MpsseChannelNo = 0;
	parameters->PowerDownPin = FT800_PD_N;
	parameters->SpiCsPin = FT800_SEL_PIN;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	gpio_function(phost->Parameters.SpiCsPin, pad_spim_ss0); /* GPIO28 as CS */
	gpio_write(phost->Parameters.SpiCsPin, 1);

	gpio_function(phost->Parameters.PowerDownPin, pad_gpio43);
	gpio_dir(phost->Parameters.PowerDownPin, pad_dir_output);

	gpio_write(phost->Parameters.PowerDownPin, 1);

	/* Initialize the context valriables */
	phost->SpiNumDummy = 1; //by default ft800/801/810/811 goes with single dummy byte for read
	phost->SpiChannel = FT_GPU_SPI_SINGLE_CHANNEL;
	phost->Status = EVE_HalStatusOpened;
	++g_HalPlatform.OpenedChannels;

	return FT_TRUE;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = FT_GPU_HAL_CLOSED;
	--g_HalPlatform.OpenedChannels;
	/* spi_close(SPIM,0); */
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
#if defined(EVE_MODULE_PANL)
	panl_bacnet_task();
#endif
}

/*************
** TRANSFER **
*************/

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_HalTransfer rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_HalStatusOpened);

	/* no-op */
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_HalStatusReading || phost->Status == EVE_HalStatusWriting);

	/* no-op */
}

static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	/* no-op */
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint16_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (ft_uint16_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	return retVal;
}

uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint32_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (ft_uint32_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	retVal |= (ft_uint32_t)transfer8(phost, (value >> 16) & 0xFF) << 16;
	retVal |= (ft_uint32_t)transfer8(phost, value >> 24) << 24;
	return retVal;
}

void EVE_Hal_transferBuffer(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (result && buffer)
	{
		for (uint32_t i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (uint32_t i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (uint32_t i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
}

void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	/*
	if (result && buffer)
	{
		for (uint32_t i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (uint32_t i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (uint32_t i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
	*/
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	uint32_t transferred;
	for (transferred = 0; transferred < size;)
	{
		char c = str[index + (transferred++)];
		// putchar(c ? c : '_');
		transfer8(phost, c);
		if (!c)
			break;
	}
	while (transferred & padMask)
	{
		++transferred;
		// putchar('_');
		transfer8(phost, 0);
	}
	return transferred;
}

#endif /* #if defined(FT900_PLATFORM) */

/* end of file */