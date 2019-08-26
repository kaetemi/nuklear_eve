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
#if defined(MPSSE_PLATFORM)

#if defined(EVE_MULTI_TARGET)
#define EVE_HalImpl_initialize EVE_HalImpl_MPSSE_initialize
#define EVE_HalImpl_release EVE_HalImpl_MPSSE_release
#define EVE_Hal_list EVE_Hal_MPSSE_list
#define EVE_Hal_info EVE_Hal_MPSSE_info
#define EVE_Hal_isDevice EVE_Hal_MPSSE_isDevice
#define EVE_HalImpl_defaults EVE_HalImpl_MPSSE_defaults
#define EVE_HalImpl_open EVE_HalImpl_MPSSE_open
#define EVE_HalImpl_close EVE_HalImpl_MPSSE_close
#define EVE_HalImpl_idle EVE_HalImpl_MPSSE_idle
#define EVE_Hal_flush EVE_Hal_MPSSE_flush
#define EVE_Hal_startTransfer EVE_Hal_MPSSE_startTransfer
#define EVE_Hal_endTransfer EVE_Hal_MPSSE_endTransfer
#define EVE_Hal_transfer8 EVE_Hal_MPSSE_transfer8
#define EVE_Hal_transfer16 EVE_Hal_MPSSE_transfer16
#define EVE_Hal_transfer32 EVE_Hal_MPSSE_transfer32
#define EVE_Hal_transferMem EVE_Hal_MPSSE_transferMem
#define EVE_Hal_transferProgmem EVE_Hal_MPSSE_transferProgmem
#define EVE_Hal_transferString EVE_Hal_MPSSE_transferString
#define EVE_Hal_hostCommand EVE_Hal_MPSSE_hostCommand
#define EVE_Hal_hostCommandExt3 EVE_Hal_MPSSE_hostCommandExt3
#define EVE_Hal_powerCycle EVE_Hal_MPSSE_powerCycle
#define EVE_UtilImpl_bootupDisplayGpio EVE_UtilImpl_MPSSE_bootupDisplayGpio
#define EVE_Hal_setSPI EVE_Hal_MPSSE_setSPI
#endif

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 //3 bytes for FT81x memory address to which data to be written

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;
uint32_t s_NumChannels = 0;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	/* Initialize the libmpsse */
	Init_libMPSSE();
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	/* Cleanup the MPSSE Lib */
	Cleanup_libMPSSE();
}

/* List the available devices */
size_t EVE_Hal_list()
{
	s_NumChannels = 0;
	SPI_GetNumChannels(&s_NumChannels);
	return s_NumChannels;
}

/* Get info of the specified device */
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx)
{
	memset(deviceInfo, 0, sizeof(EVE_DeviceInfo));
	if (deviceIdx < 0 || deviceIdx >= s_NumChannels)
		return;

	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };
	SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo);

	strcpy(deviceInfo->SerialNumber, chanInfo.SerialNumber);
	strcpy(deviceInfo->DisplayName, chanInfo.Description);
	deviceInfo->Host = EVE_HOST_MPSSE;
	deviceInfo->Opened = chanInfo.Flags & FT_FLAGS_OPENED;
}

