#include "Ft_Esd_CoCmd.h"
#include "FT_Esd_Dl.h"
#include "Ft_Esd_Core.h"

ft_void_t Ft_Esd_CoCmd_Spinner(void *owner, ft_int16_t x, ft_int16_t y, ft_uint16_t style, ft_uint16_t scale)
{
	Esd_CurrentContext->CmdOwner = owner;
	Ft_Esd_Dl_VERTEX_FORMAT(4);
	Ft_Gpu_CoCmd_Spinner(Ft_Esd_Host, x, y, style, scale);
}

ft_void_t Ft_Esd_CoCmd_ScreenSaver(void *owner)
{
	Esd_CurrentContext->CmdOwner = owner;
	Ft_Gpu_CoCmd_ScreenSaver(Ft_Esd_Host);
}

ft_void_t Ft_Esd_CoCmd_Sketch(void *owner, Ft_Esd_Rect16 globalRect, ft_uint32_t addr, ft_uint16_t format)
{
	Esd_CurrentContext->CmdOwner = owner;
	Ft_Gpu_CoCmd_Sketch(Ft_Esd_Host, globalRect.X, globalRect.Y, globalRect.Width, globalRect.Height, addr, format);
}

ft_void_t Ft_Esd_CoCmd_Stop(void *owner)
{
	if ((owner == Esd_CurrentContext->CmdOwner) || !owner)
	{
		Ft_Gpu_CoCmd_Stop(Ft_Esd_Host);
		Esd_CurrentContext->CmdOwner = NULL;
	}
}

/* end of file */
