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

#if defined(EVE_MULTI_TARGET)
#define EVE_HalImpl_initialize EVE_HalImpl_FT4222_initialize
#define EVE_HalImpl_release EVE_HalImpl_FT4222_release
#define EVE_HalImpl_defaults EVE_HalImpl_FT4222_defaults
#define EVE_HalImpl_open EVE_HalImpl_FT4222_open
#define EVE_HalImpl_close EVE_HalImpl_FT4222_close
#define EVE_HalImpl_idle EVE_HalImpl_FT4222_idle
#define EVE_Hal_flush EVE_Hal_FT4222_flush
#define EVE_Hal_startTransfer EVE_Hal_FT4222_startTransfer
#define EVE_Hal_endTransfer EVE_Hal_FT4222_endTransfer
#define EVE_Hal_transfer8 EVE_Hal_FT4222_transfer8
#define EVE_Hal_transfer16 EVE_Hal_FT4222_transfer16
#define EVE_Hal_transfer32 EVE_Hal_FT4222_transfer32
#define EVE_Hal_transferMem EVE_Hal_FT4222_transferMem
#define EVE_Hal_transferProgmem EVE_Hal_FT4222_transferProgmem
#define EVE_Hal_transferString EVE_Hal_FT4222_transferString
#define EVE_Hal_hostCommand EVE_Hal_FT4222_hostCommand
#define EVE_Hal_hostCommandExt3 EVE_Hal_FT4222_hostCommandExt3
#define EVE_Hal_powerCycle EVE_Hal_FT4222_powerCycle
#define EVE_UtilImpl_bootupDisplayGpio EVE_UtilImpl_FT4222_bootupDisplayGpio
#define EVE_Hal_setSPI EVE_Hal_FT4222_setSPI
#endif

#define FT4222_TRANSFER_SIZE_MAX (0xFFFF)
#define FT4222_WRITE_HEADER_SIZE (3)
#define FT4222_WRITE_SIZE_MAX (FT4222_TRANSFER_SIZE_MAX - FT4222_WRITE_HEADER_SIZE)

#define FT4222_READ_TIMEOUT (5000)
#define FT4222_WRITE_TIMEOUT (5000)

