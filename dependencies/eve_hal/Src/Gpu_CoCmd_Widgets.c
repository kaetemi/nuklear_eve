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

#ifdef ESD_FRAMEWORK
extern ft_uint8_t Ft_Esd_Primitive;
#endif

ft_void_t Ft_Gpu_CoCmd_Gradient(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x0, ft_int16_t y0, ft_uint32_t rgb0, ft_int16_t x1, ft_int16_t y1, ft_uint32_t rgb1)
{
	uint32_t cmd[5] = {
		CMD_GRADIENT,
		(((ft_uint32_t)y0 << 16) | (x0 & 0xffff)),
		rgb0,
		(((ft_uint32_t)y1 << 16) | (x1 & 0xffff)),
		rgb1,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Spinner(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale)
{
	uint32_t cmd[3] = {
		CMD_SPINNER,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)scale << 16) | (style & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	uint32_t cmd[3] = {
		CMD_TEXT,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (font & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
	Ft_Gpu_CoCmd_SendStr(phost, s);
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text_S(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s, int length)
{
	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	uint32_t cmd[3] = {
		CMD_TEXT,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (font & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
	Ft_Gpu_CoCmd_SendStr_S(phost, s, length);
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text_Ex(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_bool_t bottom, ft_int16_t baseLine, ft_int16_t capsHeight, const ft_char8_t *s)
{
	ft_int16_t yOffset;
	if (options & OPT_CENTERY)
		yOffset = baseLine - (capsHeight >> 1);
	else if (bottom)
		yOffset = baseLine;
	else
		yOffset = baseLine - capsHeight;
	Ft_Gpu_CoCmd_Text(phost, x, y - yOffset, font, options & ~OPT_CENTERY, s);
}

ft_void_t Ft_Gpu_CoCmd_Number(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n)
{
	if (font >= 32)
	{
		eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	uint32_t cmd[4] = {
		CMD_NUMBER,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (font & 0xffff)),
		n,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Toggle(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t font, ft_uint16_t options, ft_uint16_t state, const ft_char8_t *s)
{
	uint32_t cmd[4] = {
		CMD_TOGGLE,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)font << 16) | (w & 0xffff)),
		(((ft_uint32_t)state << 16) | options),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
	Ft_Gpu_CoCmd_SendStr(phost, s);
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Slider(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range)
{
	uint32_t cmd[5] = {
		CMD_SLIDER,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		(((ft_uint32_t)val << 16) | (options & 0xffff)),
		range,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Button(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	uint32_t cmd[4] = {
		CMD_BUTTON,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		(((ft_uint32_t)options << 16) | (font & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
	Ft_Gpu_CoCmd_SendStr(phost, s);
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Keys(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	uint32_t cmd[4] = {
		CMD_KEYS,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		(((ft_uint32_t)options << 16) | (font & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
	Ft_Gpu_CoCmd_SendStr(phost, s);
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Dial(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t val)
{
	uint32_t cmd[4] = {
		CMD_DIAL,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (r & 0xffff)),
		val,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

/* Error handling for val is not done, so better to always use range of 65535 in order that needle is drawn within display region */
ft_void_t Ft_Gpu_CoCmd_Gauge(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t major, ft_uint16_t minor, ft_uint16_t val, ft_uint16_t range)
{
	uint32_t cmd[5] = {
		CMD_GAUGE,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (r & 0xffff)),
		(((ft_uint32_t)minor << 16) | (major & 0xffff)),
		(((ft_uint32_t)range << 16) | (val & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Clock(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t h, ft_uint16_t m, ft_uint16_t s, ft_uint16_t ms)
{
	uint32_t cmd[5] = {
		CMD_CLOCK,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)options << 16) | (r & 0xffff)),
		(((ft_uint32_t)m << 16) | (h & 0xffff)),
		(((ft_uint32_t)ms << 16) | (s & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Scrollbar(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t size, ft_uint16_t range)
{
	uint32_t cmd[5] = {
		CMD_SCROLLBAR,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		(((ft_uint32_t)val << 16) | (options & 0xffff)),
		(((ft_uint32_t)range << 16) | (size & 0xffff)),
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Progress(Ft_Gpu_Hal_Context_t *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range)
{
	uint32_t cmd[5] = {
		CMD_PROGRESS,
		(((ft_uint32_t)y << 16) | (x & 0xffff)),
		(((ft_uint32_t)h << 16) | (w & 0xffff)),
		(((ft_uint32_t)val << 16) | (options & 0xffff)),
		range,
	};
	Ft_Gpu_CoCmd_SendCmdArr(phost, cmd, sizeof(cmd) / sizeof(cmd[0]));
#ifdef ESD_FRAMEWORK
	Ft_Esd_Primitive = 0;
#endif
}

/* end of file */
