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

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(BT8XXEMU_PLATFORM)

#include <bt8xxemu.h>

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	g_HalPlatform.TotalChannels = 1;
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	/* no-op */
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	/* no-op */
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
#if defined(FT_EMULATOR_MAIN)
	phost->Emulator = Ft_GpuEmu;
	phost->EmulatorFlash = Ft_EmuFlash;
#else
	BT8XXEMU_EmulatorParameters params;
	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, Ft_Emulator_Mode());

	params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->Emulator, &params);
#endif

#if defined(ESD_SIMULATION)
	Ft_MainReady__ESD(phost->Emulator);
#endif

	/* Initialize the context variables */
	phost->Status = !!phost->Emulator 
		? EVE_HalStatusOpened 
		: EVE_HalStatusClosed;

	return !!phost->Emulator;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
#if !defined(FT_EMULATOR_MAIN)
	if (phost->Emulator)
	{
		BT8XXEMU_stop(phost->Emulator);
		BT8XXEMU_destroy(phost->Emulator);
	}
	phost->Emulator = NULL;
	phost->EmulatorFlash = NULL;
#else
	phost->Emulator = NULL;
	phost->EmulatorFlash = NULL;
#endif

	phost->Status = EVE_HalStatusClosed;
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */