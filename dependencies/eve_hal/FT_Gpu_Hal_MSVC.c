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

#include "FT_Gpu_Hal_MSVC.h"

#if defined(FT4222_PLATFORM)
ft_bool_t FT4222Drv_Open(EVE_HalContext *phost)
{
	FT_STATUS status;
	//ulong_t numdevs;
	ft_uint32_t numdevs;
	ft_uint32_t index;
	FT_HANDLE fthandle;
	FT4222_Version pversion;
	FT4222_ClockRate ftclk = 0;
	ft_uint16_t max_size = 0;
	FT4222_ClockRate selclk = 0;
	FT4222_SPIClock seldiv = 0;
	/* GPIO0         , GPIO1      , GPIO2       , GPIO3         } */
	GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT, GPIO_INPUT, GPIO_INPUT, GPIO_INPUT };

	ft_bool_t ret = FT_TRUE;

	phost->SpiHandle = phost->GpioHandle = NULL;

	status = FT_CreateDeviceInfoList(&numdevs);
	if (FT_OK != status)
	{
		eve_printf_debug("FT_CreateDeviceInfoList failed");
		ret = FT_FALSE;
	}

	status = FT_ListDevices(&numdevs, NULL, FT_LIST_NUMBER_ONLY);
	if (FT_OK != status)
	{
		eve_printf_debug("FT_ListDevices failed");
		ret = FT_FALSE;
	}

	if (ret)
	{
		for (index = 0; (index < numdevs) && ret; index++)
		{
			FT_DEVICE_LIST_INFO_NODE devInfo;
			memset(&devInfo, 0, sizeof(devInfo));

			status = FT_GetDeviceInfoDetail(index,
			    &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
			    devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle);
			if (FT_OK == status)
			{
				eve_printf_debug("Dev %d:\n", index);
				eve_printf_debug(" Flags= 0x%x, (%s) (%s)\n", devInfo.Flags,
				    ((devInfo.Flags & 0x01) ? "DEVICE_OPEN" : "DEVICE_CLOSED"), ((devInfo.Flags & 0x02) ? "High-speed USB" : "Full-speed USB"));
				eve_printf_debug(" Type= 0x%x\n", devInfo.Type);
				eve_printf_debug(" ID= 0x%x\n", devInfo.ID);
				eve_printf_debug(" LocId= 0x%x\n", devInfo.LocId);
				eve_printf_debug(" SerialNumber= %s\n", devInfo.SerialNumber);
				eve_printf_debug(" Description= %s\n", devInfo.Description);
				eve_printf_debug(" ftHandle= %p\n", devInfo.ftHandle);
			}
			else
				ret = FT_FALSE;

			if (ret && !(devInfo.Flags & 0x01) && ((!strcmp(devInfo.Description, "FT4222 A") && (phost->SpiHandle == NULL)) || (!strcmp(devInfo.Description, "FT4222 B") && (phost->GpioHandle == NULL))))
			{
				/* obtain handle for the first discovered "FT4222 A" and first "FT4222 B" */
				status = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &fthandle);
				if (status != FT_OK)
				{
					eve_printf_debug("FT_OpenEx failed %d\n", status);
					ret = FT_FALSE;
				}
				else
				{
					if (!strcmp(devInfo.Description, "FT4222 A"))
					{
						//is SPI
						phost->SpiHandle = fthandle; //SPI communication handle
						eve_printf_debug("[%d]th of total connected devices is FT4222 A (SPI) : phost->hal_hanlde = %p\n", index + 1, phost->SpiHandle);
					}
					else if (!strcmp(devInfo.Description, "FT4222 B"))
					{
						//is GPIO
						phost->GpioHandle = fthandle; //GPIO communication handle
						eve_printf_debug("[%d]th of total connected devices is FT4222 B (GPIO) : phost->hal_hanlde = %p\n", index + 1, phost->SpiHandle);
					}
					else
					{
						eve_printf_debug("Error in FT4222 configuration\n");
					}
				}
			}
			else
			{
				if (
				    (!strcmp(devInfo.Description, "FT4222 A") && phost->SpiHandle != NULL) || (!strcmp(devInfo.Description, "FT4222 B") && phost->GpioHandle != NULL))
					eve_printf_debug("[%d]th of total connected devices is not the first %s detected. Hence skipping.\n", index + 1, devInfo.Description);
				else if (devInfo.Flags & 0x01)
					eve_printf_debug("[%d]th of total connected devices is already open in another context. Hence skipping.\n", index + 1);
				else
					eve_printf_debug("[%d]th of total connected devices is not FT4222 but is %s. Hence skipping.\n", index + 1, devInfo.Description);
				continue;
			}
		}
	}

	if (ret)
	{
		status = FT4222_GetVersion(phost->SpiHandle, &pversion);
		if (status != FT4222_OK)
			eve_printf_debug("FT4222_GetVersion failed\n");
		else
			eve_printf_debug("SPI:chipversion = 0x%x\t dllversion = 0x%x\n", pversion.chipVersion, pversion.dllVersion);
	}

	if (ret)
	{
		//Set default Read timeout 5s and Write timeout 5sec
		status = FT_SetTimeouts(phost->SpiHandle, FT4222_ReadTimeout, FT4222_WriteTimeout);
		if (FT_OK != status)
		{
			eve_printf_debug("FT_SetTimeouts failed!\n");
			ret = FT_FALSE;
		}
	}

	if (ret)
	{
		// no latency to usb
		status = FT_SetLatencyTimer(phost->SpiHandle, FT4222_LatencyTime);
		if (FT_OK != status)
		{
			eve_printf_debug("FT_SetLatencyTimerfailed!\n");
			ret = FT_FALSE;
		}
	}

	if (ret)
	{
		if (!Ft_Gpu_Hal_FT4222_ComputeCLK(phost, &selclk, &seldiv))
		{
			eve_printf_debug("Requested clock %d KHz is not supported in FT4222\n", phost->HalConfig.spi_clockrate_khz);
			ret = FT_FALSE;
		}
	}

	if (ret)
	{
		status = FT4222_SetClock(phost->SpiHandle, selclk);
		if (FT_OK != status)
		{
			eve_printf_debug("FT4222_SetClock failed!\n");
			ret = FT_FALSE;
		}

		status = FT4222_GetClock(phost->SpiHandle, &ftclk);

		if (FT_OK != status)
			eve_printf_debug("FT4222_SetClock failed\n");
		else
			eve_printf_debug("FT4222 clk = %d\n", ftclk);
	}

	if (ret)
	{
		/* Interface 1 is SPI master */
		status = FT4222_SPIMaster_Init(
		    phost->SpiHandle,
		    SPI_IO_SINGLE,
		    seldiv,
		    CLK_IDLE_LOW, //,CLK_IDLE_HIGH
		    CLK_LEADING, // CLK_LEADING CLK_TRAILING
		    phost->HalConfig.spi_cs_pin_no); /* slave selection output pins */
		if (FT_OK != status)
		{
			eve_printf_debug("Init FT4222 as SPI master device failed!\n");
			ret = FT_FALSE;
		}
		else
			phost->SpiChannel = FT_GPU_SPI_SINGLE_CHANNEL; //SPI_IO_SINGLE;

		status = FT4222_SPI_SetDrivingStrength(phost->SpiHandle, DS_4MA, DS_4MA, DS_4MA);
		if (FT4222_OK != status)
			eve_printf_debug("FT4222_SPI_SetDrivingStrength failed!\n");

		Ft_Gpu_Hal_Sleep(20);

		status = FT4222_SetSuspendOut(phost->GpioHandle, FT_FALSE);
		if (FT_OK != status)
		{
			eve_printf_debug("Disable suspend out function on GPIO2 failed!\n");
			ret = FT_FALSE;
		}

		status = FT4222_SetWakeUpInterrupt(phost->GpioHandle, FT_FALSE);
		if (FT_OK != status)
		{
			eve_printf_debug("Disable wakeup/interrupt feature on GPIO3 failed!\n");
			ret = FT_FALSE;
		}
		/* Interface 2 is GPIO */
		status = FT4222_GPIO_Init(phost->GpioHandle, gpio_dir);
		if (FT_OK != status)
		{
			eve_printf_debug("Init FT4222 as GPIO interface failed!\n");
			ret = FT_FALSE;
		}
	}

	/* dedicated write buffer used for SPI write. Max size is 2^uint16 */
	if ((phost->SpiWriBufPtr = malloc(FT4222_DYNAMIC_ALLOCATE_SIZE)) == NULL)
	{
		eve_printf_debug("malloc error\n");
		ret = FT_FALSE;
	}

	return ret;
}
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
ft_uint8_t Ft_Gpu_Hal_FT4222_Rd(EVE_HalContext *phost, ft_uint32_t hrdcmd, ft_uint8_t *rdbufptr, ft_uint32_t exprdbytes)
{
	FT4222_STATUS status;
	ft_uint8_t hrdpkt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; //3 byte addr + 2 or1 byte dummy
	ft_uint8_t retcode = 1; /* assume successful operation */
	ft_uint32_t bytes_per_read;

	if (rdbufptr == NULL || exprdbytes == 0)
		retcode = 0; //fail

	if (retcode)
	{
		if (phost->SpiChannel == FT_GPU_SPI_SINGLE_CHANNEL)
		{
			ft_uint16_t SizeTransfered;
			/* Compose the HOST MEMORY READ packet */
			hrdpkt[0] = (ft_uint8_t)(hrdcmd >> 16) & 0xFF;
			hrdpkt[1] = (ft_uint8_t)(hrdcmd >> 8) & 0xFF;
			hrdpkt[2] = (ft_uint8_t)(hrdcmd & 0xFF);

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    hrdpkt,
			    3 + phost->SpiNumDummy, /* 3 address and chosen dummy bytes */
			    (ft_uint16_t *)&SizeTransfered,
			    FALSE /* continue transaction */
			);
			if ((FT4222_OK != status) || ((ft_uint16_t)SizeTransfered != (3 + phost->SpiNumDummy)))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", (ft_uint16_t)SizeTransfered, status);
				retcode = 0;
				if ((ft_uint16_t)SizeTransfered != (3 + phost->SpiNumDummy))
					phost->Status = FT_GPU_HAL_STATUS_ERROR;
			}
			else
			{
				/* continue reading data bytes only if HOST MEMORY READ command sent successfully */
				if (rdbufptr != NULL)
				{
					BOOL disable_cs = FALSE; //assume multi SPI read calls
					bytes_per_read = exprdbytes;

					while (retcode && exprdbytes)
					{
						if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH)
						{
							bytes_per_read = exprdbytes;
							disable_cs = TRUE; //1 iteration of SPI read adequate
						}
						else
						{
							bytes_per_read = FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH;
							disable_cs = FALSE;
						}

						status = FT4222_SPIMaster_SingleRead(
						    phost->SpiHandle,
						    rdbufptr,
						    bytes_per_read,
						    (ft_uint16_t *)&SizeTransfered,
						    disable_cs);
						if ((FT4222_OK != status) || ((ft_uint16_t)SizeTransfered != bytes_per_read))
						{
							eve_printf_debug("FT4222_SPIMaster_SingleRead failed,SizeTransfered is %d with status %d\n", (ft_uint16_t)SizeTransfered, status);
							retcode = 0;
							if ((ft_uint16_t)SizeTransfered != bytes_per_read)
								phost->Status = FT_GPU_HAL_STATUS_ERROR;
						}

						//multiple iterations of SPI read needed
						bytes_per_read = (ft_uint16_t)SizeTransfered;

						exprdbytes -= bytes_per_read;
						rdbufptr += bytes_per_read;
					}
				}
			}
		}
		else
		{
			/* Multi channel SPI communication */
			ft_uint32_t t_hrdcmd = hrdcmd;
			ft_uint32_t read_data_index = 0;
			ft_uint32_t SizeTransfered = 0;

			while (retcode && exprdbytes)
			{
				/* Compose the HOST MEMORY READ ADDR packet */
				hrdpkt[0] = (ft_uint8_t)(t_hrdcmd >> 16) & 0xFF;
				hrdpkt[1] = (ft_uint8_t)(t_hrdcmd >> 8) & 0xFF;
				hrdpkt[2] = (ft_uint8_t)(t_hrdcmd & 0xff);

				if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH)
					bytes_per_read = exprdbytes;
				else
					bytes_per_read = FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH;

				status = FT4222_SPIMaster_MultiReadWrite(
				    phost->SpiHandle,
				    rdbufptr + read_data_index,
				    hrdpkt,
				    0,
				    3 + phost->SpiNumDummy, // 3 addr + dummy bytes
				    bytes_per_read,
				    &SizeTransfered);
				if ((FT4222_OK != status) || ((ft_uint16_t)SizeTransfered != bytes_per_read))
				{
					eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, SizeTransfered is %d with status %d\n", SizeTransfered, status);
					retcode = 0;
					if ((ft_uint16_t)SizeTransfered != bytes_per_read)
						phost->Status = FT_GPU_HAL_STATUS_ERROR;
				}

				//its multi SPI read calls
				bytes_per_read = (ft_uint16_t)SizeTransfered;

				exprdbytes -= bytes_per_read;
				read_data_index += bytes_per_read;
				t_hrdcmd += bytes_per_read;
			}
		}
	}
	return retcode;
}

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
	ft_uint8_t *temp_wrpktptr;
	ft_uint16_t per_write = 0;
	BOOL disable_cs = FALSE; //assume multi SPI write calls
	ft_uint8_t dummy_read;
	ft_uint8_t retcode = 1; /* assume successful operation */

	if (wrbufptr == NULL || bytestowr == 0)
		retcode = 0;

	if (retcode)
	{
		ft_uint16_t SizeTransfered = 0;

		temp_wrpktptr = phost->SpiWriBufPtr; //global phost write buffer of size FT4222_MAX_BYTES_PER_CALL

		if (phost->SpiChannel == FT_GPU_SPI_SINGLE_CHANNEL)
		{
			*(temp_wrpktptr + 0) = (hwraddr >> 16) | 0x80; //MSB bits 10 for WRITE
			*(temp_wrpktptr + 1) = (hwraddr >> 8) & 0xFF;
			*(temp_wrpktptr + 2) = hwraddr & 0xff;

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    temp_wrpktptr,
			    3, //3 address bytes
			    (ft_uint16_t *)&SizeTransfered,
			    FALSE);

			if ((FT4222_OK != status) || ((ft_uint16_t)SizeTransfered != 3))
			{
				eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, (ft_uint16_t)SizeTransfered, status);
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
					    wrbufptr,
					    per_write,
					    (ft_uint16_t *)&SizeTransfered,
					    disable_cs);

					if ((FT4222_OK != status) || ((ft_uint16_t)SizeTransfered != per_write))
					{
						eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, (ft_uint16_t)SizeTransfered, status);
						retcode = 0;
						if ((ft_uint16_t)SizeTransfered != per_write)
							phost->Status = FT_GPU_HAL_STATUS_ERROR;
					}

					//continue writing more bytes
					per_write = (ft_uint16_t)SizeTransfered;
					wrbufptr += per_write;
					bytestowr -= per_write;
				}
			}
		}
		else
		{
			ft_uint32_t SizeTransfered = 0;
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
				    &dummy_read,
				    temp_wrpktptr,
				    0,
				    per_write + 3, // 3 byte of mem address
				    0,
				    &SizeTransfered);
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

