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

#ifndef FT_ESD_COCMD_H
#define FT_ESD_COCMD_H

#include "Ft_Esd.h"
#include "Ft_Esd_Math.h"
#include "FT_CoPro_Cmds.h"

extern Ft_Gpu_Hal_Context_t *Ft_Esd_Host;
Ft_Gpu_Hal_Context_t *Ft_Esd_GetHost();

// TODO: Specific min/max handling for multiple target devices
// #if defined(FT810) || ...
// ESD_PARAMETER(source, Type = ft_uint32_t, Default = 0, Max = 9999) // MEMORY_ADDRESS
// #else
// ESD_PARAMETER(source, Type = ft_uint32_t, Default = 0, Max = 99) // MEMORY_ADDRESS
// #endif

// ESD_FUNCTION(Ft_Gpu_CoCmd_SetBitmap, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ESD_PARAMETER(source, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
// ESD_PARAMETER(fmt, Type = Ft_Gpu_Format, Default = 0) // BITMAP_FORMAT // TODO: Ft_Gpu_Format enum using defines and typedef as ft_uint16_t
// ESD_PARAMETER(w, Type = ft_uint16_t, Default = 0) // BITMAP_SIZE
// ESD_PARAMETER(h, Type = ft_uint16_t, Default = 0) // BITMAP_SIZE
// ft_void_t Ft_Gpu_CoCmd_SetBitmap(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t source, ft_uint16_t fmt, ft_uint16_t w, ft_uint16_t h);

// ESD_FUNCTION(Ft_Gpu_CoCmd_SetScratch, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ESD_PARAMETER(handle, Type = ft_uint32_t, Default = 0, Min = 0, Max = 31) // BITMAP_HANDLE
// ft_void_t Ft_Gpu_CoCmd_SetScratch(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t handle);

// ESD_FUNCTION(Ft_Gpu_CoCmd_RomFont, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ESD_PARAMETER(font, Type = ft_uint32_t, Default = 0, Min = 0, Max = 31) // BITMAP_HANDLE
// ESD_PARAMETER(romslot, Type = ft_uint32_t, Default = 16, Min = 16, Max = 34)
// ft_void_t Ft_Gpu_CoCmd_RomFont(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t font, ft_uint32_t romslot);

