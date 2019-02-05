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

#ifndef EVE_PLATFORM_MSVC_H
#define EVE_PLATFORM_MSVC_H

#include "FT_Platform.h"

#if defined(MSVC_PLATFORM)

/* Platform-specific HAL features */
#define FT_GPU_COCMD_BUFFERED

/* Module connected from PC to basic modules */
#ifdef VA800A_SPI
#define ENABLE_SPI_SINGLE (1)
#endif /* VA800A_SPI */

/* Sane options for Windows header */
#ifndef NOMINMAX
#define NOMINMAX
#endif
#if !defined(NTDDI_VERSION) && !defined(_WIN32_WINNT) && !defined(WINVER)
#define NTDDI_VERSION 0x05010000 /* NTDDI_WINXP */
#define _WIN32_WINNT 0x0501 /* _WIN32_WINNT_WINXP */
#define WINVER 0x0501 /* _WIN32_WINNT_WINXP */
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/* C library inclusions */
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

#include <direct.h>
#include <time.h>
#include <io.h>

/* D2xx and SPI from FTDI inclusions */
#include "ftd2xx.h"
#include "LibFT4222.h"

#if _DEBUG
#if defined(_MSC_VER)
#define eve_debug_break() __debugbreak()
#elif defined(__GCC__)
#define eve_debug_break() __builtin_trap()
#else
#define eve_debug_break() \
	do                    \
	{                     \
	} while (false)
#endif
#else
#define eve_debug_break() \
	do                    \
	{                     \
	} while (false)
#endif

#define eve_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define eve_sprintf(str, fmt, ...) sprintf(str, fmt, ##__VA_ARGS__)
#if defined(_DEBUG)
#define eve_printf_debug(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#define eve_assert(cond)                                                                                                           \
	do                                                                                                                             \
	{                                                                                                                              \
		if (!(cond))                                                                                                               \
		{                                                                                                                          \
			const char *str = #cond;                                                                                               \
			const char *sf = __FILE__;                                                                                             \
			eve_printf("EVE Assert Failed: %s (in file '%s' on line '%i')\n", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
		}                                                                                                                          \
	} while (false)
#define eve_assert_ex(cond, ex)                                                                                                                             \
	do                                                                                                                                                      \
	{                                                                                                                                                       \
		if (!(cond))                                                                                                                                        \
		{                                                                                                                                                   \
			const char *str = #cond;                                                                                                                        \
			const char *sf = __FILE__;                                                                                                                      \
			eve_printf("EVE Assert Failed: %s (%s) (in file '%s' on line '%i')\n", ex ? ex : "<NULL>", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
			eve_debug_break();                                                                                                                              \
		}                                                                                                                                                   \
	} while (false)
#define eve_assert_do(cond) eve_assert(cond)
#define eve_trace(str)                                                                                                     \
	do                                                                                                                     \
	{                                                                                                                      \
		const char *sc = __FILE__;                                                                                         \
		eve_printf("EVE Trace: %s (in file '%s' on line '%i')\n", (str) ? (str) : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
	} while (false)
#else
#define eve_printf_debug(fmt, ...) \
	do                             \
	{                              \
	} while (false)
#define eve_assert(cond) \
	do                   \
	{                    \
	} while (false)
#define eve_assert_ex(cond, ex) \
	do                          \
	{                           \
	} while (false)
#define eve_assert_do(cond)           \
	do                                \
	{                                 \
		ft_bool_t r__assert = (cond); \
	} while (false)
#define eve_trace(cond) \
	do                  \
	{                   \
	} while (false)
#endif

/* HAL inclusions */
#include "FT_DataTypes.h"
#include "FT_Gpu_Hal.h"
#include "FT_Gpu.h"
#include "Eve_CoCmd.h"
#include "FT_Hal_Utils.h"

#ifdef FT4222_PLATFORM
#define FT800_SEL_PIN 1 /* GPIO is not utilized in Lib4222 as it is directly managed by firmware */
#define FT800_PD_N GPIO_PORT0
#endif

#ifdef MPSSE_PLATFORM
#define FT800_SEL_PIN 0
#define FT800_PD_N 7
#endif

#endif

#endif /* EVE_PLATFORM_MSVC_H */

/* end of file */
