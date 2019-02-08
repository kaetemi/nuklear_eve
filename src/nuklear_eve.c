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
- EVE Screen Designer Framework Core
*/

/*
TODO:
- Rounded rectangle stroke
- Font metrics
- Unicode font
- Bitmap
- Oval
*/

#define NK_IMPLEMENTATION
#define NK_EVE_IMPLEMENTATION
#include "nuklear_eve.h"

#include "Ft_Esd_Primitives.h"

static struct
{
    struct nk_context ctx;
    Esd_Context ec;
    Ft_Esd_Rect16 scissor;
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

static void
nk_eve_color_rgba(Ft_Gpu_Hal_Context_t *phost, struct nk_color col)
{
    Esd_Dl_COLOR_RGB((((col.r) & 255UL) << 16) | (((col.g) & 255UL) << 8) | ((col.b) & 255UL));
    Esd_Dl_COLOR_A(col.a);
}

#if (EVE_MODEL >= EVE_FT810)
#define nk_eve_vertex(x, y) Esd_Dl_VERTEX2F((x), (y))
#else
#define nk_eve_vertex(x, y) Esd_Dl_VERTEX2F((x) << 4, (y) << 4)
#endif

static void
nk_eve_scissor(Ft_Gpu_Hal_Context_t *phost, float x, float y, float w, float h)
{
    Ft_Esd_Rect16 rect = {
        .X = (int)x,
        .Y = (int)y,
        .Width = (int)(w + 1),
        .Height = (int)(h + 1),
    };
    Esd_Dl_Scissor_Adjust(rect, eve.scissor);
}

static void
nk_eve_placeholder(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(8);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x, y);
    nk_eve_vertex(x, y + h);
    nk_eve_vertex(x + w, y + h);
    nk_eve_vertex(x + w, y);
    nk_eve_vertex(x, y);
    nk_eve_vertex(x + w, y + h);
    Esd_Dl_END();
    Esd_Dl_BEGIN(LINES);
    nk_eve_vertex(x, y + h);
    nk_eve_vertex(x + w, y);
    Esd_Dl_END();
}

static void
nk_eve_stroke_line(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, unsigned int line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
    Esd_Dl_BEGIN(LINES);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x0, y0);
    nk_eve_vertex(x1, y1);
    Esd_Dl_END();
}

static void
nk_eve_stroke_rect(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short r, unsigned short line_thickness, struct nk_color col)
{
    // TODO: Support rounding
    // - Same trick as donut rendering for thick line
    // - Line stroke with manually drawn round corners for thin line

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x, y);
    nk_eve_vertex(x, y + h);
    nk_eve_vertex(x + w, y + h);
    nk_eve_vertex(x + w, y);
    nk_eve_vertex(x, y);
    Esd_Dl_END();
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
    Esd_Dl_LINE_WIDTH(width);
    Esd_Dl_BEGIN(RECTS);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x0, y0);
    nk_eve_vertex(x1, y1);
    Esd_Dl_END();
}

static void
nk_eve_rect_multi_color(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color left, struct nk_color top,
    struct nk_color right, struct nk_color bottom)
{
    Ft_Esd_Rect16 rect = {
        .X = x,
        .Y = y,
        .Width = w,
        .Height = h,
    };
    Esd_Render_MultiGradient(rect,
        ESD_COMPOSE_ARGB8888(left.r, left.g, left.b, left.a),
        ESD_COMPOSE_ARGB8888(top.r, top.g, top.b, top.a),
        ESD_COMPOSE_ARGB8888(bottom.r, bottom.g, bottom.b, bottom.a),
        ESD_COMPOSE_ARGB8888(right.r, right.g, right.b, right.a));
}

