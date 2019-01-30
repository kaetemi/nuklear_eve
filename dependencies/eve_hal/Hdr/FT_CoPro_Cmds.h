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

#ifndef FT_COPRO_CMDS_H
#define FT_COPRO_CMDS_H

ft_void_t Ft_Gpu_CoCmd_SetBitmap(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t source, ft_uint16_t fmt, ft_uint16_t w, ft_uint16_t h);
ft_void_t Ft_Gpu_CoCmd_SetScratch(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t handle);
ft_void_t Ft_Gpu_CoCmd_RomFont(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t font, ft_uint32_t romslot);
ft_void_t Ft_Gpu_CoCmd_Text(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_Text_Ex(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_bool_t bottom, ft_int16_t baseLine, ft_int16_t capsHeight, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_Number(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n);
ft_void_t Ft_Gpu_CoCmd_LoadIdentity(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Toggle(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t font, ft_uint16_t options, ft_uint16_t state, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_Gauge(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t major, ft_uint16_t minor, ft_uint16_t val, ft_uint16_t range);

ft_void_t Ft_Gpu_CoCmd_RegRead(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t result);
ft_void_t Ft_Gpu_CoCmd_VideoStart(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_GetProps(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t w, ft_uint32_t h);
ft_void_t Ft_Gpu_CoCmd_MemCpy(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dest, ft_uint32_t src, ft_uint32_t num);
ft_void_t Ft_Gpu_CoCmd_Spinner(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale);
ft_void_t Ft_Gpu_CoCmd_BgColor(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t c);
ft_void_t Ft_Gpu_CoCmd_Swap(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Inflate(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr);
ft_void_t Ft_Gpu_CoCmd_Translate(Ft_Gpu_Hal_Context_t *phost, ft_int32_t tx, ft_int32_t ty);
ft_void_t Ft_Gpu_CoCmd_Stop(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_SetBase(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t base);
ft_void_t Ft_Gpu_CoCmd_Slider(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range);
ft_void_t Ft_Gpu_CoCmd_VideoFrame(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_uint32_t ptr);
ft_void_t Ft_Gpu_CoCmd_Interrupt(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ms);
ft_void_t Ft_Gpu_CoCmd_FgColor(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t c);
ft_void_t Ft_Gpu_CoCmd_Rotate(Ft_Gpu_Hal_Context_t *phost, ft_int32_t a);
ft_void_t Ft_Gpu_CoCmd_Button(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_MemWrite(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t num);
ft_void_t Ft_Gpu_CoCmd_Scrollbar(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t size, ft_uint16_t range);
ft_void_t Ft_Gpu_CoCmd_GetMatrix(Ft_Gpu_Hal_Context_t *phost, ft_int32_t a, ft_int32_t b, ft_int32_t c, ft_int32_t d, ft_int32_t e, ft_int32_t f);
ft_void_t Ft_Gpu_CoCmd_Sketch(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format);
ft_void_t Ft_Gpu_CoCmd_CSketch(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t w, ft_uint16_t h, ft_uint32_t ptr, ft_uint16_t format, ft_uint16_t freq);
ft_void_t Ft_Gpu_CoCmd_PlayVideo(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t options);
ft_void_t Ft_Gpu_CoCmd_MemSet(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t value, ft_uint32_t num);
ft_void_t Ft_Gpu_CoCmd_SetFont(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t font, ft_uint32_t ptr);
ft_void_t Ft_Gpu_CoCmd_BitmapTransform(Ft_Gpu_Hal_Context_t *phost, ft_int32_t x0, ft_int32_t y0, ft_int32_t x1, ft_int32_t y1, ft_int32_t x2, ft_int32_t y2, ft_int32_t tx0, ft_int32_t ty0, ft_int32_t tx1, ft_int32_t ty1, ft_int32_t tx2, ft_int32_t ty2, ft_uint16_t result);
ft_void_t Ft_Gpu_CoCmd_GradColor(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t c);
ft_void_t Ft_Gpu_CoCmd_Sync(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Append(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t num);
ft_void_t Ft_Gpu_CoCmd_MemZero(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t num);
ft_void_t Ft_Gpu_CoCmd_Scale(Ft_Gpu_Hal_Context_t *phost, ft_int32_t sx, ft_int32_t sy);
ft_void_t Ft_Gpu_CoCmd_Clock(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t h, ft_uint16_t m, ft_uint16_t s, ft_uint16_t ms);
ft_void_t Ft_Gpu_CoCmd_Gradient(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x0, ft_int16_t y0, ft_uint32_t rgb0, ft_int16_t x1, ft_int16_t y1, ft_uint32_t rgb1);
ft_void_t Ft_Gpu_CoCmd_SetMatrix(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Track(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t tag);

ft_void_t Ft_Gpu_CoCmd_Int_SWLoadImage(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t options);
ft_void_t Ft_Gpu_CoCmd_GetPtr(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t result);
ft_void_t Ft_Gpu_CoCmd_Progress(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range);
ft_void_t Ft_Gpu_CoCmd_ColdStart(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_MediaFifo(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t size);
ft_void_t Ft_Gpu_CoCmd_Keys(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_Dial(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t val);
ft_void_t Ft_Gpu_CoCmd_Snapshot2(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t fmt, ft_uint32_t ptr, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h);
ft_void_t Ft_Gpu_CoCmd_LoadImage(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t options);
ft_void_t Ft_Gpu_CoCmd_SetFont2(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t font, ft_uint32_t ptr, ft_uint32_t firstchar);
ft_void_t Ft_Gpu_CoCmd_SetRotate(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t r);
ft_void_t Ft_Gpu_CoCmd_DlStart(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_Snapshot(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr);
ft_void_t Ft_Gpu_CoCmd_ScreenSaver(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_MemCrc(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t ptr, ft_uint32_t num, ft_uint32_t result);

ft_void_t Ft_Gpu_CoCmd_Logo(Ft_Gpu_Hal_Context_t *phost);
ft_uint32_t Ft_Gpu_CoCmd_Calibrate(Ft_Gpu_Hal_Context_t *phost);

ft_bool_t Ft_Gpu_CoCmd_AnimStart(Ft_Gpu_Hal_Context_t *phost, int32_t ch, uint32_t aoptr, uint32_t loop);
void Ft_Gpu_CoCmd_AnimStop(Ft_Gpu_Hal_Context_t *phost, int32_t ch);
void Ft_Gpu_CoCmd_AnimXY(Ft_Gpu_Hal_Context_t *phost, int32_t ch, int16_t x, int16_t y);
void Ft_Gpu_CoCmd_AnimDraw(Ft_Gpu_Hal_Context_t *phost, int32_t ch);
void Ft_Gpu_CoCmd_AnimFrame(Ft_Gpu_Hal_Context_t *phost, int16_t x, int16_t y, uint32_t aoptr, uint32_t frame);

ft_void_t Ft_Gpu_CoCmd_SendCmd(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t cmd);
ft_void_t Ft_Gpu_CoCmd_SendCmdArr(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t *cmd, ft_size_t nb);
ft_void_t Ft_Gpu_CoCmd_SendStr(Ft_Gpu_Hal_Context_t *phost, const ft_char8_t *s);
ft_void_t Ft_Gpu_CoCmd_StartFrame(Ft_Gpu_Hal_Context_t *phost);
ft_void_t Ft_Gpu_CoCmd_EndFrame(Ft_Gpu_Hal_Context_t *phost);

#ifdef EVE_FLASH_AVAILABLE

#define FLASH_ERROR_NOTATTACHED 0xE001
#define FLASH_ERROR_HEADERMISSING 0xE002
#define FLASH_ERROR_HEADERCRCFAILED 0xE003
#define FLASH_ERROR_FIRMWAREMISMATCH 0xE004
#define FLASH_ERROR_FULLSPEEDFAILED 0xE005

/// Attach flash. Return new FLASH_STATUS
ft_uint32_t Ft_Gpu_CoCmd_FlashAttach(Ft_Gpu_Hal_Context_t *phost);

/*
Enter fast flash state. Return new FLASH_STATUS. Result will be 0 on success
0xE001 flash is not attached
0xE002 no header detected in sector 0 - is flash blank?
0xE003 sector 0 data failed integrity check
0xE004 device/blob mismatch - was correct blob loaded?
0xE005 failed full-speed test - check board wiring
*/
ft_uint32_t Ft_Gpu_CoCmd_FlashFast(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t *result);

/// Read from Flash to RAM_G. Returns FT_FALSE on co processor fault
ft_bool_t Ft_Gpu_CoCmd_FlashRead(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t size);

/// Load image from Flash to RAM_G. Output parameter format returns loaded bitmap format on success. Returns FT_FALSE on co processor fault
ft_bool_t Ft_Gpu_CoCmd_LoadImage_Flash(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_uint32_t src, ft_uint32_t *format);

/// Deflates data from Flash to RAM_G. Returns FT_FALSE on co processor fault
ft_bool_t Ft_Gpu_CoCmd_Inflate_Flash(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_uint32_t src);

#endif

ft_bool_t Ft_Gpu_CoCmd_LoadImage_ProgMem(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size, ft_uint32_t *format);
ft_bool_t Ft_Gpu_CoCmd_Inflate_ProgMem(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t dst, ft_prog_uchar8_t *src, ft_uint32_t size);

// Deprecated
#define Ft_Gpu_Copro_SendCmd Ft_Gpu_CoCmd_SendCmd
#define Ft_Gpu_CoCmd_Dlstart Ft_Gpu_CoCmd_DlStart

#endif /* FT_COPRO_CMDS_H */

/* end of file */
