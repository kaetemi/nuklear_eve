/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Abstract:

This file contains is functions for all UI fields.

Author : BRIDGETEK

Revision History:
0.1 - date 2013.04.24 - initial version
0.2 - date 2014.04.28 - Split in individual files according to platform
0.3 - date 2015.09.25 - Added VM810C50 platform macro
*/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/* platform specific macros */
#define MSVC_FT800EMU                             (1)// enable by default for emulator platform


#define VM800B43_50                             (1)
//#define VM800B35                                (1)
//#define VM801B43_50                             (1)
//#define VM810C50                                (1)
//#define ME812AU_WH50R                           (1)
//#define ME813AU_WH50C                           (1)


#ifdef VM800B43_50
/* Define all the macros specific to VM800B43_50 module */
#define FT800_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WQVGA                (1)
#define ENABLE_SPI_SINGLE                       (1)
#define RESISTANCE_THRESHOLD                    (1800)
#endif /* VM800B43_50 */

#ifdef VM800B35
#define FT800_ENABLE                            (1)
#define DISPLAY_RESOLUTION_QVGA                 (1)
#define ENABLE_SPI_SINGLE                       (1)
#define RESISTANCE_THRESHOLD                    (1800)
#endif /* VM800B35 */

#ifdef VM801B43_50
#define FT801_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WQVGA                (1)
#define ENABLE_SPI_SINGLE                       (1)

#endif

#ifdef VM810C50
#define FT810_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define RESISTANCE_THRESHOLD                    (1800)

#endif /* VM810C50 */

#ifdef ME812AU_WH50R
/* Define all the macros specific to ME812AU_WH50R module */
#define FT812_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define RESISTANCE_THRESHOLD                    (1200)
#endif /* ME812AU_WH50R */

#ifdef ME813AU_WH50C
/* Define all the macros specific to ME813AU_WH50C module */
#define FT813_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#endif /* ME813AU_WH50C */

#ifdef ME810AU_WH70R
/* Define all the macros specific to ME810AU_WH70R module */
#define FT810_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define RESISTANCE_THRESHOLD                    (1200)
#endif /* ME810AU_WH70R */

#ifdef ME811AU_WH70C
/* Define all the macros specific to ME811AU_WH70C module */
#define FT811_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#endif /* ME811AU_WH70C */

/* Custom configuration */
#if (!defined(VM800B43_50) && !defined(VM800B35) && !defined(VM801B43_50) && !defined(VM810C50) && !defined(ME812AU_WH50R) && !defined(ME813AU_WH50C) && !defined(ME810AU_WH70R) && !defined(ME811AU_WH70C))

/* Display configuration specific macros */
#define DISPLAY_RESOLUTION_QVGA                 (1)
#define DISPLAY_RESOLUTION_WQVGA                (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define DISPLAY_RESOLUTION_HVGA_PORTRAIT        (1)

/* Chip configuration specific macros */
#define FT800_ENABLE                            (1)
#define FT801_ENABLE                            (1)
#define FT810_ENABLE                            (1)
#define FT811_ENABLE                            (1)
#define FT812_ENABLE                            (1)
#define FT813_ENABLE                            (1)

/* SPI specific macros - compile time switches for SPI single, dial and quad use cases */
#define ENABLE_SPI_SINGLE                       (1)
#define ENABLE_SPI_DUAL                         (1)
#define ENABLE_SPI_QUAD                         (1)
#define RESISTANCE_THRESHOLD                    (1200)

#endif
#if defined(FT800_ENABLE) || defined(FT801_ENABLE)
#define FT80X_ENABLE                            (1)
#endif

#if (defined(FT810_ENABLE) || defined(FT811_ENABLE) || defined(FT812_ENABLE) || defined(FT813_ENABLE))
#define FT81X_ENABLE                            (1)
#endif

/* Type of file to load from SDCard or Windows file system */
#define LOADIMAGE 1  //loadimage command takes destination address and options before the actual bitmap data
#define INFLATE 2    //inflate command takes destination address before the actual bitmap
#define LOAD 3       //load bitmaps directly

/* Standard C libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <direct.h>
#include <time.h>
#include <io.h>

/*================ Type definition ================*/
typedef char             bool_t;

typedef char             char8_t;
typedef unsigned char    uchar8_t;
typedef signed char      schar8_t;
typedef float            float_t;

#define prog_uchar8_t  uchar8_t
#define prog_char8_t   char8_t
#define prog_uint16_t  uint16_t
#define PROGMEM


#ifndef _MSC_VER
#ifndef FT8XXEMU_INTTYPES_DEFINED_BASE
#define FT8XXEMU_INTTYPES_DEFINED_BASE
typedef unsigned __int8 uint8_t;
typedef signed __int8 int8_t;
typedef unsigned __int16 uint16_t;
typedef signed __int16 int16_t;
typedef unsigned __int32 uint32_t;
typedef signed __int32 int32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
#endif
#else
#include <stdint.h>
#include <stdlib.h>
#endif
#ifndef FT8XXEMU_INTTYPES_DEFINED_COLOR
#define FT8XXEMU_INTTYPES_DEFINED_COLOR
typedef uint32_t argb8888;
#endif

#ifdef WIN32
#    define FT8XXEMU_FORCE_INLINE __forceinline
#else
#    define FT8XXEMU_FORCE_INLINE inline __attribute__((always_inline))
#endif


#ifndef FT8XXEMU_INTTYPES_DEFINED_FORCEINLINE
#define FT8XXEMU_INTTYPES_DEFINED_FORCEINLINE
#ifdef _MSC_VER
#    define FT8XXEMU_FORCE_INLINE __forceinline
#else
#    define FT8XXEMU_FORCE_INLINE inline __attribute__((always_inline))
#endif
#endif



/* HAL inclusions */
#include "Gpu.h"
#include "Gpu_Hal.h"
#include "CoPro_Cmds.h"
#include "Hal_Utils.h"
#include "EmulatorMain.h"
#define BUFFER_OPTIMIZATION


#define FT800_SEL_PIN   0
#define FT800_PD_N      7


#endif /*_PLATFORM_H_*/
/* Nothing beyond this*/




