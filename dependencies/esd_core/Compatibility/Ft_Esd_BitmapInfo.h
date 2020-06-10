/*
Bitmap info structure
*/

#ifndef FT_ESD_COMPATIBILITY_BITMAPINFO_H
#define FT_ESD_COMPATIBILITY_BITMAPINFO_H

#include "ESD_Core.h"

#include "Ft_Esd.h"
#include "Ft_Esd_GpuAlloc.h"
#include "Ft_Esd_ResourceInfo.h"

ESD_TYPE(Ft_Esd_BitmapInfo, Native = Struct) // TODO: Struct support, expose values
ESD_TYPE(Ft_Esd_BitmapInfo *, Native = Pointer, Edit = Library)
#define Ft_Esd_BitmapInfo ESD_BitmapInfo

ESD_TYPE(Ft_Esd_BitmapCell, Native = Struct, Edit = Library) // TODO: Struct support, expose values
ESD_TYPE(Ft_Esd_BitmapCell *, Native = Pointer, Edit = Library)
#define Ft_Esd_BitmapCell ESD_BitmapCell

/// A function to load bitmap data(not including palette data) into RAM_G
ESD_FUNCTION(Ft_Esd_LoadBitmap, Type = ft_uint32_t, Include = "Ft_Esd_BitmapInfo.h", DisplayName = "Load Bitmap to RAM_G", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_LoadBitmap ESD_LoadBitmap

/// A function to load palette data of bitmap into RAM_G
ESD_FUNCTION(Ft_Esd_LoadPalette, Type = ft_uint32_t, Include = "Ft_Esd_BitmapInfo.h", DisplayName = "Load Palette to RAM_G", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_LoadPalette ESD_LoadPalette

///  Switch bitmap cell number
ESD_FUNCTION(Ft_Esd_SwitchBitmapCell, Type = Ft_Esd_BitmapCell, DisplayName = "Switch Bitmap Cell", Category = EsdUtilities, Macro)
ESD_PARAMETER(BitmapInfo, Type = Ft_Esd_BitmapCell, DisplayName = "Bitmap Cell")
ESD_PARAMETER(Cell, Type = ft_uint16_t, DisplayName = "Cell")
#define Ft_Esd_SwitchBitmapCell ESD_BitmapCell_Switched

/// A function to make bitmap persistent in memory by reloading the data if necessary, called during the Update cycle of each frame
ESD_UPDATE(Ft_Esd_BitmapPersist, DisplayName = "Persist Bitmap", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell)
#define Ft_Esd_BitmapPersist ESD_BitmapCell_Persist

/// A function to get bitmap information structure from bitmap cell
ESD_FUNCTION(Ft_Esd_BitmapCell_GetInfo, Type = Ft_Esd_BitmapInfo *, DisplayName = "ESD BitmapCell GetInfo", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapCell, Type = Ft_Esd_BitmapCell)
#define Ft_Esd_BitmapCell_GetInfo ESD_BitmapCell_GetInfo

/// A function to get the bitmap width from bitmap information. Returns width of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Ft_Esd_BitmapInfo_GetWidth, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetWidth", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_BitmapInfo_GetWidth ESD_BitmapInfo_GetWidth

/// A function to get the bitmap height from bitmap information. Returns height of 1 pixel for NULL bitmapInfo
ESD_FUNCTION(Ft_Esd_BitmapInfo_GetHeight, Type = ft_int32_t, DisplayName = "ESD BitmapInfo GetHeight", Category = EsdUtilities, Macro)
ESD_PARAMETER(bitmapInfo, Type = Ft_Esd_BitmapInfo *)
#define Ft_Esd_BitmapInfo_GetHeight ESD_BitmapInfo_GetHeight

#endif /* #ifndef FT_ESD_BITMAPINFO_H */

/* end of file */
