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

/// API to demonstrate calibrate widget/functionality
ft_bool_t App_CoPro_Widget_Calibrate(Ft_Gpu_Hal_Context_t *phost)
{
#if defined(EVE_SCREEN_CAPACITIVE)
    Ft_Gpu_Hal_Wr8(phost, REG_CTOUCH_EXTENDED, CTOUCH_MODE_COMPATIBILITY);
#endif

    eve_printf_debug("App_CoPro_Widget_Calibrate: Start Frame\n");
    Ft_Gpu_CoCmd_StartFrame(phost);

    Ft_Gpu_CoCmd_DlStart(phost);
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR_COLOR_RGB(64, 64, 64));
    Ft_Gpu_CoCmd_SendCmd(phost, CLEAR(1, 1, 1));
    Ft_Gpu_CoCmd_SendCmd(phost, COLOR_RGB(0xff, 0xff, 0xff));

    // Ft_Gpu_CoCmd_Text(phost, (FT_DispWidth / 2), (FT_DispHeight / 2), 27, OPT_CENTER, "Please Tap on the dot");

    ft_uint32_t result = Ft_Gpu_CoCmd_Calibrate(phost);

    eve_printf_debug("App_CoPro_Widget_Calibrate: End Frame\n");
    Ft_Gpu_CoCmd_EndFrame(phost);

    // Print the configured values
    ft_uint32_t transMatrix[6];
    Ft_Gpu_Hal_RdMem(phost, REG_TOUCH_TRANSFORM_A, (ft_uint8_t *)transMatrix, 4 * 6); //read all the 6 coefficients
    eve_printf_debug("Touch screen transform values are A 0x%x,B 0x%x,C 0x%x,D 0x%x,E 0x%x, F 0x%x\n",
        transMatrix[0], transMatrix[1], transMatrix[2], transMatrix[3], transMatrix[4], transMatrix[5]);

    return result != 0;
}

int main(void)
{
    Ft_Gpu_HalInit_t halinit;
    Ft_Gpu_Hal_Context_t host;
    nk_evefont *font;
    struct nk_context *ctx;

    int running = 1;
    int needs_refresh = 1;

    Ft_Mcu_Init();

    Ft_Gpu_Hal_Init(&halinit);
    Ft_Gpu_Hal_Open(&host);

    Eve_BootupConfig(&host);

#ifndef BT8XXEMU_PLATFORM
    if (!App_CoPro_Widget_Calibrate(phost))
    {
        eve_printf_debug("Calibrate failed\n");
    }
#endif

    /* Initialize Nuklear EVE */
    /* ... TODO ... */
    font = nk_evefont_create_rom(27);
    ctx = nk_eve_init(font, &host);

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
        // calculator(ctx);
		// overview(ctx);
		// node_editor(ctx);
        /* ----------------------------------------- */

        /* Draw */
        nk_eve_render(nk_rgb(30, 30, 30));
    }

    /* Release Nuklear EVE */
    nk_eve_shutdown();

    Ft_Gpu_Hal_Close(&host);
    Ft_Gpu_Hal_DeInit();

    return EXIT_SUCCESS;
}

/* end of file */