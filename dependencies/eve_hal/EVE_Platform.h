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

/*

Main file to include the EVE HAL in your applications.

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EVE_PLATFORM__H
#define EVE_PLATFORM__H

/* Include all public headers. */
#include "EVE_Config.h"
#if defined(_WIN32)
#include "EVE_Platform_WIN32.h"
#endif
#if defined(BT8XXEMU_PLATFORM)
#include "EVE_Platform_BT8XXEMU.h"
#endif
#if defined(FT4222_PLATFORM)
#include "EVE_Platform_FT4222.h"
#endif
#if defined(MPSSE_PLATFORM)
#include "EVE_Platform_MPSSE.h"
#endif
#if defined(FT9XX_PLATFORM)
#include "EVE_Platform_FT9XX.h"
#endif
#include "EVE_GpuTypes.h"
#include "EVE_Hal.h"
#include "EVE_Cmd.h"
#include "EVE_MediaFifo.h"
#include "EVE_GpuDefs.h"
#if defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT)
#include "EVE_ILI9488.h"
#endif
#include "EVE_Util.h"
#include "EVE_LoadFile.h"

#define eve_noop() \
	do             \
	{              \
	} while (false)

#if defined(_DEBUG)
#if defined(_MSC_VER)
#define eve_debug_break() __debugbreak()
#elif defined(__GCC__)
#define eve_debug_break() __builtin_trap()
#else
#define eve_debug_break() eve_noop()
#endif
#else
#define eve_debug_break() eve_noop()
#endif

#ifndef eve_printf
#define eve_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#ifndef eve_sprintf
#define eve_sprintf(str, fmt, ...) sprintf(str, fmt, ##__VA_ARGS__)
#endif

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
			eve_debug_break();                                                                                                     \
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
#define eve_printf_debug(fmt, ...) eve_noop()
#define eve_assert(cond) eve_noop()
#define eve_assert_ex(cond, ex) eve_noop()
#define eve_assert_do(cond)      \
	do                           \
	{                            \
		bool r__assert = (cond); \
		r__assert = r__assert;   \
	} while (false)
#define eve_trace(cond) eve_noop()
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* Scope */
#ifndef scope
#define scope if (true)
#endif

/* Breakable */
#ifndef breakable
#define breakable for (int eve__scope = 0; eve__scope < 1; ++eve__scope)
#endif

#endif /* #ifndef EVE_PLATFORM__H */

#ifdef __cplusplus
}
#endif

/* end of file */