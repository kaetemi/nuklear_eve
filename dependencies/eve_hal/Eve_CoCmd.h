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

#ifndef EVE_CO_CMD__H
#define EVE_CO_CMD__H

#include "FT_Platform.h"
#include "EVE_Cmd.h"

#define Ft_Gpu_CoCmd_SendCmd EVE_Cmd_wr32
inline static ft_void_t Ft_Gpu_CoCmd_SendCmdArr(EVE_HalContext *phost, ft_uint32_t *cmd, ft_size_t nb)
{
	EVE_Cmd_wrBuffer(phost, (uint8_t *)cmd, (uint32_t)nb * 4);
}
#define Ft_Gpu_CoCmd_SendStr(phost, str) EVE_Cmd_wrString(phost, str, EVE_CMD_STRING_MAX)
#define Ft_Gpu_CoCmd_SendStr_S EVE_Cmd_wrString
inline static ft_void_t Ft_Gpu_CoCmd_StartFrame(EVE_HalContext *phost)
{
	/* no-op */
}
inline static ft_void_t Ft_Gpu_CoCmd_EndFrame(EVE_HalContext *phost)
{
	/* no-op */
}

#define Eve_CoCmd_SendCmd Ft_Gpu_CoCmd_SendCmd
#define Eve_CoCmd_SendCmdArr Ft_Gpu_CoCmd_SendCmdArr
#define Eve_CoCmd_SendStr Ft_Gpu_CoCmd_SendStr
#define Eve_CoCmd_SendStr_S Ft_Gpu_CoCmd_SendStr_S
#define Eve_CoCmd_StartFrame Ft_Gpu_CoCmd_StartFrame
#define Eve_CoCmd_EndFrame Ft_Gpu_CoCmd_EndFrame
#define Ft_Gpu_Copro_SendCmd Ft_Gpu_CoCmd_SendCmd

#endif /* EVE_CO_CMD__H */

/* end of file */
