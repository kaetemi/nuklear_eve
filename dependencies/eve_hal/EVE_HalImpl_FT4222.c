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

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(FT4222_PLATFORM)

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	FT_DEVICE_LIST_INFO_NODE devList;
	FT_STATUS status;
	ft_uint32_t numdevs;

	status = FT_CreateDeviceInfoList(&numdevs);
	if (FT_OK == status)
	{
		eve_printf_debug("Number of D2xx devices connected = %d\n", numdevs);
		g_HalPlatform.TotalChannels = numdevs;

		FT_GetDeviceInfoDetail(0, &devList.Flags, &devList.Type, &devList.ID,
			&devList.LocId,
			devList.SerialNumber,
			devList.Description,
			&devList.ftHandle);
	}
	else
	{
		eve_printf_debug("FT_CreateDeviceInfoList failed");
		return;
	}

	eve_printf_debug("Information on channel number %d:\n", 0);
	/* print the dev info */
	eve_printf_debug(" Flags=0x%x\n", devList.Flags);
	eve_printf_debug(" Type=0x%x\n", devList.Type);
	eve_printf_debug(" ID=0x%x\n", devList.ID);
	eve_printf_debug(" LocId=0x%x\n", devList.LocId);
	eve_printf_debug(" SerialNumber=%s\n", devList.SerialNumber);
	eve_printf_debug(" Description=%s\n", devList.Description);
	eve_printf_debug(" ftHandle=0x%p\n", devList.ftHandle); /*is 0 unless open*/
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	/* no-op */
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	parameters->PowerDownPin = FT800_PD_N;
	parameters->SpiCsPin = FT800_SEL_PIN;
	parameters->SpiClockrateKHz = 12000;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
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
			eve_printf_debug("Requested clock %d KHz is not supported in FT4222\n", phost->Parameters.SpiClockrateKHz);
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
		    phost->Parameters.SpiCsPin); /* slave selection output pins */
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

	if (ret)
	{
		phost->SpiChannel = 0;
		phost->Status = EVE_HalStatusOpened;
	}

	return ret;
}

void EVE_HalImpl_close(EVE_HalContext *phost)
{
	FT4222_STATUS status;
	phost->Status = FT_GPU_HAL_CLOSED;

	if (FT4222_OK != (status = FT4222_UnInitialize(phost->SpiHandle)))
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if (FT4222_OK != (status = FT4222_UnInitialize(phost->GpioHandle)))
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if (FT_OK != (status = FT_Close(phost->SpiHandle)))
		eve_printf_debug("CLOSE failed %d\n", status);

	if (FT_OK != (status = FT_Close(phost->GpioHandle)))
		eve_printf_debug("CLOSE failed %d\n", status);
}

void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */