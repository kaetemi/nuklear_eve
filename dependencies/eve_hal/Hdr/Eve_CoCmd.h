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
ft_void_t Ft_Gpu_CoCmd_SendCmd(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t cmd);
ft_void_t Ft_Gpu_CoCmd_SendCmdArr(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t *cmd, ft_size_t nb);
ft_void_t Ft_Gpu_CoCmd_SendStr_S(Ft_Gpu_Hal_Context_t *phost, const ft_char8_t *s, int length);
ft_void_t Ft_Gpu_CoCmd_SendStr(Ft_Gpu_Hal_Context_t *phost, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_StartFrame(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_EndFrame(Ft_Gpu_Hal_Context_t *phost);

// Deprecated
#define Ft_Gpu_Copro_SendCmd Ft_Gpu_CoCmd_SendCmd

#endif /* EVE_CO_CMD__H */

/* end of file */
