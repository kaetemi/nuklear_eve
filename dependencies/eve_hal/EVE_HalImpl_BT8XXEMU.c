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
#if defined(BT8XXEMU_PLATFORM)

#include <bt8xxemu.h>

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	g_HalPlatform.TotalDevices = 1;
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	/* no-op */
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	/* no-op */
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	bool ret;

#if defined(EVE_EMULATOR_MAIN)
	phost->Emulator = EVE_GpuEmu;
	phost->EmulatorFlash = EVE_EmuFlash;
#else
	BT8XXEMU_EmulatorParameters params;
	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, Ft_Emulator_Mode());

	params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, &params);
#endif

#if defined(ESD_SIMULATION)
	Ft_MainReady__ESD(phost->Emulator);
#endif

	ret = !!phost->Emulator;
	if (ret)
	{
		phost->Status = EVE_STATUS_OPENED;
		++g_HalPlatform.OpenedDevices;
	}
	return ret;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
#if !defined(EVE_EMULATOR_MAIN)
	if (phost->Emulator)
	{
		BT8XXEMU_stop(phost->Emulator);
		BT8XXEMU_destroy(phost->Emulator);
	}
	phost->Emulator = NULL;
	phost->EmulatorFlash = NULL;
#else
	phost->Emulator = NULL;
	phost->EmulatorFlash = NULL;
#endif

	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

/*************
** TRANSFER **
*************/

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	if (rw == EVE_TRANSFER_READ)
	{
		BT8XXEMU_chipSelect(phost->Emulator, 1);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 16) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, addr & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, 0); // Dummy Read Byte
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		BT8XXEMU_chipSelect(phost->Emulator, 1);
		BT8XXEMU_transfer(phost->Emulator, ((addr >> 16) & 0xFF) | 0x80);
		BT8XXEMU_transfer(phost->Emulator, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->Emulator, addr & 0xFF);
		phost->Status = EVE_STATUS_WRITING;
	}
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	BT8XXEMU_chipSelect(phost->Emulator, 0);
	phost->Status = EVE_STATUS_OPENED;
}

static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	return BT8XXEMU_transfer(phost->Emulator, value);
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint16_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (uint16_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	return retVal;
}

uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint32_t retVal = 0;
	retVal = transfer8(phost, value & 0xFF);
	retVal |= (uint32_t)transfer8(phost, (value >> 8) & 0xFF) << 8;
	retVal |= (uint32_t)transfer8(phost, (value >> 16) & 0xFF) << 16;
	retVal |= (uint32_t)transfer8(phost, value >> 24) << 24;
	return retVal;
}

void EVE_Hal_transferBuffer(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	uint32_t i;
	if (result && buffer)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
}

void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	uint32_t i;
	if (result && buffer)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, buffer[i]);
	}
	else if (result)
	{
		for (i = 0; i < size; ++i)
			result[i] = transfer8(phost, 0);
	}
	else if (buffer)
	{
		for (i = 0; i < size; ++i)
			transfer8(phost, buffer[i]);
	}
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		while (transferred < size)
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
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_prepareSpiMaster(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

bool EVE_UtilImpl_postBootupConfig(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
