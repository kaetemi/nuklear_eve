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

#include "FT_MCU_Hal.h"
#include "FT_Gpu_Hal.h"

#if !defined(BT8XXEMU_PLATFORM) /* TODO: Can the emulator handle this? */
#if (EVE_MODEL == EVE_FT811) || (EVE_MODEL == EVE_FT813)
#define TOUCH_DATA_LEN 1172
static ft_prog_uchar8_t c_TouchDataU8[TOUCH_DATA_LEN] = {
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
ft_void_t uploadTouchFirmware(EVE_HalContext *phost)
{
	// bug fix pen up section
	eve_assert_do(Ft_Gpu_Hal_WrCmdBuf_ProgMem(phost, c_TouchDataU8, TOUCH_DATA_LEN));
	eve_assert_do(Ft_Gpu_Hal_WaitCmdFifoEmpty(phost));
}
#endif
#endif

bool EVE_Util_bootupConfig(EVE_HalContext *phost)
{
	EVE_HalParameters *parameters = &phost->Parameters;
	ft_uint8_t chipid;
	ft_uint8_t engine_status;

	Ft_Gpu_Hal_Powercycle(phost, FT_TRUE);

	/* FT81x will be in SPI Single channel after POR
	If we are here with FT4222 in multi channel, then
	an explicit switch to single channel is essential
	*/
#if (EVE_MODEL >= EVE_FT810)
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_SINGLE_CHANNEL, 1);
#endif

	/* Set the clk to external clock. Must disable it when no external clock source on the board*/
#if (!defined(ME810A_HV35R) && !defined(ME812A_WH50R) && !defined(ME813AU_WH50C))
	Ft_Gpu_HostCommand(phost, FT_GPU_EXTERNAL_OSC);
	EVE_sleep(10);
#endif

	/* Access address 0 to wake up the FT800 */
	Ft_Gpu_HostCommand(phost, FT_GPU_ACTIVE_M);
	EVE_sleep(300);

	/* Read Register ID to check if EVE is ready. */
	chipid = EVE_Hal_rd8(phost, REG_ID);
	while (chipid != 0x7C)
	{
		chipid = EVE_Hal_rd8(phost, REG_ID);
		eve_printf_debug("EVE register ID after wake up %x\n", chipid);
		EVE_sleep(100);
	}
	eve_printf_debug("EVE register ID after wake up %x\n", chipid);

#if !defined(BT8XXEMU_PLATFORM) /* TODO: Can the emulator handle this? */
#if (EVE_MODEL == EVE_FT811) || (EVE_MODEL == EVE_FT813)
#if defined(PANL70) || defined(PANL70PLUS)
	EVE_Hal_wr8(phost, REG_CPURESET, 2);
	EVE_Hal_wr16(phost, REG_CYA_TOUCH, 0x05d0);
#endif
	/* Download new firmware to fix pen up issue */
	/* It may cause resistive touch not working any more*/
	uploadTouchFirmware(phost); // FIXME: Shouldn't this be called *after* waiting for REG_ID?
#if defined(PANL70) || defined(PANL70PLUS)
	Ft_Gpu_Panl70_GOODIXGPIO(phost);
#endif
	EVE_sleep(100);
#endif
#endif

	/* Read REG_CPURESET to check if engines are ready.
	Bit 0 for coprocessor engine,
	Bit 1 for touch engine,
	Bit 2 for audio engine.
	*/
	engine_status = EVE_Hal_rd8(phost, REG_CPURESET);
	while (engine_status != 0x00)
	{
		if (engine_status & 0x01)
		{
			eve_printf_debug("coprocessor engine is not ready\n");
		}
		if (engine_status & 0x02)
		{
			eve_printf_debug("touch engine is not ready\n");
		}
		if (engine_status & 0x04)
		{
			eve_printf_debug("audio engine is not ready\n");
		}

		engine_status = EVE_Hal_rd8(phost, REG_CPURESET);
		EVE_sleep(100);
	}
	eve_printf_debug("All engines are ready\n");

	EVE_Hal_wr16(phost, REG_HCYCLE, parameters->Display.HCycle);
	EVE_Hal_wr16(phost, REG_HOFFSET, parameters->Display.HOffset);
	EVE_Hal_wr16(phost, REG_HSYNC0, parameters->Display.HSync0);
	EVE_Hal_wr16(phost, REG_HSYNC1, parameters->Display.HSync1);
	EVE_Hal_wr16(phost, REG_VCYCLE, parameters->Display.VCycle);
	EVE_Hal_wr16(phost, REG_VOFFSET, parameters->Display.VOffset);
	EVE_Hal_wr16(phost, REG_VSYNC0, parameters->Display.VSync0);
	EVE_Hal_wr16(phost, REG_VSYNC1, parameters->Display.VSync1);
	EVE_Hal_wr8(phost, REG_SWIZZLE, parameters->Display.Swizzle);
	EVE_Hal_wr8(phost, REG_PCLK_POL, parameters->Display.PCLKPol);
	EVE_Hal_wr16(phost, REG_HSIZE, parameters->Display.Width);
	EVE_Hal_wr16(phost, REG_VSIZE, parameters->Display.Height);
	EVE_Hal_wr16(phost, REG_CSPREAD, parameters->Display.CSpread);
	EVE_Hal_wr16(phost, REG_DITHER, parameters->Display.Dither);

	/* TODO: EVCharger Demo */
	// EVE_Hal_wr16(phost, REG_OUTBITS, 0x1B6);
	// EVE_Hal_wr16(phost, REG_ADAPTIVE_FRAMERATE, 1);

#ifdef EVE_SCREEN_RESISTIVE
	/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
	/* TODO: From parameters */
	EVE_Hal_wr16(phost, REG_TOUCH_RZTHRESH, RESISTANCE_THRESHOLD);
#endif

#if (EVE_MODEL >= EVE_FT810)
	EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
	EVE_Hal_wr16(phost, REG_GPIOX, 0xffff);
#else
	EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
#endif

	EVE_Hal_wrBuffer(phost, RAM_DL, (ft_uint8_t *)FT_DLCODE_BOOTUP, sizeof(FT_DLCODE_BOOTUP));
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);

	EVE_Hal_wr8(phost, REG_PCLK, parameters->Display.PCLK); /* after this display is visible on the LCD */

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* to cross check reset pin */
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0x7f);
	EVE_sleep(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	EVE_sleep(120);
#endif

#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	ILI9488_Bootup();
	eve_printf_debug("after ILI9488 bootup\n");
#endif

#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	KD2401_Bootup();
	eve_printf_debug("after KD2401 bootup\n");
#endif

	EVE_UtilImpl_prepareSpiMaster(phost);

#if (EVE_MODEL >= EVE_FT810)
	/* api to set quad and numbe of dummy bytes */
#ifdef ENABLE_SPI_QUAD
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_QUAD_CHANNEL, 2);
#elif ENABLE_SPI_DUAL
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_DUAL_CHANNEL, 2);
#else
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_SINGLE_CHANNEL, 1);
#endif
#endif

	return EVE_UtilImpl_postBootupConfig(phost);
}

/* end of file */