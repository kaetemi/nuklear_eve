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

ft_void_t Ft_Gpu_CoCmd_MemCpy(EVE_HalContext *phost, ft_uint32_t dest, ft_uint32_t src, ft_uint32_t num)
{
	uint32_t cmd[4] = {
		CMD_MEMCPY,
		dest,
		src,
		num,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_MemSet(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t value, ft_uint32_t num)
{
	uint32_t cmd[4] = {
		CMD_MEMSET,
		ptr,
		value,
		num,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_MemZero(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	uint32_t cmd[3] = {
		CMD_MEMZERO,
		ptr,
		num,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_MemWrite(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	uint32_t cmd[3] = {
		CMD_MEMWRITE,
		ptr,
		num,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_MemCrc(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num, ft_uint32_t result)
{
	uint32_t cmd[4] = {
		CMD_MEMCRC,
		ptr,
		num,
		result,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_LoadImage(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t options)
{
	uint32_t cmd[3] = {
		CMD_LOADIMAGE,
		ptr,
		options,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

ft_void_t Ft_Gpu_CoCmd_Inflate(EVE_HalContext *phost, ft_uint32_t ptr)
{
	uint32_t cmd[2] = {
		CMD_INFLATE,
		ptr,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_MediaFifo(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t size)
{
	uint32_t cmd[3] = {
		CMD_MEDIAFIFO,
		ptr,
		size,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

ft_void_t Ft_Gpu_CoCmd_Snapshot(EVE_HalContext *phost, ft_uint32_t ptr)
{
	uint32_t cmd[2] = {
		CMD_SNAPSHOT,
		ptr,
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}

#if (EVE_MODEL >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Snapshot2(EVE_HalContext *phost, ft_uint32_t fmt, ft_uint32_t ptr, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h)
{
	uint32_t cmd[5] = {
		CMD_SNAPSHOT2,
		fmt,
		ptr,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
	};
	Eve_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/// Attach flash. Return new FLASH_STATUS
ft_uint32_t Ft_Gpu_CoCmd_FlashAttach(EVE_HalContext *phost)
{
	uint32_t flashStatus = Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS);
	if (flashStatus != FLASH_STATUS_DETACHED)
		return flashStatus; // Only attach when detached
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return flashStatus; // Co processor must be ready
	Ft_Gpu_Hal_WrCmd32(phost, CMD_FLASHATTACH);
	Ft_Gpu_Hal_WaitCmdFifoEmpty(phost); // Wait for command completion
	return Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS); // Return current status
}

/// Enter fast flash state. Return new FLASH_STATUS
ft_uint32_t Ft_Gpu_CoCmd_FlashFast(EVE_HalContext *phost, ft_uint32_t *result)
{
	uint32_t flashStatus = Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_BASIC)
	{
		*result = 0xE001;
		return flashStatus;
	}
	if (flashStatus > FLASH_STATUS_BASIC)
	{
		*result = 0;
		return flashStatus;
	} // Only enter fast mode when attached
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
	{
		*result = 0xE000;
		return flashStatus;
	} // Co processor must be ready
	Ft_Gpu_Hal_WrCmd32(phost, CMD_FLASHFAST);
	uint32_t resAddr = Ft_Gpu_Hal_Rd32(phost, REG_CMD_WRITE); // Get the address where the co processor will write the result
	Ft_Gpu_Hal_WrCmd32(phost, *result); // Dummy int
	Ft_Gpu_Hal_WaitCmdFifoEmpty(phost); // Wait for command completion
	*result = Ft_Gpu_Hal_Rd32(phost, RAM_CMD + resAddr); // Fetch result
	return Ft_Gpu_Hal_Rd32(phost, REG_FLASH_STATUS); // Return current status
}

ft_bool_t Ft_Gpu_CoCmd_FlashRead(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t size)
{
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Co processor must be ready
	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame
	Ft_Gpu_Hal_WrCmd32(phost, CMD_FLASHREAD);
	Ft_Gpu_Hal_WrCmd32(phost, dst);
	Ft_Gpu_Hal_WrCmd32(phost, src);
	Ft_Gpu_Hal_WrCmd32(phost, size);
	phost->CmdFrame = cmdFrame;
	return Ft_Gpu_Hal_WaitCmdFifoEmpty(phost);
}

ft_bool_t Ft_Gpu_CoCmd_LoadImage_Flash(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t *format)
{
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Co processor must be ready
	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame
	Ft_Gpu_Hal_WrCmd32(phost, CMD_FLASHSOURCE);
	Ft_Gpu_Hal_WrCmd32(phost, src);
	Ft_Gpu_Hal_WrCmd32(phost, CMD_LOADIMAGE);
	Ft_Gpu_Hal_WrCmd32(phost, dst);
	Ft_Gpu_Hal_WrCmd32(phost, OPT_FLASH | OPT_NODL);
	phost->CmdFrame = cmdFrame;
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Image failed to load
	if (format)
		*format = Ft_Gpu_Hal_Rd32(phost, 0x3097e8);
	return FT_TRUE;
}

ft_bool_t Ft_Gpu_CoCmd_Inflate_Flash(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src)
{
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Co processor must be ready
	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame
	Ft_Gpu_Hal_WrCmd32(phost, CMD_FLASHSOURCE);
	Ft_Gpu_Hal_WrCmd32(phost, src);
	Ft_Gpu_Hal_WrCmd32(phost, CMD_INFLATE2);
	Ft_Gpu_Hal_WrCmd32(phost, dst);
	Ft_Gpu_Hal_WrCmd32(phost, OPT_FLASH);
	phost->CmdFrame = cmdFrame;
	return Ft_Gpu_Hal_WaitCmdFifoEmpty(phost); // Image failed to load
}
#endif

ft_bool_t Ft_Gpu_CoCmd_LoadImage_ProgMem(EVE_HalContext *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size, ft_uint32_t *format)
{
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Co processor must be ready
	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame
	Ft_Gpu_Hal_WrCmd32(phost, CMD_LOADIMAGE);
	Ft_Gpu_Hal_WrCmd32(phost, dst);
	Ft_Gpu_Hal_WrCmd32(phost, OPT_NODL);
	Ft_Gpu_Hal_WrCmdBuf_ProgMem(phost, src, (size + 3) & ~0x3UL);
	phost->CmdFrame = cmdFrame;
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Image failed to load
	if (format)
		*format = Ft_Gpu_Hal_Rd32(phost, 0x3097e8);
	return FT_TRUE;
}

ft_bool_t Ft_Gpu_CoCmd_Inflate_ProgMem(EVE_HalContext *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size)
{
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE; // Co processor must be ready
	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame
	Ft_Gpu_Hal_WrCmd32(phost, CMD_INFLATE);
	Ft_Gpu_Hal_WrCmd32(phost, dst);
	Ft_Gpu_Hal_WrCmdBuf_ProgMem(phost, src, (size + 3) & ~0x3UL);
	phost->CmdFrame = cmdFrame;
	return Ft_Gpu_Hal_WaitCmdFifoEmpty(phost); // Resource failed to load
}

/* end of file */
