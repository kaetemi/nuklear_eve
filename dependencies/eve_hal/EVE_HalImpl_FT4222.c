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

#define FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 //3 bytes for FT81x memory address to which data to be written

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
	parameters->PowerDownPin = GPIO_PORT0;
	parameters->SpiCsPin = 1;
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
	if (status != FT_OK)
	{
		eve_printf_debug("FT_CreateDeviceInfoList failed");
		ret = FT_FALSE;
	}

	status = FT_ListDevices(&numdevs, NULL, FT_LIST_NUMBER_ONLY);
	if (status != FT_OK)
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
		if (status != FT_OK)
		{
			eve_printf_debug("FT_SetTimeouts failed!\n");
			ret = FT_FALSE;
		}
	}

	if (ret)
	{
		// no latency to usb
		status = FT_SetLatencyTimer(phost->SpiHandle, FT4222_LatencyTime);
		if (status != FT_OK)
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
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SetClock failed!\n");
			ret = FT_FALSE;
		}

		status = FT4222_GetClock(phost->SpiHandle, &ftclk);

		if (status != FT4222_OK)
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
		if (status != FT4222_OK)
		{
			eve_printf_debug("Init FT4222 as SPI master device failed!\n");
			ret = FT_FALSE;
		}

		status = FT4222_SPI_SetDrivingStrength(phost->SpiHandle, DS_4MA, DS_4MA, DS_4MA);
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SPI_SetDrivingStrength failed!\n");
		}

		Ft_Gpu_Hal_Sleep(20);

		status = FT4222_SetSuspendOut(phost->GpioHandle, FT_FALSE);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable suspend out function on GPIO2 failed!\n");
			ret = FT_FALSE;
		}

		status = FT4222_SetWakeUpInterrupt(phost->GpioHandle, FT_FALSE);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable wakeup/interrupt feature on GPIO3 failed!\n");
			ret = FT_FALSE;
		}
		/* Interface 2 is GPIO */
		status = FT4222_GPIO_Init(phost->GpioHandle, gpio_dir);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Init FT4222 as GPIO interface failed!\n");
			ret = FT_FALSE;
		}
	}

	if (ret)
	{
		phost->SpiChannel = FT_GPU_SPI_SINGLE_CHANNEL;
		phost->Status = EVE_HalStatusOpened;
		++g_HalPlatform.OpenedChannels;
	}

	return ret;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	FT4222_STATUS status;
	phost->Status = FT_GPU_HAL_CLOSED;
	--g_HalPlatform.OpenedChannels;

	if ((status = FT4222_UnInitialize(phost->SpiHandle)) != FT4222_OK)
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if ((status = FT4222_UnInitialize(phost->GpioHandle)) != FT4222_OK)
		eve_printf_debug("FT4222_UnInitialize failed %d\n", status);

	if ((status = FT_Close(phost->SpiHandle)) != FT_OK)
		eve_printf_debug("CLOSE failed %d\n", status);

	if ((status = FT_Close(phost->GpioHandle)) != FT_OK)
		eve_printf_debug("CLOSE failed %d\n", status);
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

/*************
** TRANSFER **
*************/

static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	if (phost->SpiChannel == FT_GPU_SPI_SINGLE_CHANNEL)
	{
		ft_uint16_t sizeTransferred;
		uint8_t hrdpkt[8] = { 0 }; // 3 byte addr + 2 or 1 byte dummy
		uint32_t addr = phost->SpiRamGAddr;

		/* Compose the HOST MEMORY READ packet */
		hrdpkt[0] = (uint8_t)(addr >> 16) & 0xFF;
		hrdpkt[1] = (uint8_t)(addr >> 8) & 0xFF;
		hrdpkt[2] = (uint8_t)(addr & 0xFF);

		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    hrdpkt,
		    3 + phost->SpiNumDummy, /* 3 address and chosen dummy bytes */
		    &sizeTransferred,
		    FALSE /* continue transaction */
		);

		if ((status != FT4222_OK) || (sizeTransferred != (3 + phost->SpiNumDummy)))
		{
			eve_printf_debug("FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", sizeTransferred, status);
			if (sizeTransferred != (3 + phost->SpiNumDummy))
				phost->Status = EVE_HalStatusError;
			return false;
		}

		while (size)
		{
			uint16_t sizeTransferred;
			uint32_t bytesPerRead;
			BOOL isEndTransaction;

			if (size <= FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH)
			{
				bytesPerRead = size;
				isEndTransaction = TRUE;
			}
			else
			{
				bytesPerRead = FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH;
				isEndTransaction = FALSE;
			}

			status = FT4222_SPIMaster_SingleRead(
			    phost->SpiHandle,
			    buffer,
			    bytesPerRead,
			    &sizeTransferred,
			    isEndTransaction);

			if ((status != FT4222_OK) || (sizeTransferred != bytesPerRead))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleRead failed,sizeTransferred is %d with status %d\n", sizeTransferred, status);
				if (sizeTransferred != bytesPerRead)
					phost->Status = EVE_HalStatusError;
				return false;
			}

			size -= sizeTransferred;
			buffer += sizeTransferred;

			if (addr != REG_CMDB_WRITE)
			{
				bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE));
				addr += sizeTransferred;
				if (wrapCmdAddr)
					addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
				phost->SpiRamGAddr = addr;
			}
		}

		return true;
	}
	else
	{
		eve_debug_break();
		return false;
		// TODO
	}
}

