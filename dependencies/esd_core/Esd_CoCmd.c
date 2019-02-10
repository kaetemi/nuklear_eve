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
#include "FT_Gpu_Hal.h"

ft_void_t Ft_Gpu_CoCmd_DlStart(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_DLSTART);
}

ft_void_t Ft_Gpu_CoCmd_Append(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	uint32_t cmd[3] = {
		CMD_APPEND,
		ptr,
		num,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Swap(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_SWAP);
}

ft_void_t Ft_Gpu_CoCmd_Sketch(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format)
{
	uint32_t cmd[5] = {
		CMD_SKETCH,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		ptr,
		format,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

#if (EVE_MODEL == EVE_FT801)
ft_void_t Ft_Gpu_CoCmd_CSketch(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format, ft_uint16_t freq)
{
	uint32_t cmd[5] = {
		CMD_CSKETCH,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		ptr,
		(((ft_uint32_t)freq << 16) | (format & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_PlayVideo(EVE_HalContext *phost, ft_uint32_t options)
{
	uint32_t cmd[2] = {
		CMD_PLAYVIDEO,
		options,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
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
	uint32_t resAddr;
	uint32_t res;

	uint32_t cmd[2] = {
		CMD_CALIBRATE,
		0,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));

	// Wait for the result
	Ft_Gpu_CoCmd_EndFrame(phost);
	eve_printf_debug("Waiting for Calibrate result\n");
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return 0;
	resAddr = (Ft_Gpu_Hal_Rd32(phost, REG_CMD_WRITE) - 4) & EVE_CMD_FIFO_MASK;
	res = Ft_Gpu_Hal_Rd32(phost, RAM_CMD + resAddr); // Fetch result
	Ft_Gpu_CoCmd_StartFrame(phost);
	return res;
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
		// Don't attempt to start animation without full flash speed, it hangs the co processor
		eve_printf_debug("Attempt to send CMD_ANIMSTART without FLASH_STATUS_FULL (REG_FLASH_STATUS: %i)\n", (int)flashStatus);
		return FT_FALSE;
	}

	scope
	{
		uint32_t cmd[4] = {
			CMD_ANIMSTART,
			ch,
			aoptr,
			loop,
		};
		EVE_Cmd_wrMem(phost, (ft_uint8_t *)cmd, sizeof(cmd));
		return EVE_Cmd_waitFlush(phost);
	}
}

void Ft_Gpu_CoCmd_AnimStop(EVE_HalContext *phost, int32_t ch)
{
	uint32_t cmd[2] = {
		CMD_ANIMSTOP,
		ch,
	};
	EVE_Cmd_wrMem(phost, (ft_uint8_t *)cmd, sizeof(cmd));
}

void Ft_Gpu_CoCmd_AnimXY(EVE_HalContext *phost, int32_t ch, int16_t x, int16_t y)
{
	uint32_t cmd[3] = {
		CMD_ANIMXY,
		ch,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
	};
	EVE_Cmd_wrMem(phost, (ft_uint8_t *)cmd, sizeof(cmd));
}

void Ft_Gpu_CoCmd_AnimDraw(EVE_HalContext *phost, int32_t ch)
{
	uint32_t cmd[2] = {
		CMD_ANIMDRAW,
		ch,
	};
	EVE_Cmd_wrMem(phost, (ft_uint8_t *)cmd, sizeof(cmd));
}

void Ft_Gpu_CoCmd_AnimFrame(EVE_HalContext *phost, int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
	uint32_t cmd[4] = {
		CMD_ANIMFRAME,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		aoptr,
		frame,
	};
	EVE_Cmd_wrMem(phost, (ft_uint8_t *)cmd, sizeof(cmd));
}
#endif

// FIXME
/*
ft_void_t Ft_Gpu_CoCmd_RegRead(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t result)
{
	uint32_t cmd[3] = {
		CMD_REGREAD,
		ptr,
		0,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
*/

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_VideoStart(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_VIDEOSTART);
}

ft_void_t Ft_Gpu_CoCmd_VideoFrame(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t ptr)
{
	uint32_t cmd[3] = {
		CMD_VIDEOFRAME,
		dst,
		ptr,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

// FIXME
/*
ft_void_t Ft_Gpu_CoCmd_GetProps(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t w, ft_uint32_t h)
{
	uint32_t cmd[4] = {
		CMD_GETPROPS,
		ptr,
		w,
		h,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
*/

ft_void_t Ft_Gpu_CoCmd_TouchTransform(EVE_HalContext *phost, ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result)
{
	uint32_t cmd[4 * 6 * 2 + 4 * 2] = {
		CMD_TOUCH_TRANSFORM,
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
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Interrupt(EVE_HalContext *phost, ft_uint32_t ms)
{
	uint32_t cmd[2] = {
		CMD_INTERRUPT,
		ms,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

void Ft_Gpu_CoCmd_GetMatrix(EVE_HalContext *phost, int32_t *m)
{
	uint16_t resAddr;
	int i;

	EVE_Cmd_beginFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETMATRIX);
	resAddr = EVE_Cmd_moveWp(phost, 6 * 4);
	EVE_Cmd_endFunc(phost);
	
	/* Read result */
	EVE_Cmd_waitFlush(phost);
	EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + resAddr);
	for (i = 0; i < 6; ++i)
		m[i] = EVE_Hal_transfer32(phost, 0);
	EVE_Hal_endTransfer(phost);
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Sync(EVE_HalContext *phost)
{
	Ft_Gpu_CoCmd_SendCmd(phost, CMD_SYNC);
}
#endif

ft_void_t Ft_Gpu_CoCmd_Track(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t tag)
{
	uint32_t cmd[4] = {
		CMD_TRACK,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		tag,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Int_RAMShared(EVE_HalContext *phost, ft_uint32_t ptr)
{
	uint32_t cmd[2] = {
		CMD_INT_RAMSHARED,
		ptr,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Int_SWLoadImage(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t options)
{
	uint32_t cmd[3] = {
		CMD_INT_SWLOADIMAGE,
		ptr,
		options,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

// FIXME
/*
ft_void_t Ft_Gpu_CoCmd_GetPtr(EVE_HalContext *phost, ft_uint32_t result)
{
	uint32_t cmd[2] = {
		CMD_GETPTR,
		result,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
*/

/* Nothing beyond this */
