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

#ifndef _FT_MCU_HAL_C_
#define _FT_MCU_HAL_C_
#include "FT_MCU_Hal.h"

const ft_uint8_t FT_DLCODE_BOOTUP[12] = {
	0, 0, 0, 2, //GPU instruction CLEAR_COLOR_RGB
	7, 0, 0, 38, //GPU instruction CLEAR
	0, 0, 0, 0, //GPU instruction DISPLAY
};

/* Global variables for display resolution to support various display panels */
/* Default is WQVGA - 480x272 */
ft_int16_t FT_DispWidth = 480;
ft_int16_t FT_DispHeight = 272;
ft_int16_t FT_DispHCycle = 548;
ft_int16_t FT_DispHOffset = 43;
ft_int16_t FT_DispHSync0 = 0;
ft_int16_t FT_DispHSync1 = 41;
ft_int16_t FT_DispVCycle = 292;
ft_int16_t FT_DispVOffset = 12;
ft_int16_t FT_DispVSync0 = 0;
ft_int16_t FT_DispVSync1 = 10;
ft_uint8_t FT_DispPCLK = 5;
ft_char8_t FT_DispSwizzle = 0;
ft_char8_t FT_DispPCLKPol = 1;
ft_char8_t FT_DispCSpread = 1;
ft_char8_t FT_DispDither = 1;

#ifdef DISPLAY_RESOLUTION_QVGA
ft_void_t init_DISPLAY_RESOLUTION_QVGA()
{
	/* Values specific to QVGA LCD display */
	FT_DispWidth = 320;
	FT_DispHeight = 240;
	FT_DispHCycle = 408;
	FT_DispHOffset = 70;
	FT_DispHSync0 = 0;
	FT_DispHSync1 = 10;
	FT_DispVCycle = 263;
	FT_DispVOffset = 13;
	FT_DispVSync0 = 0;
	FT_DispVSync1 = 2;
	FT_DispPCLK = 8;
	FT_DispSwizzle = 2;
	FT_DispPCLKPol = 0;
	FT_DispCSpread = 1;
	FT_DispDither = 1;
}
#endif

#ifdef DISPLAY_RESOLUTION_WVGA
ft_void_t init_DISPLAY_RESOLUTION_WVGA()
{
	/* Values specific to QVGA LCD display */
	FT_DispWidth = 800;
	FT_DispHeight = 480;
	FT_DispHCycle = 928;
	FT_DispHOffset = 88;
	FT_DispHSync0 = 0;
	FT_DispHSync1 = 48;
	FT_DispVCycle = 525;
	FT_DispVOffset = 32;
	FT_DispVSync0 = 0;
	FT_DispVSync1 = 3;
	FT_DispPCLK = 2;
	FT_DispSwizzle = 0;
	FT_DispPCLKPol = 1;
	FT_DispCSpread = 0;
	FT_DispDither = 1;
}
#endif

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
ft_void_t init_DISPLAY_RESOLUTION_HVGA_PORTRAIT()
{
	/* Values specific to HVGA LCD display */
	FT_DispWidth = 320;
	FT_DispHeight = 480;
	FT_DispHCycle = 400;
	FT_DispHOffset = 40;
	FT_DispHSync0 = 0;
	FT_DispHSync1 = 10;
	FT_DispVCycle = 500;
	FT_DispVOffset = 10;
	FT_DispVSync0 = 0;
	FT_DispVSync1 = 5;
	FT_DispPCLK = 4;
#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	FT_DispSwizzle = 2;
	FT_DispPCLKPol = 1;
#endif
#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	FT_DispSwizzle = 0;
	FT_DispPCLKPol = 0;
	FT_DispPCLK = 5;
#endif
	FT_DispCSpread = 1;
	FT_DispDither = 1;
#ifdef ME810A_HV35R
	FT_DispPCLK = 5;
#endif
}
#endif
#endif
