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

#include "EVE_Cmd.h"
#include "EVE_Platform.h"

static inline void endFunc(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_WRITING)
	{
		EVE_Hal_endTransfer(phost);
		if (!EVE_Hal_supportCmdB(phost))
		{
			EVE_Hal_wr16(phost, REG_CMD_WRITE, phost->CmdWp);
		}
	}
}

EVE_HAL_EXPORT uint16_t EVE_Cmd_rp(EVE_HalContext *phost)
{
	uint16_t rp;
	endFunc(phost);
	rp = EVE_Hal_rd16(phost, REG_CMD_READ) & EVE_CMD_FIFO_MASK;
	if (EVE_CMD_FAULT(rp))
		phost->CmdFault = true;
	return rp;
}

EVE_HAL_EXPORT uint16_t EVE_Cmd_wp(EVE_HalContext *phost)
{
	endFunc(phost);
	if (EVE_Hal_supportCmdB(phost))
	{
		return EVE_Hal_rd16(phost, REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
	}
	else
	{
		phost->CmdWp = EVE_Hal_rd16(phost, REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
		return phost->CmdWp;
	}
}

EVE_HAL_EXPORT uint16_t EVE_Cmd_space(EVE_HalContext *phost)
{
	uint16_t space;
	uint16_t wp, rp;
	endFunc(phost);
	if (EVE_Hal_supportCmdB(phost))
	{
		space = EVE_Hal_rd16(phost, REG_CMDB_SPACE) & EVE_CMD_FIFO_MASK;
		if (EVE_CMD_FAULT(space))
			phost->CmdFault = true;
		phost->CmdSpace = space;
		return space;
	}
	else
	{
		wp = EVE_Cmd_wp(phost);
		rp = EVE_Cmd_rp(phost);
		space = (rp - wp - 4) & EVE_CMD_FIFO_MASK;
		phost->CmdSpace = space;
		return space;
	}
}

static uint32_t wrBuffer(EVE_HalContext *phost, const void *buffer, uint32_t size, bool progmem, bool string)
{
	uint32_t transfered = 0;

	do
	{
		uint32_t transfer = (size - transfered);
		uint32_t space = phost->CmdSpace;
		uint32_t req = min((string ? (transfer + 1) : transfer), (EVE_CMD_FIFO_SIZE >> 1));
		eve_assert(!string || (req == (transfer + 1)));
		if (space < req)
		{
			if (!EVE_Cmd_waitSpace(phost, req))
				return transfered; /* Coprocessor fault */
			space = phost->CmdSpace;
		}
		if (transfer > space)
			transfer = space;
		eve_assert(transfer <= EVE_CMD_FIFO_SIZE - 4);
		if (transfer)
		{
			if (phost->Status != EVE_STATUS_WRITING)
			{
				if (EVE_Hal_supportCmdB(phost))
				{
					EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, REG_CMDB_WRITE);
				}
				else
				{
					EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_CMD + phost->CmdWp);
				}
			}
			if (string)
			{
				eve_assert(transfered == 0);
				eve_assert(transfer == size); /* Cannot split string transfers */
				eve_assert(transfer <= phost->CmdSpace);
				uint32_t t = EVE_Hal_transferString(phost, (const char *)buffer, transfered, transfer, 0x3);
				if (t != transfer) /* End of string */
				{
					eve_assert(t <= phost->CmdSpace);
					transfer = t;
					size = (transfered + transfer);
				}
				eve_assert((transfered + transfer) == size);
			}
			else if (progmem)
			{
				EVE_Hal_transferProgmem(phost, NULL, (eve_progmem_const uint8_t *)(uintptr_t)(&((uint8_t *)buffer)[transfered]), transfer);
			}
			else
			{
				EVE_Hal_transferMem(phost, NULL, &((uint8_t *)buffer)[transfered], transfer);
			}
			if (!string && (transfer & 0x3))
			{
				eve_assert((transfered + transfer) == size);
				uint32_t pad = 4 - (transfer & 0x3);
				uint8_t padding[4] = { 0 };
				EVE_Hal_transferMem(phost, NULL, padding, pad);
				transfer += pad;
				eve_assert(!(transfer & 0x3));
			}
			transfered += transfer;
			if (!phost->CmdFunc) /* Keep alive while writing function */
			{
				EVE_Hal_endTransfer(phost);
			}
			eve_assert(phost->CmdSpace >= transfer);
			phost->CmdSpace -= (uint16_t)transfer;
			if (!EVE_Hal_supportCmdB(phost))
			{
				phost->CmdWp += (uint16_t)transfer;
				phost->CmdWp &= EVE_CMD_FIFO_MASK;
				if (!phost->CmdFunc) /* Defer write pointer */
				{
					EVE_Hal_wr16(phost, REG_CMD_WRITE, phost->CmdWp);
				}
			}
		}
	} while (transfered < size);
	return transfered;
}

