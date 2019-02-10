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

EVE_HalPlatform *EVE_Hal_initialize()
{
	eve_assert_ex(g_HalPlatform.TotalDevices == 0, "HAL platform already initialized\n");
	EVE_Mcu_initialize();
	EVE_Millis_initialize();
	EVE_HalImpl_initialize();
	return &g_HalPlatform;
}

void EVE_Hal_release()
{
	eve_assert_ex(g_HalPlatform.OpenedDevices == 0, "HAL context still open\n");
	EVE_HalImpl_release();
	EVE_Millis_release();
	EVE_Mcu_release();
	memset(&g_HalPlatform, 0, sizeof(EVE_HalPlatform));
}

void EVE_Hal_defaults(EVE_HalParameters *parameters)
{
	memset(parameters, 0, sizeof(EVE_HalParameters));
#if defined(DISPLAY_RESOLUTION_QVGA)
	/* Values specific to QVGA LCD display */
	parameters->Display.Width = 320;
	parameters->Display.Height = 240;
	parameters->Display.HCycle = 408;
	parameters->Display.HOffset = 70;
	parameters->Display.HSync0 = 0;
	parameters->Display.HSync1 = 10;
	parameters->Display.VCycle = 263;
	parameters->Display.VOffset = 13;
	parameters->Display.VSync0 = 0;
	parameters->Display.VSync1 = 2;
	parameters->Display.PCLK = 8;
	parameters->Display.Swizzle = 2;
	parameters->Display.PCLKPol = 0;
	parameters->Display.CSpread = 1;
	parameters->Display.Dither = 1;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	/* Values specific to QVGA LCD display */
	parameters->Display.Width = 800;
	parameters->Display.Height = 480;
	parameters->Display.HCycle = 928;
	parameters->Display.HOffset = 88;
	parameters->Display.HSync0 = 0;
	parameters->Display.HSync1 = 48;
	parameters->Display.VCycle = 525;
	parameters->Display.VOffset = 32;
	parameters->Display.VSync0 = 0;
	parameters->Display.VSync1 = 3;
	parameters->Display.PCLK = 2;
	parameters->Display.Swizzle = 0;
	parameters->Display.PCLKPol = 1;
	parameters->Display.CSpread = 0;
	parameters->Display.Dither = 1;
#elif defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	/* Values specific to HVGA LCD display */
	parameters->Display.Width = 320;
	parameters->Display.Height = 480;
	parameters->Display.HCycle = 400;
	parameters->Display.HOffset = 40;
	parameters->Display.HSync0 = 0;
	parameters->Display.HSync1 = 10;
	parameters->Display.VCycle = 500;
	parameters->Display.VOffset = 10;
	parameters->Display.VSync0 = 0;
	parameters->Display.VSync1 = 5;
	parameters->Display.PCLK = 4;
#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	parameters->Display.Swizzle = 2;
	parameters->Display.PCLKPol = 1;
#endif
#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	parameters->Display.Swizzle = 0;
	parameters->Display.PCLKPol = 0;
	parameters->Display.PCLK = 5;
#endif
	parameters->Display.CSpread = 1;
	parameters->Display.Dither = 1;
#ifdef ME810A_HV35R
	parameters->Display.PCLK = 5;
#endif
#else
	/* Default is WQVGA - 480x272 */
	parameters->Display.Width = 480;
	parameters->Display.Height = 272;
	parameters->Display.HCycle = 548;
	parameters->Display.HOffset = 43;
	parameters->Display.HSync0 = 0;
	parameters->Display.HSync1 = 41;
	parameters->Display.VCycle = 292;
	parameters->Display.VOffset = 12;
	parameters->Display.VSync0 = 0;
	parameters->Display.VSync1 = 10;
	parameters->Display.PCLK = 5;
	parameters->Display.Swizzle = 0;
	parameters->Display.PCLKPol = 1;
	parameters->Display.CSpread = 1;
	parameters->Display.Dither = 1;
#endif

	EVE_HalImpl_defaults(parameters);
}

bool EVE_Hal_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	memset(phost, 0, sizeof(EVE_HalContext));
	memcpy(&phost->Parameters, parameters, sizeof(EVE_HalParameters));
	return EVE_HalImpl_open(phost, parameters);
}

void EVE_Hal_close(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_CLOSED)
	{
		eve_printf_debug("Attempt to close HAL context that is already closed\n");
		return;
	}

	EVE_HalImpl_close(phost);
	memset(phost, 0, sizeof(EVE_HalContext));
}

void EVE_Hal_idle(EVE_HalContext *phost)
{
	EVE_HalImpl_idle(phost);
}

uint8_t EVE_Hal_rd8(EVE_HalContext *phost, uint32_t addr)
{
	uint8_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer8(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

uint16_t EVE_Hal_rd16(EVE_HalContext *phost, uint32_t addr)
{
	uint16_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer16(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr)
{
	uint32_t value;
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	value = EVE_Hal_transfer32(phost, 0);
	EVE_Hal_endTransfer(phost);
	return value;
}

void EVE_Hal_rdBuffer(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, addr);
	EVE_Hal_transferBuffer(phost, result, NULL, size);
	EVE_Hal_endTransfer(phost);
}

void EVE_Hal_wr8(EVE_HalContext *phost, uint32_t addr, uint8_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer8(phost, v);
	EVE_Hal_endTransfer(phost);
}

void EVE_Hal_wr16(EVE_HalContext *phost, uint32_t addr, uint16_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer16(phost, v);
	EVE_Hal_endTransfer(phost);
}

void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transfer32(phost, v);
	EVE_Hal_endTransfer(phost);
}

void EVE_Hal_wrBuffer(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferBuffer(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

void EVE_Hal_wrProgmem(EVE_HalContext *phost, uint32_t addr, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, addr);
	EVE_Hal_transferProgmem(phost, NULL, buffer, size);
	EVE_Hal_endTransfer(phost);
}

/* end of file */
