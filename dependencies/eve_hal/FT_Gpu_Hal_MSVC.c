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

#include "FT_Platform.h"
#if defined(MSVC_PLATFORM)

#include "FT_Gpu_Hal.h"

#ifdef FT4222_PLATFORM
#include "LibFT4222.h"
#endif

#ifdef MPSSE_PLATFORM
#include "libMPSSE_spi.h"
#endif

#if defined(FT4222_PLATFORM)
#endif
/*==========================================================================
*
*                      FOR MSVC_PLATFORM PLATFORMS ONLY
*
*=========================================================================*/

#if defined(FT4222_PLATFORM)
/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY READ command followed
*                             by read of data bytes from GPU
* Implementation           : Using FT4222_SPIMaster_SingleRead,
*                             FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite
*
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
#if 0
ft_uint8_t Ft_Gpu_Hal_FT4222_Rd(EVE_HalContext *phost, ft_uint32_t hrdcmd, ft_uint8_t *rdbufptr, ft_uint32_t exprdbytes)
{
	FT4222_STATUS status;
	ft_uint8_t hrdpkt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; //3 byte addr + 2 or1 byte dummy
	ft_uint8_t retcode = 1; /* assume successful operation */
	ft_uint32_t bytesPerRead;

	if (rdbufptr == NULL || exprdbytes == 0)
		retcode = 0; //fail

	if (retcode)
	{
		if (phost->SpiChannels == FT_GPU_SPI_SINGLE_CHANNEL)
		{
			ft_uint16_t sizeTransferred;
			/* Compose the HOST MEMORY READ packet */
			hrdpkt[0] = (ft_uint8_t)(hrdcmd >> 16) & 0xFF;
			hrdpkt[1] = (ft_uint8_t)(hrdcmd >> 8) & 0xFF;
			hrdpkt[2] = (ft_uint8_t)(hrdcmd & 0xFF);

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    hrdpkt,
			    3 + phost->SpiDummyBytes, /* 3 address and chosen dummy bytes */
			    &sizeTransferred,
			    FALSE /* continue transaction */
			);
			if ((FT4222_OK != status) || (sizeTransferred != (3 + phost->SpiDummyBytes)))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", sizeTransferred, status);
				retcode = 0;
				if (sizeTransferred != (3 + phost->SpiDummyBytes))
					phost->Status = FT_GPU_HAL_STATUS_ERROR;
			}
			else
			{
				/* continue reading data bytes only if HOST MEMORY READ command sent successfully */
				if (rdbufptr != NULL)
				{
					BOOL disable_cs = FALSE; //assume multi SPI read calls
					bytesPerRead = exprdbytes;

					while (retcode && exprdbytes)
					{
						if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH)
						{
							bytesPerRead = exprdbytes;
							disable_cs = TRUE; //1 iteration of SPI read adequate
						}
						else
						{
							bytesPerRead = FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH;
							disable_cs = FALSE;
						}

						status = FT4222_SPIMaster_SingleRead(
						    phost->SpiHandle,
						    rdbufptr,
						    bytesPerRead,
						    &sizeTransferred,
						    disable_cs);
						if ((FT4222_OK != status) || (sizeTransferred != bytesPerRead))
						{
							eve_printf_debug("FT4222_SPIMaster_SingleRead failed,sizeTransferred is %d with status %d\n", sizeTransferred, status);
							retcode = 0;
							if (sizeTransferred != bytesPerRead)
								phost->Status = FT_GPU_HAL_STATUS_ERROR;
						}

						//multiple iterations of SPI read needed
						bytesPerRead = sizeTransferred;

						exprdbytes -= bytesPerRead;
						rdbufptr += bytesPerRead;
					}
				}
			}
		}
		else
		{
			/* Multi channel SPI communication */
			ft_uint32_t t_hrdcmd = hrdcmd;
			ft_uint32_t read_data_index = 0;
			ft_uint32_t sizeTransferred = 0;

			while (retcode && exprdbytes)
			{
				/* Compose the HOST MEMORY READ ADDR packet */
				hrdpkt[0] = (ft_uint8_t)(t_hrdcmd >> 16) & 0xFF;
				hrdpkt[1] = (ft_uint8_t)(t_hrdcmd >> 8) & 0xFF;
				hrdpkt[2] = (ft_uint8_t)(t_hrdcmd & 0xff);

				if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH)
					bytesPerRead = exprdbytes;
				else
					bytesPerRead = FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH;

				status = FT4222_SPIMaster_MultiReadWrite(
				    phost->SpiHandle,
				    rdbufptr + read_data_index,
				    hrdpkt,
				    0,
				    3 + phost->SpiDummyBytes, // 3 addr + dummy bytes
				    bytesPerRead,
				    &sizeTransferred);
				if ((FT4222_OK != status) || (sizeTransferred != bytesPerRead))
				{
					eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, sizeTransferred is %d with status %d\n", sizeTransferred, status);
					retcode = 0;
					if (sizeTransferred != bytesPerRead)
						phost->Status = FT_GPU_HAL_STATUS_ERROR;
				}

				//its multi SPI read calls
				bytesPerRead = sizeTransferred;

				exprdbytes -= bytesPerRead;
				read_data_index += bytesPerRead;
				t_hrdcmd += bytesPerRead;
			}
		}
	}
	return retcode;
}
#endif
#if 0
uint8_t tempptr[64 * 1024];

