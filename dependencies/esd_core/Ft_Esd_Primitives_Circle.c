
#include "Ft_Esd_Core.h"
#include "FT_Esd_Primitives.h"

ft_void_t Esd_Render_Circle_Stroke(ft_int32_f4_t x, ft_int32_f4_t y, ft_int32_f4_t radius, ft_int32_f4_t width, ft_argb32_t color)
{
	ft_int32_f4_t innerRadius = radius - (width >> 1);
	ft_int32_f4_t outerRadius = innerRadius + width;

	// Use local rendering context, bypass ESD display list functions.
	Esd_Dl_BEGIN(POINTS);
	Esd_Dl_COLOR_ARGB(color);
#if (EVE_MODEL >= EVE_FT810)
	Esd_Dl_VERTEX_FORMAT(4);
#endif
	Eve_CoCmd_SendCmd(Ft_Esd_Host, SAVE_CONTEXT());

	// Inner alpha quantity
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_MASK(0, 0, 0, 1));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(ONE, ZERO));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, POINT_SIZE(innerRadius + 32));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));

	// Inner alpha edge mask
	Eve_CoCmd_SendCmd(Ft_Esd_Host, STENCIL_FUNC(ALWAYS, 1, 1));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, STENCIL_OP(REPLACE, REPLACE));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(ZERO, ONE_MINUS_SRC_ALPHA));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_A(255));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, POINT_SIZE(innerRadius));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));

	// Inner color, outer circle stencil mask
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_MASK(1, 1, 1, 1));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(DST_ALPHA, ONE_MINUS_DST_ALPHA));
	// Eve_CoCmd_SendCmd(Ft_Esd_Host, POINT_SIZE(innerRadius));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));

	// Outer circle
	Eve_CoCmd_SendCmd(Ft_Esd_Host, STENCIL_FUNC(NOTEQUAL, 1, 255));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, BLEND_FUNC(SRC_ALPHA, ONE_MINUS_SRC_ALPHA));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, COLOR_A(ESD_DECOMPOSE_ALPHA(color)));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, POINT_SIZE(outerRadius));
	Eve_CoCmd_SendCmd(Ft_Esd_Host, VERTEX2F(x, y));

	// Restore rendering context, ESD display list optimizations functions should be used again after this.
	Eve_CoCmd_SendCmd(Ft_Esd_Host, RESTORE_CONTEXT());
	Esd_Dl_END();
}

/* end of file */
