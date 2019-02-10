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
#ifndef FT_GPU_HAL_C
#define FT_GPU_HAL_C

#include "FT_Platform.h"
#include "FT_Gpu_Hal.h"

/* Helper api for millis */
/* api to return the time in ms. 0 after reset */

/* Globals for polling implementation */

ft_void_t Ft_Gpu_ClockSelect(EVE_HalContext *phost, FT_GPU_PLL_SOURCE_T pllsource)
{
	Ft_Gpu_HostCommand(phost, pllsource);
}
ft_void_t Ft_Gpu_PLL_FreqSelect(EVE_HalContext *phost, FT_GPU_PLL_FREQ_T freq)
{
	Ft_Gpu_HostCommand(phost, freq);
}
ft_void_t Ft_Gpu_PowerModeSwitch(EVE_HalContext *phost, FT_GPU_POWER_MODE_T pwrmode)
{
	Ft_Gpu_HostCommand(phost, pwrmode);
}
ft_void_t Ft_Gpu_CoreReset(EVE_HalContext *phost)
{
	Ft_Gpu_HostCommand(phost, FT_GPU_CORE_RESET);
}

ft_int32_t Ft_Gpu_ClockTrimming(EVE_HalContext *phost, ft_uint32_t lowFreq)
{
	ft_uint32_t f;
	ft_uint8_t i;

	/* Trim the internal clock by increase the REG_TRIM register till the measured frequency is within the acceptable range.*/
	for (i = 0; (i < 31) && ((f = Ft_Gpu_CurrentFrequency(phost)) < lowFreq); i++)
	{
		Ft_Gpu_Hal_Wr8(phost, REG_TRIM, i); /* increase the REG_TRIM register value automatically increases the internal clock */
	}
	Ft_Gpu_Hal_Wr32(phost, REG_FREQUENCY, f); /* Set the final frequency to be used for internal operations */

	return f;
}

#if (EVE_MODEL >= EVE_FT810)
//This API can only be called when PLL is stopped(SLEEP mode).  For compatibility, set frequency to the FT_GPU_12MHZ option in the FT_GPU_SETPLLSP1_T table.
ft_void_t Ft_Gpu_81X_SelectSysCLK(EVE_HalContext *phost, FT_GPU_81X_PLL_FREQ_T freq)
{
	if (FT_GPU_SYSCLK_72M == freq)
		Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x61 | (0x40 << 8) | (0x06 << 8));
	else if (FT_GPU_SYSCLK_60M == freq)
		Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x61 | (0x40 << 8) | (0x05 << 8));
	else if (FT_GPU_SYSCLK_48M == freq)
		Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x61 | (0x40 << 8) | (0x04 << 8));
	else if (FT_GPU_SYSCLK_36M == freq)
		Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x61 | (0x03 << 8));
	else if (FT_GPU_SYSCLK_24M == freq)
		Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x61 | (0x02 << 8));
	else if (FT_GPU_SYSCLK_DEFAULT == freq) //default clock
		Ft_Gpu_HostCommand_Ext3(phost, 0x61);
}

//Power down or up ROMs and ADCs.  Specified one or more elements in the FT_GPU_81X_ROM_AND_ADC_T table to power down, unspecified elements will be powered up.  The application must retain the state of the ROMs and ADCs as they're not readable from the device.
ft_void_t Ft_GPU_81X_PowerOffComponents(EVE_HalContext *phost, ft_uint8_t val)
{
	Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x49 | (val << 8));
}

//this API sets the current strength of supported GPIO/IO group(s)
ft_void_t Ft_GPU_81X_PadDriveStrength(EVE_HalContext *phost, FT_GPU_81X_GPIO_DRIVE_STRENGTH_T strength, FT_GPU_81X_GPIO_GROUP_T group)
{
	Ft_Gpu_HostCommand_Ext3(phost, (ft_uint32_t)0x70 | (group << 8) | (strength << 8));
}

//this API will hold the system reset active, Ft_Gpu_81X_ResetRemoval() must be called to release the system reset.
ft_void_t Ft_Gpu_81X_ResetActive(EVE_HalContext *phost)
{
	Ft_Gpu_HostCommand_Ext3(phost, FT_GPU_81X_RESET_ACTIVE);
}

//This API will release the system reset, and the system will exit reset and behave as after POR, settings done through SPI commands will not be affected.
ft_void_t Ft_Gpu_81X_ResetRemoval(EVE_HalContext *phost)
{
	Ft_Gpu_HostCommand_Ext3(phost, FT_GPU_81X_RESET_REMOVAL);
}
#endif

#endif
