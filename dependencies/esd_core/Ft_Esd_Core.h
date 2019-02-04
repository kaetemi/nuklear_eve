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

#ifndef FT_ESD_CORE_H
#define FT_ESD_CORE_H

#include <FT_Platform.h>

#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"
#include "FT_Esd_Dl.h"
#include "FT_Esd_CoCmd.h"
#include "Ft_Esd_BitmapHandle.h"

typedef struct
{
    Ft_Gpu_HalInit_t HalInit;
    Ft_Gpu_Hal_Context_t HalContext; //< Pointer to current s_Host
    Ft_Esd_GpuAlloc GpuAlloc; //< Pointer to current s_GAlloc
    ft_uint32_t Millis; //< Time in milliseconds for current frame
    ft_uint32_t DeltaMs; //< Delta time in milliseconds between frames
    ft_uint32_t Frame; //< Number of times Render has been called
    ft_rgb32_t ClearColor; //< Screen clear color (default is 0x212121)
    ft_uint8_t LoopState; //< Current state of loop

    ft_bool_t ShowLogo; //< True to pop up logo during next render
    ft_bool_t SpinnerPopup;

    ft_bool_t SwapIdled; //< True if idled during swap
    ft_bool_t SpinnerPopped; //< Spinner is currently visible
    void *CmdOwner; //< Owner of currently long-running coprocessor function (sketch, spinner, etc.)

    ft_uint32_t CoFgColor; //< Current coprocessor foreground color
    ft_uint32_t CoBgColor; //< Current coprocessor background color
#if (EVE_MODEL >= EVE_FT810)
    ft_uint8_t CoScratchHandle; //< Current coprocessor scratch handle (reset 15)
#endif

    Esd_HandleState HandleState;

} Esd_Context;

extern Esd_Context *Esd_CurrentContext; //< Pointer to current ESD context
extern Ft_Gpu_Hal_Context_t *Ft_Esd_Host; //< Pointer to current EVE hal context
extern Ft_Esd_GpuAlloc *Ft_Esd_GAlloc; //< Pointer to current allocator

void Esd_SetCurrent(Esd_Context *ec);

void Esd_Initialize(Esd_Context *ec);
void Esd_Release(Esd_Context *ec);
void Esd_Shutdown();

void Esd_Loop(Esd_Context *ec);

void Esd_Start(Esd_Context *ec);
void Esd_Update(Esd_Context *ec, ft_bool_t render);
void Esd_WaitSwap(Esd_Context *ec);
void Esd_Stop(Esd_Context *ec);

#endif /* #ifndef FT_ESD_CORE_H */

/* end of file */
