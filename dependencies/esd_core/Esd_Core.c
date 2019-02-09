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

#include "Esd_Core.h"
#include "Ft_Esd_Utility.h"

#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_Dl.h"
#include "Ft_Esd_CoCmd.h"
#include "Ft_Esd_BitmapHandle.h"
#include "Ft_Esd_TouchTag.h"

//
// Globals
//
Esd_Context *Esd_CurrentContext = NULL;
Ft_Gpu_Hal_Context_t *Ft_Esd_Host = NULL; // Pointer to current s_Host
Ft_Esd_GpuAlloc *Ft_Esd_GAlloc = NULL; // Pointer to current s_GAlloc

//
// External definitions
//
// TODO: Decouple global state
#if _DEBUG
void Esd_CheckTypeSizes();
#endif
// FT_Mcu_Hal.c
ft_void_t Eve_BootupConfig(Ft_Gpu_Hal_Context_t *s_Host);
ft_void_t Ft_Mcu_Init();

extern ft_int16_t FT_DispWidth;
extern ft_int16_t FT_DispHeight;

extern void Ft_Hal_LoadSDCard();
// extern void Ft_Esd_Widget_ProcessFree(); // TODO: Bind from widgets

// When not in the simulation, use the Ft_Main__Start etc symbols
// as exported by the single Application logic document included
#ifndef ESD_SIMULATION
#ifdef BT8XXEMU_PLATFORM
#define Ft_Main__Running__ESD() BT8XXEMU_isRunning(Ft_Esd_Host->Emulator)
#else
#define Ft_Main__Running__ESD() (1)
#endif
#else
int Ft_Main__Running__ESD();
int Ft_Sleep__ESD(int ms);
#endif

void Esd_ResetGpuState();
void Esd_ResetCoState(); // TODO: Call after coprocessor reset procedure

// extern void Ft_Esd_Timer_CancelGlobal(); // TODO: Bind from widgets
// extern void Ft_Esd_Timer_UpdateGlobal(); // TODO: Bind from widgets

//
// Constants
//
#define FT_WELCOME_MESSAGE "Copyright (C) Bridgetek Pte Ltd\n"

void Esd_SetCurrent(Esd_Context *ec)
{
	Esd_CurrentContext = ec;
	Ft_Esd_Host = &ec->HalContext;
	Ft_Esd_GAlloc = &ec->GpuAlloc;
}

void Esd_Defaults(Esd_Parameters *ep)
{
	memset(ep, 0, sizeof(Esd_Parameters));
}

void Esd_Initialize(Esd_Context *ec, Esd_Parameters *ep)
{
	memset(ec, 0, sizeof(Esd_Context));
	ec->ClearColor = 0x212121;
	ec->Start = (void (*)(void *))ep->Start;
	ec->Update = (void (*)(void *))ep->Update;
	ec->Render = (void (*)(void *))ep->Render;
	ec->Idle = (void (*)(void *))ep->Idle;
	ec->End = (void (*)(void *))ep->End;
	ec->UserContext = ep->UserContext;
	Esd_SetCurrent(ec);

#ifdef ESD_SIMULATION
	eve_printf_debug("\f"); // Shows horizontal line in ESD output window
#endif
	eve_printf_debug(FT_WELCOME_MESSAGE);

#if _DEBUG
	Esd_CheckTypeSizes();
#endif

	Ft_Mcu_Init();

	Ft_Gpu_HalInit_t halInit;
	Ft_Gpu_Hal_Init(&halInit);

	Ft_Gpu_Hal_Open(&ec->HalContext);
	Eve_BootupConfig(&ec->HalContext);

#ifndef ESD_SIMULATION
	// TODO: Store calibration somewhere!
	if (!Esd_Calibrate())
	{
		eve_printf_debug("Calibrate failed\n");
	}
#endif

	Ft_Esd_GpuAlloc_Reset(&ec->GpuAlloc);

	Esd_BitmapHandle_Initialize();
	Esd_ResetCoState();
}

void Esd_Release(Esd_Context *ec)
{
	Ft_Gpu_Hal_Close(&ec->HalContext);
	memset(ec, 0, sizeof(Esd_Context));

	Esd_CurrentContext = NULL;
	Ft_Esd_Host = NULL;
	Ft_Esd_GAlloc = NULL;
}

void Esd_Shutdown()
{
	Ft_Gpu_Hal_DeInit();
}

void Esd_Loop(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	if (!Ft_Main__Running__ESD() || ec->RequestStop)
		return;

	Esd_Start(ec);

	while (Ft_Main__Running__ESD() && !ec->RequestStop)
	{
		Esd_Update(ec);
		Esd_Render(ec);
		Esd_WaitSwap(ec);
	}

	Esd_Stop(ec);
}

void Esd_Start(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Initialize framework
	Esd_ResetGpuState();
	ec->Frame = 0;
	ec->Millis = ft_millis();
	// Ft_Esd_Timer_CancelGlobal(); // TODO

	// Initialize storage
	Ft_Hal_LoadSDCard();
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Initialize application
	if (ec->Start)
		ec->Start(ec->UserContext);
}

void Esd_Update(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	// Restore initial frame values
	// Ft_Gpu_CoCmd_LoadIdentity(phost); // ?
	Esd_ResetGpuState();
	// Ft_Esd_Widget_ProcessFree(); // TODO: Link this back up!!!
	Esd_BitmapHandle_FrameStart(&ec->HandleState);

	if (ec->ShowLogo)
		return;
	if (ec->ShowingLogo)
		ec->Millis = ft_millis(); // Reset time

	// Verify initialization
	Ft_Hal_LoadSDCard();
#if defined(EVE_FLASH_AVAILABLE)
	Esd_AttachFlashFast();
#endif

	// Idle at least once every frame
	ec->LoopState = ESD_LOOPSTATE_IDLE;
	if (!ec->SwapIdled)
	{
		if (ec->Idle)
			ec->Idle(ec->UserContext);
		Ft_Gpu_Hal_ESD_Idle(phost);
	}

	// Update GUI state before render
	ec->LoopState = ESD_LOOPSTATE_UPDATE;
	ft_uint32_t ms = ft_millis(); // Calculate frame time delta
	ec->DeltaMs = ms - ec->Millis;
	ec->Millis = ms;
	Ft_Esd_GpuAlloc_Update(Ft_Esd_GAlloc); // Run GC
	Ft_Esd_TouchTag_Update(NULL); // Update touch
	if (ec->Update)
		ec->Update(ec->UserContext);
	// Ft_Esd_Timer_UpdateGlobal(); // TODO

	// Return to idle state inbetween
	ec->LoopState = ESD_LOOPSTATE_IDLE;
}

void Esd_Render(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	if (ec->ShowLogo)
	{
		ec->ShowLogo = FT_FALSE;
		ec->ShowingLogo = FT_TRUE;
		Esd_BeginLogo();
		return;
	}

	if (ec->ShowingLogo)
	{
		ec->ShowingLogo = FT_FALSE;
		Esd_EndLogo();
	}

	// Process all coprocessor commands
	ec->LoopState = ESD_LOOPSTATE_RENDER;

	Ft_Gpu_CoCmd_StartFrame(phost);

	Ft_Gpu_CoCmd_SendCmd(phost, CMD_DLSTART);
	Ft_Gpu_CoCmd_SendCmd(phost, (2UL << 24) | ec->ClearColor); // Set CLEAR_COLOR_RGB from user var
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_TAG(255)); // Always default to 255, so no touch = 0, touch non-tag = 255
	Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
	if (ec->Render)
		ec->Render(ec->UserContext);

	if (ec->SpinnerPopup)
	{
		// Spinner used for switching longer loading pages with bitmaps etc
		Ft_Esd_Dl_COLOR_RGB(~(ec->ClearColor));
		Ft_Esd_CoCmd_Spinner(Esd_Update, FT_DispWidth / 2, FT_DispHeight / 2, 0, 0);
		ec->SpinnerPopup = FT_FALSE;
		ec->SpinnerPopped = FT_TRUE;
	}
	else if (ec->SpinnerPopped)
	{
		Ft_Esd_CoCmd_Stop(Esd_Update);
		ec->SpinnerPopped = FT_FALSE;
	}

	Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
	Ft_Gpu_CoCmd_Swap(phost);

	Ft_Gpu_CoCmd_EndFrame(phost);

	// Replacement for Ft_Gpu_Hal_WaitCmdfifo_empty(phost); with idle function
	ec->LoopState = ESD_LOOPSTATE_IDLE;

	// Advance frame count
	++ec->Frame;
}

void Esd_WaitSwap(Esd_Context *ec)
{
	Esd_SetCurrent(ec);
	EVE_HalContext *phost = &ec->HalContext;

	ec->SwapIdled = FT_FALSE;
	ft_uint16_t rp, wp;
	Ft_Gpu_Hal_RdCmdRpWp(&ec->HalContext, &rp, &wp);
	while (rp != wp)
	{
		// Reset if coprocessor fault
		if (FT_COCMD_FAULT(rp))
		{
			eve_printf_debug("TODO: Reset coprocessor on fault\n");
#if defined(_DEBUG) && (EVE_MODEL >= EVE_BT815)
			char err[128];
			Ft_Gpu_Hal_RdMem(phost, RAM_ERR_REPORT, err, 128);
			eve_printf_debug("%s\n", err);
#endif
			break;
		}

		// Loop an idle task instead of doing nothing
		if (ec->Idle)
			ec->Idle(ec->UserContext);
		Ft_Gpu_Hal_ESD_Idle(&ec->HalContext);
		ec->SwapIdled = FT_TRUE;

		Ft_Gpu_Hal_RdCmdRpWp(phost, &rp, &wp);
	}
}

void Esd_Stop(Esd_Context *ec)
{
	Esd_SetCurrent(ec);

	// Cleanup application (generally unreachable)
	ec->LoopState = ESD_LOOPSTATE_NONE;
	// Ft_Esd_Timer_CancelGlobal(); // TODO
	if (ec->End)
		ec->End(ec->UserContext);
}

/* end of file */