#define FT4222_LATENCY_TIME (2)

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	FT_DEVICE_LIST_INFO_NODE devList;
	FT_STATUS status;
	uint32_t numdevs;

	status = FT_CreateDeviceInfoList(&numdevs);
	if (FT_OK == status)
	{
		eve_printf_debug("Number of D2xx devices connected = %d\n", numdevs);
		g_HalPlatform.TotalDevices = numdevs;

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
void EVE_HalImpl_defaults(EVE_HalParameters *parameters, uint32_t model, EVE_DeviceInfo *device)
{
	parameters->PowerDownPin = GPIO_PORT0;
	parameters->SpiCsPin = 1;
	parameters->SpiClockrateKHz = 12000;
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
*                            Global variable phost->Parameters.SpiClockrateKHz is
*                            updated accodingly
* Return Value             : bool_t
*                               TRUE : Supported by FT4222
*                               FALSE : Not supported by FT4222
*
* Author                   :
****************************************************************************/
bool computeCLK(EVE_HalContext *phost, FT4222_ClockRate *sysclk, FT4222_SPIClock *sysdivisor)
{
	/* phost->Parameters.SpiClockrateKHz is the user requested SPI communication clock */

	if (phost->Parameters.SpiClockrateKHz <= 5000)
	{
		/* set to 5000 KHz */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_16;
		phost->SpiClockrateKHz = 5000;
	}
	else if (phost->Parameters.SpiClockrateKHz > 5000 && phost->Parameters.SpiClockrateKHz <= 10000)
	{
		/* set to 10000 KHz */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_8;
		phost->SpiClockrateKHz = 10000;
	}
	else if (phost->Parameters.SpiClockrateKHz > 10000 && phost->Parameters.SpiClockrateKHz <= 15000)
	{
		/* set to 15000 KHz */
		*sysclk = SYS_CLK_60;
		*sysdivisor = CLK_DIV_4;
		phost->SpiClockrateKHz = 15000;
	}
	else
	{
		/* set to 20000 KHz : Maximum throughput is obeserved with this clock combination */
		*sysclk = SYS_CLK_80;
		*sysdivisor = CLK_DIV_4;
		phost->SpiClockrateKHz = 20000;
	}
	eve_printf_debug("User Selected SPI clk : %d KHz\n", phost->Parameters.SpiClockrateKHz);
	eve_printf_debug("Configured clk :  Ft4222 sys clk enum = %d , divisor enum = %d\n", *sysclk, *sysdivisor);
	return true;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	FT_STATUS status;
	//ulong_t numdevs;
	uint32_t numdevs;
	uint32_t index;
	FT_HANDLE fthandle;
	FT4222_Version pversion;
	FT4222_ClockRate ftclk = 0;
	uint16_t max_size = 0;
	FT4222_ClockRate selclk = 0;
	FT4222_SPIClock seldiv = 0;
	/* GPIO0         , GPIO1      , GPIO2       , GPIO3         } */
	GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT, GPIO_INPUT, GPIO_INPUT, GPIO_INPUT };

	bool ret = true;

	phost->SpiHandle = phost->GpioHandle = NULL;

	status = FT_CreateDeviceInfoList(&numdevs);
	if (status != FT_OK)
	{
		eve_printf_debug("FT_CreateDeviceInfoList failed");
		ret = false;
	}

	status = FT_ListDevices(&numdevs, NULL, FT_LIST_NUMBER_ONLY);
	if (status != FT_OK)
	{
		eve_printf_debug("FT_ListDevices failed");
		ret = false;
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
				ret = false;

			if (ret && !(devInfo.Flags & 0x01) && ((!strcmp(devInfo.Description, "FT4222 A") && (phost->SpiHandle == NULL)) || (!strcmp(devInfo.Description, "FT4222 B") && (phost->GpioHandle == NULL))))
			{
				/* obtain handle for the first discovered "FT4222 A" and first "FT4222 B" */
				status = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &fthandle);
				if (status != FT_OK)
				{
					eve_printf_debug("FT_OpenEx failed %d\n", status);
					ret = false;
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
		status = FT_SetTimeouts(phost->SpiHandle, FT4222_READ_TIMEOUT, FT4222_WRITE_TIMEOUT);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_SetTimeouts failed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		// no latency to usb
		status = FT_SetLatencyTimer(phost->SpiHandle, FT4222_LATENCY_TIME);
		if (status != FT_OK)
		{
			eve_printf_debug("FT_SetLatencyTimerfailed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		if (!computeCLK(phost, &selclk, &seldiv))
		{
			eve_printf_debug("Requested clock %d KHz is not supported in FT4222\n", phost->Parameters.SpiClockrateKHz);
			ret = false;
		}
	}

	if (ret)
	{
		status = FT4222_SetClock(phost->SpiHandle, selclk);
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SetClock failed!\n");
			ret = false;
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
			ret = false;
		}

		status = FT4222_SPI_SetDrivingStrength(phost->SpiHandle, DS_4MA, DS_4MA, DS_4MA);
		if (status != FT4222_OK)
		{
			eve_printf_debug("FT4222_SPI_SetDrivingStrength failed!\n");
		}

		EVE_sleep(20);

		status = FT4222_SetSuspendOut(phost->GpioHandle, false);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable suspend out function on GPIO2 failed!\n");
			ret = false;
		}

		status = FT4222_SetWakeUpInterrupt(phost->GpioHandle, false);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Disable wakeup/interrupt feature on GPIO3 failed!\n");
			ret = false;
		}
		/* Interface 2 is GPIO */
		status = FT4222_GPIO_Init(phost->GpioHandle, gpio_dir);
		if (status != FT4222_OK)
		{
			eve_printf_debug("Init FT4222 as GPIO interface failed!\n");
			ret = false;
		}
	}

	if (ret)
	{
		phost->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
		phost->SpiDummyBytes = 1;
		phost->Status = EVE_STATUS_OPENED;
		++g_HalPlatform.OpenedDevices;
	}

	return ret;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	FT4222_STATUS status;
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;

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

#if defined(BUFFER_OPTIMIZATION)
static bool flush(EVE_HalContext *phost);
#endif

static inline uint32_t incrementRamGAddr(EVE_HalContext *phost, uint32_t addr, uint32_t inc)
{
#ifdef EVE_SUPPORT_CMDB
	if (addr != REG_CMDB_WRITE)
#else
	scope
#endif
	{
		bool wrapCmdAddr = (addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE));
		addr += inc;
		if (wrapCmdAddr)
			addr = RAM_CMD + (addr & EVE_CMD_FIFO_MASK);
	}
	return addr;
}

static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	uint8_t hrdpkt[8] = { 0 }; // 3 byte addr + 2 or 1 byte dummy

	while (size)
	{
		uint32_t sizeTransferred;
		uint32_t bytesPerRead;
		uint32_t addr = phost->SpiRamGAddr;

		/* Compose the HOST MEMORY READ packet */
		hrdpkt[0] = (uint8_t)(addr >> 16) & 0xFF;
		hrdpkt[1] = (uint8_t)(addr >> 8) & 0xFF;
		hrdpkt[2] = (uint8_t)(addr & 0xFF);

		if (size <= FT4222_TRANSFER_SIZE_MAX)
			bytesPerRead = size;
		else
			bytesPerRead = FT4222_TRANSFER_SIZE_MAX;

		if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
		{
			uint16_t sizeWritten;
			uint16_t sizeRead;

			status = FT4222_SPIMaster_SingleWrite(
			    phost->SpiHandle,
			    hrdpkt,
			    3 + phost->SpiDummyBytes, /* 3 address and chosen dummy bytes */
			    &sizeWritten,
			    FALSE);

			if ((status != FT4222_OK) || (sizeWritten != (3 + phost->SpiDummyBytes)))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", sizeWritten, status);
				if (sizeWritten != (3 + phost->SpiDummyBytes))
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			status = FT4222_SPIMaster_SingleRead(
			    phost->SpiHandle,
			    buffer,
			    bytesPerRead,
			    &sizeRead,
			    TRUE);

			if ((status != FT4222_OK) || (sizeRead != bytesPerRead))
			{
				eve_printf_debug("FT4222_SPIMaster_SingleRead failed,sizeTransferred is %d with status %d\n", sizeRead, status);
				if (sizeRead != bytesPerRead)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			sizeTransferred = sizeRead;
		}
		else
		{
			status = FT4222_SPIMaster_MultiReadWrite(
			    phost->SpiHandle,
			    buffer,
			    hrdpkt,
			    0,
			    3 + phost->SpiDummyBytes, /* 3 addr + dummy bytes */
			    bytesPerRead,
			    &sizeTransferred);

			if ((status != FT4222_OK) || (sizeTransferred != bytesPerRead))
			{
				eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, sizeTransferred is %d with status %d\n", sizeTransferred, status);
				if (sizeTransferred != bytesPerRead)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}
		}

		buffer += sizeTransferred;
		if (size)
		{
			eve_assert_ex(sizeTransferred <= size, "Cannot have transferred more than size\n");
			size -= sizeTransferred;
		}

#ifdef EVE_SUPPORT_CMDB
		if (addr != REG_CMDB_WRITE)
#else
		scope
#endif
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

static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	FT4222_STATUS status;

	if (buffer && (size < (sizeof(phost->SpiWrBuf) - phost->SpiWrBufIndex - FT4222_WRITE_HEADER_SIZE)))
	{
		/* Write to buffer */
		memcpy(&phost->SpiWrBuf[phost->SpiWrBufIndex + FT4222_WRITE_HEADER_SIZE], buffer, size);
		phost->SpiWrBufIndex += size;
		return true;
	}
	else
	{
		if (buffer && phost->SpiWrBufIndex)
		{
			/* Buffer is over size, flush now */
			if (!flush(phost))
				return false;

			/* Write to buffer */
			if (size < (sizeof(phost->SpiWrBuf) - FT4222_WRITE_HEADER_SIZE))
				return wrBuffer(phost, buffer, size);
		}

		if (buffer || phost->SpiWrBufIndex)
		{
			/* Flush now, or write oversize buffer */
			uint32_t addr = phost->SpiRamGAddr;

			if (!buffer)
			{
				/* Flushing */
				size = phost->SpiWrBufIndex;
				phost->SpiWrBufIndex = 0;
			}

			while (size)
			{
				uint32_t sizeTransferred;
				uint32_t bytesPerWrite;

				if (size <= FT4222_WRITE_SIZE_MAX)
					bytesPerWrite = size + FT4222_WRITE_HEADER_SIZE;
				else
					bytesPerWrite = FT4222_TRANSFER_SIZE_MAX;

				if (buffer)
				{
					/* Writing large buffer */
					memcpy(phost->SpiWrBuf + FT4222_WRITE_HEADER_SIZE, buffer, (bytesPerWrite - FT4222_WRITE_HEADER_SIZE));
				}

				/* Compose the HOST MEMORY WRITE packet */
				phost->SpiWrBuf[0] = (addr >> 16) | 0x80; /* MSB bits 10 for WRITE */
				phost->SpiWrBuf[1] = (addr >> 8) & 0xFF;
				phost->SpiWrBuf[2] = addr & 0xFF;

				if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
				{
					uint16_t sizeWritten;

					status = FT4222_SPIMaster_SingleWrite(
					    phost->SpiHandle,
					    phost->SpiWrBuf,
					    bytesPerWrite,
					    &sizeWritten,
					    TRUE);

					if ((status != FT4222_OK) || (sizeWritten != bytesPerWrite))
					{
						eve_printf_debug("%d FT4222_SPIMaster_SingleWrite failed, sizeTransferred is %d with status %d\n", __LINE__, sizeWritten, status);
						if (sizeWritten != bytesPerWrite)
							phost->Status = EVE_STATUS_ERROR;
						return false;
					}

					sizeTransferred = sizeWritten;
				}
				else
				{
					uint8_t dummyRead[8];

					status = FT4222_SPIMaster_MultiReadWrite(
					    phost->SpiHandle,
					    dummyRead,
					    phost->SpiWrBuf,
					    0,
					    bytesPerWrite,
					    0,
					    &sizeTransferred);

					if (status != FT4222_OK)
					{
						eve_printf_debug("FT4222_SPIMaster_MultiReadWrite failed, status %d\n", status);
						return false;
					}

					sizeTransferred = bytesPerWrite;
				}

				if (buffer)
				{
					buffer += sizeTransferred - FT4222_WRITE_HEADER_SIZE;
				}
				if (size)
				{
					eve_assert_ex((sizeTransferred - FT4222_WRITE_HEADER_SIZE) <= (int32_t)size, "Cannot have transferred more than size\n");
					size -= sizeTransferred - FT4222_WRITE_HEADER_SIZE;
					eve_assert_ex(!(buffer && size), "Cannot have space left after flushing buffer\n");
				}

				addr = incrementRamGAddr(phost, addr, sizeTransferred);
				phost->SpiRamGAddr = addr;
			}
		}

		return true;
	}
}

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

#if !defined(EVE_SUPPORT_CMDB)
	if (addr == REG_CMD_WRITE && rw == EVE_TRANSFER_WRITE)
	{
		/* Bypass fifo write pointer write */
		phost->SpiWpWriting = true;
	}
	else
#endif
	    if (addr != incrementRamGAddr(phost, phost->SpiRamGAddr, phost->SpiWrBufIndex) || rw == EVE_TRANSFER_READ)
	{
		/* Close any write transfer that was left open, if the address changed */
		flush(phost);
		phost->SpiRamGAddr = addr;
	}

	if (rw == EVE_TRANSFER_READ)
		phost->Status = EVE_STATUS_READING;
	else
		phost->Status = EVE_STATUS_WRITING;
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	uint32_t addr;

	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	/* Transfers to FIFO are kept open */
	addr = phost->SpiRamGAddr;
#ifdef EVE_SUPPORT_CMDB
	if (addr != REG_CMDB_WRITE && !((addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE))))
#else
	if (addr != REG_CMD_WRITE && !((addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE))))
