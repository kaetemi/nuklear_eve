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

Author : BRIDGETEK 

Revision History: 
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - made few changes.
0.3 - date 2015.03.16 - Added ALIGN_TWO_POWER_N() macro.
0.4 - date 2017.03.24 - Moved utils from Platform.h
*/

#ifndef _HAL_UTILS_H_
#define _HAL_UTILS_H_

#define POLAR_UTIL /* Enable sin()/cos() calculator utilities */

#ifdef RGB //Undefine RGB from wingdi.h in Visual Studio
#undef RGB
#endif
#define RGB(r, g, b)  ((((vc_int32_t)(r)) << 16) | (((vc_int32_t)(g)) << 8) | (b))
#define SQ(v) ((v) * (v))
#define MIN(x,y)  ((x) > (y) ? (y) : (x))
#define MAX(x,y)  ((x) > (y) ? (x) : (y))
#define PLAYCOLOR        0x00A0A080
#define NOTE(n, sharp)   (((n) - 'C') + ((sharp) * 128))
#define F16(s)           ((int32_t)((s) * 65536))
#define INVALID_TOUCH_XY   0x8000
#define ABS(x)  ((x) > (0) ? (x) : (-x))
#define ALIGN_TWO_POWER_N(Value,alignval) (((Value) + (alignval - 1))&(~(alignval - 1)))

#if !defined(ARDUINO_PLATFORM) //Arduino support these keyword
#define pgm_read_byte_near(x)   (*(x))
#define pgm_read_byte(x)        (*(x))
#define pgm_read_word(addr)   (*((prog_uint16_t *)(addr)))
#define delay(x) Gpu_Hal_Sleep(x)
#define random(x)    (rand() % (x))
#define DBGPRINT(x)  printf(x)
#endif

#if defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
/* strcpy_P and strlen_P only support Arduino */
#define strcpy_P     strcpy
#define strlen_P     strlen
#endif

void fadeout(Gpu_Hal_Context_t *phost);
void fadein(Gpu_Hal_Context_t *phost);
void GPU_DLSwap(Gpu_Hal_Context_t *phost, uint8_t DL_Swap_Type);
float_t cal_average(float_t * ptr_elements , uint16_t elements);
int16_t qsin(uint16_t a);
int16_t qcos(uint16_t a);
float_t da(float_t i, int16_t degree);

void polarxy(int32_t r, float_t th, int32_t *x, int32_t *y, int32_t ox, int32_t oy);
void polar(Gpu_Hal_Context_t *phost, int32_t r, float_t th, int32_t ox, int32_t oy);

#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
    #define GET_CURR_MILLIS() get_millis()
#elif defined(MSVC_PLATFORM) || defined(MSVC_FT800EMU)
    #define GET_CURR_MILLIS() time(NULL)
#elif defined(ARDUINO_PLATFORM)
    #define GET_CURR_MILLIS() millis()
#else
#define GET_CURR_MILLIS() 0
#endif
#endif /* _HAL_UTILS_H_ */



