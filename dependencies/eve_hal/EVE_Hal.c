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

#include "EVE_Hal.h"
#include "EVE_Platform.h"

#include "EVE_HalImpl.h"

#include <string.h>

/*********
** INIT **
*********/

EVE_HAL_EXPORT EVE_HalPlatform *EVE_Hal_initialize()
{
	EVE_Mcu_initialize();
	EVE_Millis_initialize();
	EVE_HalImpl_initialize();
	return &g_HalPlatform;
}

EVE_HAL_EXPORT void EVE_Hal_release()
{
	eve_assert_ex(g_HalPlatform.OpenedDevices == 0, "HAL context still open\n");
	EVE_HalImpl_release();
	EVE_Millis_release();
	EVE_Mcu_release();
	memset(&g_HalPlatform, 0, sizeof(EVE_HalPlatform));
}

EVE_HAL_EXPORT void EVE_Hal_defaults(EVE_HalParameters *parameters)
{
	EVE_Hal_defaultsEx(parameters, EVE_SUPPORT_CHIPID, -1);
}

EVE_HAL_EXPORT void EVE_Hal_defaultsEx(EVE_HalParameters *parameters, EVE_CHIPID_T chipId, size_t deviceIdx)
{
	memset(parameters, 0, sizeof(EVE_HalParameters));

#if defined(EVE_MULTI_TARGET)
	parameters->ChipId = chipId;
#else
	eve_assert(chipId == EVE_CHIPID);
	eve_assert(chipId == EVE_SUPPORT_CHIPID);
#endif

	eve_assert_do(EVE_HalImpl_defaults(parameters, chipId, deviceIdx));
}

EVE_HAL_EXPORT bool EVE_Hal_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	memset(phost, 0, sizeof(EVE_HalContext));
	memcpy(&phost->Parameters, parameters, sizeof(EVE_HalParameters));
	return EVE_HalImpl_open(phost, parameters);
}

EVE_HAL_EXPORT void EVE_Hal_close(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_CLOSED)
	{
		eve_printf_debug("Attempt to close HAL context that is already closed\n");
		return;
	}

	EVE_HalImpl_close(phost);
	memset(phost, 0, sizeof(EVE_HalContext));
}

EVE_HAL_EXPORT void EVE_Hal_idle(EVE_HalContext *phost)
{
	EVE_HalImpl_idle(phost);
}

/*********************
** TRANSFER HELPERS **
*********************/

EVE_HAL_EXPORT uint8_t EVE_Hal_rd8(EVE_HalContext *phost, uint32_t addr)
{
	uint8_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer8(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

EVE_HAL_EXPORT uint16_t EVE_Hal_rd16(EVE_HalContext *phost, uint32_t addr)
{
	uint16_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer16(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

EVE_HAL_EXPORT uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr)
{
	uint32_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer32(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

EVE_HAL_EXPORT void EVE_Hal_rdMem(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	EVE_Hal_transferMem(phost, result, NULL, size);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wr8(EVE_HalContext *phost, uint32_t addr, uint8_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer8(phost, v);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wr16(EVE_HalContext *phost, uint32_t addr, uint16_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer16(phost, v);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer32(phost, v);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wrMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferMem(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wrProgmem(EVE_HalContext *phost, uint32_t addr, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferProgmem(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

EVE_HAL_EXPORT void EVE_Hal_wrString(EVE_HalContext *phost, uint32_t addr, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferString(phost, str, index, size, padMask);
	EVE_Hal_endTransfer(phost);
}

/************
** UTILITY **
************/

EVE_HAL_EXPORT int32_t EVE_Hal_clockTrimming(EVE_HalContext *phost, uint32_t lowFreq)
{
	uint32_t f;
	uint8_t i;

	/* Trim the internal clock by increase the REG_TRIM register till the measured frequency is within the acceptable range.*/
	for (i = 0; (i < 31) && ((f = EVE_Hal_currentFrequency(phost)) < lowFreq); i++)
	{
		EVE_Hal_wr8(phost, REG_TRIM, i); /* increase the REG_TRIM register value automatically increases the internal clock */
	}
	EVE_Hal_wr32(phost, REG_FREQUENCY, f); /* Set the final frequency to be used for internal operations */

	return f;
}

/*********
** HOST **
*********/

EVE_HAL_EXPORT void EVE_Host_clockSelect(EVE_HalContext *phost, EVE_PLL_SOURCE_T pllsource)
{
	EVE_Hal_hostCommand(phost, pllsource);
}

EVE_HAL_EXPORT void EVE_Host_pllFreqSelect(EVE_HalContext *phost, EVE_PLL_FREQ_T freq)
{
	EVE_Hal_hostCommand(phost, freq);
}

EVE_HAL_EXPORT void EVE_Host_powerModeSwitch(EVE_HalContext *phost, EVE_POWER_MODE_T pwrmode)
{
	EVE_Hal_hostCommand(phost, pwrmode);
}

EVE_HAL_EXPORT void EVE_Host_coreReset(EVE_HalContext *phost)
{
	EVE_Hal_hostCommand(phost, EVE_CORE_RESET);
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
EVE_HAL_EXPORT void EVE_Host_selectSysClk(EVE_HalContext *phost, EVE_81X_PLL_FREQ_T freq)
{
	if (EVE_SYSCLK_72M == freq)
		EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x06 << 8));
	else if (EVE_SYSCLK_60M == freq)
		EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x05 << 8));
	else if (EVE_SYSCLK_48M == freq)
		EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x40 << 8) | (0x04 << 8));
	else if (EVE_SYSCLK_36M == freq)
		EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x03 << 8));
	else if (EVE_SYSCLK_24M == freq)
		EVE_Hal_hostCommandExt3(phost, (uint32_t)0x61 | (0x02 << 8));
	else if (EVE_SYSCLK_DEFAULT == freq) //default clock
		EVE_Hal_hostCommandExt3(phost, 0x61);
}

EVE_HAL_EXPORT void EVE_Host_powerOffComponents(EVE_HalContext *phost, uint8_t val)
{
	EVE_Hal_hostCommandExt3(phost, (uint32_t)0x49 | (val << 8));
}

EVE_HAL_EXPORT void EVE_Host_padDriveStrength(EVE_HalContext *phost, EVE_81X_GPIO_DRIVE_STRENGTH_T strength, EVE_81X_GPIO_GROUP_T group)
{
	EVE_Hal_hostCommandExt3(phost, (uint32_t)0x70 | (group << 8) | (strength << 8));
}

EVE_HAL_EXPORT void EVE_Host_resetActive(EVE_HalContext *phost)
{
	EVE_Hal_hostCommandExt3(phost, EVE_81X_RESET_ACTIVE);
}

EVE_HAL_EXPORT void EVE_Host_resetRemoval(EVE_HalContext *phost)
{
	EVE_Hal_hostCommandExt3(phost, EVE_81X_RESET_REMOVAL);
}
#endif

void EVE_Hal_displayMessage(EVE_HalContext *phost, char *str, uint16_t size)
{
	uint32_t round = ((size + 31U) & ~31U);
	uint32_t addr = RAM_G + RAM_G_SIZE - round;
	uint32_t dl = 0;

	/* Abuse back of RAM_G to store error */
	/* May invalidate user data... */
	EVE_Hal_wrMem(phost, addr, (uint8_t *)str, size);

	/* Generate bluescreen */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR_COLOR_RGB(0x00, 0x20, 0x40));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR(1, 1, 1));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_HANDLE(15)); /* Scratch handle will reset anyway after reset */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(addr));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE(NEAREST, BORDER, BORDER, 256, (round >> 2)));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT(TEXT8X8, 32, (round >> 2)));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BEGIN(BITMAPS));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 32, 15, 0));
	/* EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(RAM_ERR_REPORT)); */
	/* EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 96, 15, 0)); */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), DISPLAY());
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);
}

/* end of file */