#endif
	{
		flush(phost);
	}

#if !defined(EVE_SUPPORT_CMDB)
	phost->SpiWpWriting = false;
#endif
	phost->Status = EVE_STATUS_OPENED;
}

static bool flush(EVE_HalContext *phost)
{
	bool res = true;
	if (phost->SpiWrBufIndex)
	{
		res = wrBuffer(phost, NULL, 0);
	}
	eve_assert(!phost->SpiWrBufIndex);
#if !defined(EVE_SUPPORT_CMDB)
	if (phost->SpiWpWritten)
	{
		phost->SpiWpWritten = false;
		phost->SpiRamGAddr = REG_CMD_WRITE;
		phost->SpiWrBufIndex = 2;
		phost->SpiWrBuf[FT4222_WRITE_HEADER_SIZE + 0] = phost->SpiWpWrite & 0xFF;
		phost->SpiWrBuf[FT4222_WRITE_HEADER_SIZE + 1] = phost->SpiWpWrite >> 8;
		res = wrBuffer(phost, NULL, 0);
	}
	eve_assert(!phost->SpiWrBufIndex);
#endif
	return res;
}

void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	flush(phost);
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	eve_assert(!phost->SpiWpWriting);
#endif
	if (phost->Status == EVE_STATUS_READING)
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
#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	if (phost->SpiWpWriting)
	{
		phost->SpiWpWrite = value;
		phost->SpiWpWritten = true;
		return 0;
	}
