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

ft_void_t Eve_BootupConfig(Ft_Gpu_Hal_Context_t *s_Host)
{
	Ft_Gpu_Hal_Powercycle(s_Host, FT_TRUE);

	/* FT81x will be in SPI Single channel after POR
	If we are here with FT4222 in multi channel, then
	an explicit switch to single channel is essential
	*/
	Ft_Gpu_Hal_SetSPI(s_Host, FT_GPU_SPI_SINGLE_CHANNEL, FT_GPU_SPI_ONEDUMMY);

	/* Set the clk to external clock. Must disable it when no external clock source on the board*/
#if (!defined(ME810A_HV35R) && !defined(ME812A_WH50R) && !defined(ME813AU_WH50C))
	Ft_Gpu_HostCommand(s_Host, FT_GPU_EXTERNAL_OSC);
	Ft_Gpu_Hal_Sleep(10);
#endif

	/* Access address 0 to wake up the FT800 */
	Ft_Gpu_HostCommand(s_Host, FT_GPU_ACTIVE_M);
	Ft_Gpu_Hal_Sleep(300);
#if !(defined(ESD_SIMULATION))
#if defined(FT_811_ENABLE) || defined(FT_813_ENABLE)
#if defined(PANL70) || defined(PANL70PLUS)
	Ft_Gpu_Hal_Wr8(s_Host, REG_CPURESET, 2);
	Ft_Gpu_Hal_Wr16(s_Host, REG_CYA_TOUCH, 0x05d0);
#endif
	/* Download new firmware to fix pen up issue */
	/* It may cause resistive touch not working any more*/
	Ft_Gpu_DownloadJ1Firmware(s_Host); // FIXME: Shouldn't this be called *after* waiting for REG_ID?
#if defined(PANL70) || defined(PANL70PLUS)
	Ft_Gpu_Panl70_GOODIXGPIO(s_Host);
#endif
	Ft_Gpu_Hal_Sleep(100);
#endif
#endif

	;
	{
		ft_uint8_t chipid;
		//Read Register ID to check if EVE is ready.
		chipid = Ft_Gpu_Hal_Rd8(s_Host, REG_ID);
		while (chipid != 0x7C)
		{
			chipid = Ft_Gpu_Hal_Rd8(s_Host, REG_ID);
			eve_printf_debug("EVE register ID after wake up %x\n", chipid);
			ft_delay(100);
		}
		eve_printf_debug("EVE register ID after wake up %x\n", chipid);
	}

	/* Read REG_CPURESET to confirm 0 is returned */
	;
	{
		ft_uint8_t engine_status;
		/* Read REG_CPURESET to check if engines are ready.
		Bit 0 for coprocessor engine,
		Bit 1 for touch engine,
		Bit 2 for audio engine.
		*/
		engine_status = Ft_Gpu_Hal_Rd8(s_Host, REG_CPURESET);
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

			engine_status = Ft_Gpu_Hal_Rd8(s_Host, REG_CPURESET);
			ft_delay(100);
		}
		eve_printf_debug("All engines are ready\n");
	}

#ifdef DISPLAY_RESOLUTION_QVGA
	init_DISPLAY_RESOLUTION_QVGA();
#endif
#ifdef DISPLAY_RESOLUTION_WVGA
	init_DISPLAY_RESOLUTION_WVGA();
#endif

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
	init_DISPLAY_RESOLUTION_HVGA_PORTRAIT();
#endif

	Ft_Gpu_Hal_Wr16(s_Host, REG_HCYCLE, FT_DispHCycle);
	Ft_Gpu_Hal_Wr16(s_Host, REG_HOFFSET, FT_DispHOffset);
	Ft_Gpu_Hal_Wr16(s_Host, REG_HSYNC0, FT_DispHSync0);
	Ft_Gpu_Hal_Wr16(s_Host, REG_HSYNC1, FT_DispHSync1);
	Ft_Gpu_Hal_Wr16(s_Host, REG_VCYCLE, FT_DispVCycle);
	Ft_Gpu_Hal_Wr16(s_Host, REG_VOFFSET, FT_DispVOffset);
	Ft_Gpu_Hal_Wr16(s_Host, REG_VSYNC0, FT_DispVSync0);
	Ft_Gpu_Hal_Wr16(s_Host, REG_VSYNC1, FT_DispVSync1);
	Ft_Gpu_Hal_Wr8(s_Host, REG_SWIZZLE, FT_DispSwizzle);
	Ft_Gpu_Hal_Wr8(s_Host, REG_PCLK_POL, FT_DispPCLKPol);
	Ft_Gpu_Hal_Wr16(s_Host, REG_HSIZE, FT_DispWidth);
	Ft_Gpu_Hal_Wr16(s_Host, REG_VSIZE, FT_DispHeight);
	Ft_Gpu_Hal_Wr16(s_Host, REG_CSPREAD, FT_DispCSpread);
	Ft_Gpu_Hal_Wr16(s_Host, REG_DITHER, FT_DispDither);

#ifdef EVE_SCREEN_RESISTIVE
	/* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
	Ft_Gpu_Hal_Wr16(s_Host, REG_TOUCH_RZTHRESH, RESISTANCE_THRESHOLD);
#endif

#if defined(FT_81X_ENABLE)
	Ft_Gpu_Hal_Wr16(s_Host, REG_GPIOX_DIR, 0xffff);
	Ft_Gpu_Hal_Wr16(s_Host, REG_GPIOX, 0xffff);
#else
	Ft_Gpu_Hal_Wr8(s_Host, REG_GPIO_DIR, 0xff);
	Ft_Gpu_Hal_Wr8(s_Host, REG_GPIO, 0xff);
#endif

	Ft_Gpu_Hal_WrMem(s_Host, RAM_DL, (ft_uint8_t *)FT_DLCODE_BOOTUP, sizeof(FT_DLCODE_BOOTUP));
	Ft_Gpu_Hal_Wr8(s_Host, REG_DLSWAP, DLSWAP_FRAME);

	Ft_Gpu_Hal_Wr8(s_Host, REG_PCLK, FT_DispPCLK); //after this display is visible on the LCD

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* to cross check reset pin */
	Ft_Gpu_Hal_Wr8(s_Host, REG_GPIO, 0xff);
	ft_delay(120);
	Ft_Gpu_Hal_Wr8(s_Host, REG_GPIO, 0x7f);
	ft_delay(120);
	Ft_Gpu_Hal_Wr8(s_Host, REG_GPIO, 0xff);
	ft_delay(120);
#endif
	Ft_DisplayPanel_Init();

	/* make the spi to quad mode - addition 2 bytes for silicon */
#ifndef FT_80X_ENABLE
	/* api to set quad and numbe of dummy bytes */
#ifdef ENABLE_SPI_QUAD
	Ft_Gpu_Hal_SetSPI(s_Host, FT_GPU_SPI_QUAD_CHANNEL, FT_GPU_SPI_TWODUMMY);
#elif ENABLE_SPI_DUAL
	Ft_Gpu_Hal_SetSPI(s_Host, FT_GPU_SPI_DUAL_CHANNEL, FT_GPU_SPI_TWODUMMY);
#else
	Ft_Gpu_Hal_SetSPI(s_Host, FT_GPU_SPI_SINGLE_CHANNEL, FT_GPU_SPI_ONEDUMMY);
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
	ft_millis_init();
}

#endif

/* end of file */
