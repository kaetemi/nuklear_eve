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

/*
TODO:
- Touch input
- Stroke shapes
- Curves
- Unicode font
- Bitmap
*/

#define NK_IMPLEMENTATION
#define NK_EVE_IMPLEMENTATION
#include "nuklear_eve.h"

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
    return 20.f;
}

NK_API void
nk_eve_set_font(nk_evefont *evefont)
{
    struct nk_user_font *font = &evefont->nk;
    font->userdata = nk_handle_ptr(evefont);
    font->height = 20; /* ... TODO ... */
    font->width = nk_evefont_get_text_width;
    nk_style_set_font(&eve.ctx, font);
}

NK_API struct nk_context *
nk_eve_init(nk_evefont *evefont, Ft_Gpu_Hal_Context_t *phost)
{
    struct nk_user_font *font = &evefont->nk;
    font->userdata = nk_handle_ptr(evefont);
    font->height = 20; /* ... TODO ... */
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
nk_eve_color_rgba(Ft_Gpu_Hal_Context_t *phost, struct nk_color col)
{
    Ft_Gpu_CoCmd_SendCmd(phost, COLOR_RGB(col.r, col.g, col.b));
    Ft_Gpu_CoCmd_SendCmd(phost, COLOR_A(col.a));
}

static void
nk_eve_scissor(Ft_Gpu_Hal_Context_t *phost, float x, float y, float w, float h)
{
    Ft_Gpu_CoCmd_SendCmd(phost, SCISSOR_XY((int)x, (int)y));
    Ft_Gpu_CoCmd_SendCmd(phost, SCISSOR_SIZE((int)(x + w + 1), (int)(y + h + 1)));
}

static void
nk_eve_placeholder(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(8));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINES));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_stroke_line(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, unsigned int line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(line_thickness << 4));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINES));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_stroke_rect(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short r, unsigned short line_thickness, struct nk_color col)
{
    // TODO: Support rounding
    // - Same trick as donut rendering for thick line
    // - Line stroke with manually drawn round corners for thin line

    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(line_thickness << 4));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y + h));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x + w, y));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x, y));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
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
    nk_eve_color_rgba(phost, col);
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
nk_eve_rect_multi_color(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color left, struct nk_color top,
    struct nk_color right, struct nk_color bottom)
{
    /* ... TODO ... */
    nk_eve_placeholder(phost, x, y, w, h, top);
}

static void
nk_eve_fill_triangle(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, short x2, short y2, struct nk_color col)
{
    /* ... TODO ... */
    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(8));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x2, y2));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_stroke_triangle(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, short x2, short y2, unsigned short line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(line_thickness << 4));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x1, y1));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x2, y2));
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(x0, y0));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_fill_polygon(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts, int count, struct nk_color col)
{
    /* ... TODO ... */
}

static void
nk_eve_stroke_polygon(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts, int count,
    unsigned short line_thickness, struct nk_color col)
{
    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(line_thickness << 4));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    for (int i = 0; i < count; ++i)
    {
        Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(pnts[i].x, pnts[i].y));
    }
    // TODO: Validate if it's necessary to close the polygon
    // Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(pnts[0].x, pnts[0].x));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_stroke_polyline(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts,
    int count, unsigned short line_thickness, struct nk_color col)
{
    // TODO: This is just the same as nk_eve_stroke_polygon?

    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, LINE_WIDTH(line_thickness << 4));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(LINE_STRIP));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(0));
#endif
    for (int i = 0; i < count; ++i)
    {
        Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(pnts[i].x, pnts[i].y));
    }
    // TODO: Validate if it's necessary to close the polygon
    // Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(pnts[0].x, pnts[0].x));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_fill_circle(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color col)
{
    // Only support circles, not ovals
    int r = (int)(w > h ? h : w) << 3;
    int xc = ((int)x << 1) + (int)w;
    int yc = ((int)y << 1) + (int)h;
    nk_eve_color_rgba(phost, col);
    Ft_Gpu_CoCmd_SendCmd(phost, POINT_SIZE(r));
    Ft_Gpu_CoCmd_SendCmd(phost, BEGIN(POINTS));
#if (EVE_MODEL >= EVE_FT810)
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX_FORMAT(1));
#endif
    Ft_Gpu_CoCmd_SendCmd(phost, VERTEX2F(xc, yc));
    Ft_Gpu_CoCmd_SendCmd(phost, END());
}

