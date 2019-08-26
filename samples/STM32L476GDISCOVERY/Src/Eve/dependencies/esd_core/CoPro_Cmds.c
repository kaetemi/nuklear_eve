/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : BRIDGETEK 

Revision History: 
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - fixed bugs
1.0 - date 2015.03.16 - Updated with new APIs for FT81X
1.1 - date 2017.03.24 - Rename APIs
*/

#include "CoPro_Cmds.h"
#include <stdarg.h>

#if defined(BT815_ENABLE) || defined(BT816_ENABLE)
void Gpu_CoCmd_FillWidth(Gpu_Hal_Context_t *phost, uint32_t s)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_FILLWIDTH);
	Gpu_Copro_SendCmd(phost, s);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

void Gpu_CoCmd_Nop(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_NOP);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_Inflate2(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t options)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_INFLATE2);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, options);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

void Gpu_CoCmd_RotateAround(Gpu_Hal_Context_t *phost, int32_t x, int32_t y, int32_t a, int32_t s)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_ROTATEAROUND);
	Gpu_Copro_SendCmd(phost, x);
	Gpu_Copro_SendCmd(phost, y);
	Gpu_Copro_SendCmd(phost, a);
	Gpu_Copro_SendCmd(phost, s);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));
}

void Gpu_CoCmd_GetPoint(Gpu_Hal_Context_t *phost, int16_t x, int16_t y, uint32_t sx, uint32_t sy)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_GETPOINT);
	Gpu_Copro_SendCmd(phost, ((uint32_t)y << 16) | (x & 0xFFFF));
	Gpu_Copro_SendCmd(phost, sx);
	Gpu_Copro_SendCmd(phost, sy);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

void Gpu_CoCmd_FlashErase(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_FLASHERASE);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

/* 
Write data to flash. Constraints: 
- Destination flash address must be virgin (not used before)
- data array must be aligned 256-bit 
*/
void Gpu_CoCmd_FlashWriteExt(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t num, uint8_t *data)
{
	uint32_t i, send_data32 = 0, totalnum = (num + 3) / 4;

	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * (3 + totalnum));
	Gpu_Copro_SendCmd(phost, CMD_FLASHWRITE);
	Gpu_Copro_SendCmd(phost, dest);
	Gpu_Copro_SendCmd(phost, num);
	for (i = 0; i < num; i = i + 4)
	{
		/* Pack 4 bytes into a 32-bit data each sending package */
		send_data32 = *data++;
		send_data32 |= (*data++) << 8;
		send_data32 |= (*data++) << 16;
		send_data32 |= (*data++) << 24;
		Gpu_Copro_SendCmd(phost, send_data32);
	}
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * (3 + totalnum)));
}
void Gpu_CoCmd_FlashWrite(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_FLASHWRITE);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}
/* 
Writes the given data to flash. 
If the data matches the existing contents of flash, nothing is done. 
Otherwise the flash is erased in 4K units, and the data is written.
*/
void Gpu_CoCmd_FlashUpdate(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_FLASHUPDATE);
	Gpu_Copro_SendCmd(phost, dest);
	Gpu_Copro_SendCmd(phost, src);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

/* 
Read data from flash into main memory
*/
void Gpu_CoCmd_FlashRead(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_FLASHREAD);
	Gpu_Copro_SendCmd(phost, dest);
	Gpu_Copro_SendCmd(phost, src);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

void Gpu_CoCmd_FlashSource(Gpu_Hal_Context_t *phost, uint32_t ptr)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_FLASHSOURCE);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

void Gpu_CoCmd_FlashSpiTx(Gpu_Hal_Context_t *phost, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_FLASHSPITX);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

void Gpu_CoCmd_FlashFast(Gpu_Hal_Context_t *phost, uint32_t result)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_FLASHFAST);
	Gpu_Copro_SendCmd(phost, result);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}
void Gpu_CoCmd_FlashSpiRx(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_FLASHSPIRX);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));
}

void Gpu_CoCmd_FlashAttach(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_FLASHATTACH);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_FlashDetach(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_FLASHDETACH);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_FlashSpiDesel(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_FLASHSPIDESEL);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_ClearCache(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_CLEARCACHE);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_Int_RamShared(Gpu_Hal_Context_t *phost, uint32_t ptr)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 2);
	Gpu_Copro_SendCmd(phost, CMD_INT_RAMSHARED);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 2));
}

void Gpu_CoCmd_Sha1(Gpu_Hal_Context_t *phost, uint32_t src, uint32_t num, uint32_t hash)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_SHA1);
	Gpu_Copro_SendCmd(phost, src);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_Copro_SendCmd(phost, hash);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

void Gpu_CoCmd_ResetFonts(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE);
	Gpu_Copro_SendCmd(phost, CMD_RESETFONTS);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE));
}

void Gpu_CoCmd_AnimStart(Gpu_Hal_Context_t *phost, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_ANIMSTART);
	Gpu_Copro_SendCmd(phost, ch);
	Gpu_Copro_SendCmd(phost, aoptr);
	Gpu_Copro_SendCmd(phost, loop);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));
}

void Gpu_CoCmd_GradientA(Gpu_Hal_Context_t *phost, int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_GRADIENTA);
	Gpu_Copro_SendCmd(phost, ((uint32_t)y0 << 16) | (x0 & 0xFFFF));
	Gpu_Copro_SendCmd(phost, argb0);
	Gpu_Copro_SendCmd(phost, ((uint32_t)y1 << 16) | (x1 & 0xFFFF));
	Gpu_Copro_SendCmd(phost, argb1);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));
}

void Gpu_CoCmd_AppendF(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t num)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_APPENDF);
	Gpu_Copro_SendCmd(phost, ptr);
	Gpu_Copro_SendCmd(phost, num);
	Gpu_CoCmd_EndFunc(phost, CMD_SIZE * 3);
}
#endif

#if defined(BT815_ENABLE) || defined(BT816_ENABLE)

#if 0
void Gpu_CoCmd_FlashHelper_Init(Gpu_Hal_Context_t *phost)
{
	while (FLASH_STATUS_DETACHED == Gpu_Hal_Rd8(phost, REG_FLASH_STATUS))
	{
		Gpu_CoCmd_FlashAttach(phost);
	}
}
#endif

/* 
Switch to other flash state
Error code:
- 0x0    command succeeds
- 0xffff command fails (invalid transition state)
- 0xe001 flash is not attached
- 0xe002 no header detected in sector 0 - is flash blank?
- 0xe003 sector 0 data failed integrity check
- 0xe004 device/blob mismatch - was correct blob loaded?
- 0xe005 failed full-speed test - check board wiring
*/
uint32_t Gpu_CoCmd_FlashHelper_SwitchState(Gpu_Hal_Context_t *phost, uint8_t nextState)
{
	uint32_t ret = 0;
	//uint8_t read_data[CMD_FIFO_SIZE]; Debug only
	uint8_t curr_flash_state = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);
	uint16_t ret_addr = 0;
	if (curr_flash_state != nextState)
	{ //Only handle if nextState is diff
		if (FLASH_STATUS_DETACHED == nextState)
		{
			Gpu_CoCmd_FlashDetach(phost);
			EVE_Cmd_waitFlush(phost);
		}
		else if (FLASH_STATUS_BASIC == nextState)
		{
			if (FLASH_STATUS_FULL == curr_flash_state)
			{
				do
				{
					Gpu_CoCmd_FlashDetach(phost);
					EVE_Cmd_waitFlush(phost);
				} while (FLASH_STATUS_DETACHED != Gpu_Hal_Rd8(phost, REG_FLASH_STATUS));
			}
			Gpu_CoCmd_FlashAttach(phost);
			EVE_Cmd_waitFlush(phost);
		}
		else if (FLASH_STATUS_FULL == nextState)
		{
			if (FLASH_STATUS_BASIC != curr_flash_state)
			{
				do
				{
					Gpu_CoCmd_FlashAttach(phost);
					EVE_Cmd_waitFlush(phost);
				} while (FLASH_STATUS_BASIC != Gpu_Hal_Rd8(phost, REG_FLASH_STATUS));
			}
			Gpu_CoCmd_FlashFast(phost, 0);
			EVE_Cmd_waitFlush(phost);

			/* Read the return code in CMD_BUFFER */
			ret_addr = (EVE_Cmd_wp(phost) - 4) & FIFO_SIZE_MASK;
			ret_addr = (ret_addr + 3) & FIFO_BYTE_ALIGNMENT_MASK; //4 byte alignment

			ret = Gpu_Hal_Rd32(phost, RAM_CMD + ret_addr);
			//Gpu_Hal_RdMem(phost, RAM_CMD, read_data, CMD_FIFO_SIZE);
		}
		else
		{
			ret = 0xffff;
		}
	}
	return ret;
}

/*
Switch to other flash state to full mode
*/

uint32_t Gpu_CoCmd_FlashHelper_SwitchFullMode(Gpu_Hal_Context_t *phost)
{
	uint8_t val;
	/* Try detaching and attaching the flash followed by fast mdoe */
	Gpu_CoCmd_FlashDetach(phost);
	EVE_Cmd_waitFlush(phost);
	val = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_DETACHED != val)
	{
		printf("Error, Flash is not able to detatch %d\n", val);
		return 0;
	}

	Gpu_CoCmd_FlashAttach(phost);
	EVE_Cmd_waitFlush(phost);
	val = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_BASIC != val)
	{
		printf("Error, Flash is not able to attach %d\n", val);
		return 0;
	}

	Gpu_CoCmd_FlashFast(phost, 0);
	EVE_Cmd_waitFlush(phost);
	val = Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);

	if (FLASH_STATUS_FULL != val)
	{
		printf("Error, Flash is not able to get into full mode %d\n", val);
		return 0;
	}
	return 1;
}

/* 
Write data to flash, and align byte if needed.
Note: 
- Destination flash address must be virgin (not used before). 
Otherwise, users have to perform flash erase before using.
- Destination address must be 256-byte aligned.
- Automatically padding 0xFF to non-aligned num.
*/
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Write(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t num, uint8_t *write_data)
{
	uint32_t i;
	uint8_t padding_arr[FLASH_WRITE_ALIGN_BYTE]; /* write_data must be 256-byte aligned */
	uint32_t aligned_length = num % FLASH_WRITE_ALIGN_BYTE;

	if (dest_flash % FLASH_WRITE_ALIGN_BYTE != 0) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	if (aligned_length == 0) /* write_data is already aligned */
	{
		Gpu_CoCmd_FlashWriteExt(phost, dest_flash, num, write_data);
		EVE_Cmd_waitFlush(phost);
	}
	else
	{
		/* Write first aligned chunks of write_data */
		if (num - aligned_length > 0)
		{
			Gpu_CoCmd_FlashWriteExt(phost, dest_flash, num - aligned_length, write_data);
			EVE_Cmd_waitFlush(phost);
		}
		/* Write the rest write_data */
		write_data = write_data + num - aligned_length;
		for (i = 0; i < FLASH_WRITE_ALIGN_BYTE; i++)
		{
			if (i < aligned_length)
			{
				padding_arr[i] = *write_data++;
			}
			else
			{
				padding_arr[i] = 0xFF; /* Should use 0xFF instead of 0x00 to avoid writing overhead */
			}
		}
		Gpu_CoCmd_FlashWriteExt(phost, dest_flash + num - aligned_length, FLASH_WRITE_ALIGN_BYTE, padding_arr);
		EVE_Cmd_waitFlush(phost);
	}
	return FLASH_CMD_SUCCESS;
}

