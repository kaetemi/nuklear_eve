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

#ifndef EVE_PLATFORM_FT900_H
#define EVE_PLATFORM_FT900_H

#include "FT_Platform.h"

#if defined(FT900_PLATFORM)

/* Platform-specific HAL features */
// #define EVE_CMD_BUFFERED

/* Hardware or Module specific macros for gpio line numbers  */
#if defined(MM900EV1A) || defined(MM900EV1B) || defined(MM900EV2A) || defined(MM900EV3A) || defined(MM900EV_LITE)
#define FT800_SEL_PIN (0)
#define FT800_PD_N (43) //for mm900 module

#define FT900_UART0_TX (48)
#define FT900_UART0_RX (49)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif

#if (defined(CLEO35))
#define FT800_SEL_PIN (0)
#define FT800_PD_N (66) //for cleo35 module

#define FT900_UART0_TX (48)
#define FT900_UART0_RX (49)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif

#ifdef CLEO50
#define FT800_SEL_PIN (0)
#define FT800_PD_N (0) //for cleo50 module

#define FT900_UART0_TX (48)
#define FT900_UART0_RX (49)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif

#ifdef FT900DEMOBOARD
#define FT800_SEL_PIN (0)
#define FT800_PD_N (32)

#define FT900_UART0_TX (48)
#define FT900_UART0_RX (49)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif

#ifdef PANL35
#define FT800_SEL_PIN 0
#define FT800_PD_N 1
#define FT800_INT 0

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif

#ifdef PANL70
#define FT800_SEL_PIN 0
#define FT800_PD_N 1
#define FT800_INT 0
#define GOODIXGPIO (33)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif //PANL70

#ifdef PANL70PLUS
#define FT800_SEL_PIN 0
#define FT800_PD_N 1
#define FT800_INT 0
#define GOODIXGPIO (33)

/* Timer 1 is been utilized in case of FT900 platform */
#define FT900_FT_MILLIS_TIMER (timer_select_b)
#define FT900_TIMER_MAX_VALUE (65536L)
#define FT900_TIMER_PRESCALE_VALUE (100)
#define FT900_TIMER_OVERFLOW_VALUE (1000)
#endif //PANL70PLUS

#ifndef RTC_PRESENT
#ifdef FT900_PLATFORM_RTC_I2C
#define RTC_PRESENT (1)
#endif
#endif /*RTC_PRESENT*/

/* Standard C libraries */
#define ESD_USING_NEW_LIB // Using NewLib C Library for runtime

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Data types */
#include "FT_DataTypes_FT900.h"

/* FT900 libraries */
#include "ft900_uart_simple.h"
#include "ft900_sdhost.h"
#include "ft900_spi.h"
#include "ft900_gpio.h"
#include "ft900_rtc.h"
#include "ft900_interrupt.h"
#include "ft900_i2cm.h"
#include "ft900.h"
#include "ft900_delay.h"

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

#define eve_printf(fmt, ...) iprintf(fmt, ##__VA_ARGS__)
#define eve_sprintf(str, fmt, ...) siprintf(str, fmt, ##__VA_ARGS__)
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
#include "FT_Gpu_Hal.h"
#include "FT_Gpu.h"
#include "Eve_CoCmd.h"
#include "FT_Hal_Utils.h"
#include "FT_ILI9488.h"
#include "FT_KD2401.h"

#if defined(EVE_MODULE_PANL)
#include "panl.h"
#endif

#endif
#endif /* EVE_PLATFORM_SIMULATION_H */

/* end of file */