/***************************************************************************
* Interface Description    : Function to compute FT4222 sys clock and divisor
*                            to obtain user requested SPI communication clock
*                            Available FT4222_ClockRate (FT4222 system clock):
*                               SYS_CLK_60,
*                               SYS_CLK_24,
*                               SYS_CLK_48,
*                               SYS_CLK_80
*                            Divisors available (FT4222_SPIClock):
*                               CLK_NONE,
*                               CLK_DIV_2,
*                               CLK_DIV_4,
*                               CLK_DIV_8,
*                               CLK_DIV_16,
*                               CLK_DIV_32,
*                               CLK_DIV_64,
*                               CLK_DIV_128,
*                               CLK_DIV_256,
*                               CLK_DIV_512
* Implementation           : Good performance is observed with divisors other than CLK_DIV_2
*                            and CLK_DIV_4 from test report by firmware developers.
*                            Hence supporting the following clocks for SPI communication
*                               5000KHz
*                               10000KHz
*                               15000KHz
*                               20000KHz
*                               25000KHz
*                               30000KHz
*                            Global variable phost->HalConfig.spi_clockrate_khz is
*                            updated accodingly
* Return Value             : bool_t
*                               TRUE : Supported by FT4222
*                               FALSE : Not supported by FT4222
*
* Author                   :
****************************************************************************/
ft_bool_t Ft_Gpu_Hal_FT4222_ComputeCLK(EVE_HalContext *phost, FT4222_ClockRate *sysclk, FT4222_SPIClock *sysdivisor)
{
	//phost->HalConfig.spi_clockrate_khz is the user requested SPI communication clock

	if (phost->HalConfig.spi_clockrate_khz <= 5000)
	{ //set to 5000 KHz
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_16;
	}
	else if (phost->HalConfig.spi_clockrate_khz > 5000 && phost->HalConfig.spi_clockrate_khz <= 10000)
	{
		//set to 10000 KHz
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_8;
	}
	else if (phost->HalConfig.spi_clockrate_khz > 10000 && phost->HalConfig.spi_clockrate_khz <= 15000)
	{
		//set to 15000 KHz
		*sysclk = SYS_CLK_60;
		*sysdivisor = CLK_DIV_4;
	}
	else
	{
		//set to 20000 KHz : Maximum throughput is obeserved with this clock combination
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_4;
	}
	eve_printf_debug("User Selected SPI clk : %d KHz\n", phost->HalConfig.spi_clockrate_khz);
	eve_printf_debug("Configured clk :  Ft4222 sys clk enum = %d , divisor enum = %d\n", *sysclk, *sysdivisor);
	return (FT_TRUE);
}
#endif //FT4222_PLATFORM

