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

#include "EVE_LoadFile.h"
#include "EVE_Platform.h"
#if defined(FT9XX_PLATFORM)

#define EVE_ENABLE_FATFS 1

#if defined(EVE_ENABLE_FATFS)
#include "ff.h"
static bool s_FatFSLoaded = false;
static FATFS s_FatFS;
#endif

/**
 * @brief Mount the SDcard
 * 
 * @param phost Pointer to Hal context
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadSdCard(EVE_HalContext *phost)
{
#if defined(EVE_ENABLE_FATFS)
#ifndef PANL_APPLET
	SDHOST_STATUS status = sdhost_card_detect();
	if (status == SDHOST_CARD_INSERTED)
	{
		if (!s_FatFSLoaded && (f_mount(&s_FatFS, "", 1) != FR_OK))
		{
			eve_printf_debug("FatFS SD card mount failed\n");
		}
		else
		{
			if (!s_FatFSLoaded)
			{
				s_FatFSLoaded = true;
				eve_printf_debug("FatFS SD card mounted successfully\n");
			}
		}
	}
	else
	{
		if (s_FatFSLoaded)
		{
			eve_printf_debug("SD card not detected\n");
			s_FatFSLoaded = false;
		}
	}
#else
	s_FatFSLoaded = true;
#endif
	return s_FatFSLoaded;
#else
	return false;
#endif
}

/**
 * @brief Load a raw file into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
#if defined(EVE_ENABLE_FATFS)
	FRESULT fResult;
	FIL InfSrc;

	int32_t blocklen, filesize;
	uint8_t buffer[512L];
	uint32_t addr = address;

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			EVE_Hal_wrMem(phost, addr, buffer, blocklen);
			addr += blocklen;
		}
		f_close(&InfSrc);
		return true;
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Load file into RAM_G by CMD_INFLATE
 * 
 * @param phost  Pointer to Hal context
 * @param address Address to write
 * @param filename File to load
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
#if defined(EVE_ENABLE_FATFS)
	FRESULT fResult;
	FIL InfSrc;

	int32_t blocklen, filesize;
	uint8_t buffer[512L];

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		EVE_Cmd_wr32(phost, CMD_INFLATE);
		EVE_Cmd_wr32(phost, address);
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (char *)buffer, blocklen))
				break;
		}
		f_close(&InfSrc);
		return EVE_Cmd_waitFlush(phost);
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

/**
 * @brief Load image into RAM_G
 * 
 * @param phost  Pointer to Hal context
 * @param address Address in RAM_G
 * @param filename File to load
 * @param format Target format of image
 * @return true True if ok
 * @return false False if error
 */
bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
{
#if defined(EVE_ENABLE_FATFS)
	FRESULT fResult;
	FIL InfSrc;

	int32_t blocklen, filesize;
	uint8_t buffer[512L];

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
		EVE_Cmd_wr32(phost, address);
		EVE_Cmd_wr32(phost, OPT_NODL);
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (char *)buffer, blocklen))
				break;
		}
		f_close(&InfSrc);

		if (!EVE_Cmd_waitFlush(phost))
			return false;

		if (format)
			*format = EVE_Hal_rd32(phost, 0x3097e8);

		return true;
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
#if defined(EVE_ENABLE_FATFS)
	FRESULT fResult;
	FIL InfSrc;

	int32_t blocklen, filesize;
	uint8_t buffer[512L];

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, 512, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;
			if (!EVE_Cmd_wrMem(phost, (char *)buffer, blocklen))
				break;
			if (transfered)
				*transfered += blocklen;
		}
		f_close(&InfSrc);
		return EVE_Cmd_waitFlush(phost);
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered)
{
#if defined(EVE_ENABLE_FATFS)
	FRESULT fResult;
	FIL InfSrc;

	int32_t blocklen, filesize;
	uint32_t blockSize = min(512, ((phost->MediaFifoSize >> 3) << 2) - 4);
	uint8_t buffer[512L];

	fResult = f_open(&InfSrc, filename, FA_READ | FA_OPEN_EXISTING);
	if (fResult == FR_OK)
	{
		filesize = f_size(&InfSrc);
		while (filesize > 0)
		{
			fResult = f_read(&InfSrc, buffer, blockSize, &blocklen); // read a chunk of src file
			filesize -= blocklen;
			blocklen += 3;
			blocklen &= ~3U;

			if (transfered)
			{
				uint32_t transferedPart;
				if (!EVE_MediaFifo_wrMem(phost, buffer, blocklen, &transferedPart)) /* copy data continuously into media fifo memory */
				{
					/* Coprocessor fault */
					*transfered += transferedPart;
					break;
				}
				*transfered += transferedPart;
				if (transferedPart < blocklen)
					break; /* Early exit, processing done */
			}
			else
			{
				if (!EVE_MediaFifo_wrMem(phost, buffer, blocklen, NULL)) /* copy data continuously into media fifo memory */
					break; /* Coprocessor fault */
			}
		}
		f_close(&InfSrc);
		return transfered ? EVE_Cmd_waitFlush(phost) : EVE_MediaFifo_waitFlush(phost);
	}
	else
	{
		eve_printf_debug("Unable to open file: \"%s\"\n", filename);
		return false;
	}
#else
	eve_printf_debug("No filesystem support, cannot open: \"%s\"\n", filename);
	return false;
#endif
}

#endif

/* end of file */