static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	if (buffer && (size < (sizeof(phost->SpiWrBuf) - phost->SpiWrBufIndex)))
	{
		/* Write to buffer */
		memcpy(&phost->SpiWrBuf[phost->SpiWrBufIndex], buffer, size);
		phost->SpiWrBufIndex += size;
		return true;
	}
	else
	{
		if (buffer && phost->SpiWrBufIndex)
		{
			/* Buffer is over size, flush now */
			if (!wrBuffer(phost, NULL, 0))
				return false;

			/* Write to buffer */
			if (size < sizeof(phost->SpiWrBuf))
				return wrBuffer(phost, buffer, size);
		}
		
		if (buffer || phost->SpiWrBufIndex)
		{
			if (phost->SpiChannel == FT_GPU_SPI_SINGLE_CHANNEL)
			{
				/* Flush now, or write oversize buffer */
				ft_uint16_t sizeTransferred;
				uint8_t hrdpkt[8];
				uint32_t addr = phost->SpiRamGAddr;

				if (!buffer)
				{
					/* Flushing */
					buffer = phost->SpiWrBuf;
					size = phost->SpiWrBufIndex;
					phost->SpiWrBufIndex = 0;
				}

				/* Compose the HOST MEMORY WRITE packet */
				hrdpkt[0] = (addr >> 16) | 0x80; //MSB bits 10 for WRITE
				hrdpkt[1] = (addr >> 8) & 0xFF;
				hrdpkt[2] = addr & 0xff;

				status = FT4222_SPIMaster_SingleWrite(
					phost->SpiHandle,
					hrdpkt,
					3, // 3 address bytes
					&sizeTransferred,
					FALSE /* continue transaction */
				);

				if ((status != FT4222_OK) || (sizeTransferred != 3))
				{
					eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
					if (sizeTransferred != 3)
						phost->Status = EVE_HalStatusError;
					return false;
				}

				while (size)
				{
					uint16_t sizeTransferred;
					uint32_t bytesPerWrite;
					BOOL isEndTransaction;

					if (size <= FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH)
					{
						bytesPerWrite = size;
						isEndTransaction = TRUE;
					}
					else
					{
						bytesPerWrite = FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH;
						isEndTransaction = FALSE;
					}

					status = FT4222_SPIMaster_SingleWrite(
						phost->SpiHandle,
						(uint8_t *)buffer,
						bytesPerWrite,
						&sizeTransferred,
						isEndTransaction);

					if ((status != FT4222_OK) || ((ft_uint16_t)sizeTransferred != bytesPerWrite))
					{
						eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
						if (sizeTransferred != bytesPerWrite)
							phost->Status = EVE_HalStatusError;
						return false;
					}

					buffer += sizeTransferred;
					size -= sizeTransferred;

					if (addr != REG_CMDB_WRITE)
					{
						bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE));
						addr += sizeTransferred;
						if (wrapCmdAddr)
							addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
						phost->SpiRamGAddr = addr;
					}
				}
			}
			else
			{
				eve_debug_break();
				return false;
				// TODO
			}
		}

		return true;
	}
}

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_HalTransfer rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_HalStatusOpened);

	if (addr != phost->SpiRamGAddr || rw == EVE_HalTransferRead)
	{
		/* Close any write transfer that was left open, if the address changed */
		if (phost->SpiWrBufIndex)
			wrBuffer(phost, NULL, 0);

		phost->SpiRamGAddr = addr;
	}

	if (rw == EVE_HalTransferRead)
		phost->Status = EVE_HalStatusReading;
	else
		phost->Status = EVE_HalStatusWriting;
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	uint32_t addr;

	eve_assert(phost->Status == EVE_HalStatusReading || phost->Status == EVE_HalStatusWriting);

	/* Transfers to FIFO are kept open */
	addr = phost->SpiRamGAddr;
	if (addr != REG_CMDB_WRITE && !((addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE))))
	{
		if (phost->SpiWrBufIndex)
			wrBuffer(phost, NULL, 0);
	}

	phost->Status = EVE_HalStatusOpened;
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	if (phost->Status == EVE_HalStatusReading)
	{
		rdBuffer(phost, &value, 1);
		return value;
	}
	else
	{
		wrBuffer(phost, &value, 1);
		return 0;
	}
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint8_t buffer[2];
	if (phost->Status == EVE_HalStatusReading)
	{
		rdBuffer(phost, buffer, 2);
		return (uint16_t)buffer[0]
		    | (uint16_t)buffer[1] << 8;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = value >> 8;
		wrBuffer(phost, buffer, 2);
		return 0;
	}
}

uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint8_t buffer[4];
	if (phost->Status == EVE_HalStatusReading)
	{
		rdBuffer(phost, buffer, 4);
		return (uint32_t)buffer[0]
		    | (uint32_t)buffer[1] << 8
		    | (uint32_t)buffer[2] << 16
		    | (uint32_t)buffer[3] << 24;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		wrBuffer(phost, buffer, 4);
		return 0;
	}
}

void EVE_Hal_transferBuffer(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	if (!size)
		return 0;

	uint32_t transferred = 0;
	if (phost->Status == EVE_HalStatusWriting)
	{
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		while (transferred < size)
		{
			char c = str[index + (transferred)];
			buffer[transferred++] = c;
			if (!c)
				break;
		}
		while (transferred & padMask)
		{
			buffer[transferred++] = 0;
		}

		if (!transferred)
			return 0;

		wrBuffer(phost, buffer, transferred);
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}

#endif /* #if defined(FT4222_PLATFORM) */

/* end of file */