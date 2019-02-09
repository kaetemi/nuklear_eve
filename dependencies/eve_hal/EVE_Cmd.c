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
	if (phost->Status == EVE_HalStatusWriting)
	{
		EVE_Hal_endTransfer(phost);
#if !defined(EVE_SUPPORT_CMDB)
		EVE_Hal_wr16(phost, REG_CMD_WRITE, buffer->CmdWp);
#endif
	}
}

uint16_t EVE_Cmd_rp(EVE_HalContext *phost)
{
	endFunc(phost);
	uint16_t rp = EVE_Hal_rd16(phost, REG_CMD_READ) & EVE_CMD_FIFO_MASK;
	if (EVE_CMD_FAULT(rp))
		phost->CmdFault = true;
	return rp;
}

uint16_t EVE_Cmd_wp(EVE_HalContext *phost)
{
	endFunc(phost);
#if defined(EVE_SUPPORT_CMDB)
	return EVE_Hal_rd16(phost, REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
#else
	phost->CmdWp = EVE_Hal_rd16(phost, REG_CMD_WRITE) & EVE_CMD_FIFO_MASK;
	return phost->CmdWp;
#endif
}

uint16_t EVE_Cmd_space(EVE_HalContext *phost)
{
	endFunc(phost);
#if defined(EVE_SUPPORT_CMDB)
	uint16_t space = EVE_Hal_rd16(phost, REG_CMDB_SPACE) & EVE_CMD_FIFO_MASK;
	if (EVE_CMD_FAULT(space))
		phost->CmdFault = true;
	phost->CmdSpace = space;
	eve_assert(phost->CmdSpace == space);
	return space;
#else
	ft_uint16_t wp = EVE_Cmd_wp(phost);
	ft_uint16_t rp = EVE_Cmd_rp(phost);
	return (wp - rp - 4) & EVE_CMD_FIFO_MASK;
#endif
}

static uint32_t wrBuffer(EVE_HalContext *phost, const void *buffer, uint32_t size, bool progmem, bool string)
{
	eve_assert(string || (size & 0x3) == 0);
	uint32_t transfered = 0;

	do
	{
		uint16_t transfer = (size - transfered) & EVE_CMD_FIFO_MASK;
		uint16_t space = phost->CmdSpace;
		if (space < transfer || space < (EVE_CMD_FIFO_SIZE >> 1))
			space = EVE_Cmd_space(phost);
		if (EVE_CMD_FAULT(space))
			return transfered; /* Co processor fault */
		if (transfer > space)
			transfer = space;
		if (transfer)
		{
			if (phost->Status != EVE_HalStatusWriting)
			{
#if defined(EVE_SUPPORT_CMDB)
				EVE_Hal_startTransfer(phost, EVE_HalTransferWrite, REG_CMDB_WRITE);
#else
				EVE_Hal_startTransfer(phost, EVE_HalTransferWrite, RAM_CMD + buffer->CmdWp);
#endif
			}
			if (string)
			{
				uint32_t t = EVE_Hal_transferString(phost, (const char *)buffer, transfered, transfer, 0x3);
				if (t != transfer) /* End of string */
				{
					transfer = t;
					size = (transfered + transfer);
				}
			}
			else if (progmem)
			{
				EVE_Hal_transferProgmem(phost, NULL, (eve_progmem_const uint8_t *)(uintptr_t)(&((uint8_t *)buffer)[transfered]), transfer);
			}
			else
			{
				EVE_Hal_transferBuffer(phost, NULL, &((uint8_t *)buffer)[transfered], transfer);
			}
			transfered += transfer;
			if (!phost->CmdFunc) /* Keep alive while writing function */
			{
				EVE_Hal_endTransfer(phost);
			}
#if defined(EVE_SUPPORT_CMDB)
			eve_assert(phost->CmdSpace >= transfer);
			phost->CmdSpace -= transfer;
#else
			phost->CmdWp += transfer;
			phost->CmdWp &= EVE_CMD_FIFO_MASK;
			if (!phost->CmdFunc) /* Defer write pointer */
			{
				EVE_Hal_wr16(phost, REG_CMD_WRITE, buffer->CmdWp);
			}
#endif
		}
	} while (transfered < size);
	return transfered;
}

/* Begin writing a function, keeps the transfer open */
void EVE_Cmd_beginFunc(EVE_HalContext *phost)
{
	phost->CmdFunc = true;
}

/* End writing a function, closes the transfer */
void EVE_Cmd_endFunc(EVE_HalContext *phost)
{
	endFunc(phost);
	phost->CmdFunc = false;
}

bool EVE_Cmd_wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, buffer, size, false, false) == size;
}

bool EVE_Cmd_wrProgmem(EVE_HalContext *phost, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	eve_assert(phost->CmdBufferIndex == 0);
	return wrBuffer(phost, (void *)(uintptr_t)buffer, size, true, false) == size;
}

uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t maxLength)
{
	eve_assert(phost->CmdBufferIndex == 0);
	uint32_t transfered = wrBuffer(phost, str, maxLength + 1, false, true);
	return transfered;
}

bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value)
{
	eve_assert(phost->CmdBufferIndex < 4);

	phost->CmdBuffer[phost->CmdBufferIndex++] = value;

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrBuffer(phost, phost->CmdBuffer, 4);
	}

	return true;
}

bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value)
{
	eve_assert(phost->CmdBufferIndex < 3);

	phost->CmdBuffer[phost->CmdBufferIndex++] = value & 0xFF;
	phost->CmdBuffer[phost->CmdBufferIndex++] = value >> 8;

	if (phost->CmdBufferIndex == 4)
	{
		phost->CmdBufferIndex = 0;
		return EVE_Cmd_wrBuffer(phost, phost->CmdBuffer, 4);
	}

	return true;
}

bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value)
{
	eve_assert(phost->CmdBufferIndex == 0);

	if (phost->CmdSpace < 4 && !EVE_Cmd_waitSpace(phost, 4))
		return false;

	if (phost->Status != EVE_HalStatusWriting)
	{
#if defined(EVE_SUPPORT_CMDB)
		EVE_Hal_startTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
#else
		EVE_Hal_startTransfer(phost, FT_GPU_WRITE, RAM_CMD + buffer->CmdWp);
#endif
	}
	EVE_Hal_transfer32(phost, value);
	if (!phost->CmdFunc) /* Keep alive while writing function */
	{
		EVE_Hal_endTransfer(phost);
	}
#if defined(EVE_SUPPORT_CMDB)
	eve_assert(phost->CmdSpace >= 4);
	phost->CmdSpace -= 4;
#else
	phost->CmdWp += 4;
	phost->CmdWp &= EVE_CMD_FIFO_MASK;
	if (!phost->CmdFunc) /* Defer write pointer */
	{
		Ft_Gpu_Hal_Wr16(phost, REG_CMD_WRITE, buffer->CmdWp);
	}
#endif

	return true;
}

/* Move the write pointer forward by the specified number of bytes. Returns the previous write pointer */
uint16_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes)
{
	if (!EVE_Cmd_waitSpace(phost, bytes))
		return -1;

	uint16_t wp = EVE_Cmd_wp(phost);
#if !defined(EVE_SUPPORT_CMDB)
	EVE_Hal_wr16(phost, REG_CMD_WRITE, wp + bytes);
#endif

	return wp;
}

bool EVE_Cmd_waitFlush(EVE_HalContext *phost)
{
	uint16_t rp, wp;

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;
	while ((rp = EVE_Cmd_rp(phost)) != (wp = EVE_Cmd_wp(phost)))
	{
		// eve_printf_debug("Waiting for CoCmd FIFO... rp: %i, wp: %i\n", (int)rp, (int)wp);
		if (EVE_CMD_FAULT(rp))
		{
			/* Co processor fault */
			phost->CmdWaiting = false;
			eve_printf_debug("Co processor fault while waiting for CoCmd FIFO\n");
#if defined(_DEBUG) && (EVE_MODEL >= EVE_BT815)
			char err[128];
			EVE_Hal_rdBuffer(phost, err, RAM_ERR_REPORT, 128);
			eve_printf_debug("%s\n", err);
#endif
			eve_debug_break();
			return false;
		}
		if (phost->Parameters.CbCmdWait)
		{
			if (!phost->Parameters.CbCmdWait(phost))
			{
				/* Wait aborted */
				phost->CmdWaiting = false;
				eve_printf_debug("Wait for CoCmd FIFO aborted\n");
				return false;
			}
		}
	}

	/* Command buffer empty */
	phost->CmdWaiting = false;
	return true;
}

bool EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size)
{
	uint16_t space;

	if (size > EVE_CMD_FIFO_SIZE)
	{
		eve_printf_debug("Requested free space exceeds CoCmd FIFO\n");
		return false;
	}

	eve_assert(!phost->CmdWaiting);
	phost->CmdWaiting = true;

	space = phost->CmdSpace;
	if (space < size || space < (EVE_CMD_FIFO_SIZE >> 1))
		space = EVE_Cmd_space(phost);

	while (space < size)
	{
		if (FT_COCMD_FAULT(space))
		{
			/* Co processor fault */
			phost->CmdWaiting = false;
			eve_printf_debug("Co processor fault while waiting for CoCmd FIFO\n");
#if defined(_DEBUG) && (EVE_MODEL >= EVE_BT815)
			char err[128];
			EVE_Hal_rdBuffer(phost, err, RAM_ERR_REPORT, 128);
			eve_printf_debug("%s\n", err);
#endif
			eve_debug_break();
			return false;
		}
		if (phost->Parameters.CbCmdWait)
		{
			if (!phost->Parameters.CbCmdWait(phost))
			{
				/* Wait aborted */
				phost->CmdWaiting = false;
				eve_printf_debug("Wait for CoCmd FIFO aborted\n");
				return true;
			}
		}
		space = EVE_Cmd_space(phost);
	}

	/* Sufficient space */
	phost->CmdWaiting = false;
	return true;
}

/* end of file */
