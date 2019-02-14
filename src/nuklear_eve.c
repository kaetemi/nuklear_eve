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

References:
- https://github.com/vurtun/nuklear/blob/master/nuklear.h
- https://github.com/vurtun/nuklear/blob/master/demo/gdi/nuklear_gdi.h
- https://github.com/vurtun/nuklear/blob/master/demo/allegro5/nuklear_allegro5.h
- https://github.com/keharriso/love-nuklear/blob/master/src/nuklear_love.c

Known limitations:
- Ovals are not supported, only circles

TODO:
- Unicode font (glyph width)
- Bitmap (NK_COMMAND_IMAGE)
- Cross integration with ESD Layout
- NK_COMMAND_ARC
- NK_COMMAND_ARC_FILLED

*/

#include "EVE_Config.h"

#define NK_IMPLEMENTATION
#define NK_EVE_IMPLEMENTATION
#if defined(FT900_PLATFORM) || defined(FT93X_PLATFORM)
#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_SQRT sqrtf
#define NK_SIN sinf
#define NK_COS cosf
#define NK_STRTOD strtod
#define NK_VSNPRINTF vsnprintf
#endif
#include "nuklear_eve.h"

#include "Ft_Esd_Core.h"
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
    Esd_FontInfo *font_info;
    union
    {
        EVE_Gpu_Fonts *font_block;
        EVE_Gpu_FontsExt *font_ext_block;
    };
};

NK_API void
nk_evefont_del(nk_evefont *font)
{
    if (!font)
        return;
    if (font->font_block)
    {
        free(font->font_block);
        font->font_block = NULL;
    }
    free(font);
}

static float
nk_evefont_get_text_width(nk_handle handle, float height, const char *text, int len);

static bool
nk_evefont_setup(struct nk_evefont *evefont)
{
    struct nk_user_font *font = &evefont->nk;
    eve_assert(evefont->font_info);
    eve_assert(font);
    if (!evefont->font_block)
    {
        uint32_t fontAddr;
        if (evefont->font_info->Type == ESD_FONT_ROM)
        {
            Esd_RomFontInfo *romfont = (Esd_RomFontInfo *)evefont->font_info;
            uint32_t ft = Ft_Gpu_Hal_Rd32(Ft_Esd_Host, ROMFONT_TABLEADDRESS);
            fontAddr = ft + (FT_GPU_FONT_TABLE_SIZE * (romfont->RomFont - 16));
            eve_assert(sizeof(EVE_Gpu_Fonts) == FT_GPU_FONT_TABLE_SIZE);
        }
        else
        {
            fontAddr = Esd_LoadFont(evefont->font_info);
        }
        if (fontAddr == GA_INVALID)
            return false;
        if (evefont->font_info->Type == ESD_FONT_EXTENDED)
        {
            eve_debug_break();
            /* ... TODO ... */
        }
        else
        {
            evefont->font_block = calloc(1, sizeof(EVE_Gpu_Fonts));
            EVE_Hal_rdMem(&eve.ec.HalContext, (uint8_t *)evefont->font_block, fontAddr, sizeof(EVE_Gpu_Fonts));
        }
        evefont->nk.height = evefont->font_info->CapsHeight;
        evefont->nk.width = nk_evefont_get_text_width;
    }
    else
    {
        if (evefont->font_info->Type != ESD_FONT_ROM)
            return Esd_LoadFont(evefont->font_info) != GA_INVALID;
    }
    return true;
}

static float
nk_evefont_get_text_width(nk_handle handle, float height, const char *text, int len)
{
    struct nk_evefont *evefont = (struct nk_evefont *)handle.ptr;
    eve_assert(evefont);
    eve_assert(evefont->nk.userdata.ptr == handle.ptr);
    if (evefont->font_info->Type == ESD_FONT_EXTENDED)
    {
        eve_debug_break();
        /* ... TODO ... */
        return 20.f;
    }
    else
    {
        int width, i, c;
        if (!nk_evefont_setup(evefont) && !evefont->font_block)
            return 20.f;
        eve_assert(evefont->font_block);
        eve_assert(EVE_GPU_NUMCHAR_PERFONT == 128);
        eve_assert(sizeof(evefont->font_block->FontWidth) == 128);
        width = 0;
        for (i = 0; (c = text[i]) != 0 && i < len; ++i)
            width += evefont->font_block->FontWidth[c & 0x7F];
        return (float)width;
    }
}

NK_API void
nk_eve_set_font(struct nk_evefont *evefont)
{
    struct nk_user_font *font = &evefont->nk;
    nk_evefont_setup(evefont);
    nk_style_set_font(&eve.ctx, font);
}

NK_API struct nk_evefont *
nk_evefont_create(struct Esd_FontInfo *font_info)
{
    eve_assert(font_info);
    if (!font_info)
        return NULL;
    struct nk_evefont *evefont = (struct nk_evefont *)calloc(1, sizeof(struct nk_evefont));
    if (!evefont)
        return NULL;
    evefont->font_info = font_info;
    evefont->nk.userdata = nk_handle_ptr(evefont);
    // evefont->nk.height = 20; /* ... TODO ... */
    // evefont->nk.width = nk_evefont_get_text_width;
    eve_assert(!evefont->font_block);
    return evefont;
}

NK_API struct nk_evefont *
nk_evefont_create_rom(int handle)
{
    return nk_evefont_create(Esd_GetRomFont(handle));
}

static void
nk_eve_color_rgba(EVE_HalContext *phost, struct nk_color col)
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
nk_eve_scissor(EVE_HalContext *phost, float x, float y, float w, float h)
{
    Ft_Esd_Rect16 rect;
    rect.X = (int)x;
    rect.Y = (int)y;
    rect.Width = (int)(w + 1);
    rect.Height = (int)(h + 1);
    Esd_Dl_Scissor_Adjust(rect, eve.scissor);
}

static void
nk_eve_placeholder(EVE_HalContext *phost, short x, short y, unsigned short w,
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
nk_eve_stroke_line(EVE_HalContext *phost, short x0, short y0, short x1,
    short y1, unsigned int line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 3);
    Esd_Dl_BEGIN(LINES);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    nk_eve_vertex(x0, y0);
    nk_eve_vertex(x1, y1);
    Esd_Dl_END();
}

static void
nk_eve_stroke_rect(EVE_HalContext *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short r, unsigned short line_thickness, struct nk_color col)
{
    int offset = (line_thickness & 1) ? 0 : 8;
    Esd_Render_Rect_Stroke(
        ((int)x << 4) - offset, ((int)y << 4) - offset,
        ((int)w << 4) + (offset << 1), ((int)h << 4) + (offset << 1),
        (int)r << 4, (int)line_thickness << 4,
        ESD_COMPOSE_ARGB8888(col.r, col.g, col.b, col.a));
}

