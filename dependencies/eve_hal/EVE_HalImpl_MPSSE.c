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

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH 65535

#define LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH 65535
#define LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH 65532 //3 bytes for FT81x memory address to which data to be written

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	FT_STATUS status;
	DWORD numdevs;

	/* Initialize the libmpsse */
	Init_libMPSSE();
	SPI_GetNumChannels(&g_HalPlatform.TotalDevices);

	if (g_HalPlatform.TotalDevices > 0)
	{
		FT_DEVICE_LIST_INFO_NODE devList;
		memset(&devList, 0, sizeof(devList));
		SPI_GetChannelInfo(0, &devList);

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
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	//Cleanup the MPSSE Lib
	Cleanup_libMPSSE();
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	parameters->MpsseChannelNo = 0;
	parameters->PowerDownPin = 7;
	parameters->SpiClockrateKHz = 12000; //in KHz
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	FT_STATUS status;
	ChannelConfig channelConf; /* channel configuration */

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
	phost->SpiDummyBytes = 1; // by default ft800/801/810/811 goes with single dummy byte for read
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

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	// eve_assert(!(phost->CmdFrame && (addr == REG_CMDB_WRITE)));
	if (rw == EVE_TRANSFER_READ)
	{
		uint8_t transferArray[5];
		uint32_t sizeTransferred;

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
		uint8_t transferArray[3];
		uint32_t sizeTransferred;

		/* Compose the read packet */
		transferArray[0] = (0x80 | (addr >> 16));
		transferArray[1] = addr >> 8;
		transferArray[2] = addr;

		SPI_Write((FT_HANDLE)phost->SpiHandle, transferArray, 3, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

		phost->Status = EVE_STATUS_WRITING;
	}
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	// just disable the CS
	FT_STATUS status = SPI_ToggleCS((FT_HANDLE)phost->SpiHandle, FALSE);

	if (status != FT_OK)
		phost->Status = EVE_STATUS_ERROR;
	else
		phost->Status = EVE_STATUS_OPENED;
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
	uint32_t sizeTransferred = 0;

	FT_STATUS status = SPI_Write(phost->SpiHandle, (uint8 *)buffer, size, &sizeTransferred, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);

	if (status != FT_OK || sizeTransferred != size)
	{
		phost->Status = EVE_STATUS_ERROR;
		return false;
	}

	return true;
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

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
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
	if (phost->Status == EVE_STATUS_WRITING)
	{
		uint8_t buffer[EVE_CMD_STRING_MAX + 1];

		while (transferred < size)
		{
			char c = str[index + (transferred++)];
			// putchar(c ? c : '_');
			if (!c)
				break;
		}

		wrBuffer(phost, &str[index], transferred);

		while (transferred & padMask)
		{
			++transferred;
			// putchar('_');
			transfer8(phost, 0);
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

/*********
** MISC **
*********/

bool EVE_UtilImpl_prepareSpiMaster(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

bool EVE_UtilImpl_postBootupConfig(EVE_HalContext *phost)
{
	/* no-op */
	return true;
}

#endif /* #if defined(MPSSE_PLATFORM) */

/* end of file */