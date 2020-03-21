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

#include "EVE_Util.h"
#include "EVE_Platform.h"
#include "EVE_HalImpl.h"

static eve_progmem_const uint8_t c_DlCodeBootup[12] = {
	0, 0, 0, 2, // GPU instruction CLEAR_COLOR_RGB
	7, 0, 0, 38, // GPU instruction CLEAR
	0, 0, 0, 0, //G PU instruction DISPLAY
};

/* VERIFY: Can the emulator handle this? */
#if (!defined(BT8XXEMU_PLATFORM) || defined(EVE_MULTI_TARGET)) \
    && ((EVE_SUPPORT_CHIPID == EVE_FT811) || (EVE_SUPPORT_CHIPID == EVE_FT813) || defined(EVE_MULTI_TARGET))
#define TOUCH_DATA_LEN 1172
static eve_progmem_const uint8_t c_TouchDataU8[TOUCH_DATA_LEN] = {
	26, 255, 255, 255, 32, 32, 48, 0, 4, 0, 0, 0, 2, 0, 0, 0, 34,
	255, 255, 255, 0, 176, 48, 0, 120, 218, 237, 84, 255, 107, 92,
	69, 16, 159, 125, 155, 107, 141, 201, 121, 247, 106, 130, 9,
	225, 244, 238, 37, 246, 146, 52, 63, 53, 98, 172, 53, 48, 243,
	8, 36, 166, 182, 63, 136, 216, 246, 7, 205, 219, 75, 122, 119,
	185, 139, 196, 128, 34, 33, 136, 123, 65, 240, 7, 31, 44, 105,
	41, 69, 72, 12, 210, 166, 6, 17, 12, 104, 64, 165, 73, 133, 134,
	98, 80, 74, 43, 148, 22, 20, 133, 40, 20, 34, 233, 15, 82, 250,
	131, 10, 113, 246, 229, 106, 197, 191, 161, 111, 217, 55, 179,
	59, 59, 59, 243, 153, 47, 251, 135, 15, 58, 204, 11, 109, 114,
	89, 149, 84, 169, 242, 172, 2, 109, 10, 73, 53, 167, 92, 158,
	89, 21, 6, 89, 230, 236, 126, 86, 89, 206, 242, 169, 178, 61,
	187, 115, 62, 85, 190, 199, 133, 129, 44, 132, 249, 90, 12, 243,
	124, 67, 49, 169, 76, 81, 22, 195, 60, 211, 113, 40, 133, 249,
	253, 104, 114, 113, 148, 197, 131, 105, 169, 158, 3, 19, 132,
	65, 28, 187, 240, 25, 112, 3, 147, 99, 235, 163, 80, 98, 63, 10,
	123, 181, 208, 73, 213, 29, 24, 246, 226, 121, 112, 199, 146,
	74, 98, 56, 22, 195, 135, 245, 32, 14, 194, 17, 180, 182, 100,
	46, 169, 220, 114, 244, 103, 171, 114, 252, 41, 222, 143, 97,
	150, 53, 64, 167, 202, 110, 240, 86, 186, 45, 16, 90, 14, 191,
	158, 110, 196, 163, 216, 130, 241, 232, 159, 42, 251, 88, 4,
	169, 90, 80, 42, 163, 226, 248, 168, 206, 97, 132, 136, 169, 69,
	113, 20, 27, 245, 203, 40, 131, 12, 54, 87, 37, 143, 253, 43,
	105, 97, 137, 9, 172, 103, 181, 26, 172, 79, 163, 102, 184, 142,
	57, 83, 0, 61, 139, 144, 159, 79, 135, 193, 62, 61, 0, 157, 250,
	5, 20, 186, 67, 91, 255, 133, 238, 140, 104, 167, 30, 192, 197,
	72, 2, 85, 137, 221, 89, 134, 101, 140, 177, 191, 238, 137, 75,
	105, 137, 23, 97, 18, 226, 31, 175, 98, 31, 174, 165, 141, 90,
	193, 21, 88, 194, 117, 60, 207, 243, 115, 248, 202, 254, 171,
	171, 37, 248, 20, 87, 112, 17, 59, 180, 59, 246, 35, 199, 177,
	83, 175, 35, 140, 93, 224, 187, 172, 45, 169, 172, 108, 29, 47,
	192, 59, 186, 70, 159, 1, 208, 155, 136, 40, 71, 190, 195, 101,
	222, 117, 131, 221, 90, 142, 110, 48, 221, 85, 165, 177, 42,
	173, 169, 82, 89, 165, 78, 149, 138, 136, 110, 224, 13, 108,
	200, 0, 132, 1, 50, 103, 114, 55, 176, 145, 254, 51, 132, 208,
	183, 177, 137, 64, 111, 128, 192, 13, 104, 162, 86, 145, 165,
	198, 255, 141, 86, 209, 68, 79, 146, 84, 87, 33, 163, 95, 132,
	39, 120, 182, 242, 76, 115, 78, 123, 40, 142, 7, 200, 227, 21,
	103, 50, 103, 171, 109, 123, 27, 242, 115, 193, 0, 197, 176, 70,
	63, 141, 125, 212, 35, 14, 8, 95, 28, 20, 246, 116, 155, 30,
	132, 152, 118, 3, 40, 29, 162, 62, 60, 158, 9, 131, 8, 197, 120,
	49, 99, 115, 35, 39, 228, 200, 168, 48, 81, 197, 2, 175, 221,
	224, 205, 140, 173, 148, 4, 87, 13, 148, 94, 19, 235, 216, 205,
	62, 212, 179, 14, 199, 221, 234, 21, 223, 207, 112, 254, 163,
	90, 169, 197, 247, 50, 110, 249, 16, 249, 156, 113, 137, 83, 98,
	146, 163, 23, 215, 115, 16, 146, 173, 102, 142, 158, 74, 160,
	205, 181, 28, 217, 67, 93, 156, 121, 40, 156, 202, 76, 130, 169,
	184, 193, 12, 9, 125, 82, 132, 149, 121, 209, 65, 219, 198, 12,
	47, 80, 7, 15, 182, 163, 97, 52, 171, 206, 211, 20, 186, 229,
	195, 148, 42, 15, 137, 45, 116, 244, 29, 188, 138, 105, 177, 76,
	139, 149, 18, 113, 141, 160, 192, 1, 218, 222, 14, 131, 101, 90,
	224, 17, 195, 239, 51, 113, 156, 194, 111, 245, 21, 49, 137,
	171, 100, 61, 238, 21, 166, 50, 67, 95, 83, 63, 154, 202, 71,
	116, 141, 45, 205, 208, 37, 94, 133, 188, 186, 201, 177, 134,
	82, 86, 117, 84, 109, 46, 137, 52, 237, 214, 102, 248, 22, 253,
	82, 41, 137, 91, 148, 37, 29, 97, 147, 195, 86, 126, 89, 156,
	165, 45, 214, 188, 43, 54, 43, 160, 63, 36, 139, 169, 142, 17,
	73, 175, 31, 255, 230, 232, 164, 175, 244, 161, 197, 38, 71,
	118, 121, 70, 109, 226, 43, 252, 14, 100, 249, 142, 135, 180,
	156, 48, 185, 45, 172, 176, 247, 71, 244, 29, 104, 240, 25, 3,
	37, 216, 78, 179, 159, 101, 171, 9, 63, 225, 95, 166, 6, 126,
	71, 24, 29, 156, 165, 199, 253, 126, 108, 119, 88, 51, 146, 216,
	19, 191, 137, 102, 191, 107, 186, 68, 109, 78, 247, 244, 95,
	130, 99, 94, 134, 49, 183, 128, 158, 84, 207, 58, 245, 129, 9,
	220, 145, 78, 16, 218, 85, 102, 226, 94, 102, 142, 121, 247, 51,
	243, 146, 119, 63, 51, 3, 14, 71, 157, 94, 245, 86, 169, 151,
	134, 156, 99, 212, 206, 189, 148, 208, 9, 109, 239, 43, 242,
	125, 57, 135, 249, 19, 111, 120, 245, 182, 79, 121, 218, 62,
	110, 231, 222, 203, 57, 220, 245, 52, 237, 217, 126, 246, 253,
	183, 89, 95, 59, 59, 145, 238, 165, 15, 28, 91, 45, 86, 239,
	140, 215, 231, 199, 56, 34, 59, 239, 154, 239, 159, 244, 38, 57,
	63, 167, 156, 41, 126, 95, 14, 83, 82, 189, 203, 183, 244, 51,
	186, 33, 39, 12, 234, 244, 105, 156, 79, 111, 161, 100, 186,
	232, 37, 25, 187, 195, 253, 252, 137, 35, 245, 2, 243, 54, 207,
	243, 8, 176, 143, 109, 175, 225, 57, 92, 131, 71, 248, 220, 151,
	222, 113, 255, 28, 26, 114, 131, 139, 17, 130, 111, 24, 65, 63,
	235, 157, 198, 90, 188, 238, 213, 251, 63, 120, 130, 59, 151,
	187, 130, 95, 193, 207, 252, 26, 214, 221, 175, 215, 240, 186,
	211, 85, 217, 197, 107, 67, 123, 113, 15, 217, 42, 252, 201,
	155, 245, 133, 94, 131, 122, 255, 87, 207, 80, 143, 215, 238,
	91, 47, 126, 247, 106, 248, 204, 159, 153, 187, 116, 219, 49,
	116, 45, 115, 147, 182, 48, 242, 4, 190, 240, 127, 118, 224,
	193, 247, 224, 3, 89, 247, 15, 148, 99, 211, 103, 26, 255, 255,
	255, 20, 33, 48, 0, 4, 0, 0, 0, 15, 0, 0, 0, 26, 255, 255, 255,
	32, 32, 48, 0, 4, 0, 0, 0, 0, 0, 0, 0
};

/* Download new touch firmware for FT811 and FT813 chip */
static inline void uploadTouchFirmware(EVE_HalContext *phost)
{
	/* bug fix pen up section */
	eve_assert_do(EVE_Cmd_wrProgmem(phost, c_TouchDataU8, TOUCH_DATA_LEN));
	eve_assert_do(EVE_Cmd_waitFlush(phost));
}
#else
static inline void uploadTouchFirmware(EVE_HalContext *phost)
{
	/* no-op */
}
#endif

EVE_HAL_EXPORT void EVE_Util_clearScreen(EVE_HalContext *phost)
{
	EVE_Hal_wrProgmem(phost, RAM_DL, (eve_progmem_const uint8_t *)c_DlCodeBootup, sizeof(c_DlCodeBootup));
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);
}

EVE_HAL_EXPORT void EVE_Util_bootupDefaults(EVE_HalContext *phost, EVE_BootupParameters *parameters)
{
	memset(parameters, 0, sizeof(EVE_BootupParameters));

	uint32_t chipId = EVE_CHIPID;

#if !defined(ME810A_HV35R) && !defined(ME812A_WH50R) && !defined(ME813A_WH50C)
	parameters->ExternalOsc = true;
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810) || defined(EVE_MULTI_TARGET)
#ifdef ENABLE_SPI_QUAD
	parameters->SpiChannels = EVE_SPI_QUAD_CHANNEL;
	parameters->SpiDummyBytes = 2;
#elif ENABLE_SPI_DUAL
	parameters->SpiChannels = EVE_SPI_DUAL_CHANNEL;
	parameters->SpiDummyBytes = 2;
#else
	parameters->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
	parameters->SpiDummyBytes = 1;
#endif
#endif

#if defined(DISPLAY_RESOLUTION_QVGA)
	/* Values specific to QVGA LCD display */
	parameters->Width = 320;
	parameters->Height = 240;
	parameters->HCycle = 408;
	parameters->HOffset = 70;
	parameters->HSync0 = 0;
	parameters->HSync1 = 10;
	parameters->VCycle = 263;
	parameters->VOffset = 13;
	parameters->VSync0 = 0;
	parameters->VSync1 = 2;
	parameters->PCLK = 8;
	parameters->Swizzle = 2;
	parameters->PCLKPol = 0;
	parameters->CSpread = 1;
	parameters->Dither = 1;
#elif defined(DISPLAY_RESOLUTION_WVGA)
	/* Values specific to WVGA LCD display */
	parameters->Width = 800;
	parameters->Height = 480;
	parameters->HCycle = 928;
	parameters->HOffset = 88;
	parameters->HSync0 = 0;
	parameters->HSync1 = 48;
	parameters->VCycle = 525;
	parameters->VOffset = 32;
	parameters->VSync0 = 0;
	parameters->VSync1 = 3;
	parameters->PCLK = 2;
	parameters->Swizzle = 0;
	parameters->PCLKPol = 1;
	parameters->CSpread = 0;
	parameters->Dither = 1;
#elif defined(DISPLAY_RESOLUTION_HVGA_PORTRAIT)
	/* Values specific to HVGA LCD display */
	parameters->Width = 320;
	parameters->Height = 480;
	parameters->HCycle = 400;
	parameters->HOffset = 40;
	parameters->HSync0 = 0;
	parameters->HSync1 = 10;
	parameters->VCycle = 500;
	parameters->VOffset = 10;
	parameters->VSync0 = 0;
	parameters->VSync1 = 5;
	parameters->PCLK = 4;
#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	parameters->Swizzle = 2;
	parameters->PCLKPol = 1;
#endif
#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	parameters->Swizzle = 0;
	parameters->PCLKPol = 0;
	parameters->PCLK = 5;
#endif
	parameters->CSpread = 1;
	parameters->Dither = 1;
#ifdef ME810A_HV35R
	parameters->PCLK = 5;
#endif
#else
	if (chipId >= EVE_BT815)
	{
		/* Values specific to WVGA LCD display */
		parameters->Width = 800;
		parameters->Height = 480;
		parameters->HCycle = 928;
		parameters->HOffset = 88;
		parameters->HSync0 = 0;
		parameters->HSync1 = 48;
		parameters->VCycle = 525;
		parameters->VOffset = 32;
		parameters->VSync0 = 0;
		parameters->VSync1 = 3;
		parameters->PCLK = 2;
		parameters->Swizzle = 0;
		parameters->PCLKPol = 1;
		parameters->CSpread = 0;
		parameters->Dither = 1;
	}
	else if (chipId >= EVE_FT810)
	{
		/* Default is WQVGA - 480x272 */
		parameters->Width = 480;
		parameters->Height = 272;
		parameters->HCycle = 548;
		parameters->HOffset = 43;
		parameters->HSync0 = 0;
		parameters->HSync1 = 41;
		parameters->VCycle = 292;
		parameters->VOffset = 12;
		parameters->VSync0 = 0;
		parameters->VSync1 = 10;
		parameters->PCLK = 5;
		parameters->Swizzle = 0;
		parameters->PCLKPol = 1;
		parameters->CSpread = 1;
		parameters->Dither = 1;
	}
	else
	{
		/* Values specific to QVGA LCD display */
		parameters->Width = 320;
		parameters->Height = 240;
		parameters->HCycle = 408;
		parameters->HOffset = 70;
		parameters->HSync0 = 0;
		parameters->HSync1 = 10;
		parameters->VCycle = 263;
		parameters->VOffset = 13;
		parameters->VSync0 = 0;
		parameters->VSync1 = 2;
		parameters->PCLK = 8;
		parameters->Swizzle = 2;
		parameters->PCLKPol = 0;
		parameters->CSpread = 1;
		parameters->Dither = 1;
	}
#endif
}

EVE_HAL_EXPORT bool EVE_Util_bootup(EVE_HalContext *phost, EVE_BootupParameters *parameters)
{
	uint32_t chipId;
	uint8_t id;
	uint8_t engine_status;

	/* FT81x will be in SPI Single channel after POR */
	EVE_Hal_powerCycle(phost, true);

	/* Set the clk to external clock. Must disable it when no external clock source on the board*/
	if (parameters->ExternalOsc)
	{
		EVE_Hal_hostCommand(phost, EVE_EXTERNAL_OSC);
		EVE_sleep(10);
	}

	/* Access address 0 to wake up the FT800 */
	EVE_Hal_hostCommand(phost, EVE_ACTIVE_M);
	EVE_sleep(300);

	/* Wait for valid chip ID */
	chipId = EVE_Hal_rd32(phost, ROM_CHIPID);
	chipId = ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8);
	while (chipId < EVE_FT800 || chipId > EVE_BT816)
	{
		eve_printf_debug("EVE ROM_CHIPID after wake up %x\n", chipId);

		EVE_sleep(20);
		if (phost->Parameters.CbCmdWait)
			if (!phost->Parameters.CbCmdWait(phost))
				return false;

		chipId = EVE_Hal_rd32(phost, ROM_CHIPID);
		chipId = ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8);
	}

#ifdef EVE_MULTI_TARGET
	const uint32_t expectedChipId = phost->Parameters.ChipId;
#else
	const uint32_t expectedChipId = EVE_SUPPORT_CHIPID;