/* Check whether the context is the specified device */
bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx)
{
	if (!phost)
		return false;
	if (phost->Host != EVE_HOST_MPSSE)
		return false;
	if (deviceIdx < 0 || deviceIdx >= s_NumChannels)
		return false;

	if (!phost->SpiHandle)
		return false;

	FT_DEVICE_LIST_INFO_NODE chanInfo = { 0 };
	if (!SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo))
		return false;

	return phost->SpiHandle == chanInfo.ftHandle;
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters, EVE_CHIPID_T chipId, size_t deviceIdx)
{
	if (deviceIdx < 0 || deviceIdx >= s_NumChannels)
	{
		if (!s_NumChannels)
			EVE_Hal_list();

		// Select first open device
		deviceIdx = 0;
		for (uint32_t i = 0; i < s_NumChannels; ++i)
		{
			FT_DEVICE_LIST_INFO_NODE chanInfo;
			if (SPI_GetChannelInfo((uint32_t)deviceIdx, &chanInfo) != FT_OK)
				continue;
			if (!(chanInfo.Flags & FT_FLAGS_OPENED))
			{ 
				deviceIdx = i;
				break;
			}
		}
	}
	parameters->MpsseChannelNo = deviceIdx & 0xFF;
	parameters->PowerDownPin = 7;
	parameters->SpiClockrateKHz = 12000; /* in KHz */
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	FT_STATUS status;
	ChannelConfig channelConf; /* channel configuration */

#ifdef EVE_MULTI_TARGET
	if (parameters->ChipId >= EVE_BT815)
		phost->GpuDefs = &EVE_GpuDefs_BT81X;
	else if (parameters->ChipId >= EVE_FT810)
		phost->GpuDefs = &EVE_GpuDefs_FT81X;
	else
		phost->GpuDefs = &EVE_GpuDefs_FT80X;
#endif
	phost->ChipId = parameters->ChipId;

	/* configure the spi settings */
	channelConf.ClockRate = phost->Parameters.SpiClockrateKHz * 1000;
	phost->SpiClockrateKHz = phost->Parameters.SpiClockrateKHz;
	channelConf.LatencyTimer = 2;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000; /* FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out) */

	/* Open the first available channel */
	status = SPI_OpenChannel(phost->Parameters.MpsseChannelNo, (FT_HANDLE *)&phost->SpiHandle);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI open channel failed %d %p\n", phost->Parameters.MpsseChannelNo, phost->SpiHandle);
		return false;
	}
	status = SPI_InitChannel((FT_HANDLE)phost->SpiHandle, &channelConf);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI init channel failed %d %p\n", phost->Parameters.MpsseChannelNo, phost->SpiHandle);
		return false;
	}

	eve_printf_debug("\nhandle=0x%p status=0x%x\n", phost->SpiHandle, status);

	/* Initialize the context variables */
	phost->SpiDummyBytes = 1; /* by default ft800/801/810/811 goes with single dummy byte for read */
	phost->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
	phost->Status = EVE_STATUS_OPENED;
	++g_HalPlatform.OpenedDevices;
	return true;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
	SPI_CloseChannel(phost->SpiHandle);
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
	if (!EVE_Hal_supportCmdB(phost) || (addr != REG_CMDB_WRITE))
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
	uint32_t sizeTransferred = 0;

	FT_STATUS status = SPI_Read(phost->SpiHandle, buffer, size, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);

	if (status != FT_OK || sizeTransferred != size)
	{
		phost->Status = EVE_STATUS_ERROR;
		return false;
	}

	return true;
}

static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
#if defined(BUFFER_OPTIMIZATION)
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
			if (!flush(phost))
				return false;

			/* Write to buffer */
			if (size < sizeof(phost->SpiWrBuf))
				return wrBuffer(phost, buffer, size);
		}

		if (buffer || phost->SpiWrBufIndex)
		{
			uint32_t sizeTransferred;
			uint8_t hrdpkt[8];
			uint32_t addr = phost->SpiRamGAddr;
			FT_STATUS status;

			if (!buffer)
			{
				/* Flushing */
				buffer = phost->SpiWrBuf;
				size = phost->SpiWrBufIndex;
				phost->SpiWrBufIndex = 0;
			}

			/* Compose the HOST MEMORY WRITE packet */
			hrdpkt[0] = (addr >> 16) | 0x80; /* MSB bits 10 for WRITE */
			hrdpkt[1] = (addr >> 8) & 0xFF;
			hrdpkt[2] = addr & 0xFF;

			/* printf("wr %x\n", addr); */
			status = SPI_Write((FT_HANDLE)phost->SpiHandle, hrdpkt, 3, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

			if ((status != FT_OK) || (sizeTransferred != 3))
			{
				eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
				if (sizeTransferred != 3)
					phost->Status = EVE_STATUS_ERROR;
				return false;
			}

			uint32_t sizeRemaining = size;
			while (sizeRemaining)
			{
				uint32_t transferSize = min(0xFFFF, sizeRemaining);
				FT_STATUS status = SPI_Write(phost->SpiHandle, (uint8 *)buffer, transferSize, &sizeTransferred, 
					(transferSize == sizeRemaining) ? (SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE) : SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
				sizeRemaining -= sizeTransferred;
			
				if (status != FT_OK || !sizeTransferred)
				{
					eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
					phost->Status = EVE_STATUS_ERROR;
					return false;
				}
			}

			addr = incrementRamGAddr(phost, addr, size);
			phost->SpiRamGAddr = addr;
		}

		return true;
	}
#else
	uint32_t sizeTransferred = 0;
	uint32_t sizeRemaining = size;

	while (sizeRemaining)
	{
		FT_STATUS status = SPI_Write(phost->SpiHandle, (uint8 *)buffer, min(0xFFFF, sizeRemaining), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
		sizeRemaining -= sizeTransferred;

		if (status != FT_OK || !sizeTransferred)
		{
			eve_printf_debug("%d SPI_Write failed, sizeTransferred is %d with status %d\n", __LINE__, sizeTransferred, status);
			phost->Status = EVE_STATUS_ERROR;
			return false;
		}
	}

	return true;
#endif
}

static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	uint32_t sizeTransferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		wrBuffer(phost, &value, sizeof(value));
	}
	else
	{
		rdBuffer(phost, &value, sizeof(value));
	}
	return value;
}

