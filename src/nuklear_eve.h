/*
Nuklear EVE
Copyright (C) 2019  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef NK_EVE_H_
#define NK_EVE_H_

#include "Esd_Core.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
/* #define NK_IMPLEMENTATION */
/* #define NK_EVE_IMPLEMENTATION */
#include "nuklear.h"

typedef struct nk_evefont nk_evefont;
NK_API struct nk_context *nk_eve_init(nk_evefont *evefont);
NK_API void nk_eve_update();
NK_API void nk_eve_render(struct nk_color clear);
NK_API void nk_eve_shutdown(void);

/* font */
NK_API nk_evefont *nk_evefont_create_rom(int rom_font);
NK_API void nk_evefont_del(nk_evefont *font);
NK_API void nk_eve_set_font(nk_evefont *font);

#endif

/* end of file */