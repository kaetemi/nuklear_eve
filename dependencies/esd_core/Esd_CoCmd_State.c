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

ft_void_t Ft_Gpu_CoCmd_ColdStart(EVE_HalContext *phost)
{
	Eve_CoCmd_SendCmd(phost, CMD_COLDSTART);
}

ft_void_t Ft_Gpu_CoCmd_LoadIdentity(EVE_HalContext *phost)
{
	Eve_CoCmd_SendCmd(phost, CMD_LOADIDENTITY);
}

ft_void_t Ft_Gpu_CoCmd_Translate(EVE_HalContext *phost, ft_int32_t tx, ft_int32_t ty)
{
	uint32_t cmd[3] = {
		CMD_TRANSLATE,
		tx,
		ty,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Rotate(EVE_HalContext *phost, ft_int32_t a)
{
	uint32_t cmd[2] = {
		CMD_ROTATE,
		a,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Scale(EVE_HalContext *phost, ft_int32_t sx, ft_int32_t sy)
{
	uint32_t cmd[3] = {
		CMD_SCALE,
		sx,
		sy,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_SetMatrix(EVE_HalContext *phost)
{
	Eve_CoCmd_SendCmd(phost, CMD_SETMATRIX);
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_SetRotate(EVE_HalContext *phost, ft_uint32_t r)
{
	uint32_t cmd[2] = {
		CMD_SETROTATE,
		r,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

ft_void_t Ft_Gpu_CoCmd_BitmapTransform(EVE_HalContext *phost, ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result)
{
	uint32_t cmd[EVE_CMD_SIZE * 6 * 2 + EVE_CMD_SIZE * 2] = {
		CMD_BITMAP_TRANSFORM,
		x0,
		y0,
		x1,
		y1,
		x2,
		y2,
		tx0,
		ty0,
		tx1,
		ty1,
		tx2,
		ty2,
		result,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_FgColor(EVE_HalContext *phost, ft_uint32_t c)
{
	uint32_t cmd[2] = {
		CMD_FGCOLOR,
		c,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_BgColor(EVE_HalContext *phost, ft_uint32_t c)
{
	uint32_t cmd[2] = {
		CMD_BGCOLOR,
		c,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_GradColor(EVE_HalContext *phost, ft_uint32_t c)
{
	uint32_t cmd[2] = {
		CMD_GRADCOLOR,
		c,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_SetFont(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t ptr)
{
	uint32_t cmd[3] = {
		CMD_SETFONT,
		font,
		ptr,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_SetFont2(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t ptr, ft_uint32_t firstchar)
{
	uint32_t cmd[4] = {
		CMD_SETFONT2,
		font,
		ptr,
		firstchar,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_SetBase(EVE_HalContext *phost, ft_uint32_t base)
{
	uint32_t cmd[2] = {
		CMD_SETBASE,
		base,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_SetBitmap(EVE_HalContext *phost, ft_uint32_t source, ft_uint16_t fmt, ft_uint16_t w, ft_uint16_t h)
{
	uint32_t cmd[4] = {
		CMD_SETBITMAP,
		source,
		(((ft_uint32_t)w << 16) | (fmt & 0xffff)),
		h,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_SetScratch(EVE_HalContext *phost, ft_uint32_t handle)
{
	uint32_t cmd[2] = {
		CMD_SETSCRATCH,
		handle,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_RomFont(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t romslot)
{
	uint32_t cmd[3] = {
		CMD_ROMFONT,
		font,
		romslot,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

/* end of file */