#endif

	/* Validate chip ID to ensure the correct HAL is used */
	/* ROM_CHIPID is valid accross all EVE devices */
	if (((chipId = EVE_Hal_rd32(phost, ROM_CHIPID)) & 0xFFFF) != (((expectedChipId >> 8) & 0xFF) | ((expectedChipId & 0xFF) << 8)))
		eve_printf_debug("Mismatching EVE chip id %x, expect model %x\n", ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8), expectedChipId);
	eve_printf_debug("EVE chip id %x %x.%x\n", ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8), ((chipId >> 16) & 0xFF), ((chipId >> 24) & 0xFF));

	/* Switch to the proper chip ID if applicable */
#ifdef EVE_MULTI_TARGET
	phost->ChipId = ((chipId >> 8) & 0xFF) | ((chipId & 0xFF) << 8);
	if (phost->ChipId >= EVE_BT815)
		phost->GpuDefs = &EVE_GpuDefs_BT81X;
	else if (phost->ChipId >= EVE_FT810)
		phost->GpuDefs = &EVE_GpuDefs_FT81X;
	else if (phost->ChipId >= EVE_FT800)
		phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif

	/* Read Register ID to check if EVE is ready. */
	while (id = EVE_Hal_rd8(phost, REG_ID) != 0x7C)
	{
		eve_printf_debug("EVE register ID after wake up %x\n", id);

		EVE_sleep(20);
		if (phost->Parameters.CbCmdWait)
			if (!phost->Parameters.CbCmdWait(phost))
				return false;
	}
	eve_printf_debug("EVE register ID after wake up %x\n", id);

	if ((EVE_CHIPID == EVE_FT811 || EVE_CHIPID == EVE_FT813) && (EVE_HOST != EVE_HOST_BT8XXEMU))
	{
		if (EVE_HOST == EVE_HOST_PANL70)
		{
			EVE_Hal_wr8(phost, REG_CPURESET, 2);
			EVE_Hal_wr16(phost, REG_CYA_TOUCH, 0x05d0);
		}
		/* Download new firmware to fix pen up issue */
		/* It may cause resistive touch not working any more*/
		uploadTouchFirmware(phost);
		if (EVE_HOST == EVE_HOST_PANL70)
		{
			EVE_UtilImpl_bootupDisplayGpio(phost);
		}
		EVE_Hal_flush(phost);
		EVE_sleep(100);
	}

	/* Read REG_CPURESET to check if engines are ready.
	Bit 0 for coprocessor engine,
	Bit 1 for touch engine,
	Bit 2 for audio engine.
	*/
	while ((engine_status = EVE_Hal_rd8(phost, REG_CPURESET)) != 0x00)
	{
		if (engine_status & 0x01)
		{
			eve_printf_debug("Coprocessor engine is not ready\n");
		}
		if (engine_status & 0x02)
		{
			eve_printf_debug("Touch engine is not ready\n");
		}
		if (engine_status & 0x04)
		{
			eve_printf_debug("Audio engine is not ready\n");
		}

		EVE_sleep(20);
		if (phost->Parameters.CbCmdWait)
			if (!phost->Parameters.CbCmdWait(phost))
				return false;
	}
	eve_printf_debug("All engines are ready\n");

	if (EVE_CHIPID < EVE_FT810)
	{
		eve_assert(parameters->Width < 512);
		eve_assert(parameters->Height < 512);
	}
	else
	{
		eve_assert(parameters->Width < 2048);
		eve_assert(parameters->Height < 2048);
	}

	EVE_Hal_wr16(phost, REG_HCYCLE, parameters->HCycle);
	EVE_Hal_wr16(phost, REG_HOFFSET, parameters->HOffset);
	EVE_Hal_wr16(phost, REG_HSYNC0, parameters->HSync0);
	EVE_Hal_wr16(phost, REG_HSYNC1, parameters->HSync1);
	EVE_Hal_wr16(phost, REG_VCYCLE, parameters->VCycle);
	EVE_Hal_wr16(phost, REG_VOFFSET, parameters->VOffset);
	EVE_Hal_wr16(phost, REG_VSYNC0, parameters->VSync0);
	EVE_Hal_wr16(phost, REG_VSYNC1, parameters->VSync1);
	EVE_Hal_wr8(phost, REG_SWIZZLE, parameters->Swizzle);
	EVE_Hal_wr8(phost, REG_PCLK_POL, parameters->PCLKPol);
	EVE_Hal_wr16(phost, REG_HSIZE, parameters->Width);
	EVE_Hal_wr16(phost, REG_VSIZE, parameters->Height);
	EVE_Hal_wr16(phost, REG_CSPREAD, parameters->CSpread);
	EVE_Hal_wr16(phost, REG_DITHER, parameters->Dither);

	/* TODO: EVCharger Demo */
	// EVE_Hal_wr16(phost, REG_OUTBITS, 0x1B6);
	// EVE_Hal_wr16(phost, REG_ADAPTIVE_FRAMERATE, 1);

	if (EVE_Hal_isScreenResistive(phost))
	{
		/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
		/* TODO: From parameters */
		EVE_Hal_wr16(phost, REG_TOUCH_RZTHRESH, RESISTANCE_THRESHOLD);
	}

	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
		EVE_Hal_wr16(phost, REG_GPIOX, 0xffff);
	}
	else
	{
		EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
		EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	}

	EVE_Util_clearScreen(phost);

	EVE_Hal_wr8(phost, REG_PCLK, parameters->PCLK); /* after this display is visible on the LCD */
	phost->PCLK = parameters->PCLK;

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* to cross check reset pin */
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0x7f);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_Hal_flush(phost);
	EVE_sleep(120);
