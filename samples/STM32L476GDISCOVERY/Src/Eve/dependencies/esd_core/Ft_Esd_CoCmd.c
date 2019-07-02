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

#include "Ft_Esd_CoCmd.h"
#include "Gpu_Hal.h"

ft_void_t Ft_Gpu_CoCmd_DlStart(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_DLSTART);
}

ft_void_t Ft_Gpu_CoCmd_Append(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_APPEND);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

ft_void_t Ft_Gpu_CoCmd_Swap(EVE_HalContext *phost)
{
	EVE_Cmd_wr32(phost, CMD_SWAP);
}

ft_void_t Ft_Gpu_CoCmd_Sketch(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_SKETCH);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, format);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));
}

#if (EVE_MODEL == EVE_FT801)
ft_void_t Ft_Gpu_CoCmd_CSketch(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format, ft_uint16_t freq)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_CSKETCH);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, (((uint32_t)freq << 16) | (format & 0xffff)));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));
}
#endif

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_PlayVideo(EVE_HalContext *phost, ft_uint32_t options)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_PLAYVIDEO);
	Gpu_Copro_SendCmd(phost, options);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}
#endif

ft_void_t Ft_Gpu_CoCmd_Logo(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_LOGO);
}

ft_void_t Ft_Gpu_CoCmd_ScreenSaver(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_SCREENSAVER);
}

ft_uint32_t Ft_Gpu_CoCmd_Calibrate(EVE_HalContext *phost)
{
	uint16_t resAddr;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATE);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Wait for the result */
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}

ft_void_t Ft_Gpu_CoCmd_Stop(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_STOP);
}

#if (EVE_MODEL >= EVE_BT815)
ft_bool_t Ft_Gpu_CoCmd_AnimStart(EVE_HalContext *phost, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	uint32_t flashStatus = Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_FULL)
	{
		// Don't attempt to start animation without full flash speed, it hangs the coprocessor
		eve_printf_debug("Attempt to send CMD_ANIMSTART without FLASH_STATUS_FULL (REG_FLASH_STATUS: %i)\n", (int)flashStatus);
		return FT_FALSE;
	}

	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_ANIMSTART);
	Gpu_Copro_SendCmd(phost, ch);
	Gpu_Copro_SendCmd(phost, aoptr);
	Gpu_Copro_SendCmd(phost, loop);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
	return EVE_Cmd_waitFlush(phost);
}

void Ft_Gpu_CoCmd_AnimStop(EVE_HalContext *phost, int32_t ch)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_ANIMSTOP);
	Gpu_Copro_SendCmd(phost, ch);
	Gpu_CoCmd_EndFunc(phost, CMD_SIZE * 2);
}

void Ft_Gpu_CoCmd_AnimXY(EVE_HalContext *phost, int32_t ch, int16_t x, int16_t y)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_ANIMXY);
	Gpu_Copro_SendCmd(phost, ch);
	Gpu_Copro_SendCmd(phost, ((uint32_t)y << 16) | (x & 0xFFFF));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

void Ft_Gpu_CoCmd_AnimDraw(EVE_HalContext *phost, int32_t ch)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_ANIMDRAW);
	Gpu_Copro_SendCmd(phost, ch);
	Gpu_CoCmd_EndFunc(phost, CMD_SIZE * 2);
}

void Ft_Gpu_CoCmd_AnimFrame(EVE_HalContext *phost, int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_ANIMFRAME);
	Gpu_Copro_SendCmd(phost, ((uint32_t)y << 16) | (x & 0xFFFF));
	Gpu_Copro_SendCmd(phost, aoptr);
	Gpu_Copro_SendCmd(phost, frame);
	Gpu_CoCmd_EndFunc(phost, CMD_SIZE * 4);
}
#endif

bool ESD_Cmd_regRead(uint32_t ptr, uint32_t *result)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint16_t resAddr;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_REGREAD);
	EVE_Cmd_wr32(phost, ptr);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	return true;
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_VideoStart(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_VIDEOSTART);
}

ft_void_t Ft_Gpu_CoCmd_VideoFrame(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t ptr)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_VIDEOFRAME);
	Gpu_Copro_SendCmd(phost, dst);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}
#endif

bool ESD_Cmd_getProps(uint32_t *ptr, uint32_t *w, uint32_t *h)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint16_t resAddr;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPROPS);
	resAddr = EVE_Cmd_moveWp(phost, 12);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (ptr)
		*ptr = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	if (w)
		*w = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
	if (h)
		*h = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 8);
	return true;
}

/* Get the end memory address of data inflated by CMD_INFLATE */
bool ESD_Cmd_getPtr(uint32_t *result)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint16_t resAddr;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPTR);
	resAddr = EVE_Cmd_moveWp(phost, 12);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	return true;
}

ft_void_t Ft_Gpu_CoCmd_TouchTransform(EVE_HalContext *phost, ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 6 * 2 + CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_TOUCH_TRANSFORM);
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

ft_void_t Ft_Gpu_CoCmd_Interrupt(EVE_HalContext *phost, ft_uint32_t ms)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_INTERRUPT);
	Gpu_Copro_SendCmd(phost, ms);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

bool ESD_Cmd_getMatrix(int32_t *m)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint16_t resAddr;
	int i;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETMATRIX);
	resAddr = EVE_Cmd_moveWp(phost, 6 * 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (m)
	{
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + resAddr);
		for (i = 0; i < 6; ++i)
			m[i] = EVE_Hal_transfer32(phost, 0);
		EVE_Hal_endTransfer(phost);
	}
	return true;
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Sync(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_SYNC);
}
#endif

ft_void_t Ft_Gpu_CoCmd_Track(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t tag)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_TRACK);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, tag);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Int_RAMShared(EVE_HalContext *phost, ft_uint32_t ptr)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_INT_RAMSHARED);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

ft_void_t Ft_Gpu_CoCmd_Int_SWLoadImage(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t options)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_INT_SWLOADIMAGE);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, options);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}
#endif

/* Nothing beyond this */
