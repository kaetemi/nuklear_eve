/*
Nuklear EVE
Copyright (C) 2019  Bridgetek Pte Lte
Author: Jan Boon <jan.boon@kaetemi.be>
*/

#ifndef NK_EVE_H_
#define NK_EVE_H_

#include "Ft_Esd_Core.h"

#if !defined(_MSC_VER) || (_MSC_VER >= 1900)
#define NK_INCLUDE_FIXED_TYPES
#endif
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
/* #define NK_IMPLEMENTATION */
/* #define NK_EVE_IMPLEMENTATION */
#pragma warning(push)
#pragma warning(disable: 4996)
#include "nuklear.h"
#pragma warning(pop)

typedef struct nk_evefont nk_evefont;
NK_API struct nk_context *nk_eve_init(struct nk_evefont *evefont);
NK_API void nk_eve_update(void);
NK_API bool nk_eve_render(struct nk_color clear);
NK_API void nk_eve_shutdown(void);
NK_API EVE_HalContext *nk_eve_hal(void);

/* font */
NK_API struct nk_evefont *nk_evefont_create(struct Esd_FontInfo *font_info);
NK_API struct nk_evefont *nk_evefont_create_rom(int rom_font);
NK_API void nk_evefont_del(struct nk_evefont *evefont);
NK_API void nk_eve_set_font(struct nk_evefont *evefont);

#endif

/* end of file */