static void
nk_eve_stroke_circle(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short line_thickness, struct nk_color col)
{
    /* ... TODO ... */
    // - Donut rendering for thick line
    // - Line stroke for thin line?
    nk_eve_placeholder(phost, x, y, w, h, col);
}

static void
nk_eve_stroke_curve(Ft_Gpu_Hal_Context_t *phost, struct nk_vec2i p1,
    struct nk_vec2i p2, struct nk_vec2i p3, struct nk_vec2i p4,
    unsigned short line_thickness, struct nk_color col)
{
    /* ... TODO ... */
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

static void
nk_eve_draw_image(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w, unsigned short h,
    struct nk_image img, struct nk_color col)
{
    /* ... TODO ... */
    nk_eve_placeholder(phost, x, y, w, h, col);
}

static void
nk_eve_clear(Ft_Gpu_Hal_Context_t *phost, struct nk_color col)
{
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(col.r, col.g, col.b));
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_A(col.a));
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
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
            nk_eve_scissor(phost, s->x, s->y, s->w, s->h);
        }
        break;
        case NK_COMMAND_LINE:
        {
            const struct nk_command_line *l = (const struct nk_command_line *)cmd;
            nk_eve_stroke_line(phost, l->begin.x, l->begin.y, l->end.x,
                l->end.y, l->line_thickness, l->color);
        }
        break;
        case NK_COMMAND_RECT:
        {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
            nk_eve_stroke_rect(phost, r->x, r->y, r->w, r->h,
                r->rounding, r->line_thickness, r->color);
        }
        break;
        case NK_COMMAND_RECT_FILLED:
        {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            nk_eve_fill_rect(phost, r->x, r->y, r->w, r->h,
                r->rounding, r->color);
        }
        break;
        case NK_COMMAND_CIRCLE:
        {
            const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
            nk_eve_stroke_circle(phost, c->x, c->y, c->w, c->h, c->line_thickness, c->color);
        }
        break;
        case NK_COMMAND_CIRCLE_FILLED:
        {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
            nk_eve_fill_circle(phost, c->x, c->y, c->w, c->h, c->color);
        }
        break;
        case NK_COMMAND_TRIANGLE:
        {
            const struct nk_command_triangle *t = (const struct nk_command_triangle *)cmd;
            nk_eve_stroke_triangle(phost, t->a.x, t->a.y, t->b.x, t->b.y,
                t->c.x, t->c.y, t->line_thickness, t->color);
        }
        break;
        case NK_COMMAND_TRIANGLE_FILLED:
        {
            const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
            nk_eve_fill_triangle(phost, t->a.x, t->a.y, t->b.x, t->b.y,
                t->c.x, t->c.y, t->color);
        }
        break;
        case NK_COMMAND_POLYGON:
        {
            const struct nk_command_polygon *p = (const struct nk_command_polygon *)cmd;
            nk_eve_stroke_polygon(phost, p->points, p->point_count, p->line_thickness, p->color);
        }
        break;
        case NK_COMMAND_POLYGON_FILLED:
        {
            const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled *)cmd;
            nk_eve_fill_polygon(phost, p->points, p->point_count, p->color);
        }
        break;
        case NK_COMMAND_POLYLINE:
        {
            const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
            nk_eve_stroke_polyline(phost, p->points, p->point_count, p->line_thickness, p->color);
        }
        break;
        case NK_COMMAND_TEXT:
        {
            const struct nk_command_text *t = (const struct nk_command_text *)cmd;
            nk_eve_draw_text(phost, t->x, t->y, t->w, t->h,
                (const char *)t->string, t->length,
                (nk_evefont *)t->font->userdata.ptr,
                t->foreground);
        }
        break;
        case NK_COMMAND_CURVE:
        {
            const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
            nk_eve_stroke_curve(phost, q->begin, q->ctrl[0], q->ctrl[1],
                q->end, q->line_thickness, q->color);
        }
        break;
        case NK_COMMAND_RECT_MULTI_COLOR:
        {
            const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color *)cmd;
            nk_eve_rect_multi_color(phost, r->x, r->y, r->w, r->h, r->left, r->top, r->right, r->bottom);
        }
        break;
        case NK_COMMAND_IMAGE:
        {
            const struct nk_command_image *i = (const struct nk_command_image *)cmd;
            nk_eve_draw_image(phost, i->x, i->y, i->w, i->h, i->img, i->col);
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