
#include "Esd_Core.h"
#include "FT_Esd_Primitives.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_BitmapHandle.h"

// Multi gradient rendering state
Ft_Esd_GpuHandle s_MultiGradient_GpuHandle;
ft_uint32_t s_MultiGradient_Cell;

// Switch to use coprocessor for scaling matrix. Uses more bitmap matrix entries in the display list
#define ESD_MULTIGRADIENT_CO_SCALE 0

// The maximum number of multi gradients that can be on screen at once, multiplied by two
#define ESD_MULTIGRADIENT_MAX_NB (1 << 6)

ft_void_t Esd_Render_MultiGradient(Ft_Esd_Rect16 globalRect, ft_argb32_t topLeft, ft_argb32_t topRight, ft_argb32_t bottomLeft, ft_argb32_t bottomRight)
{
	// Get address of RAM_G used for gradient palette
	ft_uint32_t addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, s_MultiGradient_GpuHandle);
	if (addr == GA_INVALID)
	{
		// Allocate enough memory for 32 gradients.
		// Two bytes * four pixels * 64 gradients, 32 per frame flip.
		s_MultiGradient_GpuHandle = Ft_Esd_GpuAlloc_Alloc(Ft_Esd_GAlloc, 2 * 4 * ESD_MULTIGRADIENT_MAX_NB, GA_GC_FLAG);
		addr = Ft_Esd_GpuAlloc_Get(Ft_Esd_GAlloc, s_MultiGradient_GpuHandle);
	}
	if (addr == GA_INVALID)
	{
		eve_printf_debug("Unable to allocate RAM_G for MultiGradient\n");
		return;
	}

	// Select cell address directly
	addr += (s_MultiGradient_Cell * 8);

	// Rect
	ft_int32_t x = globalRect.X, y = globalRect.Y, width = globalRect.Width, height = globalRect.Height;

	// Check if the colors have alpha, if so we'll use ARGB4, otherwise RGB565
	ft_bool_t alpha = topLeft < 0xFF000000 || topRight < 0xFF000000 || bottomLeft < 0xFF000000 || bottomRight < 0xFF000000;
	ft_uint16_t colors[4];
	if (alpha)
	{
		colors[0] = ESD_COLOR_ARGB4(topLeft);
		colors[1] = ESD_COLOR_ARGB4(topRight);
		colors[2] = ESD_COLOR_ARGB4(bottomLeft);
		colors[3] = ESD_COLOR_ARGB4(bottomRight);
	}
	else
	{
		colors[0] = ESD_COLOR_RGB565(topLeft);
		colors[1] = ESD_COLOR_RGB565(topRight);
		colors[2] = ESD_COLOR_RGB565(bottomLeft);
		colors[3] = ESD_COLOR_RGB565(bottomRight);
	}

	// Write gradient palette to RAM_G
	Ft_Gpu_CoCmd_MemWrite(Ft_Esd_Host, addr, 8);
	Eve_CoCmd_SendCmdArr(Ft_Esd_Host, (ft_uint32_t *)colors, 2);

	// Set required state
	Esd_Dl_COLOR_ARGB(ESD_ARGB_WHITE);

	// Use the scratch handle
	Esd_Dl_BITMAP_HANDLE(ESD_CO_SCRATCH_HANDLE);
#if (EVE_MODEL >= EVE_FT810)
	Esd_Dl_VERTEX_FORMAT(0);
#endif
	Esd_Dl_BEGIN(BITMAPS);

	// Use local rendering context, bypass ESD display list functions.
	// This is useful here, since we're changing bitmap transform matrices, which may use a lot of display list entries.
	Eve_CoCmd_SendCmd(Ft_Esd_Host, SAVE_CONTEXT());

	// Initialize the bitmap options
#if (EVE_MODEL >= EVE_FT810)
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_LAYOUT_H(0, 0));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SIZE_H(0, 0));
#endif
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_LAYOUT(alpha ? ARGB4 : RGB565, 4, 2));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SIZE(BILINEAR, REPEAT, REPEAT, width, height));

	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_SOURCE(addr));

	// Set the scaling matrix
#if ESD_MULTIGRADIENT_CO_SCALE
	Ft_Gpu_CoCmd_LoadIdentity(Ft_Esd_Host);
	Ft_Gpu_CoCmd_Scale(Ft_Esd_Host, width << 16, height << 16);
	Ft_Gpu_CoCmd_SetMatrix(Ft_Esd_Host);
#else
#if (EVE_MODEL >= EVE_FT810)
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_A_EXT(1, 0x8000 / width));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_E_EXT(1, 0x8000 / height));
#else
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_A(0x0800 / width));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BITMAP_TRANSFORM_E(0x0800 / height));
#endif
#endif

#if (EVE_MODEL >= EVE_FT810)
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));
#else
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2II(x, y, ESD_CO_SCRATCH_HANDLE, 0));
#endif

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
#if ESD_MULTIGRADIENT_CO_SCALE
	Ft_Gpu_CoCmd_LoadIdentity(Ft_Esd_Host);
#endif
	Eve_CoCmd_SendCmd(Ft_Esd_Host, RESTORE_CONTEXT());
	Esd_Dl_END();

	// Move to the next cell in the bitmap for next gradient
	++s_MultiGradient_Cell;
	s_MultiGradient_Cell &= (ESD_MULTIGRADIENT_MAX_NB - 1);
}

/* end of file */
