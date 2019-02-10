/**
* This source code ("the Software") is provided by Bridgetek Pte Ltd
* ("Bridgetek") subject to the licence terms set out
*   http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
* You must read the Licence Terms before downloading or using the Software.
* By installing or using the Software you agree to the Licence Terms. If you
* do not agree to the Licence Terms then do not download or use the Software.
*
* Without prejudice to the Licence Terms, here is a summary of some of the key
* terms of the Licence Terms (and in the event of any conflict between this
* summary and the Licence Terms then the text of the Licence Terms will
* prevail).
*
* The Software is provided "as is".
* There are no warranties (or similar) in relation to the quality of the
* Software. You use it at your own risk.
* The Software should not be used in, or for, any medical device, system or
* appliance. There are exclusions of Bridgetek liability for certain types of loss
* such as: special loss or damage; incidental loss or damage; indirect or
* consequential loss or damage; loss of income; loss of business; loss of
* profits; loss of revenue; loss of contracts; business interruption; loss of
* the use of money or anticipated savings; loss of information; loss of
* opportunity; loss of goodwill or reputation; and/or loss of, damage to or
* corruption of data.
* There is a monetary cap on Bridgetek's liability.
* The Software may have subsequently been amended by another user and then
* distributed by that other user ("Adapted Software").  If so that user may
* have additional licence terms that apply to those amendments. However, Bridgetek
* has no liability in relation to those amendments.
*/

#ifndef EVE_GPU_TYPES__H
#define EVE_GPU_TYPES__H
#include "EVE_Config.h"

#define EVE_GPU_NUMCHAR_PERFONT (128)
#define EVE_GPU_FONT_TABLE_SIZE (148)

/* FT81x and FT80x font table structure */
/* Font table address in ROM can be found by reading the address from 0xFFFFC location. */
/* 16 font tables are present at the address read from location 0xFFFFC */
typedef struct EVE_Gpu_Fonts
{
	/* All the values are in bytes */
	/* Width of each character font from 0 to 127 */
	uint8_t FontWidth[EVE_GPU_NUMCHAR_PERFONT];
	/* Bitmap format of font wrt bitmap formats supported by FT800 - L1, L4, L8 */
	uint32_t FontBitmapFormat;
	/* Font line stride in FT800 ROM */
	uint32_t FontLineStride;
	/* Font width in pixels */
	uint32_t FontWidthInPixels;
	/* Font height in pixels */
	uint32_t FontHeightInPixels;
	/* Pointer to font graphics raw data */
	uint32_t PointerToFontGraphicsData;
} EVE_Gpu_Fonts;

#if defined(EVE_SUPPORT_UNICODE)

/* BT815 inclusion - extended font table for unicode support */
typedef struct EVE_Gpu_FontsExt
{
	uint32_t Signature;
	uint32_t Size;
	uint32_t Format;
	uint32_t Swizzle;
	uint32_t LayoutWidth;
	uint32_t LayoutHeight;
	uint32_t PixelWidth;
	uint32_t PixelHeight;
	uint32_t StartOfGraphicData;
	uint32_t OffsetGlyphData;
	// uint32_t OffsetWidthData; // to be OffsetWidthData = Starting adress + StartOfGraphicData * 4.
} EVE_Gpu_FontsExt;

#endif

#endif /* #ifndef EVE_GPU_TYPES__H */

/* end of file */