/* Begin writing a function, keeps the transfer open */
EVE_HAL_EXPORT void EVE_Cmd_startFunc(EVE_HalContext *phost)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	phost->CmdFunc = true;
}

/* End writing a function, closes the transfer */
EVE_HAL_EXPORT void EVE_Cmd_endFunc(EVE_HalContext *phost)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	endFunc(phost);
	phost->CmdFunc = false;
}

EVE_HAL_EXPORT bool EVE_Cmd_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, buffer, size, false, false) == size;
}

EVE_HAL_EXPORT bool EVE_Cmd_wrProgmem(EVE_HalContext *phost, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, (void *)(uintptr_t)buffer, size, true, false) == size;
}

EVE_HAL_EXPORT uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t maxLength)
{
	uint32_t transfered;
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);
	transfered = wrBuffer(phost, str, maxLength, false, true);
	return transfered;
}

EVE_HAL_EXPORT bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex < 4);

	phost->CmdBuffer[phost->CmdBufferIndex++] = value;

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrMem(phost, phost->CmdBuffer, 4);
	}

	return true;
}

EVE_HAL_EXPORT bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex < 3);

	phost->CmdBuffer[phost->CmdBufferIndex++] = value & 0xFF;
	phost->CmdBuffer[phost->CmdBufferIndex++] = value >> 8;

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrMem(phost, phost->CmdBuffer, 4);
	}

	return true;
}

EVE_HAL_EXPORT bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value)
{
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);

	if (phost->CmdSpace < 4 && !EVE_Cmd_waitSpace(phost, 4))
		return false;

	if (phost->Status != EVE_STATUS_WRITING)
	{
		if (EVE_Hal_supportCmdB(phost))
		{
			EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, REG_CMDB_WRITE);
		}
		else
		{
			EVE_Hal_startTransfer(phost, EVE_TRANSFER_WRITE, RAM_CMD + phost->CmdWp);
		}
	}
	EVE_Hal_transfer32(phost, value);
	if (!phost->CmdFunc) /* Keep alive while writing function */
	{
		EVE_Hal_endTransfer(phost);
	}
	eve_assert(phost->CmdSpace >= 4);
	phost->CmdSpace -= 4;
	if (!EVE_Hal_supportCmdB(phost))
	{
		phost->CmdWp += 4;
		phost->CmdWp &= EVE_CMD_FIFO_MASK;
		if (!phost->CmdFunc) /* Defer write pointer */
		{
			EVE_Hal_wr16(phost, REG_CMD_WRITE, phost->CmdWp);
		}
	}

	return true;
}

/* Move the write pointer forward by the specified number of bytes. Returns the previous write pointer */
EVE_HAL_EXPORT uint16_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes)
{
	uint16_t wp, prevWp;
	eve_assert(!phost->CmdWaiting);
	eve_assert(phost->CmdBufferIndex == 0);

	if (!EVE_Cmd_waitSpace(phost, bytes))
		return -1;

	prevWp = EVE_Cmd_wp(phost);
	wp = (prevWp + bytes) & EVE_CMD_FIFO_MASK;
	if (!EVE_Hal_supportCmdB(phost))
	{
		phost->CmdWp = wp;
	}
	EVE_Hal_wr16(phost, REG_CMD_WRITE, wp);

	return prevWp;
}