#if defined(BUFFER_OPTIMIZATION)
static bool flush(EVE_HalContext *phost)
{
	bool res = true;
	if (phost->SpiWrBufIndex)
	{
		res = wrBuffer(phost, NULL, 0);
	}
	eve_assert(!phost->SpiWrBufIndex);
	if (!EVE_Hal_supportCmdB(phost))
	{
		if (phost->SpiWpWritten)
		{
			phost->SpiWpWritten = false;
			phost->SpiRamGAddr = REG_CMD_WRITE;
			phost->SpiWrBufIndex = 2;
			phost->SpiWrBuf[0] = phost->SpiWpWrite & 0xFF;
			phost->SpiWrBuf[1] = phost->SpiWpWrite >> 8;
			res = wrBuffer(phost, NULL, 0);
		}
		eve_assert(!phost->SpiWrBufIndex);
	}
	return res;
}
#endif

void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
#if defined(BUFFER_OPTIMIZATION)
	flush(phost);
#endif
}

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost) && addr == REG_CMD_WRITE && rw == EVE_TRANSFER_WRITE)
	{
		/* Bypass fifo write pointer write */
		phost->SpiWpWriting = true;
	}
	else if (addr != incrementRamGAddr(phost, phost->SpiRamGAddr, phost->SpiWrBufIndex) || rw == EVE_TRANSFER_READ)
	{
		/* Close any write transfer that was left open, if the address changed */
		flush(phost);
		phost->SpiRamGAddr = addr;
	}
