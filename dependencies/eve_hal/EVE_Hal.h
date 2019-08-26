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
*    In addition, there are some helper functions defined for FT800 coprocessor engine as well as phost commands.
*
*/

#ifndef EVE_HAL__H
#define EVE_HAL__H
#include "EVE_GpuTypes.h"

/**********
** ENUMS **
**********/

typedef enum EVE_MODE_T
{
	EVE_MODE_UNKNOWN = 0,
	EVE_MODE_I2C,
	EVE_MODE_SPI,
} EVE_MODE_T;

typedef enum EVE_STATUS_T
{
	EVE_STATUS_CLOSED = 0,
	EVE_STATUS_OPENED,
	EVE_STATUS_READING,
	EVE_STATUS_WRITING,
	EVE_STATUS_ERROR,
} EVE_STATUS_T;

typedef enum EVE_TRANSFER_T
{
	EVE_TRANSFER_NONE = 0,
	EVE_TRANSFER_READ,
	EVE_TRANSFER_WRITE,
} EVE_TRANSFER_T;

typedef enum EVE_CHIPID_T
{
	EVE_CHIPID_FT800 = 0x0800,
	EVE_CHIPID_FT801 = 0x0801,
	EVE_CHIPID_FT810 = 0x0810,
	EVE_CHIPID_FT811 = 0x0811,
	EVE_CHIPID_FT812 = 0x0812,
	EVE_CHIPID_FT813 = 0x0813,
	EVE_CHIPID_BT815 = 0x0815,
	EVE_CHIPID_BT816 = 0x0816
} EVE_CHIPID_T;

/************
** STRUCTS **
************/

typedef struct EVE_HalContext EVE_HalContext;
typedef bool (*EVE_Callback)(EVE_HalContext *phost);

#if defined(EVE_MULTI_TARGET)
typedef struct EVE_GpuDefs
{
	uint32_t RegId;
	uint32_t RegCpuReset;
	uint32_t RegJ1Int;
	uint32_t RegCmdRead;
	uint32_t RegTouchTransformA;
	uint32_t RegCrc;
	uint32_t RegTrim;
	uint32_t RegTouchDirectXY;
	uint32_t RegDatestamp;
	uint32_t RegCmdBSpace;
	uint32_t RegTracker;
	uint32_t RamDl;
	uint32_t RomFontTableAddress;
	uint32_t RamGSize;
	uint32_t LowFreqBound;
	uint32_t BitmapAddrMask;
	uint32_t ScissorXYMask;
	uint32_t ScissorSizeMask;
} EVE_GpuDefs;

extern EVE_GpuDefs EVE_GpuDefs_FT80X;
extern EVE_GpuDefs EVE_GpuDefs_FT81X;
extern EVE_GpuDefs EVE_GpuDefs_BT81X;

typedef enum EVE_HOST_T
{
	EVE_HOST_UNKNOWN = 0,
	EVE_HOST_BT8XXEMU,
	EVE_HOST_FT4222,
	EVE_HOST_MPSSE,
	EVE_HOST_FT9XX,
	EVE_HOST_PANL70
} EVE_HOST_T;

typedef struct EVE_DeviceInfo
{
	char DisplayName[256];
	char SerialNumber[16];
	EVE_HOST_T Host;
	bool Opened;

} EVE_DeviceInfo;
#endif

/* Hal parameters */
typedef struct EVE_HalParameters
{
	void *UserContext;
	EVE_Callback CbCmdWait; /* Called anytime the code is waiting during CMD write. Return false to abort wait */

#if defined(EVE_MULTI_TARGET)
	EVE_HOST_T Host;
	EVE_CHIPID_T ChipId;
#endif

#if defined(BT8XXEMU_PLATFORM)
	char EmulatorParameters[1640];
	char EmulatorFlashParameters[1144];
#endif

#if defined(MPSSE_PLATFORM)
	uint32_t MpsseChannelNo; /* MPSSE channel number */
#endif

#if defined(FT9XX_PLATFORM) || defined(FT4222_PLATFORM)
	uint32_t DeviceIdx;
	uint8_t SpiCsPin; /* SPI chip select number of FT8XX chip */
#endif

#if defined(FT9XX_PLATFORM) || defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
	uint8_t PowerDownPin; /* FT8XX power down pin number */
#endif

#if defined(FT4222_PLATFORM) || defined(MPSSE_PLATFORM)
	uint16_t SpiClockrateKHz; /* In kHz */
#endif

} EVE_HalParameters;

typedef struct EVE_HalContext
{
	union
	{
		void *UserContext;
		EVE_HalParameters Parameters;
	};

	EVE_STATUS_T Status;

#if defined(EVE_MULTI_TARGET)
	EVE_HOST_T Host;
	EVE_CHIPID_T ChipId;
	uint16_t Revision;
	const EVE_GpuDefs *GpuDefs;
#endif

	uint8_t PCLK;

#if defined(BT8XXEMU_PLATFORM)
	void *Emulator; /* FT8XXEMU_Emulator */
	void *EmulatorFlash; /* FT8XXEMU_Flash */
#endif

#if defined(FT4222_PLATFORM) | defined(MPSSE_PLATFORM)
	void *SpiHandle;
#endif

#if defined(FT4222_PLATFORM)
	void *GpioHandle; /* LibFT4222 uses this member to store GPIO handle */
#endif

#if defined(FT4222_PLATFORM) | defined(MPSSE_PLATFORM)
	/* Currently configured SPI clock rate. In kHz.
	May be different from requested the clock rate in parameters */
	uint16_t SpiClockrateKHz;
#endif

#if defined(BUFFER_OPTIMIZATION) || defined(FT4222_PLATFORM)
	uint8_t SpiWrBuf[0xFFFF];
	uint32_t SpiWrBufIndex;
	uint32_t SpiRamGAddr; /* Current RAM_G address of ongoing SPI write transaction */
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_TARGET)
	bool SpiWpWriting;
	bool SpiWpWritten;
	uint16_t SpiWpWrite;
#endif
#endif

#if (EVE_SUPPORT_CHIPID >= EVE_FT810) || defined(EVE_MULTI_TARGET)
	EVE_SPI_CHANNELS_T SpiChannels; /* Variable to contain single/dual/quad channels */
	uint8_t SpiDummyBytes; /* Number of dummy bytes as 1 or 2 for SPI read */
#endif

	/* Buffer cmd smaller than a full cmd command */
	uint8_t CmdBuffer[4];
	uint8_t CmdBufferIndex;

	uint16_t CmdSpace; /* Free space */
#if !defined(EVE_SUPPORT_CMDB) || defined(EVE_MULTI_TARGET)
	uint16_t CmdWp; /* Write pointer */
#endif

	bool CmdFunc; /* Flagged while transfer to cmd is kept open */
	bool CmdFault; /* Flagged when coprocessor is in fault mode and needs to be reset */
	bool CmdWaiting; /* Flagged while waiting for CMD write (to check during any function that may be called by CbCmdWait) */

} EVE_HalContext;

typedef struct EVE_HalPlatform
{
	uint32_t OpenedDevices;

} EVE_HalPlatform;

/*********
** INIT **
*********/

/* Initialize HAL platform */
EVE_HAL_EXPORT EVE_HalPlatform *EVE_Hal_initialize();

/* Release HAL platform */
EVE_HAL_EXPORT void EVE_Hal_release();

/* List the available devices */
EVE_HAL_EXPORT size_t EVE_Hal_list();

/* Get info of the specified device. Devices of type EVE_HOST_UNKNOWN should be ignored */
EVE_HAL_EXPORT void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx);

/* Check whether the context is the specified device */
EVE_HAL_EXPORT bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx);

/* Get the default configuration parameters */
EVE_HAL_EXPORT void EVE_Hal_defaults(EVE_HalParameters *parameters);
EVE_HAL_EXPORT void EVE_Hal_defaultsEx(EVE_HalParameters *parameters, EVE_CHIPID_T chipId, size_t deviceIdx);

/* Opens a new HAL context using the specified parameters */
EVE_HAL_EXPORT bool EVE_Hal_open(EVE_HalContext *phost, EVE_HalParameters *parameters);

/* Close a HAL context */
EVE_HAL_EXPORT void EVE_Hal_close(EVE_HalContext *phost);

/* Idle. Call regularly to update frequently changing internal state.
This is also called while waiting for cmd, in addition to the user idle callback */
EVE_HAL_EXPORT void EVE_Hal_idle(EVE_HalContext *phost);

/*************
** TRANSFER **
*************/

EVE_HAL_EXPORT void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
EVE_HAL_EXPORT uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value);
EVE_HAL_EXPORT uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value);
EVE_HAL_EXPORT uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value);
EVE_HAL_EXPORT void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
EVE_HAL_EXPORT void EVE_Hal_endTransfer(EVE_HalContext *phost);

/* Flush any pending write transfers */
EVE_HAL_EXPORT void EVE_Hal_flush(EVE_HalContext *phost);

/*********************
** TRANSFER HELPERS **
*********************/

EVE_HAL_EXPORT uint8_t EVE_Hal_rd8(EVE_HalContext *phost, uint32_t addr);
EVE_HAL_EXPORT uint16_t EVE_Hal_rd16(EVE_HalContext *phost, uint32_t addr);
EVE_HAL_EXPORT uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr);
EVE_HAL_EXPORT void EVE_Hal_rdMem(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size);

