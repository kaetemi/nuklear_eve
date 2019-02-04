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

#include "Ft_Esd_Core.h"

/// A function to get milliseconds for current frame
ESD_FUNCTION(Ft_Esd_GetMillis, Type = ft_uint32_t, DisplayName = "Get Milliseconds", Category = EsdUtilities, Macro)
ft_uint32_t Ft_Esd_GetMillis() { return Esd_CurrentContext->Millis; }

/// A function to get the difference in milliseconds since last frame Update call
ESD_FUNCTION(Ft_Esd_GetDeltaMs, Type = ft_uint32_t, DisplayName = "Get Delta Ms", Category = EsdUtilities, Macro)
ft_uint32_t Ft_Esd_GetDeltaMs() { return Esd_CurrentContext->DeltaMs; }

/// A function to get the current HAL context data structure
ESD_FUNCTION(Ft_Esd_GetHost, Type = Ft_Gpu_Hal_Context_t *, DisplayName = "Get EVE Host", Category = EsdUtilities, Macro)
Ft_Gpu_Hal_Context_t *Ft_Esd_GetHost() { return Ft_Esd_Host; }

#ifdef ESD_SIMULATION

int Ft_Esd_DesignerMode = 0;

ft_uint32_t Ft_Esd_GAlloc_GetTotalUsed(Ft_Esd_GpuAlloc *ga)
{
    if (!Ft_Esd_GAlloc)
        return 0;
    return Ft_Esd_GpuAlloc_GetTotalUsed(Ft_Esd_GAlloc);
}

ft_uint32_t Ft_Esd_GAlloc_GetTotal(Ft_Esd_GpuAlloc *ga)
{
    if (!Ft_Esd_GAlloc)
        return 0;
    return Ft_Esd_GpuAlloc_GetTotal(Ft_Esd_GAlloc);
}

#endif

/* end of file */