/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY WRITE command
*
* Implementation           : Uisng FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite
*
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
ft_uint8_t Ft_Gpu_Hal_FT4222_Wr(EVE_HalContext *phost, ft_uint32_t hwraddr, const ft_uint8_t *wrbufptr, ft_uint32_t bytestowr)
{

	FT4222_STATUS status;
	uint8_t *temp_wrpktptr;
	uint16_t per_write = 0;
	BOOL disable_cs = FALSE; //assume multi SPI write calls
	uint8_t dummyRead;
	uint8_t retcode = 1; /* assume successful operation */

	if (wrbufptr == NULL || bytestowr == 0)
		retcode = 0;

	if (retcode)
	{
		uint16_t sizeTransferred = 0;

		temp_wrpktptr = tempptr; //global phost write buffer of size FT4222_MAX_BYTES_PER_CALL

		if (phost->SpiChannels == FT_GPU_SPI_SINGLE_CHANNEL)
		{
			*(temp_wrpktptr + 0) = (hwraddr >> 16) | 0x80; //MSB bits 10 for WRITE
			*(temp_wrpktptr + 1) = (hwraddr >> 8) & 0xFF;
			*(temp_wrpktptr + 2) = hwraddr & 0xff;

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    temp_wrpktptr,
			    3, //3 address bytes
			    &sizeTransferred,
			    FALSE);

			if ((FT4222_OK != status) || (sizeTransferred != 3))
			{
				eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
				retcode = 0;
			}

			if (retcode)
			{
				while (retcode && bytestowr)
				{
					if (bytestowr <= FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH)
					{
						per_write = bytestowr;
						disable_cs = TRUE;
					}
					else
					{
						per_write = FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH;
						disable_cs = FALSE;
					}

					status = FT4222_SPIMaster_SingleWrite(
					    phost->SpiHandle,
					    (uint8_t *)wrbufptr,
					    per_write,
					    &sizeTransferred,
					    disable_cs);

					if ((FT4222_OK != status) || ((ft_uint16_t)sizeTransferred != per_write))
					{
						eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
						retcode = 0;
						if (sizeTransferred != per_write)
							phost->Status = FT_GPU_HAL_STATUS_ERROR;
					}

					//continue writing more bytes
					per_write = sizeTransferred;
					wrbufptr += per_write;
					bytestowr -= per_write;
				}
			}
		}
		else
		{
			ft_uint32_t sizeTransferred = 0;
			//multi channel SPI communication
			while (bytestowr && retcode)
			{
				*(temp_wrpktptr + 0) = (hwraddr >> 16) | 0x80; //MSB bits 10 for WRITE
				*(temp_wrpktptr + 1) = (hwraddr >> 8) & 0xFF;
				*(temp_wrpktptr + 2) = hwraddr & 0xff;

				if (bytestowr <= FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH) //3 for address
					per_write = bytestowr;
				else
					per_write = FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH;

				memcpy((temp_wrpktptr + 3), wrbufptr, per_write);

				status = FT4222_SPIMaster_MultiReadWrite(
				    phost->SpiHandle,
				    &dummyRead,
				    temp_wrpktptr,
				    0,
				    per_write + 3, // 3 byte of mem address
				    0,
				    &sizeTransferred);
				if (FT4222_OK != status)
				{
					eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, status %d\n", status);
					retcode = 0;
					phost->Status = FT_GPU_HAL_STATUS_ERROR;
				}

				hwraddr += per_write;
				bytestowr -= per_write;
				wrbufptr += per_write;
			}
		}
	}
	return retcode;
}
#endif

#endif //FT4222_PLATFORM

#endif

/* end of file */