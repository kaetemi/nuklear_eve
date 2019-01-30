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

*/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/* platform specific macros */
#define MSVC_PLATFORM                             (1)    // enable by default for MSVC platform


//#define VM800B43_50                           (1)
//#define VM800B35                              (1)
//#define VM801B43_50                           (1)
//#define VM810C50                              (1)
//#define ME812AU_WH50R                         (1)
#define ME813AU_WH50C                         (1)

#ifdef VM800B43_50
/* Define all the macros specific to VM800B43_50 module */
#define FT800_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define DISPLAY_RESOLUTION_WQVGA                (1)
#define RESISTANCE_THRESHOLD                    (1800)
#define MSVC_PLATFORM_SPI_LIBMPSSE              (1)    
#endif /* VM800B43_50 */

#ifdef VM800B35
#define FT800_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define DISPLAY_RESOLUTION_QVGA                 (1)
#define RESISTANCE_THRESHOLD                    (1800)
#define MSVC_PLATFORM_SPI_LIBMPSSE              (1)    
#endif /* VM800B35 */

#ifdef VM801B43_50
#define FT801_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define DISPLAY_RESOLUTION_WQVGA                (1)
#define RESISTANCE_THRESHOLD                    (1800)
#define MSVC_PLATFORM_SPI_LIBMPSSE              (1)    
#endif

#ifdef VM810C50
/* Define all the macros specific to VM810C50 module */
#define FT810_ENABLE                            (1)
#define ENABLE_SPI_SINGLE                       (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define RESISTANCE_THRESHOLD                    (1800)
#define MSVC_PLATFORM_SPI_LIBMPSSE              (1)    
#endif /* VM810C50 */

#ifdef ME812AU_WH50R
/* Define all the macros specific to ME812AU_WH50R module */
#define FT812_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define RESISTANCE_THRESHOLD                    (1200)
#define    MSVC_PLATFORM_SPI_LIBFT4222          (1)
#endif /* ME812AU_WH50R */

#ifdef ME813AU_WH50C
/* Define all the macros specific to ME813AU_WH50C module */
#define FT813_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define    MSVC_PLATFORM_SPI_LIBFT4222          (1)
#endif /* ME813AU_WH50C */

#ifdef ME810AU_WH70R
/* Define all the macros specific to ME810AU_WH70R module */
#define FT810_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define RESISTANCE_THRESHOLD                    (1200)
#define    MSVC_PLATFORM_SPI_LIBFT4222          (1)
#endif /* ME810AU_WH70R */

#ifdef ME811AU_WH70C
/* Define all the macros specific to ME811AU_WH70C module */
#define FT811_ENABLE                            (1)
#define DISPLAY_RESOLUTION_WVGA                 (1)
#define ENABLE_SPI_QUAD                         (1)
#define    MSVC_PLATFORM_SPI_LIBFT4222          (1)
#endif /* ME811AU_WH70C */

/* Custom configuration */
#if (!defined(VM800B43_50) && !defined(VM800B35) && !defined(VM801B43_50) && !defined(VM810C50)  && !defined(ME812AU_WH50R) && !defined(ME813AU_WH50C) && !defined(ME810AU_WH70R) && !defined(ME811AU_WH70C))

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
#define ENABLE_SPI_SINGLE                        (1)
#define ENABLE_SPI_DUAL                          (1)
#define ENABLE_SPI_QUAD                          (1)

/* Display driver configurations - mainly for ME900EV1 modules */
#define ENABLE_ILI9488_HVGA_PORTRAIT            (1)

/* Enable MPSSE or Lib4222 based on module */
#define MSVC_PLATFORM_SPI_LIBMPSSE                (1)    
#define    MSVC_PLATFORM_SPI_LIBFT4222            (1)

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

/* C library inclusions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <direct.h>
#include <time.h>
#include <io.h>
#include <stdint.h>

/*================ Type definition ================*/
typedef char             bool_t;
typedef char             char8_t;
typedef unsigned char    uchar8_t;
typedef signed char        schar8_t;
typedef float            float_t;

#define BYTE_SIZE  (1)
#define SHORT_SIZE (2)
#define WORD_SIZE  (4)
#define DWORD_SIZE (8)

#define NUMBITS_IN_BYTE  (1*8)
#define NUMBITS_IN_SHORT (2*8)
#define NUMBITS_IN_WORD  (4*8)
#define NUMBITS_IN_DWORD (8*8)

#define prog_uchar8_t  uchar8_t
#define prog_char8_t   char8_t
#define prog_uint16_t  uint16_t

#define strcpy_P     strcpy
#define strlen_P     strlen

#define PROGMEM



/* D2xx and SPI from BRIDGETEK inclusions */
#include "ftd2xx.h"

#ifdef MSVC_PLATFORM_SPI_LIBMPSSE
#include "LibMPSSE_spi.h"
#endif

#ifdef MSVC_PLATFORM_SPI_LIBFT4222
#include "LibFT4222.h"
#endif

/* HAL inclusions */
#include "Gpu_Hal.h"
#include "Gpu.h"
#include "CoPro_Cmds.h"
#include "Hal_Utils.h"

/* Macros specific to optimization */
#define BUFFER_OPTIMIZATION          (1)
#define BUFFER_OPTIMIZATION_DLRAM    (1)
#define BUFFER_OPTIMIZATION_CMDRAM   (1)
#define MSVC_PLATFORM_SPI            (1)

#ifdef MSVC_PLATFORM
#ifdef MSVC_PLATFORM_SPI_LIBMPSSE
#define FT800_SEL_PIN   0
#define FT800_PD_N      7
#endif
#ifdef MSVC_PLATFORM_SPI_LIBFT4222
#define FT800_SEL_PIN   1    /* GPIO is not utilized in Lib4222 as it is directly managed by firmware */
#define FT800_PD_N      GPIO_PORT0
#endif
#endif

#endif /*_PLATFORM_H_*/
/* Nothing beyond this*/

