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

#include "Esd_CoCmd.h"
#include "Gpu_Hal.h"

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
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_TRANSLATE);
	Gpu_Copro_SendCmd(phost, tx);
	Gpu_Copro_SendCmd(phost, ty);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

ft_void_t Ft_Gpu_CoCmd_Rotate(EVE_HalContext *phost, ft_int32_t a)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_ROTATE);
	Gpu_Copro_SendCmd(phost, a);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_Scale(EVE_HalContext *phost, ft_int32_t sx, ft_int32_t sy)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_SCALE);
	Gpu_Copro_SendCmd(phost, sx);
	Gpu_Copro_SendCmd(phost, sy);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

ft_void_t Ft_Gpu_CoCmd_SetMatrix(EVE_HalContext *phost)
{
	Eve_CoCmd_SendCmd(phost, CMD_SETMATRIX);
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_SetRotate(EVE_HalContext *phost, ft_uint32_t r)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_SETROTATE);
	Gpu_Copro_SendCmd(phost, r);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}
#endif

ft_void_t Ft_Gpu_CoCmd_BitmapTransform(EVE_HalContext *phost, ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 6 * 2 + CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_BITMAP_TRANSFORM);
	Gpu_Copro_SendCmd(phost, x0);
	Gpu_Copro_SendCmd(phost, y0);
	Gpu_Copro_SendCmd(phost, x1);
	Gpu_Copro_SendCmd(phost, y1);
	Gpu_Copro_SendCmd(phost, x2);
	Gpu_Copro_SendCmd(phost, y2);
	Gpu_Copro_SendCmd(phost, tx0);
	Gpu_Copro_SendCmd(phost, ty0);
	Gpu_Copro_SendCmd(phost, tx1);
	Gpu_Copro_SendCmd(phost, ty1);
	Gpu_Copro_SendCmd(phost, tx2);
	Gpu_Copro_SendCmd(phost, ty2);
	Gpu_Copro_SendCmd(phost, result);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 6 * 2 + CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_FgColor(EVE_HalContext *phost, ft_uint32_t c)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_FGCOLOR);
	Gpu_Copro_SendCmd(phost, c);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_BgColor(EVE_HalContext *phost, ft_uint32_t c)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_BGCOLOR);
	Gpu_Copro_SendCmd(phost, c);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_GradColor(EVE_HalContext *phost, ft_uint32_t c)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_GRADCOLOR);
	Gpu_Copro_SendCmd(phost, c);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_SetFont(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t ptr)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_SETFONT);
	Gpu_Copro_SendCmd(phost, font);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_SetFont2(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t ptr, ft_uint32_t firstchar)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_SETFONT2);
	Gpu_Copro_SendCmd(phost, font);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, firstchar);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

ft_void_t Ft_Gpu_CoCmd_SetBase(EVE_HalContext *phost, ft_uint32_t base)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_SETBASE);
	Gpu_Copro_SendCmd(phost, base);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_SetBitmap(EVE_HalContext *phost, ft_uint32_t source, ft_uint16_t fmt, ft_uint16_t w, ft_uint16_t h)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_SETBITMAP);
	Gpu_Copro_SendCmd(phost, source);
	EVE_Cmd_wr16(phost, fmt);
	EVE_Cmd_wr16(phost, w);
	Gpu_Copro_SendCmd(phost, h);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

ft_void_t Ft_Gpu_CoCmd_SetScratch(EVE_HalContext *phost, ft_uint32_t handle)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_SETSCRATCH);
	Gpu_Copro_SendCmd(phost, handle);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_RomFont(EVE_HalContext *phost, ft_uint32_t font, ft_uint32_t romslot)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_ROMFONT);
	Gpu_Copro_SendCmd(phost, font);
	Gpu_Copro_SendCmd(phost, romslot);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}
#endif

/* end of file */