/* API to initialize the SPI interface */
ft_bool_t Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit)
{
	halinit->total_channel_num = 1;
#ifdef MPSSE_PLATFORM
	/* Initialize the libmpsse */
	Init_libMPSSE();
	SPI_GetNumChannels(&halinit->total_channel_num);
#endif

	if (halinit->total_channel_num > 0)
	{
		FT_DEVICE_LIST_INFO_NODE devList;

		memset(&devList, 0, sizeof(devList));
#ifdef MPSSE_PLATFORM
		SPI_GetChannelInfo(0, &devList);
#endif
#ifdef FT4222_PLATFORM
		FT_STATUS status;
		ft_uint32_t numdevs;

		status = FT_CreateDeviceInfoList(&numdevs);
		if (FT_OK == status)
		{
			eve_printf_debug("Number of D2xx devices connected = %d\n", numdevs);
			halinit->total_channel_num = numdevs;

			FT_GetDeviceInfoDetail(0, &devList.Flags, &devList.Type, &devList.ID,
			    &devList.LocId,
			    devList.SerialNumber,
			    devList.Description,
			    &devList.ftHandle);
		}
		else
		{
			eve_printf_debug("FT_CreateDeviceInfoList failed");
			return FT_FALSE;
		}
#endif
		eve_printf_debug("Information on channel number %d:\n", 0);
		/* print the dev info */
		eve_printf_debug(" Flags=0x%x\n", devList.Flags);
		eve_printf_debug(" Type=0x%x\n", devList.Type);
		eve_printf_debug(" ID=0x%x\n", devList.ID);
		eve_printf_debug(" LocId=0x%x\n", devList.LocId);
		eve_printf_debug(" SerialNumber=%s\n", devList.SerialNumber);
		eve_printf_debug(" Description=%s\n", devList.Description);
		eve_printf_debug(" ftHandle=0x%x\n", devList.ftHandle); /*is 0 unless open*/
	}
	return FT_TRUE;
}