EVE_HAL_EXPORT void EVE_Hal_wr8(EVE_HalContext *phost, uint32_t addr, uint8_t v);
EVE_HAL_EXPORT void EVE_Hal_wr16(EVE_HalContext *phost, uint32_t addr, uint16_t v);
EVE_HAL_EXPORT void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v);
EVE_HAL_EXPORT void EVE_Hal_wrMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT void EVE_Hal_wrProgmem(EVE_HalContext *phost, uint32_t addr, eve_progmem_const uint8_t *buffer, uint32_t size);
EVE_HAL_EXPORT void EVE_Hal_wrString(EVE_HalContext *phost, uint32_t addr, const char *str, uint32_t index, uint32_t size, uint32_t padMask);

/*********
** CAPS **
*********/

/* Screen based on chip id. This function compiles as a constant on single supported chipid target */
static inline bool EVE_Hal_isScreenCapacitive(EVE_HalContext *phost)
{
	return (EVE_CHIPID & 0x01) == 0x01;
}

/* Screen based on chip id. This function compiles as a constant on single supported chipid target */
static inline bool EVE_Hal_isScreenResistive(EVE_HalContext *phost)
{
	return !EVE_Hal_isScreenCapacitive(phost);
}

static inline bool EVE_Hal_supportFlash(EVE_HalContext *phost)
{
#ifdef EVE_SUPPORT_FLASH
	return EVE_CHIPID >= EVE_BT815;
#else
	return false;
#endif
}

static inline bool EVE_Hal_supportCmdB(EVE_HalContext *phost)
{
#ifdef EVE_SUPPORT_CMDB
	return EVE_CHIPID >= EVE_FT810;
#else
	return false;
#endif
}

/************
** UTILITY **
************/

EVE_HAL_EXPORT void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd);

/* This API sends a 3byte command to the phost */
EVE_HAL_EXPORT void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd);

/* Toggle PD_N pin of FT800 board for a power cycle */
EVE_HAL_EXPORT void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up);

/* Switch EVE to different SPI channel mode */
EVE_HAL_EXPORT void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy);

EVE_HAL_EXPORT uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost);

EVE_HAL_EXPORT int32_t EVE_Hal_clockTrimming(EVE_HalContext *phost, uint32_t lowFreq);

/*********
** HOST **
*********/

EVE_HAL_EXPORT void EVE_Host_clockSelect(EVE_HalContext *phost, EVE_PLL_SOURCE_T pllsource);
EVE_HAL_EXPORT void EVE_Host_pllFreqSelect(EVE_HalContext *phost, EVE_PLL_FREQ_T freq);
EVE_HAL_EXPORT void EVE_Host_powerModeSwitch(EVE_HalContext *phost, EVE_POWER_MODE_T pwrmode);
EVE_HAL_EXPORT void EVE_Host_coreReset(EVE_HalContext *phost);

#if (EVE_SUPPORT_CHIPID >= EVE_FT810)
/* This API can only be called when PLL is stopped(SLEEP mode).
For compatibility, set frequency to the EVE_GPU_12MHZ option in the EVE_SETPLLSP1_T table. */
EVE_HAL_EXPORT void EVE_Host_selectSysClk(EVE_HalContext *phost, EVE_81X_PLL_FREQ_T freq);

/* Power down or up ROMs and ADCs.
Specified one or more elements in the EVE_81X_ROM_AND_ADC_T 
table to power down, unspecified elements will be powered up.
The application must retain the state of the ROMs and ADCs 
as they're not readable from the device. */
EVE_HAL_EXPORT void EVE_Host_powerOffComponents(EVE_HalContext *phost, uint8_t val);

/* This API sets the current strength of supported GPIO/IO group(s) */
EVE_HAL_EXPORT void EVE_Host_padDriveStrength(EVE_HalContext *phost, EVE_81X_GPIO_DRIVE_STRENGTH_T strength, EVE_81X_GPIO_GROUP_T group);

/* This API will hold the system reset active, 
EVE_Host_resetRemoval() must be called to release the system reset. */
EVE_HAL_EXPORT void EVE_Host_resetActive(EVE_HalContext *phost);

/* This API will release the system reset, 
and the system will exit reset and behave as after POR, 
settings done through SPI commands will not be affected. */
EVE_HAL_EXPORT void EVE_Host_resetRemoval(EVE_HalContext *phost);
#endif

/*********
** MISC **
*********/

EVE_HAL_EXPORT uint32_t EVE_millis();
EVE_HAL_EXPORT void EVE_sleep(uint32_t ms);

#endif /* #ifndef EVE_HAL__H */

/* end of file */