#if defined(_DEBUG) && (EVE_SUPPORT_CHIPID >= EVE_BT815)
#if 0 // TODO
static void displayError(EVE_HalContext *phost, char *err)
{
	uint32_t addr = RAM_G + RAM_G_SIZE - 128;
	uint32_t dl = 0;

	/* Abuse back of RAM_G to store error */
	/* May invalidate user data... */
	EVE_Hal_wrMem(phost, addr, err, 128);

	/* Generate bluescreen */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR_COLOR_RGB(0x00, 0x20, 0x40));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), CLEAR(1, 1, 1));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_HANDLE(15)); /* Scratch handle will reset anyway after reset */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(addr));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SIZE(NEAREST, BORDER, BORDER, 256, 32));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT_H(0, 0));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_LAYOUT(TEXT8X8, 32, 32));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BEGIN(BITMAPS));
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 32, 15, 0));
	/* EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), BITMAP_SOURCE(RAM_ERR_REPORT)); */
	/* EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), VERTEX2II(32, 96, 15, 0)); */
	EVE_Hal_wr32(phost, RAM_DL + ((dl++) << 2), DISPLAY());
	EVE_Hal_wr8(phost, REG_DLSWAP, DLSWAP_FRAME);
}
#endif
#endif

static bool checkWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (EVE_CMD_FAULT(rpOrSpace))
	{
#if defined(_DEBUG) && (EVE_SUPPORT_CHIPID >= EVE_BT815)
#if 0 // TODO
		char err[128];
#endif
#endif
		/* Coprocessor fault */
		phost->CmdWaiting = false;
		eve_printf_debug("Coprocessor fault\n");
#if defined(_DEBUG) && (EVE_SUPPORT_CHIPID >= EVE_BT815)
#if 0 // TODO
		if (EVE_CHIPID >= EVE_BT815)
		{
			EVE_Hal_rdMem(phost, err, RAM_ERR_REPORT, 128);
			eve_printf_debug("%s\n", err);
			displayError(phost, err);
		}
#endif
#endif
		/* eve_debug_break(); */
		return false;
	}

	return true;
}

static bool handleWait(EVE_HalContext *phost, uint16_t rpOrSpace)
{
	/* Check for coprocessor fault */
	if (!checkWait(phost, rpOrSpace))
		return false;

	/* Process any idling */
	EVE_Hal_idle(phost);

	/* Process user idling */
	if (phost->Parameters.CbCmdWait)
	{
		if (!phost->Parameters.CbCmdWait(phost))
		{
			/* Wait aborted */
			phost->CmdWaiting = false;
			eve_printf_debug("Wait for coprocessor aborted\n");
			return false;
		}
	}
	return true;
}

bool EVE_Cmd_waitFlush(EVE_HalContext *phost)
{
	uint16_t rp, wp;

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;
	while ((rp = EVE_Cmd_rp(phost)) != (wp = EVE_Cmd_wp(phost)))
	{
		// eve_printf_debug("Waiting for CoCmd FIFO... rp: %i, wp: %i\n", (int)rp, (int)wp);
		if (!handleWait(phost, rp))
			return false;
	}

	/* Command buffer empty */
	phost->CmdWaiting = false;
	return true;
}

EVE_HAL_EXPORT bool EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size)
{
	uint16_t space;

	if (size > (EVE_CMD_FIFO_SIZE - 4))
	{
		eve_printf_debug("Requested free space exceeds coprocessor FIFO\n");
		return false;
	}

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

	space = phost->CmdSpace;
	if (space < size)
		space = EVE_Cmd_space(phost);
	if (!checkWait(phost, space))
		return false;

	while (space < size)
	{
		if (!handleWait(phost, space))
			return false;
		space = EVE_Cmd_space(phost);
	}

	/* Sufficient space */
	phost->CmdWaiting = false;
	return true;
}

EVE_HAL_EXPORT bool EVE_Cmd_waitLogo(EVE_HalContext *phost)
{
	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

	uint16_t rp, wp;
	do
	{
		rp = EVE_Cmd_rp(phost);
		wp = EVE_Cmd_wp(phost);
		if (!handleWait(phost, rp))
			return false;

	} while ((wp != 0) || (rp != 0));

	phost->CmdWaiting = false;
	return true;
}

/* end of file */
