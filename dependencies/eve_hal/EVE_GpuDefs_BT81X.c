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

#include "EVE_Config.h"
#ifdef EVE_MULTI_TARGET
#include "EVE_Hal.h"

#undef EVE_MULTI_TARGET
#define BT_81X_ENABLE
#include "EVE_GpuDefs.h"

EVE_GpuDefs EVE_GpuDefs_BT81X = {
	REG_CLOCK,
	REG_CMD_READ,
	REG_CMD_WRITE,
	REG_CMD_DL,
	REG_CMDB_SPACE,
	REG_CMDB_WRITE,
	REG_TRIM,
	REG_FREQUENCY,
	REG_DLSWAP,
	REG_ID,
	REG_CPURESET,
	REG_HCYCLE,
	REG_HOFFSET,
	REG_HSYNC0,
	REG_HSYNC1,
	REG_VCYCLE,
	REG_VOFFSET,
	REG_VSYNC0,
	REG_VSYNC1,
	REG_SWIZZLE,
	REG_PCLK_POL,
	REG_HSIZE,
	REG_VSIZE,
	REG_CSPREAD,
	REG_DITHER,
	REG_TOUCH_RZTHRESH,
	REG_GPIO_DIR,
	REG_GPIO,
	REG_PCLK,
	REG_PLAYBACK_PLAY,
	RAM_DL,
	RAM_CMD,
	ROM_CHIPID,
	ROMFONT_TABLEADDRESS,

};

#endif

/* end of file */