static void
nk_eve_fill_polygon(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts, int count, struct nk_color col)
{
    short xmin = 4095, xmax = 0, ymin = 4095, ymax = 0;

    if (!count)
        return;

    /* Set color */
    nk_eve_color_rgba(phost, col);

    /* Find boundaries */
    for (int i = 0; i < count; ++i)
    {
        if (pnts[i].x < xmin)
            xmin = pnts[i].x;
        if (pnts[i].x > xmax)
            xmax = pnts[i].x;
        if (pnts[i].y < ymin)
            ymin = pnts[i].y;
        if (pnts[i].y > ymax)
            ymax = pnts[i].y;
    }

    /* Clear stencil */
    Ft_Esd_Rect16 boundary = {
        .X = xmin,
        .Y = ymin,
        .Width = xmax - xmin,
        .Height = ymax - ymin,
    };
    Ft_Esd_Rect16 scissor = Esd_Dl_Scissor_Set(boundary);

    /* Prepare state */
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    Esd_Dl_SAVE_CONTEXT();
    Esd_Dl_CLEAR(0, 1, 0);

    /* Draw polygon mask */
    Eve_CoCmd_SendCmd(phost, COLOR_MASK(0, 0, 0, 0));
    Eve_CoCmd_SendCmd(phost, STENCIL_OP(KEEP, INVERT));
    Eve_CoCmd_SendCmd(phost, STENCIL_FUNC(ALWAYS, 255, 255));
    Esd_Dl_BEGIN(EDGE_STRIP_B);
    for (int i = 0; i < count; ++i)
    {
        nk_eve_vertex(pnts[i].x, pnts[i].y);
    }
    Esd_Dl_END();

    /* Draw color */
    Eve_CoCmd_SendCmd(phost, COLOR_MASK(1, 1, 1, 1));
    Eve_CoCmd_SendCmd(phost, STENCIL_FUNC(EQUAL, 255, 255));
    Esd_Dl_BEGIN(RECTS);
    nk_eve_vertex(xmin, ymin);
    nk_eve_vertex(xmax, ymax);
    Esd_Dl_END();

    /* Restore state */
    Esd_Dl_RESTORE_CONTEXT();
    Esd_Dl_Scissor_Reset(scissor);
}

static void
nk_eve_fill_triangle(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, short x2, short y2, struct nk_color col)
{
    struct nk_vec2i pnts[4];
    pnts[0].x = x0;
    pnts[0].y = y0;
    pnts[1].x = x1;
    pnts[1].y = y1;
    pnts[2].x = x2;
    pnts[2].y = y2;
    pnts[3].x = x0;
    pnts[3].y = y0;
    nk_eve_fill_polygon(phost, pnts, 4, col);

    /*
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(8);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x0, y0);
    nk_eve_vertex(x1, y1);
    nk_eve_vertex(x2, y2);
    nk_eve_vertex(x0, y0);
    Esd_Dl_END();
    */
}

static void
nk_eve_stroke_polygon(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts, int count,
    unsigned short line_thickness, struct nk_color col)
{
    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    for (int i = 0; i < count; ++i)
    {
        nk_eve_vertex(pnts[i].x, pnts[i].y);
    }
    // TODO: Validate if it's necessary to close the polygon
    // nk_eve_vertex(pnts[0].x, pnts[0].x);
    Esd_Dl_END();
}

static void
nk_eve_stroke_polyline(Ft_Gpu_Hal_Context_t *phost, const struct nk_vec2i *pnts,
    int count, unsigned short line_thickness, struct nk_color col)
{
    // TODO: This is just the same as nk_eve_stroke_polygon?

    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    for (int i = 0; i < count; ++i)
    {
        nk_eve_vertex(pnts[i].x, pnts[i].y);
    }
    // TODO: Validate if it's necessary to close the polygon
    // nk_eve_vertex(pnts[0].x, pnts[0].x));
    Esd_Dl_END();
}

static void
nk_eve_stroke_triangle(Ft_Gpu_Hal_Context_t *phost, short x0, short y0, short x1,
    short y1, short x2, short y2, unsigned short line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x0, y0);
    nk_eve_vertex(x1, y1);
    nk_eve_vertex(x2, y2);
    nk_eve_vertex(x0, y0);
    Esd_Dl_END();
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
    Esd_Dl_POINT_SIZE(r);
    Esd_Dl_BEGIN(POINTS);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(1);
    Esd_Dl_VERTEX2F(xc, yc);
#else
    Esd_Dl_VERTEX2F(xc << 3, yc << 3);
#endif
    Esd_Dl_END();
}

static void
nk_eve_stroke_circle(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short line_thickness, struct nk_color col)
{
    // Only support circles, not ovals
    int r = (int)(w > h ? h : w) << 3;
    int xc = ((int)x << 1) + (int)w;
    int yc = ((int)y << 1) + (int)h;
    Esd_Render_Circle_Stroke(xc << 3, yc << 3, r, line_thickness, ESD_COMPOSE_ARGB8888(col.r, col.g, col.b, col.a));
}