ft_void_t Ft_Gpu_Hal_ESD_Idle(EVE_HalContext *phost)
{
}

ft_bool_t Ft_Gpu_Hal_Open(EVE_HalContext *phost)
{
	phost->HalConfig.channel_no = 0;
	phost->HalConfig.pdn_pin_no = FT800_PD_N;
	phost->HalConfig.spi_cs_pin_no = FT800_SEL_PIN;
	phost->HalConfig.spi_clockrate_khz = 12000; //in KHz

#if defined(MSVC_PLATFORM)
	FT_STATUS status;
#ifdef MPSSE_PLATFORM
	ChannelConfig channelConf; //channel configuration

	/* configure the spi settings */
	channelConf.ClockRate = phost->HalConfig.spi_clockrate_khz * 1000;
	channelConf.LatencyTimer = 2;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000; /*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

	/* Open the first available channel */
	status = SPI_OpenChannel(phost->HalConfig.channel_no, (FT_HANDLE *)&phost->hal_handle);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI open channel failed %d %d\n", phost->HalConfig.channel_no, phost->hal_handle);
		return FT_FALSE;
	}
	status = SPI_InitChannel((FT_HANDLE)phost->hal_handle, &channelConf);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI init channel failed %d %d\n", phost->HalConfig.channel_no, phost->hal_handle);
		return FT_FALSE;
	}

	eve_printf_debug("\nhandle=0x%x status=0x%x\n", phost->hal_handle, status);
