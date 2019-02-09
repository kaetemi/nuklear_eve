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
*
* File Description:
*    This file defines the generic APIs of phost access layer for the FT800 or EVE compatible silicon.
*    Application shall access FT800 or EVE resources over these APIs,regardless of I2C or SPI protocol.
*    I2C and SPI is selected by compiler switch "FT_I2C_MODE"  and "FT_SPI_MODE". In addition, there are
*    some helper functions defined for FT800 coprocessor engine as well as phost commands.
*
*/
#ifndef FT_GPU_HAL_MSVC_H
#define FT_GPU_HAL_MSVC_H
#include "FT_Gpu_Hal.h"

#if defined(MSVC_PLATFORM)

#ifdef FT4222_PLATFORM
#include "LibFT4222.h"
#endif

#ifdef MPSSE_PLATFORM
#include "libMPSSE_spi.h"
#endif

#if defined(FT4222_PLATFORM)
#define FT4222_DYNAMIC_ALLOCATE_SIZE 65535 //Temporary context buffer used only for Ft4222 write. Size limited because of uint16 bytestowrite parameter

#define FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 //3 bytes for FT81x memory address to which data to be written

#define FT4222_ReadTimeout 5000
#define FT4222_WriteTimeout 5000

#define FT4222_LatencyTime 2

#elif defined(MPSSE_PLATFORM)
#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 //3 bytes for FT81x memory address to which data to be written

#endif

#if defined(FT4222_PLATFORM)
//extern struct Ft_Gpu_Hal_Context_t;
//ft_bool_t	  FT4222Drv_Open(EVE_HalContext *phost);
ft_bool_t Ft_Gpu_Hal_FT4222_ComputeCLK(EVE_HalContext *phost, FT4222_ClockRate *sysclk, FT4222_SPIClock *divisor);
ft_uint8_t Ft_Gpu_Hal_FT4222_Rd(EVE_HalContext *phost, ft_uint32_t hrdcmd, ft_uint8_t *rdbufptr, ft_uint32_t exprdbytes);
ft_uint8_t Ft_Gpu_Hal_FT4222_Wr(EVE_HalContext *phost, ft_uint32_t hwraddr, const ft_uint8_t *wrbufptr, ft_uint32_t bytestowr);
#endif
#if defined(MPSSE_PLATFORM)
//uint8_t    Gpu_Hal_LIBMPSSE_Rd(Gpu_Hal_Context_t *phost, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered);
//uint8_t    Gpu_Hal_LIBMPSSE_Wr(Gpu_Hal_Context_t *phost, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered);
#endif

#endif
#endif /* FT_GPU_HAL_MSVC_H */
