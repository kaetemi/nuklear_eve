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
#if defined(BT8XXEMU_PLATFORM)

#include <stdio.h>

void Ft_Hal_LoadSDCard()
{

}

ft_bool_t Ft_Hal_LoadRawFile(EVE_HalContext *phost, ft_uint32_t address, const char *filename)
{
	FILE *afile;
	ft_uint32_t ftsize = 0;
	ft_uint8_t pbuff[8192];
	ft_uint16_t blocklen;
	ft_uint32_t addr = address;

	afile = fopen(filename, "rb");
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		return FT_FALSE;
	}
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : ftsize;
		fread(pbuff, 1, blocklen, afile);
		ftsize -= blocklen;
		Ft_Gpu_Hal_WrMem(phost, addr, pbuff, blocklen);
		addr += blocklen;
	}
	fclose(afile);

	return FT_TRUE;
}

ft_bool_t Ft_Hal_LoadInflateFile(EVE_HalContext *phost, ft_uint32_t address, const char *filename)
{
	if (!Ft_Gpu_Hal_WaitCmdFreespace(phost, 8))
		return FT_FALSE; // Space for CMD_INFLATE

	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame

	FILE *afile;
	ft_uint32_t ftsize = 0;
	ft_uint8_t pbuff[8192];
	ft_uint16_t blocklen;

	afile = fopen(filename, "rb");		// read Binary (rb)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		phost->CmdFrame = cmdFrame;
		return FT_FALSE;
	}
	Ft_Gpu_Hal_WrCmd32(phost, CMD_INFLATE);
	Ft_Gpu_Hal_WrCmd32(phost, address);
	fseek(afile, 0, SEEK_END);
	ftsize = ftell(afile);
	fseek(afile, 0, SEEK_SET);
	while (ftsize > 0)
	{
		blocklen = ftsize > 8192 ? 8192 : ftsize;
		fread(pbuff, 1, blocklen, afile);							/* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;

		if (!Ft_Gpu_Hal_WrCmdBuf(phost, (char *)pbuff, blocklen)) /* copy data continuously into command memory */
			break;
	}

	fclose(afile);												/* close the opened compressed file */

	phost->CmdFrame = cmdFrame;
	return Ft_Gpu_Hal_WaitCmdFifoEmpty(phost);
}

ft_bool_t Ft_Hal_LoadImageFile(EVE_HalContext *phost, ft_uint32_t address, const char *filename, ft_uint32_t *format)
{
	if (!Ft_Gpu_Hal_WaitCmdFreespace(phost, 12))
		return FT_FALSE; // Space for CMD_LOADIMAGE

	ft_bool_t cmdFrame = phost->CmdFrame;
	phost->CmdFrame = FT_FALSE; // Can safely bypass active frame

	FILE *afile;
	ft_uint32_t ftsize = 0;
	ft_uint8_t pbuff[8192];
	ft_uint16_t blocklen;

	afile = fopen(filename, "rb");		// read Binary (rb)
	if (afile == NULL)
	{
		eve_printf_debug("Unable to open: %s\n", filename);
		phost->CmdFrame = cmdFrame;
		return 0;
	}
	Ft_Gpu_Hal_WrCmd32(phost, CMD_LOADIMAGE);
	Ft_Gpu_Hal_WrCmd32(phost, address);
	Ft_Gpu_Hal_WrCmd32(phost, OPT_NODL);
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
		fread(pbuff, 1, blocklen, afile);							/* copy the data into pbuff and then transfter it to command buffer */
		ftsize -= blocklen;
		blocklen += 3;
		blocklen -= blocklen % 4;

		if (!Ft_Gpu_Hal_WrCmdBuf(phost, (char *)pbuff, blocklen))
			break;
	}

	fclose(afile);												/* close the opened jpg file */

	phost->CmdFrame = cmdFrame;
	if (!Ft_Gpu_Hal_WaitCmdFifoEmpty(phost))
		return FT_FALSE;

	if (format)
		*format = Ft_Gpu_Hal_Rd32(phost, 0x3097e8);

	return FT_TRUE;
}

#endif

/* end of file */
