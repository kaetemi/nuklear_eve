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
#if 0

ft_void_t Eve_CoCmd_SendCmdArr(EVE_HalContext *phost, ft_uint32_t *cmd, ft_size_t nb)
{
#if defined(_DEBUG)
	phost->CmdFrame = FT_FALSE;
#endif
	Ft_Gpu_Hal_WrCmdBuf(phost, (void *)cmd, (ft_uint32_t)(nb * sizeof(ft_uint32_t)));
#if defined(_DEBUG)
	phost->CmdFrame = FT_TRUE;
#endif
}

ft_void_t Eve_CoCmd_SendCmd(EVE_HalContext *phost, ft_uint32_t cmd)
{
	if (!Ft_Gpu_Hal_WaitCmdFreespace(phost, 4))
		return;
#if defined(_DEBUG)
	phost->CmdFrame = FT_FALSE;
#endif
	Ft_Gpu_Hal_WrCmd32(phost, cmd);
#if defined(_DEBUG)
	phost->CmdFrame = FT_TRUE;
#endif
}

ft_void_t Eve_CoCmd_SendStr_S(EVE_HalContext *phost, const ft_char8_t *s, int length)
{
	if (!Ft_Gpu_Hal_WaitCmdFreespace(phost, (length + 3) & ~3))
		return;
#if defined(_DEBUG)
	phost->CmdFrame = FT_FALSE;
#endif
#if (EVE_MODEL < EVE_FT810)
	ft_uint16_t wp = Ft_Gpu_Hal_Rd16(phost, REG_CMD_WRITE);
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, RAM_CMD + wp);
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
#endif
	ft_int16_t len = Ft_Gpu_Hal_TransferString_S(phost, s, length);
	Ft_Gpu_Hal_EndTransfer(phost);
#if (EVE_MODEL < EVE_FT810)
	Ft_Gpu_Hal_Wr16(phost, REG_CMD_WRITE, (wp + len) & FIFO_SIZE_MASK);
#endif
#if defined(_DEBUG)
	phost->CmdFrame = FT_TRUE;
#endif
}

ft_void_t Eve_CoCmd_SendStr(EVE_HalContext *phost, const ft_char8_t *s)
{
	if (!Ft_Gpu_Hal_WaitCmdFreespace(phost, (strlen(s) + 3) & ~3))
		return; // TODO: This strlen can be avoided
#if defined(_DEBUG)
	phost->CmdFrame = FT_FALSE;
#endif
#if (EVE_MODEL < EVE_FT810)
	ft_uint16_t wp = Ft_Gpu_Hal_Rd16(phost, REG_CMD_WRITE);
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, RAM_CMD + wp);
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
#endif
	ft_int16_t len = Ft_Gpu_Hal_TransferString(phost, s);
	Ft_Gpu_Hal_EndTransfer(phost);
#if (EVE_MODEL < EVE_FT810)
	Ft_Gpu_Hal_Wr16(phost, REG_CMD_WRITE, (wp + len) & FIFO_SIZE_MASK);
#endif
#if defined(_DEBUG)
	phost->CmdFrame = FT_TRUE;
#endif
}

ft_void_t Eve_CoCmd_StartFrame(EVE_HalContext *phost)
{
	phost->CmdFrame = FT_TRUE;
}

ft_void_t Eve_CoCmd_EndFrame(EVE_HalContext *phost)
{
	phost->CmdFrame = FT_FALSE;
}

#endif

/* Nothing beyond this */