#endif
#ifdef FT4222_PLATFORM
	FT4222Drv_Open(phost);
#endif
#endif
	/* Initialize the context valriables */
	phost->SpiNumDummy = 1; //by default ft800/801/810/811 goes with single dummy byte for read
	phost->SpiChannel = 0;
	phost->Status = FT_GPU_HAL_OPENED;
	return FT_TRUE;
}
ft_void_t Ft_Gpu_Hal_Close(EVE_HalContext *phost)
{
	phost->Status = FT_GPU_HAL_CLOSED;
#ifdef MSVC_PLATFORM

#ifdef MPSSE_PLATFORM
	/* Close the channel*/
	SPI_CloseChannel(phost->hal_handle);
#elif defined(FT4222_PLATFORM)
	FT4222_STATUS status;
	if (FT4222_OK != (status = FT4222_UnInitialize(phost->SpiHandle)))
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if (FT4222_OK != (status = FT4222_UnInitialize(phost->GpioHandle)))
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if (FT_OK != (status = FT_Close(phost->SpiHandle)))
		eve_printf_debug("CLOSE failed %d\n", status);

	if (FT_OK != (status = FT_Close(phost->GpioHandle)))
		eve_printf_debug("CLOSE failed %d\n", status);
#endif

#endif
}

ft_void_t Ft_Gpu_Hal_DeInit()
{
#ifdef MSVC_PLATFORM

#ifdef MPSSE_PLATFORM
	//Cleanup the MPSSE Lib
	Cleanup_libMPSSE();
#endif

#endif
}