/*
Writes the given data to flash. 
If the data matches the existing contents of flash, nothing is done. 
Otherwise the flash is erased in 4K units, and the data is written.
@dest_flash: destination address in flash memory. Must be 4096-byte aligned
@src_ram: source data in main memory. Must be 4-byte aligned
@num: number of bytes to write, should be multiple of 4096, otherwise, dummy data will be padded
*/
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Update(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num)
{
	uint32_t last_chunk = (num % 4096); /* must be multiple of 4096. Cut off the extended data */

	if ((dest_flash % FLASH_UPDATE_ALIGN_BYTE != 0) || ((src_ram % 4) != 0)) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	if (num < FLASH_UPDATE_ALIGN_BYTE)
	{
		Gpu_CoCmd_FlashUpdate(phost, dest_flash, src_ram, FLASH_UPDATE_ALIGN_BYTE);
		EVE_Cmd_waitFlush(phost);
		Gpu_Hal_WaitCmdfifo_empty(phost);
	}
	else if (last_chunk == 0) /* num is multiple of 4k */
	{
		Gpu_CoCmd_FlashUpdate(phost, dest_flash, src_ram, num);
		EVE_Cmd_waitFlush(phost);
	}
	else /* num is not fit in multiple of 4k */
	{
		Gpu_CoCmd_FlashUpdate(phost, dest_flash, src_ram, num - last_chunk);
		EVE_Cmd_waitFlush(phost);

		/* 4k is quite big for allocating new stack/heap data. So reuse the pointer and write dummy data to flash */
		Gpu_CoCmd_FlashUpdate(phost, dest_flash + num - last_chunk, src_ram + num - last_chunk, FLASH_UPDATE_ALIGN_BYTE);
		EVE_Cmd_waitFlush(phost);
	}
	return FLASH_CMD_SUCCESS;
}

/* Read date from flash to array
@dest_ram: address in ram where the flash copy data to
@src_flash: source address in flash memory. Must be 64-byte aligned. From 0 to 64*1024 for 64MB flash
@num: number of bytes would be read
@read_data: pointer to user read data
*/
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Read(Gpu_Hal_Context_t *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data)
{
	num = num - (num % 4); /* Only read lesser or equal aligned bytes */

	if ((src_flash % FLASH_READ_ALIGN_BYTE != 0) || ((dest_ram % 4) != 0)) /* Check aligned address */
	{
		return FLASH_CMD_ALIGNED_ERR;
	}

	Gpu_CoCmd_FlashRead(phost, dest_ram, src_flash, num);
	EVE_Cmd_waitFlush(phost);

	Gpu_Hal_RdMem(phost, dest_ram, read_data, num);
	return FLASH_CMD_SUCCESS;
}

/* 
Erase entire flash.
*/
void Gpu_CoCmd_FlashHelper_Erase(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_FlashErase(phost);
	EVE_Cmd_waitFlush(phost);
}

/* 
Clears the graphics system's flash cache. It should be executed	after 
modifying graphics data in flash, otherwise bitmaps from flash may render
"stale" data. This command must be executed when the display list is in use,
immediately after a CMD SWAP command.
*/
void Gpu_CoCmd_FlashHelper_ClearCache(Gpu_Hal_Context_t *phost)
{
	Gpu_CoCmd_ClearCache(phost);
	EVE_Cmd_waitFlush(phost);
}

/* 
Flash state/status:
- FLASH_STATUS_BASIC    2UL
- FLASH_STATUS_DETACHED 1UL
- FLASH_STATUS_FULL     3UL
- FLASH_STATUS_INIT     0UL
*/
uint8_t Gpu_CoCmd_FlashHelper_GetState(Gpu_Hal_Context_t *phost)
{
	return Gpu_Hal_Rd8(phost, REG_FLASH_STATUS);
}
#endif

/* Nothing beyond this */
