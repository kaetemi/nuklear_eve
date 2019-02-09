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

#include <string.h>

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
}

/* end of file */
