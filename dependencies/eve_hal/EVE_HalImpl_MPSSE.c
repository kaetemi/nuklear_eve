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

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	/* Initialize the libmpsse */
	Init_libMPSSE();
	SPI_GetNumChannels(&g_HalPlatform.TotalChannels);

	if (g_HalPlatform.TotalChannels > 0)
	{
		FT_DEVICE_LIST_INFO_NODE devList;
		memset(&devList, 0, sizeof(devList));
		SPI_GetChannelInfo(0, &devList);

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
		}

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
	phost->Parameters.MpsseChannelNo = 0;
	phost->Parameters.PowerDownPin = 7;
	phost->Parameters.SpiCsPin = 0;
	phost->Parameters.SpiClockrateKHz = 12000; //in KHz
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	FT_STATUS status;
	ChannelConfig channelConf; /* channel configuration */

	/* configure the spi settings */
	channelConf.ClockRate = phost->Parameters.SpiClockrateKHz * 1000;
	channelConf.LatencyTimer = 2;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000; /* FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out) */

	 /* Open the first available channel */
	status = SPI_OpenChannel(phost->Parameters.MpsseChannelNo, (FT_HANDLE *)&phost->SpiHandle);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI open channel failed %d %d\n", phost->Parameters.MpsseChannelNo, phost->SpiHandle);
		return FT_FALSE;
	}
	status = SPI_InitChannel((FT_HANDLE)phost->SpiHandle, &channelConf);
	if (FT_OK != status)
	{
		eve_printf_debug("SPI init channel failed %d %d\n", phost->Parameters.MpsseChannelNo, phost->SpiHandle);
		return FT_FALSE;
	}

	eve_printf_debug("\nhandle=0x%x status=0x%x\n", phost->SpiHandle, status);

	/* Initialize the context variables */
	phost->SpiNumDummy = 1; //by default ft800/801/810/811 goes with single dummy byte for read
	phost->SpiChannel = 0;
	phost->Status = EVE_HalStatusOpened;
	++g_HalPlatform.OpenedChannels;
	return FT_TRUE;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = FT_GPU_HAL_CLOSED;
	--g_HalPlatform.OpenedChannels;
	SPI_CloseChannel(phost->SpiHandle);
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
	/* no-op */
}

#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */