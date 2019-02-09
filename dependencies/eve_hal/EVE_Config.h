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

#ifndef EVE_CONFIG_H
#define EVE_CONFIG_H

#include "EVE_IntTypes.h"

/*

This file processes the input definitions, 
the available list of which is specified further below for ESD using these macros.
- ESD_TARGET_GRAPHICS(definition, ...)
- ESD_TARGET_DISPLAY(definition, ...)
- ESD_TARGET_PLATFORM(definition, ...)
- ESD_TARGET_FLASH(definition, ...)

*/

#define ESD_TARGET_GRAPHICS(name, ...)
#define ESD_TARGET_DISPLAY(name, ...)
#define ESD_TARGET_PLATFORM(name, ...)
#define ESD_TARGET_FLASH(name, ...)

#define EVE_FT800 0x0800
#define EVE_FT801 0x0801
#define EVE_FT810 0x0810
#define EVE_FT811 0x0811
#define EVE_FT812 0x0812
#define EVE_FT813 0x0813
#define EVE_BT815 0x0815
#define EVE_BT816 0x0816

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Uses regex matching for supported targets.
See https://regexr.com/ for help on regex.

To match only PANL70:
\bPANL70\b
To match platforms starting with MM9
\bMM9
\bMM9\w*\b
Match anything (default):
\b\w+\b
Match nothing:
(?=a)b

IntegratedPlatform specifies that the platform chip is part of the target, the specified string is a display name
IntegratedFlash specifies that the flash chip is part of the target, the specified string is a display name
OverrideSupport specifies that the supported flash of the target overrides the Supported option
of any other target, overriding the Integrated options as well

Example:
SupportedPlatforms = "8XXEMU|\bMM9|FT4222|MPSSE"
SupportedFlash = "\bW25Q\w*\b|\bMX25L\w*\b

*/

ESD_TARGET_GRAPHICS(ME810A_HV35R, DisplayName = "ME810A-HV35R", IntegratedDisplay = "ILI9488 (320x480)", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT810)\b")
ESD_TARGET_GRAPHICS(ME812A_WH50R, DisplayName = "ME812A-WH50R", SupportedDisplays = "\b\w+WVGA\w*\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT812)\b")
// ME813A_WV7C
ESD_TARGET_GRAPHICS(ME813AU_WH50C, DisplayName = "ME813AU-WH50C", SupportedDisplays = "\b\w+WVGA\w*\b", IntegratedPlatform = "FT4222", SupportedArchitectures = "\bFT4222\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT813)\b")
ESD_TARGET_GRAPHICS(PANL35, DisplayName = "PanL35", IntegratedDisplay = "KD2401 (320x480)", IntegratedPlatform = "FT903", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
ESD_TARGET_GRAPHICS(PANL70, DisplayName = "PanL70", IntegratedDisplay = "WVGA (800x480)", IntegratedPlatform = "FT930", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
ESD_TARGET_GRAPHICS(PANL70PLUS, DisplayName = "PanL70 Plus", IntegratedDisplay = "WVGA (800x480)", IntegratedPlatform = "FT930", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT811)\b")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_VM810C, DisplayName = "VM810C", SupportedFlash = "(?=a)b", LibraryTargets = "\b(FT81X|FT810)\b")
ESD_TARGET_GRAPHICS(EVE_GRAPHICS_VM816C, DisplayName = "VM816C", IntegratedFlash = "W25Q128", LibraryTargets = "\b(BT81X|BT816)\b")

// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT800, DisplayName = "FT800 (Generic)", SupportedDisplays = "\b\w+(QVGA|HVGA|AT043B35)\w*\b", LibraryTargets="\b(FT80X|FT800)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT801, DisplayName = "FT801 (Generic)", SupportedDisplays = "\b\w+(QVGA|HVGA|AT043B35)\w*\b", LibraryTargets="\b(FT80X|FT801)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT810, DisplayName = "FT810 (Generic)", LibraryTargets="\b(FT81X|FT810)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT811, DisplayName = "FT811 (Generic)", LibraryTargets="\b(FT81X|FT811)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT812, DisplayName = "FT812 (Generic)", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_FT813, DisplayName = "FT813 (Generic)", LibraryTargets="\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_BT815, DisplayName = "BT815 (Generic)", LibraryTargets="\b(BT81X|BT815)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_BT816, DisplayName = "BT816 (Generic)", LibraryTargets="\b(BT81X|BT816)\b")

// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_CONNECTEVE, DisplayName = "ConnectEVE", IntegratedDisplay = "AT043B35 (480x272)", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT80X|FT800)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI35RES, DisplayName = "mikromedia HMI 3.5\" Res", IntegratedDisplay = "Riverdi (320x240)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI43RES, DisplayName = "mikromedia HMI 4.3\" Res", IntegratedDisplay = "Riverdi (480x272)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI50RES, DisplayName = "mikromedia HMI 5\" Res", IntegratedDisplay = "Riverdi (800x480)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI70RES, DisplayName = "mikromedia HMI 7\" Res", IntegratedDisplay = "Riverdi (800x480)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT812)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI35CAP, DisplayName = "mikromedia HMI 3.5\" Cap", IntegratedDisplay = "Riverdi (320x240)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI43CAP, DisplayName = "mikromedia HMI 4.3\" Cap", IntegratedDisplay = "Riverdi (480x272)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI50CAP, DisplayName = "mikromedia HMI 5\" Cap", IntegratedDisplay = "Riverdi (800x480)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT813)\b")
// ESD_TARGET_GRAPHICS(EVE_GRAPHICS_MIKROE_HMI70CAP, DisplayName = "mikromedia HMI 7\" Cap", IntegratedDisplay = "Riverdi (800x480)", IntegratedPlatform = "FT900Q", SupportedArchitectures = "\bFT32\b", SupportedFlash = "(?=a)b", LibraryTargets="\b(FT81X|FT813)\b")

ESD_TARGET_DISPLAY(EVE_DISPLAY_QVGA, DisplayName = "QVGA (320x240)")
ESD_TARGET_DISPLAY(EVE_DISPLAY_WVGA, DisplayName = "WVGA (800x480)")
// ESD_TARGET_DISPLAY(EVE_DISPLAY_AT043B35, DisplayName = "AT043B35 (480x272)")
// ESD_TARGET_DISPLAY(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT, DisplayName = "ILI9488 (320x480)")
// ESD_TARGET_DISPLAY(EVE_DISPLAY_KD2401_HVGA_PORTRAIT, DisplayName = "KD2401 (320x480)")

ESD_TARGET_PLATFORM(EVE_PLATFORM_BT8XXEMU, DisplayName = "Emulator", Icon = ":/icons/game-monitor.png", OverrideSupport, SupportedFlash = "\b\w*_W25Q\w*\b|\b\w*_MX25L\w*\b", SupportedArchitectures = "\bBT8XXEMU\b")
ESD_TARGET_PLATFORM(MM900EV1A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV1B, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV2A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV3A, SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(MM900EV_LITE, DisplayName = "MM900EV-Lite", SupportedArchitectures = "\bFT32\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_FT4222, DisplayName = "FT4222", Icon = ":/icons/terminal.png", SupportedArchitectures = "\bFT4222\b")
ESD_TARGET_PLATFORM(EVE_PLATFORM_MPSSE, DisplayName = "MPSSE", Icon = ":/icons/terminal.png", SupportedArchitectures = "\bMPSSE\b")

ESD_TARGET_FLASH(EVE_FLASH_W25Q16, DisplayName = "W25Q16")
ESD_TARGET_FLASH(EVE_FLASH_W25Q32, DisplayName = "W25Q32")
ESD_TARGET_FLASH(EVE_FLASH_W25Q64, DisplayName = "W25Q64")
ESD_TARGET_FLASH(EVE_FLASH_W25Q128, DisplayName = "W25Q128")

ESD_TARGET_FLASH(EVE_FLASH_MX25L16, DisplayName = "MX25L16")
ESD_TARGET_FLASH(EVE_FLASH_MX25L32, DisplayName = "MX25L32")
ESD_TARGET_FLASH(EVE_FLASH_MX25L64, DisplayName = "MX25L64")
ESD_TARGET_FLASH(EVE_FLASH_MX25L128, DisplayName = "MX25L128")
ESD_TARGET_FLASH(EVE_FLASH_MX25L256, DisplayName = "MX25L256", FlashFirmware = "mx25l.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L512, DisplayName = "MX25L512", FlashFirmware = "mx25l.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L1024, DisplayName = "MX25L1024", FlashFirmware = "mx25l.blob")
ESD_TARGET_FLASH(EVE_FLASH_MX25L2048, DisplayName = "MX25L2048", FlashFirmware = "mx25l.blob")

// MM930Mini?
// MM930Lite?
// MM932LC?

/*

The following values are set based on the input definitions.
Do not set these values manually; instead, use the specific user definitions.

Graphics target:
- EVE_MODEL=EVE_FT800
- EVE_MODEL=EVE_FT801
- EVE_MODEL=EVE_FT810
- EVE_MODEL=EVE_FT811
- EVE_MODEL=EVE_FT812
- EVE_MODEL=EVE_FT813
- EVE_MODEL=EVE_BT815
- EVE_MODEL=EVE_BT816

Platform target:
- BT8XXEMU_PLATFORM
- FT900_PLATFORM
- FT4222_PLATFORM
- MPSSE_PLATFORM

Display resolution:
- DISPLAY_RESOLUTION_HVGA_PORTRAIT
- DISPLAY_RESOLUTION_WVGA
- DISPLAY_RESOLUTION_QVGA

Flash, with size in megabytes:
- EVE_FLASH_AVAILABLE
- EVE_FLASH_SIZE=16

Additionally, the following support flags are set:
- EVE_SUPPORT_FLASH (Set even if EVE_FLASH_AVAILABLE is not set)
- EVE_SUPPORT_UNICODE
- EVE_SUPPORT_ASTC
- EVE_SUPPORT_PNG
- EVE_SUPPORT_VIDEO
- EVE_SCREEN_CAPACITIVE
- EVE_SCREEN_RESISTIVE

*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Validate the configured options.

*/

#if defined(ME810A_HV35R) || defined(ME812A_WH50R) || defined(ME813A_WV7C) || defined(ME813AU_WH50C) \
    || defined(EVE_MODULE_PANL)                                                                       \
    || defined(EVE_GRAPHICS_VM810C) || defined(EVE_GRAPHICS_VM816C)                                  \
    || defined(EVE_GRAPHICS_FT800) || defined(EVE_GRAPHICS_FT801)                                    \
    || defined(EVE_GRAPHICS_FT810) || defined(EVE_GRAPHICS_FT811)                                    \
    || defined(EVE_GRAPHICS_FT812) || defined(EVE_GRAPHICS_FT813)                                    \
    || defined(EVE_GRAPHICS_BT816) || defined(EVE_GRAPHICS_BT815)
#define EVE_GRAPHICS_AVAILABLE
#endif

#if defined(EVE_DISPLAY_QVGA) || defined(EVE_DISPLAY_WVGA) \
    || defined(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT) || EVE_DISPLAY_KD2401_HVGA_PORTRAIT
#define EVE_DISPLAY_AVAILABLE
#endif

#if defined(ESD_SIMULATION) || defined(EVE_PLATFORM_BT8XXEMU) \
    || defined(MM900EV1A) || defined(MM900EV1B)               \
    || defined(MM900EV2A) || defined(MM900EV3A)               \
    || defined(MM900EV_LITE)                                  \
    || defined(EVE_PLATFORM_FT4222) || defined(EVE_PLATFORM_MPSSE)
#define EVE_PLATFORM_AVAILABLE
#endif

#if defined(EVE_FLASH_W25Q16) || defined(EVE_FLASH_W25Q32)        \
    || defined(EVE_FLASH_W25Q64) || defined(EVE_FLASH_W25Q128)    \
    || defined(EVE_FLASH_MX25L16) || defined(EVE_FLASH_MX25L32)   \
    || defined(EVE_FLASH_MX25L64) || defined(EVE_FLASH_MX25L128)  \
    || defined(EVE_FLASH_MX25L256) || defined(EVE_FLASH_MX25L512) \
    || defined(EVE_FLASH_MX25L1024) || defined(EVE_FLASH_MX25L2048)
#define EVE_FLASH_AVAILABLE
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

The selected graphics module must set one of the following options.
- FT800_ENABLE
- FT801_ENABLE
- FT810_ENABLE
- FT811_ENABLE
- FT812_ENABLE
- FT813_ENABLE
- BT815_ENABLE
- BT816_ENABLE

It may also set platform, display, and flash values if none are configured.

*/

#if defined(ME810A_HV35R)

#define FT_810_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_ILI9488_HVGA_PORTRAIT
#endif

#elif defined(ME812A_WH50R)

#define FT_812_ENABLE
#define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#elif defined(ME813A_WV7C)

#define FT_813_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#elif defined(ME813AU_WH50C)

#define FT_813_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT4222_PLATFORM
#endif

#elif defined(EVE_GRAPHICS_VM816C)

#define BT_816_ENABLE
// #define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT4222_PLATFORM
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_W25Q128
#endif

#elif defined(EVE_GRAPHICS_VM810C)

#define FT_810_ENABLE
// #define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT4222_PLATFORM
#endif

#elif defined(PANL35)

#define FT_811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_KD2401_HVGA_PORTRAIT
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT900_PLATFORM
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(PANL70)

#define FT_811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT900_PLATFORM
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(PANL70PLUS)

#define FT_811_ENABLE
#define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_PLATFORM_AVAILABLE
#define EVE_PLATFORM_AVAILABLE
#define FT900_PLATFORM
#define malloc pvPortMalloc
#define calloc vPortCalloc
#define free vPortFree
#define PANL_HEAP_SIZE (20 * 1024)
#define TINYPRINTF_OVERRIDE_LIBC (0)
#endif

#elif defined(EVE_GRAPHICS_FT800)

#define FT_801_ENABLE
// #define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#elif defined(EVE_GRAPHICS_FT801)

#define FT_801_ENABLE

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_QVGA
#endif

#elif defined(EVE_GRAPHICS_FT811)

#define FT_811_ENABLE
// #define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#elif defined(EVE_GRAPHICS_BT815)

#define BT_815_ENABLE
// #define ENABLE_SPI_QUAD

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#elif defined(EVE_GRAPHICS_BT816)

#define BT_816_ENABLE
// #define ENABLE_SPI_QUAD
#define RESISTANCE_THRESHOLD (1800)

#ifndef EVE_DISPLAY_AVAILABLE
#define EVE_DISPLAY_AVAILABLE
#define DISPLAY_RESOLUTION_WVGA
#endif

#ifndef EVE_FLASH_AVAILABLE
#define EVE_FLASH_AVAILABLE
#define EVE_FLASH_MX25L128
#endif

#endif

/// Re-Mapping FT800 Series to FT80X
#if defined(FT_800_ENABLE) || defined(FT_801_ENABLE)
#define FT_80X_ENABLE
#endif

/// Re-Mapping FT810 Series to FT81X
#if defined(FT_810_ENABLE) || defined(FT_811_ENABLE) || defined(FT_812_ENABLE) || defined(FT_813_ENABLE)
#define FT_81X_ENABLE
#endif

/// Re-Mapping BT810 Series to BT81X
#if defined(BT_815_ENABLE) || defined(BT_816_ENABLE)
#define BT_81X_ENABLE
#endif

/// Model numbered macro for versioning convenience.
/// Matches the BT8XXEMU_EmulatorMode enum values.
#if defined(FT_800_ENABLE)
#define EVE_MODEL EVE_FT800
#elif defined(FT_801_ENABLE)
#define EVE_MODEL EVE_FT801
#elif defined(FT_810_ENABLE)
#define EVE_MODEL EVE_FT810
#elif defined(FT_811_ENABLE)
#define EVE_MODEL EVE_FT811
#elif defined(FT_812_ENABLE)
#define EVE_MODEL EVE_FT812
#elif defined(FT_813_ENABLE)
#define EVE_MODEL EVE_FT813
#elif defined(BT_815_ENABLE)
#define EVE_MODEL EVE_BT815
#elif defined(BT_816_ENABLE)
#define EVE_MODEL EVE_BT816
#endif

/// Feature support.
/// Avoid hardcoding specific EVE models throughout the libraries.
#if (EVE_MODEL >= EVE_BT815)
#define EVE_SUPPORT_FLASH
#define EVE_SUPPORT_UNICODE
#define EVE_SUPPORT_ASTC
#endif
#if (EVE_MODEL >= EVE_FT810)
#define EVE_SUPPORT_PNG
#define EVE_SUPPORT_VIDEO
#define EVE_SUPPORT_CMDB
#endif
#if ((EVE_MODEL & 0x01) == 0x01)
#define EVE_SCREEN_CAPACITIVE
#else
#define EVE_SCREEN_RESISTIVE
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(EVE_DISPLAY_QVGA)
#define DISPLAY_RESOLUTION_QVGA
#elif defined(EVE_DISPLAY_WVGA)
#define DISPLAY_RESOLUTION_WVGA
#elif defined(EVE_DISPLAY_ILI9488_HVGA_PORTRAIT)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_ILI9488_HVGA_PORTRAIT
#elif defined(EVE_DISPLAY_KD2401_HVGA_PORTRAIT)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT
#define ENABLE_KD2401_HVGA_PORTRAIT
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(EVE_FLASH_W25Q16)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 2
#elif defined(EVE_FLASH_W25Q32)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 4
#elif defined(EVE_FLASH_W25Q64)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 8
#elif defined(EVE_FLASH_W25Q128)
#define EVE_FLASH_W25Q
#define EVE_FLASH_SIZE 16
#elif defined(EVE_FLASH_MX25L16)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 2
#elif defined(EVE_FLASH_MX25L32)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 4
#elif defined(EVE_FLASH_MX25L64)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 8
#elif defined(EVE_FLASH_MX25L128)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 16
#elif defined(EVE_FLASH_MX25L256)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 32
#elif defined(EVE_FLASH_MX25L512)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 64
#elif defined(EVE_FLASH_MX25L1024)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 128
#elif defined(EVE_FLASH_MX25L2048)
#define EVE_FLASH_MX25L
#define EVE_FLASH_SIZE 256
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#define FT_FALSE false
#define FT_TRUE true

typedef char ft_char8_t;
typedef signed char ft_schar8_t;
typedef unsigned char ft_uchar8_t;

#define ft_int8_t int8_t
#define ft_uint8_t uint8_t
#define ft_int16_t int16_t
#define ft_uint16_t uint16_t
#define ft_uint32_t uint32_t
#define ft_int32_t int32_t
#define ft_void_t void
#define ft_int64_t int64_t
#define ft_uint64_t uint64_t
#define ft_float_t float
#define ft_double_t double
#define ft_bool_t bool
#define ft_size_t size_t
/*
typedef volatile ft_uint8_t ft_vuint8_t;
typedef volatile ft_uint16_t ft_vuint16;
typedef volatile ft_uint32_t ft_vuint32;

typedef volatile ft_int8_t ft_vint8;
typedef volatile ft_int16_t ft_vint16;
typedef volatile ft_int32_t ft_vint32;
*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/* Scope */
#ifndef scope
#define scope ;
#endif

/* Breakable */
#ifndef breakable
#define breakable for (int eve__scope = 0; eve__scope < 1; ++eve__scope)
#endif

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(FT900_EMU)
#define eve_progmem __flash__ const
#define eve_progmem_const __flash__ const
#else
#define eve_progmem
#define eve_progmem_const const
#endif

typedef eve_progmem int8_t eve_prog_int8_t;
typedef eve_progmem uint8_t eve_prog_uint8_t;
typedef eve_progmem uint16_t eve_prog_uint16_t;

#define FT_PROGMEM eve_progmem
#define FT_PROGMEM_CONST eve_progmem_const
#define ft_prog_char8_t eve_prog_int8_t
#define ft_prog_uchar8_t eve_prog_uint8_t
#define ft_prog_uint16_t eve_prog_uint16_t

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_FT4222)
// #define EVE_CMD_BUFFERED
#endif

// #define EVE_CMD_BUFFERED

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/*

Ultimately, the platform selection must set one of the following internal platform flags.
- BT8XXEMU_PLATFORM
- FT900_PLATFORM
- FT4222_PLATFORM
- MPSSE_PLATFORM
These may only be set by one of the platform target definitions, and should not be set manually by the user.

*/

#if defined(ESD_SIMULATION) || defined(EVE_PLATFORM_BT8XXEMU)

#define BT8XXEMU_PLATFORM

#elif defined(MM900EV1A) || defined(MM900EV1B) || defined(MM900EV2A) || defined(MM900EV3A) || defined(MM900EV_LITE)

#define FT900_PLATFORM

#elif defined(EVE_MODULE_PANL)

#define FT900_PLATFORM
#define EVE_MODULE_PANL

#elif defined(EVE_PLATFORM_FT4222) || defined(EVE_PLATFORM_MPSSE)

#if defined(EVE_PLATFORM_FT4222)
#define FT4222_PLATFORM
#else
#define MPSSE_PLATFORM
#endif

#define MSVC_PLATFORM

#endif

/// Remap to PC_PLATFORM
#if defined(BT8XXEMU_PLATFORM) || defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
#define PC_PLATFORM
#endif

#endif /* EVE_CONFIG_H */

/* end of file */