static void
nk_eve_fill_rect(EVE_HalContext *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short r, struct nk_color col)
{
    if (r < 1)
        r = 1;

    scope
    {
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
}

static void
nk_eve_rect_multi_color(EVE_HalContext *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color left, struct nk_color top,
    struct nk_color right, struct nk_color bottom)
{
    Esd_Render_MultiGradient(x, y, w, h,
        ESD_COMPOSE_ARGB8888(left.r, left.g, left.b, left.a),
        ESD_COMPOSE_ARGB8888(top.r, top.g, top.b, top.a),
        ESD_COMPOSE_ARGB8888(bottom.r, bottom.g, bottom.b, bottom.a),
        ESD_COMPOSE_ARGB8888(right.r, right.g, right.b, right.a));
}

static void
nk_eve_fill_polygon(EVE_HalContext *phost, const struct nk_vec2i *pnts, int count, struct nk_color col)
{
    short xmin = 4095, xmax = 0, ymin = 4095, ymax = 0;
    Ft_Esd_Rect16 boundary, scissor;
    int i;

    if (!count)
        return;

    /* Set color */
    nk_eve_color_rgba(phost, col);

    /* Find boundaries */
    for (i = 0; i < count; ++i)
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
    boundary.X = xmin;
    boundary.Y = ymin;
    boundary.Width = xmax - xmin;
    boundary.Height = ymax - ymin;
    scissor = Esd_Dl_Scissor_Set(boundary);

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
    for (i = 0; i < count; ++i)
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
nk_eve_fill_triangle(EVE_HalContext *phost, short x0, short y0, short x1,
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
nk_eve_stroke_polygon(EVE_HalContext *phost, const struct nk_vec2i *pnts, int count,
    unsigned short line_thickness, struct nk_color col)
{
    int i;

    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 3);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    for (i = 0; i < count; ++i)
    {
        nk_eve_vertex(pnts[i].x, pnts[i].y);
    }
    // TODO: Validate if it's necessary to close the polygon
    // nk_eve_vertex(pnts[0].x, pnts[0].x);
    Esd_Dl_END();
}

static void
nk_eve_stroke_polyline(EVE_HalContext *phost, const struct nk_vec2i *pnts,
    int count, unsigned short line_thickness, struct nk_color col)
{
    // TODO: This is just the same as nk_eve_stroke_polygon?
    int i;

    if (!count)
        return;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 3);
    Esd_Dl_BEGIN(LINE_STRIP);
#if (EVE_MODEL >= EVE_FT810)
    Esd_Dl_VERTEX_FORMAT(0);
#endif
    for (i = 0; i < count; ++i)
    {
        nk_eve_vertex(pnts[i].x, pnts[i].y);
    }
    // TODO: Validate if it's necessary to close the polygon
    // nk_eve_vertex(pnts[0].x, pnts[0].x));
    Esd_Dl_END();
}

static void
nk_eve_stroke_triangle(EVE_HalContext *phost, short x0, short y0, short x1,
    short y1, short x2, short y2, unsigned short line_thickness, struct nk_color col)
{
    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 3);
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
nk_eve_fill_circle(EVE_HalContext *phost, short x, short y, unsigned short w,
    unsigned short h, struct nk_color col)
{
    // Only support circles, not ovals
    int r = (int)(w > h ? h : w) << 3;
    int xc = ((int)x << 1) + (int)w - 1;
    int yc = ((int)y << 1) + (int)h - 1;
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
nk_eve_stroke_circle(EVE_HalContext *phost, short x, short y, unsigned short w,
    unsigned short h, unsigned short line_thickness, struct nk_color col)
{
    // Only support circles, not ovals
    int r = (int)(w > h ? h : w) << 3;
    int xc = ((int)x << 1) + (int)w - 1;
    int yc = ((int)y << 1) + (int)h - 1;
    int offset = (line_thickness & 1) ? 0 : 8;
    Esd_Render_Circle_Stroke(xc << 3, yc << 3, r, ((int)line_thickness << 4) + offset, ESD_COMPOSE_ARGB8888(col.r, col.g, col.b, col.a));
}

static void
nk_eve_stroke_curve(EVE_HalContext *phost,
    struct nk_vec2i p1, struct nk_vec2i p2,
    struct nk_vec2i p3, struct nk_vec2i p4, unsigned int num_segments,
    unsigned short line_thickness, struct nk_color col)
{
    unsigned int i_step;
    float t_step;

    if (num_segments < 1)
        num_segments = 1;
    t_step = 1.0f / (float)num_segments;

    nk_eve_color_rgba(phost, col);
    Esd_Dl_LINE_WIDTH(line_thickness << 3);
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
        Esd_Dl_VERTEX2F((uint16_t)(x * 16.f), (uint16_t)(y * 16.f));
    }
    Esd_Dl_VERTEX2F(p4.x << 4, p4.y << 4);
    Esd_Dl_END();
}

static void
nk_eve_draw_text(EVE_HalContext *phost, short x, short y, unsigned short w, unsigned short h,
    const char *text, int len, nk_evefont *evefont, struct nk_color cfg)
{
    if (!len)
        return;

    eve_assert(evefont->font_info);
    uint8_t handle = Ft_Esd_Dl_Font_Setup(evefont->font_info);
    if (handle == FT_ESD_BITMAPHANDLE_INVALID)
        return;

    y = y - evefont->font_info->BaseLine + evefont->font_info->CapsHeight;
    Esd_Dl_COLOR_RGB((((cfg.r) & 255UL) << 16) | (((cfg.g) & 255UL) << 8) | ((cfg.b) & 255UL));
    Esd_Dl_COLOR_A(cfg.a);
    Ft_Gpu_CoCmd_Text_S(phost, x, y, handle, 0, text, len);
    // Ft_Gpu_CoCmd_Text(phost, x, y, handle, 0, text);
}

static void
nk_eve_draw_image(EVE_HalContext *phost, short x, short y, unsigned short w, unsigned short h,
    struct nk_image img, struct nk_color col)
{
    /* ... TODO ... */
    nk_eve_placeholder(phost, x, y, w, h, col);
}

static void
nk_eve_cb_start(void *context)
{
}

static void
nk_eve_cb_update(void *context)
{
    nk_input_begin(&eve.ctx);
    scope
    {
        ft_bool_t touching = !!Ft_Esd_TouchTag_CurrentTag(NULL);
        ft_int16_t touchX = Ft_Esd_TouchTag_TouchX(NULL);
        ft_int16_t touchY = Ft_Esd_TouchTag_TouchY(NULL);
        nk_input_motion(&eve.ctx, touchX, touchY);
        nk_input_button(&eve.ctx, NK_BUTTON_LEFT, touchX, touchY, touching);
    }
    nk_input_end(&eve.ctx);
}

static void
nk_eve_cb_render(void *context)
{
    const struct nk_command *cmd;
    EVE_HalContext *phost = &eve.ec.HalContext;
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
            const struct nk_command *next = nk__next(&eve.ctx, cmd);
            /* Optimization for rect with outline behind */
            if (next && next->type == NK_COMMAND_RECT_FILLED && r->color.a == 255)
            {
                const struct nk_command_rect_filled *rs = (const struct nk_command_rect_filled *)next;
                if (rs->color.a == 255
                    && rs->x == r->x && rs->y == r->y
                    && rs->w == r->w && rs->h == r->h
                    && rs->rounding == r->rounding)
                {
                    short half_thickness = r->line_thickness >> 1;
                    short inner_rounding = r->rounding - half_thickness;
                    short outer_rounding = inner_rounding + r->line_thickness;
                    short outer_x0 = r->x - half_thickness;
                    short outer_y0 = r->y - half_thickness;
                    short outer_x1 = r->x + r->w + half_thickness;
                    short outer_y1 = r->y + r->h + half_thickness;
                    nk_eve_fill_rect(phost, outer_x0, outer_y0, outer_x1 - outer_x0, outer_y1 - outer_y0,
                        outer_rounding, rs->color);
                    nk_eve_fill_rect(phost, rs->x, rs->y, rs->w, rs->h,
                        rs->rounding, rs->color);
                    cmd = next;
                    break;
                }
            }
            nk_eve_stroke_rect(phost, r->x, r->y, r->w, r->h,
                r->rounding, r->line_thickness, r->color);
        }
        break;
        case NK_COMMAND_RECT_FILLED:
        {
            const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
            const struct nk_command *next = nk__next(&eve.ctx, cmd);
            /* Optimization for rect with outline. This is the common case */
            if (next && next->type == NK_COMMAND_RECT && r->color.a == 255)
            {
                const struct nk_command_rect *rs = (const struct nk_command_rect *)next;
                if (rs->color.a == 255
                    && rs->x == r->x && rs->y == r->y
                    && rs->w == r->w && rs->h == r->h
                    && rs->rounding == r->rounding)
                {
                    short half_thickness = rs->line_thickness >> 1;
                    short inner_rounding = r->rounding - half_thickness;
                    short outer_rounding = inner_rounding + rs->line_thickness;
                    short outer_x0 = r->x - half_thickness;
                    short outer_y0 = r->y - half_thickness;
                    short outer_x1 = r->x + r->w + half_thickness;
                    short outer_y1 = r->y + r->h + half_thickness;
                    nk_eve_fill_rect(phost, outer_x0, outer_y0, outer_x1 - outer_x0, outer_y1 - outer_y0,
                        outer_rounding, rs->color);
                    scope
                    {
                        short inner_x0 = outer_x0 + rs->line_thickness;
                        short inner_y0 = outer_y0 + rs->line_thickness;
                        short inner_x1 = outer_x1 - rs->line_thickness;
                        short inner_y1 = outer_y1 - rs->line_thickness;
                        nk_eve_fill_rect(phost, inner_x0, inner_y0, inner_x1 - inner_x0, inner_y1 - inner_y0,
                            inner_rounding, r->color);
                    }
                    cmd = next;
                    break;
                }
            }
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
nk_eve_init(struct nk_evefont *evefont)
{
    Esd_Parameters ep;
    struct nk_user_font *font = &evefont->nk;
    nk_init_default(&eve.ctx, font);
    Esd_Defaults(&ep);
    ep.Start = nk_eve_cb_start;
    ep.Update = nk_eve_cb_update;
    ep.Render = nk_eve_cb_render;
    ep.Idle = nk_eve_cb_idle;
    ep.End = nk_eve_cb_end;
    Esd_Initialize(&eve.ec, &ep);
    Esd_Start(&eve.ec);
    eve_assert_do(nk_evefont_setup(evefont));
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
nk_eve_update(void)
{
    Esd_Update(&eve.ec);
}

NK_API bool
nk_eve_render(struct nk_color clear)
{
    Esd_Render(&eve.ec);
    nk_clear(&eve.ctx);
    return Esd_WaitSwap(&eve.ec);
}

NK_API EVE_HalContext *
nk_eve_hal(void)
{
    return &eve.ec.HalContext;
}

/* end of file */
