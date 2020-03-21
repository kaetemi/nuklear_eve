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

ft_void_t Ft_Gpu_CoCmd_MemCpy(EVE_HalContext *phost, ft_uint32_t dest, ft_uint32_t src, ft_uint32_t num)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMCPY);
	EVE_Cmd_wr32(phost, dest);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, num);
	EVE_Cmd_endFunc(phost);
}

ft_void_t Ft_Gpu_CoCmd_MemSet(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t value, ft_uint32_t num)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMSET);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, value);
	EVE_Cmd_wr32(phost, num);
	EVE_Cmd_endFunc(phost);
}

ft_void_t Ft_Gpu_CoCmd_MemZero(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMZERO);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, num);
	EVE_Cmd_endFunc(phost);
}

ft_void_t Ft_Gpu_CoCmd_MemWrite(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t num)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMWRITE);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, num);
	EVE_Cmd_endFunc(phost);
}

bool ESD_Cmd_memCrc(ft_uint32_t ptr, ft_uint32_t num, ft_uint32_t *result)
{
	EVE_HalContext *phost = Ft_Esd_Host;
	uint16_t resAddr;

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMCRC);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, num);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);

	/* Read result */
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	return true;
}

ft_void_t Ft_Gpu_CoCmd_LoadImage(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t options)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, options);
	EVE_Cmd_endFunc(phost);
}

ft_void_t Ft_Gpu_CoCmd_Inflate(EVE_HalContext *phost, ft_uint32_t ptr)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_endFunc(phost);
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_MediaFifo(EVE_HalContext *phost, ft_uint32_t ptr, ft_uint32_t size)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEDIAFIFO);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, size);
	EVE_Cmd_endFunc(phost);
}
#endif

ft_void_t Ft_Gpu_CoCmd_Snapshot(EVE_HalContext *phost, ft_uint32_t ptr)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_SNAPSHOT);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_endFunc(phost);
}

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
ft_void_t Ft_Gpu_CoCmd_Snapshot2(EVE_HalContext *phost, ft_uint32_t fmt, ft_uint32_t ptr, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_SNAPSHOT2);
	EVE_Cmd_wr32(phost, fmt);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	EVE_Cmd_endFunc(phost);
}
#endif

#ifdef EVE_FLASH_AVAILABLE
/// Attach flash. Return new FLASH_STATUS
ft_uint32_t Ft_Gpu_CoCmd_FlashAttach(EVE_HalContext *phost)
{
	uint32_t flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus != FLASH_STATUS_DETACHED)
		return flashStatus; // Only attach when detached
	if (!EVE_Cmd_waitFlush(phost))
		return flashStatus; // Coprocessor must be ready
	EVE_Cmd_wr32(phost, CMD_FLASHATTACH);
	EVE_Cmd_waitFlush(phost); // Wait for command completion
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS); // Return current status
}

/// Enter fast flash state. Return new FLASH_STATUS
ft_uint32_t Ft_Gpu_CoCmd_FlashFast(EVE_HalContext *phost, ft_uint32_t *result)
{
	uint32_t resAddr;
	uint32_t flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0xE001;
		return flashStatus;
	}
	if (flashStatus > FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0;
		return flashStatus;
	} // Only enter fast mode when attached
	if (!EVE_Cmd_waitFlush(phost))
	{
		if (result)
			*result = 0xE000;
		return flashStatus;
	} // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHFAST);
	resAddr = EVE_Cmd_moveWp(phost, 4); // Get the address where the coprocessor will write the result
	EVE_Cmd_endFunc(phost);
	EVE_Cmd_waitFlush(phost); // Wait for command completion
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr); // Fetch result
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS); // Return current status
}

ft_bool_t Ft_Gpu_CoCmd_FlashRead(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t size)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHREAD);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, size);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost);
}

ft_bool_t Ft_Gpu_CoCmd_LoadImage_Flash(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH | OPT_NODL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Image failed to load
	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);
	return true;
}

ft_bool_t Ft_Gpu_CoCmd_Inflate_Flash(EVE_HalContext *phost, ft_uint32_t dst, ft_uint32_t src)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_INFLATE2);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost); // Image failed to load
}
#endif

ft_bool_t Ft_Gpu_CoCmd_LoadImage_ProgMem(EVE_HalContext *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size, ft_uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_NODL);
	EVE_Cmd_wrProgmem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Image failed to load
	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);
	return true;
}

ft_bool_t Ft_Gpu_CoCmd_Inflate_ProgMem(EVE_HalContext *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false; // Coprocessor must be ready
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wrProgmem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost); // Resource failed to load
}

/* end of file */