#endif
	uint8_t buffer[2];
	if (phost->Status == EVE_STATUS_READING)
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
#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	eve_assert(!phost->SpiWpWriting);
#endif
	uint8_t buffer[4];
	if (phost->Status == EVE_STATUS_READING)
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

void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	eve_assert(!phost->SpiWpWriting);
#endif

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

#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	eve_assert(!phost->SpiWpWriting);
#endif

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
	{
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

#if defined(BUFFER_OPTIMIZATION) && !defined(EVE_SUPPORT_CMDB)
	eve_assert(!phost->SpiWpWriting);
#endif
	eve_assert(size <= EVE_CMD_STRING_MAX);
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		for (;;)
		{
			char c = str[index + (transferred)];
			buffer[transferred++] = c;
			// putchar(c ? c : '_');
			if (!c)
			{
				break;
			}
			if (transferred >= size)
			{
				buffer[transferred++] = 0;
				// putchar('_');
				break;
			}
		}
		while (transferred & padMask)
		{
			buffer[transferred++] = 0;
			// putchar('_');
		}

		eve_assert(transferred);

		wrBuffer(phost, buffer, transferred);
	}
	else
	{
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}

/************
** UTILITY **
************/

void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
	FT4222_STATUS status;
	uint8_t dummyRead;

	uint8_t transferArray[3];

	uint16_t sizeTransferred;
	uint32_t sizeOfRead;

	flush(phost);

	transferArray[0] = cmd;
	transferArray[1] = 0;
	transferArray[2] = 0;

	switch (phost->SpiChannels)
	{
	case EVE_SPI_SINGLE_CHANNEL:
		/* FYI : All HOST CMDs should only be executed in single channel mode*/
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    transferArray,
		    sizeof(transferArray),
		    &sizeTransferred,
		    true);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	case EVE_SPI_DUAL_CHANNEL:
	case EVE_SPI_QUAD_CHANNEL:
		/* only reset command among phost commands can be executed in multi channel mode*/
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummyRead,
		    transferArray,
		    0,
		    sizeof(transferArray),
		    0,
		    &sizeOfRead);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
}

