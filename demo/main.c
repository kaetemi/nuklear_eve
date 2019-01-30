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

#ifndef NK_ASSERT
#include <assert.h>
#define NK_ASSERT(expr) assert(expr)
#endif

#include "nuklear_eve.h"
// #include "demo/style.c"
#include "demo/calculator.c"
#include "demo/overview.c"
#include "demo/node_editor.c"

int main(void)
{
    struct nk_context *ctx;

    int running = 1;
    int needs_refresh = 1;

    /* Initialize Nuklear EVE */
    /* ... TODO ... */
    /* TODO: ctx = nk_eve_init(host); */

    /* Main Loop */
    while (running)
    {
        /* Input */
        nk_input_begin(ctx);
        /* ... TODO ... */
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
                NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
        {
            enum
            {
                EASY,
                HARD
            };
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY))
                op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD))
                op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        calculator(ctx);
        overview(ctx);
        node_editor(ctx);
        /* ----------------------------------------- */

        /* Draw */
        /* TODO: nk_eve_render(nk_rgb(30,30,30)); */
    }

    /* Release Nuklear EVE */
    /* ... TODO ... */

    return EXIT_SUCCESS;
}

/* end of file */