ESD_RENDER(Ft_Gpu_CoCmd_Text, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Text")
// ft_void_t Ft_Gpu_CoCmd_Text(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t* s);

ESD_RENDER(Ft_Gpu_CoCmd_Text_Ex, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(bottom, Type = ft_bool_t, Default = 0)
ESD_PARAMETER(baseLine, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(capsHeight, Type = ft_int16_t, Default = 0)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Text")
// ft_void_t Ft_Gpu_CoCmd_Text_Ex(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_bool_t bottom, ft_int16_t baseLine, ft_int16_t capsHeight, const ft_char8_t *s)

ESD_RENDER(Ft_Gpu_CoCmd_Number, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 16, Max = 34) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = ft_uint16_t, Default = 256) // TODO: Option flags / Bit enum?
ESD_PARAMETER(n, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Number(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n);

ESD_FUNCTION(Ft_Gpu_CoCmd_LoadIdentity, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ft_void_t Ft_Gpu_CoCmd_LoadIdentity(Ft_Gpu_Hal_Context_t *phost);

ESD_RENDER(Ft_Gpu_CoCmd_Toggle, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(state, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "on\x00off")
// ft_void_t Ft_Gpu_CoCmd_Toggle(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t font, ft_uint16_t options, ft_uint16_t state, const ft_char8_t* s);

ESD_RENDER(Ft_Gpu_CoCmd_Gauge, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(major, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(minor, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Gauge(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t major, ft_uint16_t minor, ft_uint16_t val, ft_uint16_t range);

// FIXME: Result parameter (result) is an output that needs to be read back
/* ESD_FUNCTION(Ft_Gpu_CoCmd_RegRead, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(result, Type = ft_uint32_t, Default = 0) */
// ft_void_t Ft_Gpu_CoCmd_RegRead(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t result);

// Not exposed to logic editor
/* ESD_FUNCTION(Ft_Gpu_CoCmd_VideoStart, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST */
// ft_void_t Ft_Gpu_CoCmd_VideoStart(Ft_Gpu_Hal_Context_t *phost);

// FIXME: Result parameter (width, h) is an output that needs to be read back
/* ESD_FUNCTION(Ft_Gpu_CoCmd_GetProps, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(width, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(height, Type = ft_uint32_t, Default = 0) */
// ft_void_t Ft_Gpu_CoCmd_GetProps(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t w, ft_uint32_t h);

ESD_FUNCTION(Ft_Gpu_CoCmd_MemCpy, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(dest, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(src, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
// ft_void_t Ft_Gpu_CoCmd_MemCpy(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t dest, ft_uint32_t src, ft_uint32_t num);

// ESD_RENDER(Ft_Gpu_CoCmd_Spinner, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
// ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
// ESD_PARAMETER(style, Type = ft_uint16_t, Default = 0) // TODO: Spinner style enum
// ESD_PARAMETER(scale, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Spinner(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale);

ESD_FUNCTION(Ft_Gpu_CoCmd_BgColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR
// ft_void_t Ft_Gpu_CoCmd_BgColor(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t c);

// ESD_FUNCTION(Ft_Gpu_CoCmd_Swap, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ft_void_t Ft_Gpu_CoCmd_Swap(Ft_Gpu_Hal_Context_t *phost);

// Not exposed directly to logic editor
/* ESD_FUNCTION(Ft_Gpu_CoCmd_Inflate, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS */
// ft_void_t Ft_Gpu_CoCmd_Inflate(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr);

ESD_FUNCTION(Ft_Gpu_CoCmd_Translate, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(tx, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Translate(Ft_Gpu_Hal_Context_t *phost,ft_int32_t tx, ft_int32_t ty);

// Not exposed directly to logic editor
/* ESD_FUNCTION(Ft_Gpu_CoCmd_Stop, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST */
// ft_void_t Ft_Gpu_CoCmd_Stop(Ft_Gpu_Hal_Context_t *phost);

ESD_FUNCTION(Ft_Gpu_CoCmd_SetBase, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(base, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_SetBase(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t base);

ESD_RENDER(Ft_Gpu_CoCmd_Slider, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Slider(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_VideoFrame(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t dst, ft_uint32_t ptr);

ESD_FUNCTION(Ft_Gpu_CoCmd_Interrupt, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ms, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Interrupt(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ms);

ESD_FUNCTION(Ft_Gpu_CoCmd_FgColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR
// ft_void_t Ft_Gpu_CoCmd_FgColor(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t c);

ESD_FUNCTION(Ft_Gpu_CoCmd_Rotate, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(a, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Rotate(Ft_Gpu_Hal_Context_t *phost,ft_int32_t a);

ESD_RENDER(Ft_Gpu_CoCmd_Button, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")
// ft_void_t Ft_Gpu_CoCmd_Button(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t* s);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_MemWrite(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t num);

ESD_RENDER(Ft_Gpu_CoCmd_Scrollbar, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(size, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Scrollbar(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t size, ft_uint16_t range);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_GetMatrix(Ft_Gpu_Hal_Context_t *phost,ft_int32_t a, ft_int32_t b, ft_int32_t c, ft_int32_t d, ft_int32_t e, ft_int32_t f);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_Sketch(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_CSketch(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format,ft_uint16_t freq);

// Not exposed directly to logic editor (fullscreen takeover)
// ft_void_t Ft_Gpu_CoCmd_PlayVideo(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t options);

ESD_FUNCTION(Ft_Gpu_CoCmd_MemSet, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(value, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
// ft_void_t Ft_Gpu_CoCmd_MemSet(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t value, ft_uint32_t num);

// Not exposed directly to logic editor (fullscreen takeover)
// ft_void_t Ft_Gpu_CoCmd_Calibrate(Ft_Gpu_Hal_Context_t *phost);

// ESD_FUNCTION(Ft_Gpu_CoCmd_SetFont, Type = ft_void_t, Category = _GroupHidden)
// ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ESD_PARAMETER(font, Type = ft_uint16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
// ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
// ft_void_t Ft_Gpu_CoCmd_SetFont(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t font, ft_uint32_t ptr);

ESD_FUNCTION(Ft_Gpu_CoCmd_BitmapTransform, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(x2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(y2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty0, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty1, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(tx2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(ty2, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(result, Type = ft_uint16_t, Default = 0) // TODO: Check this
// ft_void_t Ft_Gpu_CoCmd_BitmapTransform(Ft_Gpu_Hal_Context_t *phost,ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result);

ESD_FUNCTION(Ft_Gpu_CoCmd_GradColor, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(c, Type = ft_rgb32_t, Default = 0) // COLOR
// ft_void_t Ft_Gpu_CoCmd_GradColor(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t c);

ESD_FUNCTION(Ft_Gpu_CoCmd_Sync, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ft_void_t Ft_Gpu_CoCmd_Sync(Ft_Gpu_Hal_Context_t *phost);

ESD_FUNCTION(Ft_Gpu_CoCmd_Append, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0, Min = 0, Max = 2048)
// ft_void_t Ft_Gpu_CoCmd_Append(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t num);

ESD_FUNCTION(Ft_Gpu_CoCmd_MemZero, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
ESD_PARAMETER(num, Type = ft_uint32_t, Default = 0) // MEMORY_ADDRESS
// ft_void_t Ft_Gpu_CoCmd_MemZero(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t num);

ESD_FUNCTION(Ft_Gpu_CoCmd_Scale, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(sx, Type = ft_int32_t, Default = 0)
ESD_PARAMETER(sy, Type = ft_int32_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Scale(Ft_Gpu_Hal_Context_t *phost,ft_int32_t sx, ft_int32_t sy);

ESD_RENDER(Ft_Gpu_CoCmd_Clock, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(height, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(m, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(s, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(ms, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Clock(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t h, ft_uint16_t m, ft_uint16_t s, ft_uint16_t ms);

ESD_FUNCTION(Ft_Gpu_CoCmd_Gradient, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x0, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y0, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(rgb0, Type = ft_rgb32_t, Default = 0) // COLOR
ESD_PARAMETER(x1, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y1, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(rgb1, Type = ft_rgb32_t, Default = 0) // COLOR
ft_void_t Ft_Gpu_CoCmd_Gradient(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x0, ft_int16_t y0, ft_uint32_t rgb0, ft_int16_t x1, ft_int16_t y1, ft_uint32_t rgb1);

ESD_FUNCTION(Ft_Gpu_CoCmd_SetMatrix, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
// ft_void_t Ft_Gpu_CoCmd_SetMatrix(Ft_Gpu_Hal_Context_t *phost);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_Track(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t tag);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_Int_SWLoadImage(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t options);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_GetPtr(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t result);

ESD_RENDER(Ft_Gpu_CoCmd_Progress, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
ESD_PARAMETER(range, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Progress(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range);

// Not exposed directly to logic editor
/* ESD_FUNCTION(Ft_Gpu_CoCmd_ColdStart, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST */
ft_void_t Ft_Gpu_CoCmd_ColdStart(Ft_Gpu_Hal_Context_t *phost);

// Not exposed directly to logic editor
// ft_void_t Ft_Gpu_CoCmd_MediaFifo(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t size);

ESD_RENDER(Ft_Gpu_CoCmd_Keys, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(font, Type = ft_int16_t, Default = 21, Min = 0, Max = 31) // BITMAP_HANDLE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(s, Type = const ft_char8_t *, Default = "Button")
// ft_void_t Ft_Gpu_CoCmd_Keys(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t* s);

ESD_RENDER(Ft_Gpu_CoCmd_Dial, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(r, Type = ft_int16_t, Default = 40) // SCREEN_SIZE
ESD_PARAMETER(options, Type = Ft_CoPro_Opt, Default = 0) // TODO: Option flags / Bit enum?
ESD_PARAMETER(val, Type = ft_uint16_t, Default = 0)
// ft_void_t Ft_Gpu_CoCmd_Dial(Ft_Gpu_Hal_Context_t *phost,ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t val);

// Not exposed directly to logic editor
/* ESD_FUNCTION(Ft_Gpu_CoCmd_Snapshot2, Type = ft_void_t, Include = "FT_Esd_CoPro_Cmds.h")
ESD_PARAMETER(phost, Type = Ft_Gpu_Hal_Context_t *, Default = Ft_Esd_GetHost, Hidden, Internal, Static) // PHOST
ESD_PARAMETER(fmt, Type = ft_uint32_t, Default = 0) // BITMAP_FORMAT
ESD_PARAMETER(ptr, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(width, Type = ft_int16_t, Default = 60) // SCREEN_SIZE
ESD_PARAMETER(height, Type = ft_int16_t, Default =40) // SCREEN_SIZE */
// ft_void_t Ft_Gpu_CoCmd_Snapshot2(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t fmt, ft_uint32_t ptr, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h);

/*
ft_void_t Ft_Gpu_CoCmd_LoadImage(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t options);
ft_void_t Ft_Gpu_CoCmd_SetFont2(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t font, ft_uint32_t ptr, ft_uint32_t firstchar);
ft_void_t Ft_Gpu_CoCmd_SetRotate(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t r);
ft_void_t Ft_Gpu_CoCmd_DlStart(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Snapshot(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr);
ft_void_t Ft_Gpu_CoCmd_ScreenSaver(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_MemCrc(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t ptr, ft_uint32_t num, ft_uint32_t result);


ft_void_t Ft_Gpu_CoCmd_Logo(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Calibrate(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t result);

ft_void_t Ft_Gpu_Copro_SendCmd(Ft_Gpu_Hal_Context_t *phost,ft_uint32_t cmd);
ft_void_t Ft_Gpu_CoCmd_SendStr(Ft_Gpu_Hal_Context_t *phost,const ft_char8_t *s);
ft_void_t // Ft_Gpu_CoCmd_StartFunc(Ft_Gpu_Hal_Context_t *phost,ft_uint16_t count);
ft_void_t // Ft_Gpu_CoCmd_EndFunc(Ft_Gpu_Hal_Context_t *phost);

*/

ESD_FUNCTION(Ft_Esd_CoCmd_Spinner, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(x, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(y, Type = ft_int16_t, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(style, Type = ft_uint16_t, Default = 0) // TODO: Spinner style enum
ESD_PARAMETER(scale, Type = ft_uint16_t, Default = 0)
ft_void_t Ft_Esd_CoCmd_Spinner(void *owner, ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale);

ESD_FUNCTION(Ft_Esd_CoCmd_ScreenSaver, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ft_void_t Ft_Esd_CoCmd_ScreenSaver(void *owner);

ESD_FUNCTION(Ft_Esd_CoCmd_Sketch, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ESD_PARAMETER(globalRect, Type = Ft_Esd_Rect16, Default = 0) // SCREEN_SIZE
ESD_PARAMETER(addr, Type = ft_uint32_t, Default = 0)
ESD_PARAMETER(format, Type = ft_uint16_t, Default = L8)
ft_void_t Ft_Esd_CoCmd_Sketch(void *owner, Ft_Esd_Rect16 globalRect, ft_uint32_t addr, ft_uint16_t format);

ESD_FUNCTION(Ft_Esd_CoCmd_Stop, Type = ft_void_t, Category = _GroupHidden)
ESD_PARAMETER(owner, Type = void *, Default = 0, Hidden, Internal)
ft_void_t Ft_Esd_CoCmd_Stop(void *owner);

#endif /* #ifndef FT_ESD_COCMD_H */

/* end of file */