void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	FT4222_STATUS status;
	uint8_t dummyRead;
	uint8_t transferArray[3];

	uint16_t sizeTransferred;
	uint32_t sizeOfRead;

	flush(phost);

	transferArray[0] = cmd;
	transferArray[1] = (cmd >> 8) & 0xff;
	transferArray[2] = (cmd >> 16) & 0xff;

	switch (phost->SpiChannels)
	{
	case EVE_SPI_SINGLE_CHANNEL:
		status = FT4222_SPIMaster_SingleWrite(
		    phost->SpiHandle,
		    transferArray,
		    sizeof(transferArray),
		    &sizeTransferred,
		    true);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	case EVE_SPI_DUAL_CHANNEL:
	case EVE_SPI_QUAD_CHANNEL:
		/* FYI : Mostly all HOST CMDs can be executed in single channel mode
		* except system reset cmd */
		status = FT4222_SPIMaster_MultiReadWrite(
		    phost->SpiHandle,
		    &dummyRead,
		    transferArray,
		    0,
		    sizeof(transferArray),
		    0,
		    &sizeOfRead);
		if (FT4222_OK != status)
			eve_printf_debug("SPI write failed = %d\n", status);
		break;
	default:
		eve_printf_debug("No transfer\n");
	}
}

void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	FT4222_STATUS ftstatus;
	FT4222_SPIMode spimode;

	flush(phost);

	/* switch FT4222 to relevant multi channel SPI communication mode */
	if (numchnls == EVE_SPI_DUAL_CHANNEL)
		spimode = SPI_IO_DUAL;
	else if (numchnls == EVE_SPI_QUAD_CHANNEL)
		spimode = SPI_IO_QUAD;
	else
		spimode = SPI_IO_SINGLE;

	ftstatus = FT4222_SPIMaster_SetLines(phost->SpiHandle, spimode);
	if (FT4222_OK != ftstatus)
		eve_printf_debug("FT4222_SPIMaster_SetLines failed with status %d\n", ftstatus);

	/* FT81x swicthed to dual/quad mode, now update global HAL context */
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
}

void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	flush(phost);

	if (up)
	{
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->Parameters.PowerDownPin, 0)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		EVE_sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->Parameters.PowerDownPin, 1)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		EVE_sleep(20);
	}
	else
	{
		FT4222_STATUS status = FT4222_OTHER_ERROR;

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->Parameters.PowerDownPin, 1)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		EVE_sleep(20);

		if (FT4222_OK != (status = FT4222_GPIO_Write(phost->GpioHandle, phost->Parameters.PowerDownPin, 0)))
			eve_printf_debug("FT4222_GPIO_Write error = %d\n", status);
		EVE_sleep(20);
	}

	/* Reset to single channel SPI mode */
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL, 1);
}

void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	flush(phost);
#if (EVE_MODEL < EVE_FT810)
	return;
#else
	uint8_t writebyte = 0;

	if ((numchnls > EVE_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
		return; // error

	/* Switch EVE to multi channel SPI mode */
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= EVE_SPI_TWO_DUMMY_BYTES;
	EVE_Hal_wr8(phost, REG_SPI_WIDTH, writebyte);

	/* Switch FT4222 to multi channel SPI mode */
	setSPI(phost, numchnls, numdummy);
#endif
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

#endif /* #if defined(FT4222_PLATFORM) */

/* end of file */