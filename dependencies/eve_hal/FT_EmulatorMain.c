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

#include "FT_Emulator.h"
#ifdef FT_EMULATOR_MAIN

#undef main

BT8XXEMU_Emulator *Ft_GpuEmu = NULL;
BT8XXEMU_Flash *Ft_EmuFlash = NULL;

ft_int32_t s_ArgC;
ft_char8_t **s_ArgV;

#if defined(ESD_SIMULATION)

void LogMessage__ESD(const char *str);
void LogWarning__ESD(const char *str);
void LogError__ESD(const char *str);

void Esd_SetFlashFirmware__ESD(const wchar_t *path);

static void emulatorLog(BT8XXEMU_Emulator *sender, void *context, BT8XXEMU_LogType type, const char *message)
{
	if (type == BT8XXEMU_LogMessage)
		LogMessage__ESD(message);
	else if (type == BT8XXEMU_LogWarning)
		LogWarning__ESD(message);
	else
		LogError__ESD(message);
}

static void flashLog(BT8XXEMU_Flash *sender, void *context, BT8XXEMU_LogType type, const char *message)
{
	if (type == BT8XXEMU_LogMessage)
		LogMessage__ESD(message);
	else if (type == BT8XXEMU_LogWarning)
		LogWarning__ESD(message);
	else
		LogError__ESD(message);
}

#endif

void Ft_Main(ft_int32_t argc, ft_char8_t *argv[]);

void emulatorMain(BT8XXEMU_Emulator *sender, void *context)
{
	Ft_Main(s_ArgC, s_ArgV);
}

ft_int32_t main(ft_int32_t argc, ft_char8_t *argv[])
{
	s_ArgC = argc;
	s_ArgV = argv;

#if defined(EVE_FLASH_AVAILABLE)
	BT8XXEMU_FlashParameters flashParams;
	BT8XXEMU_Flash_defaults(BT8XXEMU_VERSION_API, &flashParams);
#if defined(ESD_SIMULATION)
	flashParams.Log = flashLog;
#else
	flashParams.StdOut = true;
#endif
	// ESD takes care of the flash image location and size.
#if defined(EVE_FLASH_SIZE)
	// Flash size set by ESD will be at minimum this size.
	// It may be larger in case there are more assets.
	flashParams.SizeBytes = EVE_FLASH_SIZE * 1024 * 1024;
#elif !defined(ESD_SIMULATION)
	// Not running under ESD, figure out the flash size by file size.
	// (TODO)
	flashParams.SizeBytes = 2097152;
#endif
#if !defined(ESD_SIMULATION)
	wcscpy(flashParams.DataFilePath, L"__Flash.bin");
	// flashParams.Persistent = true; // For test purpose
#endif
#if defined(ESD_SIMULATION)
#	if defined(EVE_FLASH_MX25L)
	wcscpy(flashParams.DeviceType, L"mx25lemu");
#		if defined(ESD_SIMULATION)
#			if defined(EVE_FLASH_SIZE) && (EVE_FLASH_SIZE >= 32)
	Esd_SetFlashFirmware__ESD(L"mx25l.blob");
#			else
	Esd_SetFlashFirmware__ESD(L"unified.blob");
#			endif
#		endif
#	else
	// Fallback emulation
	wcscpy(flashParams.DeviceType, L"mx25lemu");
	Esd_SetFlashFirmware__ESD(L"unified.blob");
#	endif
#endif
	// The flash image will be loaded as a memory mapped file.
	// Both the flash emulation and ESD are mapping the same file independently, 
	// so there are no memory conflicts, and the mapping is mostly persistent.
	BT8XXEMU_Flash *flash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, &flashParams);
#endif

	BT8XXEMU_EmulatorParameters params;

	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, (BT8XXEMU_EmulatorMode)EVE_MODEL);

	params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	params.Main = emulatorMain;
#if defined(ESD_SIMULATION)
	params.Log = emulatorLog;
#else
	params.Flags |= BT8XXEMU_EmulatorEnableStdOut;
#endif
	// params.Graphics = graphics;
#if defined(EVE_FLASH_AVAILABLE)
	params.Flash = flash;
	Ft_EmuFlash = flash;
#endif

	BT8XXEMU_run(BT8XXEMU_VERSION_API, &Ft_GpuEmu, &params);

	if (Ft_GpuEmu)
	{
		BT8XXEMU_stop(Ft_GpuEmu);
		BT8XXEMU_destroy(Ft_GpuEmu);
		Ft_GpuEmu = NULL;
	}
#if defined(EVE_FLASH_AVAILABLE)
	if (Ft_EmuFlash)
	{
		BT8XXEMU_Flash_destroy(Ft_EmuFlash);
		Ft_EmuFlash = NULL;
	}
#endif

	return 0;
}

#endif

/* end of file */