#endif

	if (rw == EVE_TRANSFER_READ)
	{
		uint8_t transferArray[5];
		uint32_t sizeTransferred;

#if defined(BUFFER_OPTIMIZATION)
		eve_assert(!phost->SpiWrBufIndex);
#endif

		/* Compose the read packet */
		transferArray[0] = addr >> 16;
		transferArray[1] = addr >> 8;
		transferArray[2] = addr;
		transferArray[3] = 0; // Dummy Read byte
		transferArray[4] = 0; // Dummy Read byte

		SPI_Write((FT_HANDLE)phost->SpiHandle, transferArray, 3 + phost->SpiDummyBytes, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

		phost->Status = EVE_STATUS_READING;
	}
	else
	{
#if defined(BUFFER_OPTIMIZATION)
		phost->Status = EVE_STATUS_WRITING;
#else
		uint8_t transferArray[3];
		uint32_t sizeTransferred;

		/* Compose the write packet */
		transferArray[0] = (0x80 | (addr >> 16));
		transferArray[1] = addr >> 8;
		transferArray[2] = addr;

		SPI_Write((FT_HANDLE)phost->SpiHandle, transferArray, 3, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

		phost->Status = EVE_STATUS_WRITING;
#endif
	}
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	FT_STATUS status;
#if defined(BUFFER_OPTIMIZATION)
	uint32_t addr;
#endif

	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

#if defined(BUFFER_OPTIMIZATION)
	/* Transfers to FIFO are kept open */
	addr = phost->SpiRamGAddr;
	if (addr != (EVE_Hal_supportCmdB(phost) ? REG_CMDB_WRITE : REG_CMD_WRITE) && !((addr >= RAM_CMD) && (addr < (addr + EVE_CMD_FIFO_SIZE))))
	{
		flush(phost);
	}

	if (phost->Status == EVE_STATUS_READING)
	{
		/* just disable the CS */
		status = SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);

		if (status != FT_OK)
			phost->Status = EVE_STATUS_ERROR;
		else
			phost->Status = EVE_STATUS_OPENED;
	}
	else if (phost->Status == EVE_STATUS_WRITING)
	{
		if (!EVE_Hal_supportCmdB(phost))
		{
			phost->SpiWpWriting = false;
		}
		phost->Status = EVE_STATUS_OPENED;
	}
#else
	/* just disable the CS */
	status = SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);

	if (status != FT_OK)
		phost->Status = EVE_STATUS_ERROR;
	else
		phost->Status = EVE_STATUS_OPENED;
#endif
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
#endif
	return transfer8(phost, value);
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		if (phost->SpiWpWriting)
		{
			phost->SpiWpWrite = value;
			phost->SpiWpWritten = true;
			return 0;
		}
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
#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
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

#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
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

#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
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

#if defined(BUFFER_OPTIMIZATION)
	if (!EVE_Hal_supportCmdB(phost))
	{
		eve_assert(!phost->SpiWpWriting);
	}
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
	uint8_t transferArray[3];
	uint32_t sizeTransferred;

	transferArray[0] = cmd;
	transferArray[1] = 0;
	transferArray[2] = 0;

#if defined(BUFFER_OPTIMIZATION)
	flush(phost);
#endif
	SPI_Write(phost->SpiHandle, transferArray, sizeof(transferArray), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
}

void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	uint8_t transferArray[3];
	uint32_t sizeTransferred;

	transferArray[0] = cmd;
	transferArray[1] = (cmd >> 8) & 0xff;
	transferArray[2] = (cmd >> 16) & 0xff;

#if defined(BUFFER_OPTIMIZATION)
	flush(phost);
#endif
	SPI_Write(phost->SpiHandle, transferArray, sizeof(transferArray), &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
}

/* Toggle PD_N pin of FT800 board for a power cycle */
void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
#if defined(BUFFER_OPTIMIZATION)
	flush(phost);
#endif
	if (up)
	{
		// FT_WriteGPIO(phost->SpiHandle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		FT_WriteGPIO(phost->SpiHandle, (1 << phost->Parameters.PowerDownPin) | 0x3B, (0 << phost->Parameters.PowerDownPin) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board

		EVE_sleep(20);

		// FT_WriteGPIO(phost->SpiHandle, 0xBB, 0x88);//PDN set to 1
		FT_WriteGPIO(phost->SpiHandle, (1 << phost->Parameters.PowerDownPin) | 0x3B, (1 << phost->Parameters.PowerDownPin) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		EVE_sleep(20);
	}
	else
	{
		// FT_WriteGPIO(phost->SpiHandle, 0xBB, 0x88);//PDN set to 1
		FT_WriteGPIO(phost->SpiHandle, (1 << phost->Parameters.PowerDownPin) | 0x3B, (1 << phost->Parameters.PowerDownPin) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		EVE_sleep(20);

		// FT_WriteGPIO(phost->SpiHandle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
		FT_WriteGPIO(phost->SpiHandle, (1 << phost->Parameters.PowerDownPin) | 0x3B, (0 << phost->Parameters.PowerDownPin) | 0x08); //PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board

		EVE_sleep(20);
	}
}

void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
#if defined(BUFFER_OPTIMIZATION)
	flush(phost);
#endif
	/* no-op */
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

#endif /* #if defined(MPSSE_PLATFORM) */

/* end of file */