static void
nk_eve_stroke_curve(Ft_Gpu_Hal_Context_t *phost,
    struct nk_vec2i p1, struct nk_vec2i p2,
    struct nk_vec2i p3, struct nk_vec2i p4, unsigned int num_segments,
    unsigned short line_thickness, struct nk_color col)
{
    unsigned int i_step;
    float t_step;
    struct nk_vec2i last = p1;

    if (num_segments < 1)
        num_segments = 1;
    t_step = 1.0f / (float)num_segments;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 4);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(4);
#endif

    Esd_Dl_BEGIN(LINE_STRIP);
    Esd_Dl_VERTEX2F(p1.x << 4, p1.y << 4);
    for (i_step = 1; i_step < num_segments; ++i_step)
    {
        float t = t_step * (float)i_step;
        float u = 1.0f - t;
        float w1 = u * u * u;
        float w2 = 3 * u * u * t;
        float w3 = 3 * u * t * t;
        float w4 = t * t * t;
        float x = w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x;
        float y = w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y;
        Esd_Dl_VERTEX2F(x * 16.f, y * 16.f);
    }
    Esd_Dl_VERTEX2F(p4.x << 4, p4.y << 4);
    Esd_Dl_END();
}

static void
nk_eve_draw_text(Ft_Gpu_Hal_Context_t *phost, short x, short y, unsigned short w, unsigned short h,
    const char *text, int len, nk_evefont *font, struct nk_color cfg)
{
    if (!len)
        return;

    Esd_Dl_COLOR_RGB((((cfg.r) & 255UL) << 16) | (((cfg.g) & 255UL) << 8) | ((cfg.b) & 255UL));
    Esd_Dl_COLOR_A(cfg.a);
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
    Esd_Dl_CLEAR_COLOR_RGB((((col.r) & 255UL) << 16) | (((col.g) & 255UL) << 8) | ((col.b) & 255UL));
    Esd_Dl_CLEAR_COLOR_A(col.a);
    Esd_Dl_CLEAR(1, 1, 1);
}

static void
nk_eve_cb_start(void *context)
{
}

static void
nk_eve_cb_update(void *context)
{
    nk_input_begin(&eve.ctx);
    ft_bool_t touching = !!Ft_Esd_TouchTag_CurrentTag(NULL);
    ft_int16_t touchX = Ft_Esd_TouchTag_TouchX(NULL);
    ft_int16_t touchY = Ft_Esd_TouchTag_TouchY(NULL);
    nk_input_motion(&eve.ctx, touchX, touchY);
    nk_input_button(&eve.ctx, NK_BUTTON_LEFT, touchX, touchY, touching);
    nk_input_end(&eve.ctx);
}

static void
nk_eve_cb_render(void *context)
{
    const struct nk_command *cmd;
    Ft_Gpu_Hal_Context_t *phost = &eve.ec.HalContext;
    eve.scissor = Esd_Dl_Scissor_Get();

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
                q->end, 22, q->line_thickness, q->color);
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

    Esd_Dl_Scissor_Reset(eve.scissor);
}

static void
nk_eve_cb_idle(void *context)
{
}

static void
nk_eve_cb_end(void *context)
{
}

NK_API struct nk_context *
nk_eve_init(nk_evefont *evefont)
{
    struct nk_user_font *font = &evefont->nk;
    font->userdata = nk_handle_ptr(evefont);
    font->height = 20; /* ... TODO ... */
    font->width = nk_evefont_get_text_width;

    nk_init_default(&eve.ctx, font);
    Esd_Parameters ep;
    Esd_Defaults(&ep);
    ep.Start = nk_eve_cb_start;
    ep.Update = nk_eve_cb_update;
    ep.Render = nk_eve_cb_render;
    ep.Idle = nk_eve_cb_idle;
    ep.End = nk_eve_cb_end;
    Esd_Initialize(&eve.ec, &ep);
    Esd_Start(&eve.ec);
    // eve.ctx.clip.copy = nk_eve_clipboard_copy;
    // eve.ctx.clip.paste = nk_eve_clipboard_paste;
    // eve.ctx.clip.userdata = nk_handle_ptr(0);
    // eve.phost = phost;
    return &eve.ctx;
}

NK_API void
nk_eve_shutdown(void)
{
    Esd_Stop(&eve.ec);
    Esd_Release(&eve.ec);
    Esd_Shutdown();
    nk_free(&eve.ctx);
}

NK_API void
nk_eve_update()
{
    Esd_Update(&eve.ec);
}

NK_API void
nk_eve_render(struct nk_color clear)
{
    Esd_Render(&eve.ec);
    Esd_WaitSwap(&eve.ec);
    nk_clear(&eve.ctx);
}

/* end of file */
