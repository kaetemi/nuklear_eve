/*
Nuklear EVE
Copyright (C) 2019  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

/*
This C source file demonstrates the Nuklear EVE implementation.
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#ifndef NK_ASSERT
#include <assert.h>
#define NK_ASSERT(expr) assert(expr)
#endif

#include "nuklear_eve.h"

#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4477)
#pragma warning(disable : 4244)
/*#include "demo/style.c"*/
#include "demo/calculator.c"
#include "demo/overview.c"
#include "demo/node_editor.c"
#pragma warning(pop)

#include "Ft_Esd_Utility.h"

int main(void)
{
    struct nk_evefont *font;
#if (EVE_MODEL >= EVE_FT810)
    struct nk_evefont *font_large;
#endif
    struct nk_context *ctx;

    int show_calculator = 0;
    int show_overview = 0;
    int show_node_editor = 0;

    int overview_resized = 0;

    /* Initialize Nuklear EVE */
    font = nk_evefont_create_rom(27);
    ctx = nk_eve_init(font);

#if (EVE_MODEL >= EVE_FT810)
    font_large = nk_evefont_create_rom(32);
#endif
    
    /* style.c */
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/

    /* Main Loop */
    while (true)
    {
        /* Input */
        nk_eve_update();

        /* GUI */
        if (nk_begin(ctx, "Nuklear EVE", nk_rect(50, 50, 300, 400),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum
            {
                EASY,
                HARD
            };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 200, 1);
            nk_checkbox_label(ctx, "Show Calculator", &show_calculator);

            nk_layout_row_static(ctx, 30, 200, 1);
            nk_checkbox_label(ctx, "Show Overview", &show_overview);

            nk_layout_row_static(ctx, 30, 200, 1);
            nk_checkbox_label(ctx, "Show Node Editor", &show_node_editor);

            nk_layout_row_static(ctx, 30, 200, 1);
            if (nk_button_label(ctx, "Show Bridgetek Logo"))
                Esd_ShowLogo();

            nk_layout_row_static(ctx, 30, 200, 1);
            if (nk_button_label(ctx, "Play Sound"))
            {
                EVE_Hal_wr32(nk_eve_hal(), REG_SOUND, (60 << 8) | 0x46);
                EVE_Hal_wr32(nk_eve_hal(), REG_PLAY, 1);
            }
            
#if (EVE_MODEL >= EVE_FT810)
            nk_eve_set_font(font_large);
            nk_layout_row_static(ctx, 50, 210, 1);
            nk_label(ctx, "Big Font", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_TOP);
            nk_eve_set_font(font);
#endif
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        if (show_calculator)
            calculator(ctx);
        if (show_overview)
        {
            overview(ctx);
            if (!overview_resized)
            {
                nk_window_set_size(ctx, "Overview", nk_vec2(400, 480));
                overview_resized = 1;
            }
        }
        else
        {
            overview_resized = 0;
        }
        if (show_node_editor)
            node_editor(ctx);
        /* ----------------------------------------- */

        /* Draw */
        if (!nk_eve_render(nk_rgb(30, 30, 30)))
        {
            /* Coprocessor fault */
            show_calculator = 0;
            show_overview = 0;
            show_node_editor = 0;
        }
    }

    /* Release Nuklear EVE */
    nk_evefont_del(font);
#if (EVE_MODEL >= EVE_FT810)
    nk_evefont_del(font_large);
#endif
    nk_eve_shutdown();

    return EXIT_SUCCESS;
}

/* end of file */
