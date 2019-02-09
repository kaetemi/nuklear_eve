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
#if defined(MSVC_PLATFORM)

#include "FT_MCU_Hal.h"
#include "FT_Gpu_Hal.h"

ft_void_t Eve_BootupConfig(Ft_Gpu_Hal_Context_t *phost)
{
	EVE_HalParameters *parameters = &phost->Parameters;

	Ft_Gpu_Hal_Powercycle(phost, FT_TRUE);

	/* FT81x will be in SPI Single channel after POR
	If we are here with FT4222 in multi channel, then
	an explicit switch to single channel is essential
	*/
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_SINGLE_CHANNEL, 1);

	/* Set the clk to external clock. Must disable it when no external clock source on the board*/
#if (!defined(ME810A_HV35R) && !defined(ME812A_WH50R) && !defined(ME813AU_WH50C))
	Ft_Gpu_HostCommand(phost, FT_GPU_EXTERNAL_OSC);
	Ft_Gpu_Hal_Sleep(10);
#endif

	/* Access address 0 to wake up the FT800 */
	Ft_Gpu_HostCommand(phost, FT_GPU_ACTIVE_M);
	Ft_Gpu_Hal_Sleep(300);
#if !(defined(ESD_SIMULATION))
#if defined(FT_811_ENABLE) || defined(FT_813_ENABLE)
#if defined(PANL70) || defined(PANL70PLUS)
	EVE_Hal_wr8(phost, REG_CPURESET, 2);
	EVE_Hal_wr16(phost, REG_CYA_TOUCH, 0x05d0);
#endif
	/* Download new firmware to fix pen up issue */
	/* It may cause resistive touch not working any more*/
	Ft_Gpu_DownloadJ1Firmware(phost); // FIXME: Shouldn't this be called *after* waiting for REG_ID?
#if defined(PANL70) || defined(PANL70PLUS)
	Ft_Gpu_Panl70_GOODIXGPIO(phost);
#endif
	Ft_Gpu_Hal_Sleep(100);
#endif
#endif

	/* Read Register ID to check if EVE is ready. */
	scope
	{
		ft_uint8_t chipid;
		chipid = EVE_Hal_rd8(phost, REG_ID);
		while (chipid != 0x7C)
		{
			chipid = EVE_Hal_rd8(phost, REG_ID);
			eve_printf_debug("EVE register ID after wake up %x\n", chipid);
			ft_delay(100);
		}
		eve_printf_debug("EVE register ID after wake up %x\n", chipid);
	}

	/* RAM_G self test */
#ifndef NDEBUG	
	scope
	{
		EVE_Hal_wr8(phost, 0, 123);
		EVE_Hal_wr8(phost, 123, 210);
		eve_assert(EVE_Hal_rd8(phost, 0) == 123);
		eve_assert(EVE_Hal_rd8(phost, 123) == 210);
		EVE_Hal_wr32(phost, 1234, 54321);
		eve_assert(EVE_Hal_rd32(phost, 1234) == 54321);
		eve_printf_debug("%i, %i, %i\n", EVE_Hal_rd8(phost, 0), EVE_Hal_rd8(phost, 123), EVE_Hal_rd32(phost, 1234));
	}
#endif

	/* Read REG_CPURESET to confirm 0 is returned */
	scope
	{
		ft_uint8_t engine_status;
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
			ft_delay(100);
		}
		eve_printf_debug("All engines are ready\n");
	}

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

#ifdef EVE_SCREEN_RESISTIVE
	/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
	EVE_Hal_wr16(phost, REG_TOUCH_RZTHRESH, RESISTANCE_THRESHOLD);
#endif

#if defined(FT_81X_ENABLE)
	EVE_Hal_wr16(phost, REG_GPIOX_DIR, 0xffff);
	EVE_Hal_wr16(phost, REG_GPIOX, 0xffff);
#else
	EVE_Hal_wr8(phost, REG_GPIO_DIR, 0xff);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
#endif

	EVE_Hal_wrBuffer(phost, RAM_DL, (ft_uint8_t *)FT_DLCODE_BOOTUP, sizeof(FT_DLCODE_BOOTUP));
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);

	EVE_Hal_wr8(phost, REG_PCLK, parameters->Display.PCLK); //after this display is visible on the LCD

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* to cross check reset pin */
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	ft_delay(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0x7f);
	ft_delay(120);
	EVE_Hal_wr8(phost, REG_GPIO, 0xff);
	ft_delay(120);
#endif
	Ft_DisplayPanel_Init();

	/* make the spi to quad mode - addition 2 bytes for silicon */
#ifndef FT_80X_ENABLE
	/* api to set quad and numbe of dummy bytes */
#ifdef ENABLE_SPI_QUAD
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_QUAD_CHANNEL, FT_GPU_SPI_TWODUMMY);
#elif ENABLE_SPI_DUAL
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_DUAL_CHANNEL, FT_GPU_SPI_TWODUMMY);
#else
	Ft_Gpu_Hal_SetSPI(phost, FT_GPU_SPI_SINGLE_CHANNEL, 1);
#endif

#endif
}

static ft_void_t init_sdhost()
{
	;
}

ft_void_t Ft_Mcu_Init()
{
	init_sdhost();
	EVE_Millis_initialize();
}

#endif

/* end of file */