/*The APIs for reading/writing transfer continuously only with small buffer system*/
ft_void_t Ft_Gpu_Hal_StartTransfer(EVE_HalContext *phost, FT_GPU_TRANSFERDIR_T rw, ft_uint32_t addr)
{
	eve_assert(!(phost->CmdFrame && (addr == REG_CMDB_WRITE)));
	if (FT_GPU_READ == rw)
	{
#ifdef MSVC_PLATFORM
		ft_uint8_t Transfer_Array[4];
		ft_uint32_t SizeTransfered;
		ft_uint16_t SizeTransfered16;

		/* Compose the read packet */
		Transfer_Array[0] = addr >> 16;
		Transfer_Array[1] = addr >> 8;
		Transfer_Array[2] = addr;

		Transfer_Array[3] = 0; //Dummy Read byte
#ifdef MPSSE_PLATFORM
		SPI_Write((FT_HANDLE)phost->hal_handle, Transfer_Array, sizeof(Transfer_Array), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
#elif defined(FT4222_PLATFORM)
		// TODO --------------------------------------------------------------------
		eve_printf_debug("Ft_Gpu_Hal_StartTransfer not implemented for FT4222_PLATFORM");
		eve_debug_break();
#endif
#endif
		phost->Status = FT_GPU_HAL_READING;
	}
	else
	{
#ifdef MSVC_PLATFORM
		ft_uint8_t Transfer_Array[3];
		ft_uint32_t SizeTransfered;
		ft_uint16_t SizeTransfered16;

		/* Compose the read packet */
		Transfer_Array[0] = (0x80 | (addr >> 16));
		Transfer_Array[1] = addr >> 8;
		Transfer_Array[2] = addr;
#ifdef MPSSE_PLATFORM
		SPI_Write((FT_HANDLE)phost->hal_handle, Transfer_Array, 3, &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
#elif defined(FT4222_PLATFORM)
		// TODO --------------------------------------------------------------------
		eve_printf_debug("Ft_Gpu_Hal_StartTransfer not implemented for FT4222_PLATFORM");
		eve_debug_break();
#endif
#endif
		phost->Status = FT_GPU_HAL_WRITING;
	}
}

ft_uint8_t Ft_Gpu_Hal_Transfer8(EVE_HalContext *phost, ft_uint8_t value)
{
//#if defined(MSVC_PLATFORM) && !defined(ESD_SIMULATION)
#if defined(MSVC_PLATFORM)
	ft_uint32_t SizeTransfered = 0;
	if (phost->Status == FT_GPU_HAL_WRITING)
	{
#ifdef MPSSE_PLATFORM
		SPI_Write(phost->hal_handle, &value, sizeof(value), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
#elif defined(FT4222_PLATFORM)
		// TODO --------------------------------------------------------------------
		eve_printf_debug("Ft_Gpu_Hal_Transfer8 not implemented for FT4222_PLATFORM");
		eve_debug_break();
#endif
	}
	else
	{
#ifdef MPSSE_PLATFORM
		SPI_Read(phost->hal_handle, &value, sizeof(value), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
#elif defined(FT4222_PLATFORM)
		// TODO --------------------------------------------------------------------
		eve_printf_debug("Ft_Gpu_Hal_Transfer8 not implemented for FT4222_PLATFORM");
		eve_debug_break();
#endif
	}

	if (SizeTransfered != sizeof(value))
		phost->Status = FT_GPU_HAL_STATUS_ERROR;
	return value;
#endif
}

ft_void_t Ft_Gpu_Hal_EndTransfer(EVE_HalContext *phost)
{
#ifdef MSVC_PLATFORM
#ifdef MPSSE_PLATFORM
	//just disbale the CS - send 0 bytes with CS disable
	SPI_ToggleCS((FT_HANDLE)phost->hal_handle, FALSE);
#elif defined(FT4222_PLATFORM)
	// TODO --------------------------------------------------------------------
	eve_printf_debug("Ft_Gpu_Hal_EndTransfer not implemented for FT4222_PLATFORM");
	eve_debug_break();
#endif
#endif
	phost->Status = FT_GPU_HAL_OPENED;
}

ft_uint8_t Ft_Gpu_Hal_Rd8(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint8_t value;
#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Rd(phost, addr, &value, sizeof(value)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Rd failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer8(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
	return value;
}
ft_uint16_t Ft_Gpu_Hal_Rd16(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint16_t value;

#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Rd(phost, addr, &value, sizeof(value)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Rd failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer16(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
	return value;
}
ft_uint32_t Ft_Gpu_Hal_Rd32(EVE_HalContext *phost, ft_uint32_t addr)
{
	ft_uint32_t value;

#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Rd(phost, addr, &value, sizeof(value)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Rd failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer32(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
	return value;
}

ft_void_t Ft_Gpu_Hal_Wr8(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t v)
{
#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Wr(phost, addr, &v, sizeof(v)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Wr failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer8(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
}

ft_void_t Ft_Gpu_Hal_Wr16(EVE_HalContext *phost, ft_uint32_t addr, ft_uint16_t v)
{
#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Wr(phost, addr, &v, sizeof(v)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Wr failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer16(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
}

ft_void_t Ft_Gpu_Hal_Wr32(EVE_HalContext *phost, ft_uint32_t addr, ft_uint32_t v)
{
#if defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Wr(phost, addr, &v, sizeof(v)))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Wr failed\n");
	}
#else
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer32(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
#endif
}

ft_void_t Ft_Gpu_HostCommand(EVE_HalContext *phost, ft_uint8_t cmd)
{
#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
	FT4222_STATUS status;
	ft_uint8_t dummy_read;
#endif
#ifdef MSVC_PLATFORM
	ft_uint8_t Transfer_Array[3];
	ft_uint32_t SizeTransfered;
	ft_uint16_t SizeTransfered16;

	Transfer_Array[0] = cmd;
	Transfer_Array[1] = 0;
	Transfer_Array[2] = 0;
#ifdef MPSSE_PLATFORM
	SPI_Write(phost->hal_handle, Transfer_Array, sizeof(Transfer_Array), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#elif defined(FT4222_PLATFORM)
	switch (phost->SpiChannel)
	{
	case FT_GPU_SPI_SINGLE_CHANNEL:
		/* FYI : All HOST CMDs should only be executed in single channel mode*/
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    Transfer_Array,
		    sizeof(Transfer_Array),
		    (ft_uint16_t *)&SizeTransfered,
		    FT_TRUE);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	case FT_GPU_SPI_DUAL_CHANNEL:
	case FT_GPU_SPI_QUAD_CHANNEL:
		/* only reset command among phost commands can be executed in multi channel mode*/
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummy_read,
		    Transfer_Array,
		    0,
		    sizeof(Transfer_Array),
		    0,
		    &SizeTransfered);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
#endif
#endif
}
//This API sends a 3byte command to the phost
ft_void_t Ft_Gpu_HostCommand_Ext3(EVE_HalContext *phost, ft_uint32_t cmd)
{
#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
	FT4222_STATUS status;
	ft_uint8_t dummy_read;
#endif
#ifdef MSVC_PLATFORM
	ft_uint8_t Transfer_Array[3];
	ft_uint32_t SizeTransfered;
	ft_uint16_t SizeTransfered16;

	Transfer_Array[0] = cmd;
	Transfer_Array[1] = (cmd >> 8) & 0xff;
	Transfer_Array[2] = (cmd >> 16) & 0xff;
#ifdef MPSSE_PLATFORM
	SPI_Write(phost->hal_handle, Transfer_Array, sizeof(Transfer_Array), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#elif defined(FT4222_PLATFORM)

	switch (phost->SpiChannel)
	{
	case FT_GPU_SPI_SINGLE_CHANNEL:
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    Transfer_Array,
		    sizeof(Transfer_Array),
		    (ft_uint16_t *)&SizeTransfered,
		    FT_TRUE);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	case FT_GPU_SPI_DUAL_CHANNEL:
	case FT_GPU_SPI_QUAD_CHANNEL:
		/* FYI : Mostly all HOST CMDs can be executed in single channel mode
		* except system reset cmd */
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummy_read,
		    Transfer_Array,
		    0,
		    sizeof(Transfer_Array),
		    0,
		    &SizeTransfered);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
#endif
#endif
}

ft_bool_t Ft_Gpu_Hal_WrCmdBuf(EVE_HalContext *phost, ft_uint8_t *buffer, ft_uint32_t count)
{
	ft_int32_t length = 0, availablefreesize = 0;

#if defined(FT4222_PLATFORM)
	FT4222_STATUS status;
	ft_uint8_t *wrpktptr;
	ft_uint8_t dummy_read;
	wrpktptr = phost->SpiWriBufPtr; //Using global buf , FT4222_DYNAMIC_ALLOCATE_SIZE
#endif
	count = (count + 3) & ~0x3;
	do
	{
		length = count;
		availablefreesize = Ft_Gpu_Hal_Rd16(phost, REG_CMDB_SPACE) & FIFO_SIZE_MASK; // max: 4095 ; last 4's multiple = 4092
		if (FT_COCMD_FAULT(availablefreesize))
			return FT_FALSE; // Co processor fault
		if (length > availablefreesize)
			length = availablefreesize;
#ifdef MPSSE_PLATFORM
		{
			Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
			ft_uint32_t SizeTransfered32 = 0;
			SPI_Write(phost->hal_handle, buffer, length, &SizeTransfered32, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
			Ft_Gpu_Hal_EndTransfer(phost);
		}
#elif defined(FT4222_PLATFORM)
		{
			ft_uint16_t SizeTransfered = 0;

			*(wrpktptr + 0) = (ft_uint8_t)((REG_CMDB_WRITE >> 16) | 0x80);
			*(wrpktptr + 1) = (ft_uint8_t)((REG_CMDB_WRITE) >> 8);
			*(wrpktptr + 2) = (ft_uint8_t)(REG_CMDB_WRITE & 0xff);
			memcpy((wrpktptr + 3), buffer, length);

			if (phost->SpiChannel == FT_GPU_SPI_SINGLE_CHANNEL)
			{
				status = FT4222_SPIMaster_SingleWrite(
				    phost->SpiHandle,
				    wrpktptr,
				    (length + 3), //3 for address phase of serial protocol
				    (ft_uint16_t *)&SizeTransfered,
				    FT_TRUE);
				if ((FT4222_OK != status) || (SizeTransfered != (length + 3)))
				{
					eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, SizeTransfered, status);
					break;
				}
			}
			else
			{
				ft_uint32_t sizeOfRead = 0;
				/* DUAL and QAUD */
				status = FT4222_SPIMaster_MultiReadWrite(
				    phost->SpiHandle,
				    &dummy_read,
				    wrpktptr,
				    0,
				    (length + 3), //3 for address phase of serial protocol
				    0,
				    &sizeOfRead);
				if (FT4222_OK != status)
				{
					eve_printf_debug("%d FT4222_SPIMaster_MultiReadWrite failed, sizeOfRead is %d with status %d\n", __LINE__, sizeOfRead, status);
					break;
				}
			}
		}
#endif
		//printf("\n Length: %d", length);
		buffer += length;
		count -= length;
	} while (count > 0);
	//printf("\n\n\n");
	return FT_TRUE;
}

ft_bool_t Ft_Gpu_Hal_WrCmdBuf_ProgMem(EVE_HalContext *phost, ft_prog_uchar8_t *buffer, ft_uint32_t count)
{
	return Ft_Gpu_Hal_WrCmdBuf(phost, buffer, count);
}

ft_void_t Ft_Gpu_Hal_WrCmd32(EVE_HalContext *phost, ft_uint32_t cmd)
{
	eve_assert_ex(!phost->CmdFrame, "Did you mean 'Ft_Gpu_CoCmd_SendCmd'?");
	Ft_Gpu_Hal_Wr32(phost, REG_CMDB_WRITE, cmd);
}

/* Toggle PD_N pin of FT800 board for a power cycle*/
ft_void_t Ft_Gpu_Hal_Powercycle(EVE_HalContext *phost, ft_bool_t up)
{
	if (up)
	{
#ifdef MSVC_PLATFORM
#ifdef MPSSE_PLATFORM
		//FT_WriteGPIO(phost->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		FT_WriteGPIO(phost->hal_handle, (1 << phost->HalConfig.pdn_pin_no) | 0x3B, (0 << phost->HalConfig.pdn_pin_no) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board

		Ft_Gpu_Hal_Sleep(20);

		//FT_WriteGPIO(phost->hal_handle, 0xBB, 0x88);//PDN set to 1
		FT_WriteGPIO(phost->hal_handle, (1 << phost->HalConfig.pdn_pin_no) | 0x3B, (1 << phost->HalConfig.pdn_pin_no) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		Ft_Gpu_Hal_Sleep(20);
#endif

#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->HalConfig.pdn_pin_no, 0)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		Ft_Gpu_Hal_Sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->HalConfig.pdn_pin_no, 1)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		Ft_Gpu_Hal_Sleep(20);
#endif
#endif /*MSVC_PLATFORM*/
#if defined(FT900_PLATFORM) && !defined(ESD_SIMULATION)
		gpio_write(phost->HalConfig.pdn_pin_no, 0);
		ft_delay(20);
		gpio_write(phost->HalConfig.pdn_pin_no, 1);
		ft_delay(20);
#endif
	}
	else
	{
#ifdef MSVC_PLATFORM
#ifdef MPSSE_PLATFORM
		//FT_WriteGPIO(phost->hal_handle, 0xBB, 0x88);//PDN set to 1
		FT_WriteGPIO(phost->hal_handle, (1 << phost->HalConfig.pdn_pin_no) | 0x3B, (1 << phost->HalConfig.pdn_pin_no) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		Ft_Gpu_Hal_Sleep(20);

		//FT_WriteGPIO(phost->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		FT_WriteGPIO(phost->hal_handle, (1 << phost->HalConfig.pdn_pin_no) | 0x3B, (0 << phost->HalConfig.pdn_pin_no) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board

		Ft_Gpu_Hal_Sleep(20);
#endif
#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->HalConfig.pdn_pin_no, 1)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		Ft_Gpu_Hal_Sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->HalConfig.pdn_pin_no, 0)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		Ft_Gpu_Hal_Sleep(20);
#endif
#endif
#if defined(FT900_PLATFORM) && !defined(ESD_SIMULATION)
		gpio_write(phost->HalConfig.pdn_pin_no, 1);
		ft_delay(20);
		gpio_write(phost->HalConfig.pdn_pin_no, 0);
		ft_delay(20);
#endif
	}
}
ft_void_t Ft_Gpu_Hal_WrMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_uint8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;

#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
	if (!Ft_Gpu_Hal_FT4222_Wr(phost, addr, buffer, length))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Wr failed\n");
	}

#else

	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
#if defined(FT900_PLATFORM) && !defined(ESD_SIMULATION)
	spi_writen(SPIM, buffer, length);
#endif
#if defined(ESD_SIMULATION)
	while (length--)
	{
		Ft_Gpu_Hal_Transfer8(phost, *buffer);
		buffer++;
	}
#endif

#ifdef MSVC_PLATFORM
	{

#ifdef MPSSE_PLATFORM
		SPI_Write((FT_HANDLE)phost->hal_handle, buffer, length, &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
#endif
	}
#endif

	Ft_Gpu_Hal_EndTransfer(phost);
#endif
}

ft_void_t Ft_Gpu_Hal_WrMem_ProgMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_prog_uchar8_t *buffer, ft_uint32_t length)
{
	Ft_Gpu_Hal_WrMem(phost, addr, buffer, length);
}

ft_void_t Ft_Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
#ifdef MPSSE_PLATFORM
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	ft_uint32_t SizeTransfered = 0;
	SPI_Read((FT_HANDLE)phost->hal_handle, buffer, length, &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	Ft_Gpu_Hal_EndTransfer(phost);
#elif defined(FT4222_PLATFORM)
	if (!Ft_Gpu_Hal_FT4222_Rd(phost, addr, buffer, length))
	{
		eve_printf_debug("Ft_Gpu_Hal_FT4222_Rd failed\n");
	}
#endif
}

ft_void_t Ft_Gpu_Hal_Sleep(ft_uint32_t ms)
{
	Sleep(ms);
}

ft_int16_t Ft_Gpu_Hal_SetSPI(EVE_HalContext *phost, FT_GPU_SPI_NUMCHANNELS_T numchnls, FT_GPU_SPI_NUMDUMMYBYTES numdummy)
{
	ft_uint8_t writebyte = 0;
#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
	FT4222_STATUS ftstatus;
	FT4222_SPIMode spimode;
#endif

	if ((numchnls > FT_GPU_SPI_QUAD_CHANNEL) || (numdummy > FT_GPU_SPI_TWODUMMY) || (numdummy < FT_GPU_SPI_ONEDUMMY))
	{
		return -1; //error
	}

	//swicth EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == FT_GPU_SPI_TWODUMMY)
		writebyte |= FT_SPI_TWO_DUMMY_BYTE;
	Ft_Gpu_Hal_Wr8(phost, REG_SPI_WIDTH, writebyte);

#if defined(FT4222_PLATFORM) && !defined(ESD_SIMULATION)
	//swicth FT4222 to relevant multi channel SPI communication mode
	if (numchnls == FT_GPU_SPI_DUAL_CHANNEL)
		spimode = SPI_IO_DUAL;
	else if (numchnls == FT_GPU_SPI_QUAD_CHANNEL)
		spimode = SPI_IO_QUAD;
	else
		spimode = SPI_IO_SINGLE;

	ftstatus = FT4222_SPIMaster_SetLines(phost->SpiHandle, spimode);
	if (FT4222_OK != ftstatus)
		eve_printf_debug("FT4222_SPIMaster_SetLines failed with status %d\n", ftstatus);
#endif

	//FT81x swicthed to dual/quad mode, now update global HAL context
	phost->SpiChannel = numchnls;
	phost->SpiNumDummy = numdummy;

	return 0;
}

ft_uint32_t Ft_Gpu_CurrentFrequency(EVE_HalContext *phost)
{
	ft_uint32_t t0, t1;
	ft_uint32_t addr = REG_CLOCK;
	ft_uint8_t spidata[4];
	ft_int32_t r = 15625;

	t0 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t0 read */

	//may not be precise
	Sleep(15625 / 1000);

	t1 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

ft_void_t Ft_DisplayPanel_Init()
{

#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
	ILI9488_Bootup();
	eve_printf_debug("after ILI9488 bootup\n");
#endif

#ifdef ENABLE_KD2401_HVGA_PORTRAIT
	KD2401_Bootup();
	eve_printf_debug("after KD2401 bootup\n");
#endif
}

static DWORD s_Millis_Start;

ft_void_t ft_millis_init()
{
	s_Millis_Start = GetTickCount();
}

/* global counter to loopback after ~49.71 days */
ft_uint32_t ft_millis()
{
	return GetTickCount() - s_Millis_Start;
}

ft_void_t ft_millis_exit()
{
	// no-op
}

#endif

/* end of file */