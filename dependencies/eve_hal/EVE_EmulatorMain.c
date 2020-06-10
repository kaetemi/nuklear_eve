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

#include "EVE_EmulatorMain.h"
#include "EVE_Platform.h"
#ifdef EVE_EMULATOR_MAIN

#undef main

BT8XXEMU_Emulator *EVE_GpuEmu = NULL;
BT8XXEMU_Flash *EVE_EmuFlash = NULL;

int s_ArgC;
char **s_ArgV;

#if defined(ESD_SIMULATION)

void LogMessage__ESD(const char *str);
void LogWarning__ESD(const char *str);
void LogError__ESD(const char *str);

void ESD_SetFlashFirmware__ESD(const wchar_t *path);

/**
 * @brief Log a message
 * 
 * @param sender Pointer to BT8XXEMU_Emulator sender
 * @param context unused
 * @param type Type of message
 * @param message Message to log
 */
static void emulatorLog(BT8XXEMU_Emulator *sender, void *context, BT8XXEMU_LogType type, const char *message)
{
	if (type == BT8XXEMU_LogMessage)
		LogMessage__ESD(message);
	else if (type == BT8XXEMU_LogWarning)
		LogWarning__ESD(message);
	else
		LogError__ESD(message);
}

/**
 * @brief Log a message on flash
 * 
 * @param sender Pointer to BT8XXEMU_Emulator sender
 * @param context unused
 * @param type Type of message
 * @param message Message to log
 */
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

void EVE_emuMain(int argc, char *argv[]);

/**
 * @brief Main function of the application
 * 
 * @param sender Pointer to BT8XXEMU_Emulator sender
 * @param context Unused
 */
void emulatorMain(BT8XXEMU_Emulator *sender, void *context)
{
	EVE_emuMain(s_ArgC, s_ArgV);
}

/**
 * @brief Main function
 * 
 * @param argc number of arguments
 * @param argv Argument array
 * @return int Exit code
 */
int main(int argc, char *argv[])
{
#if defined(EVE_FLASH_AVAILABLE)
	BT8XXEMU_FlashParameters flashParams;
	BT8XXEMU_Flash *flash;
#endif

	BT8XXEMU_EmulatorParameters params;

	s_ArgC = argc;
	s_ArgV = argv;

#if defined(EVE_FLASH_AVAILABLE)
	flashParams;
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
#pragma warning(push)
#pragma warning(disable : 4996)
	wcscpy(flashParams.DataFilePath, L"__Flash.bin");
#pragma warning(pop)
	// flashParams.Persistent = true; // For test purpose
#endif
#if defined(ESD_SIMULATION)
#if defined(EVE_FLASH_MX25L)
	wcscpy(flashParams.DeviceType, L"mx25lemu");
#if defined(ESD_SIMULATION)
#if defined(EVE_FLASH_SIZE) && (EVE_FLASH_SIZE >= 32)
	ESD_SetFlashFirmware__ESD(L"mx25l.blob");
#else
	ESD_SetFlashFirmware__ESD(L"unified.blob");
#endif
#endif
#else
	// Fallback emulation
	wcscpy(flashParams.DeviceType, L"mx25lemu");
	ESD_SetFlashFirmware__ESD(L"unified.blob");
#endif
#endif
	// The flash image will be loaded as a memory mapped file.
	// Both the flash emulation and ESD are mapping the same file independently,
	// so there are no memory conflicts, and the mapping is mostly persistent.
	flash = BT8XXEMU_Flash_create(BT8XXEMU_VERSION_API, &flashParams);
#endif

	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, (BT8XXEMU_EmulatorMode)EVE_SUPPORT_CHIPID);

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
	EVE_EmuFlash = flash;
#endif

	BT8XXEMU_run(BT8XXEMU_VERSION_API, &EVE_GpuEmu, &params);

	if (EVE_GpuEmu)
	{
		BT8XXEMU_stop(EVE_GpuEmu);
		BT8XXEMU_destroy(EVE_GpuEmu);
		EVE_GpuEmu = NULL;
	}
#if defined(EVE_FLASH_AVAILABLE)
	if (EVE_EmuFlash)
	{
		BT8XXEMU_Flash_destroy(EVE_EmuFlash);
		EVE_EmuFlash = NULL;
	}
#endif

	return 0;
}

#endif

/* end of file */