#endif

#if defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT)
	EVE_ILI9488_bootup();
	eve_printf_debug("after ILI9488 bootup\n");
#endif

	/* Refresh fifo */
	uint16_t wp = EVE_Cmd_wp(phost);
	uint16_t rp = EVE_Cmd_rp(phost);
	EVE_Cmd_space(phost);
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;

	/* Coprocessor needs a reset */
	if (wp || rp)
	{
		eve_printf_debug("Coprocessor fifo not empty after powerdown\n");
		EVE_Util_resetCoprocessor(phost);
	}

	/* Wait for coprocessor ready */
	eve_printf_debug("Check coprocessor\n");
	EVE_Cmd_wr32(phost, CMD_DLSTART);
	EVE_Cmd_wr32(phost, CMD_COLDSTART);

#ifdef EVE_FLASH_AVAILABLE
	if (EVE_CHIPID >= EVE_BT815)
	{
		/* Reattach flash to avoid inconsistent state */
		EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
	}
#endif

	EVE_Cmd_waitFlush(phost);
	EVE_Hal_flush(phost);

	/* Switch to configured default SPI channel mode */
#if (EVE_SUPPORT_CHIPID >= EVE_FT810) || defined(EVE_MULTI_TARGET)
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_Hal_setSPI(phost, parameters->SpiChannels, parameters->SpiDummyBytes);
	}
#endif

	return true;
}

EVE_HAL_EXPORT void EVE_Util_shutdown(EVE_HalContext *phost)
{
	if (EVE_CHIPID >= EVE_FT810)
	{
		EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
		EVE_Hal_wr16(phost, REG_GPIOX, 0);
	}
	else
	{
		EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
		EVE_Hal_wr8(phost, REG_GPIO, 0);
	}
	EVE_Hal_wr8(phost, REG_PCLK, 0);
	EVE_Hal_powerCycle(phost, false);
}

/* Whether the device has an OTP that requires reactivation in case of reset during CMD_LOGO */
static inline bool EVE_Util_hasOTP(EVE_HalContext *phost)
{
	return (EVE_CHIPID >= EVE_FT810) && (EVE_CHIPID < EVE_BT815);
}

/* Whether the device has an OTP that requires the video patch to be reapplied */
#define EVE_VIDEOPATCH_ADDR 0x309162 // NOTE: This is only valid for BT815 and BT816
static inline bool EVE_Util_hasVideoPatch(EVE_HalContext *phost)
{
	return (EVE_CHIPID >= EVE_BT815) && (EVE_CHIPID <= EVE_BT816);
}

EVE_HAL_EXPORT bool EVE_Util_resetCoprocessor(EVE_HalContext *phost)
{
	uint16_t videoPatchVector;
	uint16_t rd, wr;

	eve_printf_debug("Reset coprocessor\n");

	if (EVE_Util_hasVideoPatch(phost))
	{
		/* BT81X video patch */
		videoPatchVector = EVE_Hal_rd16(phost, EVE_VIDEOPATCH_ADDR);
	}

	/* Set REG_CPURESET to 1, to hold the coprocessor in the reset condition */
	EVE_Hal_wr8(phost, REG_CPURESET, 1);
	EVE_Hal_flush(phost);
	EVE_sleep(100);

	/* Set REG_CMD_READ and REG_CMD_WRITE to zero */
	EVE_Hal_wr16(phost, REG_CMD_READ, 0);
	EVE_Hal_wr16(phost, REG_CMD_WRITE, 0);
	EVE_Hal_wr16(phost, REG_CMD_DL, 0);
	EVE_Hal_wr8(phost, REG_PCLK, phost->PCLK); /* j1 will set the pclk to 0 for that error case */

	/* Stop playing audio in case video with audio was playing during reset */
	EVE_Hal_wr8(phost, REG_PLAYBACK_PLAY, 0);

	if (EVE_Util_hasOTP(phost))
	{
		/* Enable patched rom in case the reset is requested while CMD_LOGO is running.
		This is necessary as CMD_LOGO may swap to the other rom page */
		EVE_Hal_wr8(phost, REG_ROMSUB_SEL, 3);
	}

	/* Default */
	phost->CmdFault = false;

	/* Set REG_CPURESET to 0, to restart the coprocessor */
	EVE_Hal_wr8(phost, REG_CPURESET, 0);
	EVE_Hal_flush(phost);
	EVE_sleep(100);
	eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 0);
	eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

	/* Refresh fifo */
	EVE_Cmd_wp(phost);
	EVE_Cmd_rp(phost);
	EVE_Cmd_space(phost);
	EVE_Cmd_waitFlush(phost);
	phost->MediaFifoAddress = 0;
	phost->MediaFifoSize = 0;

	if (EVE_Util_hasOTP(phost))
	{
		/* Clear cmd with CMD_STOP, exiting CMD_EXECUTE may loop over, depending on OTP */
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_CMD);
		for (int i = 0; i < 4096; i += 4)
			EVE_Hal_transfer32(phost, CMD_STOP);
		EVE_Hal_endTransfer(phost);

		/* Go back into the patched coprocessor main loop */
		EVE_Hal_wr8(phost, REG_ROMSUB_SEL, 3);
		EVE_Cmd_startFunc(phost);
		EVE_Cmd_wr32(phost, CMD_EXECUTE);
		EVE_Cmd_wr32(phost, 0x7ffe);
		EVE_Cmd_wr32(phost, 0);
		EVE_Cmd_endFunc(phost);
		EVE_Hal_flush(phost);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 12);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

		/* Difficult to check when CMD_EXECUTE is processed when there's an OTP,
		since the read pointer keeps looping back to 0. */
		EVE_sleep(100);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 12);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);

		/* Need to manually stop previous command from repeating infinitely,
		however, this may cause the coprocessor to overshoot the command fifo,
		hence it's been filled with harmless CMD_STOP commands. */
		;
		EVE_Hal_wr16(phost, REG_CMD_WRITE, 0);
		EVE_Hal_flush(phost);
		EVE_sleep(100);

		/* Refresh fifo */
		EVE_Cmd_wp(phost);
		EVE_Cmd_rp(phost);
		EVE_Cmd_space(phost);
		EVE_Cmd_waitFlush(phost);
		eve_assert((wr = EVE_Hal_rd16(phost, REG_CMD_WRITE)) == 0);
		eve_assert((rd = EVE_Hal_rd16(phost, REG_CMD_READ)) == 0);
	}

	if (EVE_Util_hasVideoPatch(phost))
	{
		/* BT81X video patch */
		EVE_Hal_wr16(phost, EVE_VIDEOPATCH_ADDR, videoPatchVector);
	}

	/* Cold start. Ensure that the coprocessor is ready. */
	EVE_Cmd_wr32(phost, CMD_DLSTART);
	EVE_Cmd_wr32(phost, CMD_COLDSTART);

#ifdef EVE_FLASH_AVAILABLE
	if (EVE_CHIPID >= EVE_BT815)
	{
		/* Reattach flash to avoid inconsistent state */
		EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
	}
#endif

	/* Wait for coprocessor to be ready */
	return EVE_Cmd_waitFlush(phost);
}

EVE_HAL_EXPORT bool EVE_Util_bootupConfig(EVE_HalContext *phost)
{
	EVE_BootupParameters parameters;
	EVE_Util_bootupDefaults(phost, &parameters);
	return EVE_Util_bootup(phost, &parameters);
}

/* end of file */
