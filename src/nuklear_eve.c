/*
Nuklear EVE
Copyright (C) 2019  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
This C source file provides the Nuklear implementation for Bridgetek EVE.
It depends on:
- Nuklear (https://github.com/vurtun/nuklear/)
- EVE Hardware Abstraction Layer
*/

#define NK_IMPLEMENTATION
#define NK_EVE_IMPLEMENTATION
#include "nuklear_eve.h"

#include "FT_Gpu.h"

static struct
{
    struct nk_context ctx;
    Ft_Gpu_Hal_Context_t *phost;
} eve;

struct nk_evefont
{
    struct nk_user_font nk;
    int rom_handle;
};

NK_API nk_evefont *
nk_evefont_create_rom(int handle)
{
    nk_evefont *font = (nk_evefont *)calloc(1, sizeof(nk_evefont));
    if (!font)
        return NULL;
    font->rom_handle = handle;
    return font;
}

NK_API void
nk_evefont_del(nk_evefont *font)
{
    if (!font)
        return;
    free(font);
}

static float
nk_evefont_get_text_width(nk_handle handle, float height, const char *text, int len)
{
    /* ... TODO ... */
    return 120.f;
}

NK_API void
nk_eve_set_font(nk_evefont *evefont)
{
    struct nk_user_font *font = &evefont->nk;
    font->userdata = nk_handle_ptr(evefont);
    font->height = 16; /* ... TODO ... */
    font->width = nk_evefont_get_text_width;
    nk_style_set_font(&eve.ctx, font);
}

NK_API struct nk_context *
nk_eve_init(nk_evefont *evefont, Ft_Gpu_Hal_Context_t *phost)
{
    struct nk_user_font *font = &evefont->nk;
    font->userdata = nk_handle_ptr(evefont);
    font->height = 16; /* ... TODO ... */
    font->width = nk_evefont_get_text_width;

    nk_init_default(&eve.ctx, font);
    // eve.ctx.clip.copy = nk_eve_clipboard_copy;
    // eve.ctx.clip.paste = nk_eve_clipboard_paste;
    // eve.ctx.clip.userdata = nk_handle_ptr(0);
    eve.phost = phost;
    return &eve.ctx;
}

NK_API void
nk_eve_shutdown(void)
{
    nk_free(&eve.ctx);
}

static void
nk_eve_clear(Ft_Gpu_Hal_Context_t *phost, struct nk_color col)
{
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(col.r, col.g, col.b));
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_A(col.a));
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
}

static void
nk_eve_fill_rect(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short r, struct nk_color col)
{
    if (r < 1)
        r = 1;
    ft_int32_t radius = (int)r << 4;
    ft_int32_t width = radius + 8;
    ft_int32_t x0 = x + r;
    ft_int32_t y0 = y + r;
    ft_int32_t x1 = x + w - 1 - r;
    ft_int32_t y1 = y + h - 1 - r;
    Ft_Gpu_CoCmd_SendCmd(phost, COLOR_RGB(col.r, col.g, col.b));
    Ft_Gpu_CoCmd_SendCmd(phost, COLOR_A(col.a));
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(width));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(RECTS));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_draw_text(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w, unsigned short h,
	const char *text, int len, nk_evefont *font, struct nk_color cfg)
{
	if (!len)
		return;

	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_RGB(cfg.r, cfg.g, cfg.b));
	Ft_Gpu_CoCmd_SendCmd(phost, COLOR_A(cfg.a));
	Ft_Gpu_CoCmd_Text_S(phost, x, y, font->rom_handle, 0, text, len);
}

NK_API void
nk_eve_render(struct nk_color clear)
{
    const struct nk_command *cmd;
    Ft_Gpu_Hal_Context_t *phost = eve.phost;

    Ft_Gpu_CoCmd_StartFrame(phost);
    Ft_Gpu_CoCmd_SendCmd(phost, CMD_DLSTART);
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_TAG(255)); // Always default to 255, so no touch = 0, touch non-tag = 255
    nk_eve_clear(phost, clear);

    nk_foreach(cmd, &eve.ctx)
    {
        switch (cmd->type)
        {
        case NK_COMMAND_NOP:
            break;
        case NK_COMMAND_SCISSOR:
        {
            const struct nk_command_scissor *s = (const struct nk_command_scissor *)cmd;
        }
        break;
        case NK_COMMAND_LINE:
        {
            const struct nk_command_line *l = (const struct nk_command_line *)cmd;
        }
        break;
        case NK_COMMAND_RECT:
        {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
        }
        break;
        case NK_COMMAND_RECT_FILLED:
        {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            nk_eve_fill_rect(phost, r->x, r->y, r->w, r->h,
                (unsigned short)r->rounding, r->color);
        }
        break;
        case NK_COMMAND_CIRCLE:
        {
            const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
        }
        break;
        case NK_COMMAND_CIRCLE_FILLED:
        {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
        }
        break;
        case NK_COMMAND_TRIANGLE:
        {
            const struct nk_command_triangle *t = (const struct nk_command_triangle *)cmd;
        }
        break;
        case NK_COMMAND_TRIANGLE_FILLED:
        {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
        }
        break;
        case NK_COMMAND_POLYGON:
        {
            const struct nk_command_polygon *p = (const struct nk_command_polygon *)cmd;
        }
        break;
        case NK_COMMAND_POLYGON_FILLED:
        {
            const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled *)cmd;
        }
        break;
        case NK_COMMAND_POLYLINE:
        {
            const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
        }
        break;
        case NK_COMMAND_TEXT:
        {
            const struct nk_command_text *t = (const struct nk_command_text *)cmd;
			nk_eve_draw_text(phost, t->x, t->y, t->w, t->h,
				(const char*)t->string, t->length,
				(nk_evefont*)t->font->userdata.ptr,
				t->foreground);
        }
        break;
        case NK_COMMAND_CURVE:
        {
            const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
        }
        break;
        case NK_COMMAND_RECT_MULTI_COLOR:
        {
            const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color *)cmd;
        }
        break;
        case NK_COMMAND_IMAGE:
        {
            const struct nk_command_image *i = (const struct nk_command_image *)cmd;
        }
        break;
        case NK_COMMAND_ARC:
        case NK_COMMAND_ARC_FILLED:
        default:
            break;
        }
    }

    Ft_Gpu_CoCmd_SendCmd(phost, DISPLAY());
    Ft_Gpu_CoCmd_Swap(phost);

    Ft_Gpu_CoCmd_EndFrame(phost);

    /* Wait for frame */
    ft_uint16_t rp, wp;
    Ft_Gpu_Hal_RdCmdRpWp(phost, &rp, &wp);
    while (rp != wp)
    {
        /* Reset if coprocessor fault */
        if (FT_COCMD_FAULT(rp))
        {
            eve_printf_debug("TODO: Reset coprocessor on fault\n");
#if defined(_DEBUG) && (EVE_MODEL >= EVE_BT815)
            char err[128];
            Ft_Gpu_Hal_RdMem(phost, RAM_ERR_REPORT, err, 128);
            eve_printf_debug("%s\n", err);
#endif
            break;
        }
        Ft_Gpu_Hal_RdCmdRpWp(phost, &rp, &wp);
    }

    nk_clear(&eve.ctx);
}

/* end of file */