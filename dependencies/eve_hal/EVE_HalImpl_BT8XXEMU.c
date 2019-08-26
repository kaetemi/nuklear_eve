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

#if defined(EVE_MULTI_TARGET)
#define EVE_HalImpl_initialize EVE_HalImpl_BT8XXEMU_initialize
#define EVE_HalImpl_release EVE_HalImpl_BT8XXEMU_release
#define EVE_Hal_list EVE_Hal_BT8XXEMU_list
#define EVE_Hal_info EVE_Hal_BT8XXEMU_info
#define EVE_Hal_isDevice EVE_Hal_BT8XXEMU_isDevice
#define EVE_HalImpl_defaults EVE_HalImpl_BT8XXEMU_defaults
#define EVE_HalImpl_open EVE_HalImpl_BT8XXEMU_open
#define EVE_HalImpl_close EVE_HalImpl_BT8XXEMU_close
#define EVE_HalImpl_idle EVE_HalImpl_BT8XXEMU_idle
#define EVE_Hal_flush EVE_Hal_BT8XXEMU_flush
#define EVE_Hal_startTransfer EVE_Hal_BT8XXEMU_startTransfer
#define EVE_Hal_endTransfer EVE_Hal_BT8XXEMU_endTransfer
#define EVE_Hal_transfer8 EVE_Hal_BT8XXEMU_transfer8
#define EVE_Hal_transfer16 EVE_Hal_BT8XXEMU_transfer16
#define EVE_Hal_transfer32 EVE_Hal_BT8XXEMU_transfer32
#define EVE_Hal_transferMem EVE_Hal_BT8XXEMU_transferMem
#define EVE_Hal_transferProgmem EVE_Hal_BT8XXEMU_transferProgmem
#define EVE_Hal_transferString EVE_Hal_BT8XXEMU_transferString
#define EVE_Hal_hostCommand EVE_Hal_BT8XXEMU_hostCommand
#define EVE_Hal_hostCommandExt3 EVE_Hal_BT8XXEMU_hostCommandExt3
#define EVE_Hal_powerCycle EVE_Hal_BT8XXEMU_powerCycle
#define EVE_UtilImpl_bootupDisplayGpio EVE_UtilImpl_BT8XXEMU_bootupDisplayGpio
#define EVE_Hal_setSPI EVE_Hal_BT8XXEMU_setSPI
#endif

#include <bt8xxemu.h>

/*********
** INIT **
*********/

#if defined(ESD_SIMULATION)
void Ft_MainReady__ESD(BT8XXEMU_Emulator *emulator);
#endif

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	// TODO: g_HalPlatform.TotalDevices = 1;
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	/* no-op */
	// sizeof(BT8XXEMU_EmulatorParameters) == 1640
	// sizeof(BT8XXEMU_FlashParameters) == 1144
}

/* List the available devices */
size_t EVE_Hal_list()
{
	return 1;
}

/* Get info of the specified device */
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	strcpy_s(deviceInfo->DisplayName,
	    sizeof(deviceInfo->DisplayName), "BT8XX Emulator");
	strcpy_s(deviceInfo->SerialNumber,
	    sizeof(deviceInfo->SerialNumber), "BT8XXEMU");
	deviceInfo->Opened = false;
	deviceInfo->Host = EVE_HOST_BT8XXEMU;
}

/* Check whether the context is the specified device */
bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	if (!phost)
		return false;
	if (phost->Host != EVE_HOST_BT8XXEMU)
		return false;
	return true;
}

/* Get the default configuration parameters */
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, EVE_CHIPID_T chipId, size_t deviceIdx)
{
	BT8XXEMU_EmulatorParameters *params = (void *)parameters->EmulatorParameters;
	if (sizeof(BT8XXEMU_EmulatorParameters) > sizeof(parameters->EmulatorParameters))
		return false;

	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, params, chipId);
	params->Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	return true;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	bool ret;
	BT8XXEMU_EmulatorParameters *params;

	if (sizeof(BT8XXEMU_EmulatorParameters) > sizeof(parameters->EmulatorParameters))
		return false;

	params = (void *)parameters->EmulatorParameters;
	if (!params->Mode)
		return false;

#ifdef EVE_MULTI_TARGET
	if (params->Mode >= BT8XXEMU_EmulatorBT815)
		phost->GpuDefs = &EVE_GpuDefs_BT81X;
	else if (params->Mode >= BT8XXEMU_EmulatorFT810)
		phost->GpuDefs = &EVE_GpuDefs_FT81X;
	else
		phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif
	phost->ChipId = parameters->ChipId;

#if defined(EVE_EMULATOR_MAIN)
	phost->Emulator = EVE_GpuEmu;
	phost->EmulatorFlash = EVE_EmuFlash;
#else
	BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);
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

void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	/* no-op */
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

void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
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
	if (!size)
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

	eve_assert(size <= EVE_CMD_STRING_MAX);
#if 1
	// BT8XXEMU
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		for (;;)
		{
			char c = str[index + (transferred++)];
			transfer8(phost, c);
			// putchar(c ? c : '_');
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				++transferred;
				transfer8(phost, 0);
				// putchar('_');
				break;
			}
		}
		while (transferred & padMask)
		{
			++transferred;
			transfer8(phost, 0);
			// putchar('_');
		}
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
#elif 1
	// FT4222
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		for (;;)
		{
			char c = str[index + (transferred)];
			buffer[transferred++] = c;
			// putchar(c ? c : '_');
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				buffer[transferred++] = 0;
				// putchar('_');
				break;
			}
		}
		while (transferred & padMask)
		{
			buffer[transferred++] = 0;
			// putchar('_');
		}

		eve_assert(transferred);

		EVE_Hal_transferMem(phost, NULL, buffer, transferred);
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
#else
	// FT900
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		transferred += (uint32_t)strnlen(str, size) + 1;
		eve_assert(str[transferred - 1] == '\0');
		EVE_Hal_transferMem(phost, NULL, str, transferred);
		if (transferred & padMask)
		{
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = { 0 };
			EVE_Hal_transferMem(phost, NULL, padding, pad);
			transferred += pad;
			eve_assert(!(transferred & 0x3));
		}
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
#endif
}

/************
** UTILITY **
************/

void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
	/* no-op */
}

/* This API sends a 3byte command to the phost */
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	/* no-op */
}

/* Toggle PD_N pin of FT800 board for a power cycle */
void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
#if !defined(EVE_EMULATOR_MAIN)
	BT8XXEMU_EmulatorParameters *params;

	// ESD would need to call MainReady__ESD again...
	// TODO: Implement powercycle in BT8XXEMU
	if (up)
	{
		if (phost->Emulator)
		{
			BT8XXEMU_stop(phost->Emulator);
			BT8XXEMU_destroy(phost->Emulator);
			phost->Emulator = NULL;
		}

		params = (void *)phost->Parameters.EmulatorParameters;
		BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);
	}
	else
	{
		if (!phost->Emulator)
		{
			params = (void *)phost->Parameters.EmulatorParameters;
			BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, params);
		}

		BT8XXEMU_stop(phost->Emulator);
		BT8XXEMU_destroy(phost->Emulator);
		phost->Emulator = NULL;
	}
#endif
}

void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	/* no-op */
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
