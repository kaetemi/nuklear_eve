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

#include "FT_Platform.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(BT8XXEMU_PLATFORM)
#include "bt8xxemu_inttypes.h"
#endif

#define FT_FALSE false
#define FT_TRUE true

typedef char ft_char8_t;
typedef signed char ft_schar8_t;
typedef unsigned char ft_uchar8_t;

typedef int8_t ft_int8_t;
typedef uint8_t ft_uint8_t;
typedef int16_t ft_int16_t;
typedef uint16_t ft_uint16_t;
typedef uint32_t ft_uint32_t;
typedef int32_t ft_int32_t;
typedef void ft_void_t;
typedef int64_t ft_int64_t;
typedef uint64_t ft_uint64_t;
typedef float ft_float_t;
typedef double ft_double_t;
typedef _Bool ft_bool_t;
typedef size_t ft_size_t;

/// Include Platform specific data types
#if defined(PC_PLATFORM)
#include <FT_DataTypes_PC.h>
#elif defined(FT900_PLATFORM)
#include <FT_DataTypes_FT900.h>
#endif

typedef volatile ft_uint8_t ft_vuint8_t;
typedef volatile ft_uint16_t ft_vuint16;
typedef volatile ft_uint32_t ft_vuint32;

typedef volatile ft_int8_t ft_vint8;
typedef volatile ft_int16_t ft_vint16;
typedef volatile ft_int32_t ft_vint32;

typedef FT_PROGMEM ft_char8_t ft_prog_char8_t;
typedef FT_PROGMEM ft_uchar8_t ft_prog_uchar8_t;
typedef FT_PROGMEM ft_uint16_t ft_prog_uint16_t;

/* end of file */
