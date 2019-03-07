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

#include "Ft_Esd_CoCmd.h"
#include "Gpu_Hal.h"

#include "Ft_Esd_Dl.h"

#if ESD_DL_OPTIMIZE
extern ft_uint8_t Ft_Esd_Primitive;
#endif

ft_void_t Ft_Gpu_CoCmd_Gradient(EVE_HalContext *phost, ft_int16_t x0, ft_int16_t y0, ft_uint32_t rgb0, ft_int16_t x1, ft_int16_t y1, ft_uint32_t rgb1)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_GRADIENT);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y0 << 16) | (x0 & 0xffff)));
	Gpu_Copro_SendCmd(phost, rgb0);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y1 << 16) | (x1 & 0xffff)));
	Gpu_Copro_SendCmd(phost, rgb1);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Spinner(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 3);
	Gpu_Copro_SendCmd(phost, CMD_SPINNER);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)scale << 16) | (style & 0xffff)));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 3));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

#if (EVE_MODEL >= EVE_BT815)
/* Count number of arguments in Cmd_Text for string format*/
static uint8_t countArgs(const char *str)
{
	uint8_t count = 0;
	const char *tmp = str;

	while (tmp = strstr(tmp, "%"))
	{
		if (*(tmp + 1) == '%')
		{
			tmp += 2;
		}
		else
		{
			count++;
			tmp++;
		}
	}
	return count;
}
#endif

void Gpu_CoCmd_Text(Gpu_Hal_Context_t *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_MODEL >= EVE_BT815)
	uint8_t i, num;
#endif
	va_start(args, s);
#if (EVE_MODEL >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	//l = GetFormatStringList(s, args);
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); //Only check % characters if option OPT_FORMAT is set
	    //printf("num = %d %d\n",num, (len + 1 + 3) & ~3);
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_MODEL >= EVE_BT815)
	for (i = 0; i < num; i++)
		EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	uint16_t transfered = EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
	// eve_printf_debug(" -- strlen: %i, transfered: %i\n", (int)strlen(s), (int)transfered);

	// eve_cmd_assert_flush(phost);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text_S(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s, int length)
{
	if (font >= 32)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	EVE_Cmd_wr16(phost, x);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	EVE_Cmd_wr16(phost, y);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	EVE_Cmd_wr16(phost, font);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	EVE_Cmd_wr16(phost, options);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	uint16_t transfered = EVE_Cmd_wrString(phost, s, length);
	EVE_Cmd_endFunc(phost);
	// eve_printf_debug("wp: %i\n", EVE_Cmd_wp(phost));
	// eve_printf_debug(" -- strlen: %i, length: %i, transfered: %i\n", (int)strlen(s), (int)length, (int)transfered);

	// eve_cmd_assert_flush(phost);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Text_Ex(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_bool_t bottom, ft_int16_t baseLine, ft_int16_t capsHeight, const ft_char8_t *s)
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

ft_void_t Ft_Gpu_CoCmd_Number(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t font, ft_uint16_t options, ft_int32_t n)
{
	if (font >= 32)
	{
		eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}

	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_NUMBER);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	Gpu_Copro_SendCmd(phost, n);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

void Gpu_CoCmd_Toggle(Gpu_Hal_Context_t *phost, int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *s, ...)
{
	va_list args;
#if (EVE_MODEL >= EVE_BT815)
	uint8_t i, num;
#endif
	va_start(args, s);
#if (EVE_MODEL >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TOGGLE);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr16(phost, state);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
#if (EVE_MODEL >= EVE_BT815)
	for (i = 0; i < num; i++)
		EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Toggle(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t font, ft_uint16_t options, ft_uint16_t state, const ft_char8_t *s)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TOGGLE);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr16(phost, state);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Slider(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_SLIDER);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)val << 16) | (options & 0xffff)));
	Gpu_Copro_SendCmd(phost, range);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

void Gpu_CoCmd_Button(Gpu_Hal_Context_t *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
#if (EVE_MODEL >= EVE_BT815)
	uint8_t i, num;
#endif

	va_start(args, s);
#if (EVE_MODEL >= EVE_BT815) /* OPT_FORMAT not defined in FT8xx chip */
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0); //Only check % characters if option OPT_FORMAT is set
#endif

	EVE_Cmd_startFunc(phost);
	Gpu_Copro_SendCmd(phost, CMD_BUTTON);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	Gpu_CoCmd_SendStr(phost, s);
#if (EVE_MODEL >= EVE_BT815)
	for (i = 0; i < num; i++)
		Gpu_Copro_SendCmd(phost, (uint32_t)va_arg(args, uint32_t));
#endif
	EVE_Cmd_endFunc(phost);
	va_end(args);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Button(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	EVE_Cmd_startFunc(phost);
	Gpu_Copro_SendCmd(phost, CMD_BUTTON);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	Gpu_CoCmd_SendStr(phost, s);
	EVE_Cmd_endFunc(phost);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Keys(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_int16_t font, ft_uint16_t options, const ft_char8_t *s)
{
	EVE_Cmd_startFunc(phost);
	Gpu_Copro_SendCmd(phost, CMD_KEYS);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	Gpu_CoCmd_SendStr(phost, s);
	EVE_Cmd_endFunc(phost);

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Dial(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t val)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 4);
	Gpu_Copro_SendCmd(phost, CMD_DIAL);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (r & 0xffff)));
	Gpu_Copro_SendCmd(phost, val);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 4));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

/* Error handling for val is not done, so better to always use range of 65535 in order that needle is drawn within display region */
ft_void_t Ft_Gpu_CoCmd_Gauge(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t major, ft_uint16_t minor, ft_uint16_t val, ft_uint16_t range)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_GAUGE);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (r & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)minor << 16) | (major & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)range << 16) | (val & 0xffff)));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Clock(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t r, ft_uint16_t options, ft_uint16_t h, ft_uint16_t m, ft_uint16_t s, ft_uint16_t ms)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_CLOCK);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)options << 16) | (r & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)m << 16) | (h & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)ms << 16) | (s & 0xffff)));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Scrollbar(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t size, ft_uint16_t range)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_SCROLLBAR);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)val << 16) | (options & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)range << 16) | (size & 0xffff)));
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

ft_void_t Ft_Gpu_CoCmd_Progress(EVE_HalContext *phost, ft_int16_t x, ft_int16_t y, ft_int16_t w, ft_int16_t h, ft_uint16_t options, ft_uint16_t val, ft_uint16_t range)
{
	Gpu_CoCmd_StartFunc(phost, CMD_SIZE * 5);
	Gpu_Copro_SendCmd(phost, CMD_PROGRESS);
	Gpu_Copro_SendCmd(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	Gpu_Copro_SendCmd(phost, (((uint32_t)val << 16) | (options & 0xffff)));
	Gpu_Copro_SendCmd(phost, range);
	Gpu_CoCmd_EndFunc(phost, (CMD_SIZE * 5));

#if ESD_DL_OPTIMIZE
	Ft_Esd_Primitive = 0;
#endif
}

/* end of file */
