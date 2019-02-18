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
#if !defined(FT9XX_PLATFORM)

#include <stdio.h>

bool EVE_Util_loadSdCard(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;
	uint32_t addr = address;

#pragma warning(push)
#pragma warning(disable : 4996)
	afile = fopen(filename, "rb");
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : ftsize;
		fread(pbuff, 1, blocklen, afile);
		ftsize -= blocklen;
		EVE_Hal_wrMem(phost, addr, pbuff, blocklen);
		addr += blocklen;
	}
	fclose(afile);

	return true;
}

bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename)
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

	if (!EVE_Cmd_waitSpace(phost, 8))
		return false; // Space for CMD_INFLATE

#pragma warning(push)
#pragma warning(disable : 4996)
	afile = fopen(filename, "rb"); // read Binary (rb)
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return false;
	}
	EVE_Cmd_wr32(phost, CMD_INFLATE);
	EVE_Cmd_wr32(phost, address);
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : ftsize;
		fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;

		if (!EVE_Cmd_wrMem(phost, (char *)pbuff, blocklen)) /* copy data continuously into command memory */
			break;
	}

	fclose(afile); /* close the opened compressed file */

	return EVE_Cmd_waitFlush(phost);
}

bool EVE_Util_loadImageFile(EVE_HalContext *phost, uint32_t address, const char *filename, uint32_t *format)
{
	FILE *afile;
	uint32_t ftsize = 0;
	uint8_t pbuff[8192];
	uint16_t blocklen;

#pragma warning(push)
#pragma warning(disable : 4996)
	afile = fopen(filename, "rb"); // read Binary (rb)
#pragma warning(pop)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return 0;
	}
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, address);
	EVE_Cmd_wr32(phost, OPT_NODL);
	// TODO: Let it write into the scratch display list handle,
	//  and read it out and write into the bitmapInfo the proper
	//  values to use. Replace compressed bool with uint8 enum to
	//  specify the loading mechanism
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : ftsize;
		fread(pbuff, 1, blocklen, afile); /* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen -= blocklen % 4;

		if (!EVE_Cmd_wrMem(phost, (char *)pbuff, blocklen))
			break;
	}

	fclose(afile); /* close the opened jpg file */

	if (!EVE_Cmd_waitFlush(phost))
		return false;

	if (format)
		*format = EVE_Hal_rd32(phost, 0x3097e8);

	return true;
}

#endif

/* end of file */
