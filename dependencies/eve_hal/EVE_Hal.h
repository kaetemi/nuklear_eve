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

#ifndef EVE_HAL__H
#define EVE_HAL__H
#include "EVE_Config.h"

#define EVE_FIFO_SIZE_MASK (0xFFF) // FIFO valid range from 0 to 4095
#define EVE_FIFO_BYTE_ALIGNMENT_MASK (0xFFC)

typedef enum EVE_HalMode
{
	EVE_HalModeUnknown = 0,
	EVE_HalModeI2C,
	EVE_HalModeSPI,
} EVE_HalMode;

typedef enum EVE_HalStatus
{
	EVE_HalStatusClosed = 0,
	EVE_HalStatusOpened,
	EVE_HalStatusReading,
	EVE_HalStatusWriting,
	EVE_HalStatusError,
} EVE_HalStatus;

typedef enum EVE_HalTransfer
{
	EVE_HalTransferNone = 0,
	EVE_HalTransferRead,
	EVE_HalTransferWrite,
} EVE_HalTransfer;

/* Display parameters */
typedef struct EVE_DisplayParameters
{
	int16_t Width;
	int16_t Height;
	int16_t HCycle;
	int16_t HOffset;
	int16_t HSync0;
	int16_t HSync1;
	int16_t VCycle;
	int16_t VOffset;
	int16_t VSync0;
	int16_t VSync1;
	uint8_t PCLK;
	int8_t Swizzle;
	int8_t PCLKPol;
	int8_t CSpread;
	bool Dither;
} Eve_DisplayParameters;

typedef int (*EVE_Callback)(void *phost);

/* Hal parameters */
typedef struct EVE_HalParameters
{
	void *UserContext;
	EVE_Callback CbCmdWait; //< IN. Called anytime the code is waiting during CMD write. Return FT_FALSE to abort wait

	Eve_DisplayParameters Display;

	union
	{
		uint8_t SpiCsPin; //< SPI chip select number of FT8XX chip
		uint8_t I2CAddr; //< I2C address of FT8XX chip
	};
	union
	{
		uint16_t SpiClockrateKHz; //< In kHz
		uint16_t I2CClockrateKHz; //< In kHz
	};

#ifdef MPSSE_PLATFORM
	uint8_t MpsseChannelNo; //< MPSSE channel number
#endif

	uint8_t PowerDownPin; //< FT8XX power down pin number
} EVE_HalParameters;

typedef struct EVE_HalContext
{
	union
	{
		void *UserContext;
		EVE_HalParameters Parameters;
	};

	EVE_HalStatus Status;

	/* uint16_t CmdFifoWp; Coprocessor fifo write pointer */

#ifdef BT8XXEMU_PLATFORM
	void *Emulator; /* FT8XXEMU_Emulator */
	void *EmulatorFlash; /* FT8XXEMU_Flash */
#endif

#ifdef FT4222_PLATFORM
	void *SpiHandle;
	void *GpioHandle; /* LibFT4222 uses this member to store GPIO handle */
	uint8_t *SpiWriBufPtr;
#endif

#if defined(FT900_PLATFORM) || defined(FT4222_PLATFORM)
	uint8_t SpiChannel; /* Variable to contain single/dual/quad channels */
	uint8_t SpiNumDummy; /* Number of dummy bytes as 1 or 2 for SPI read */
#endif

	bool CmdFrame; /* Flagged while inside a co cmd software buffering frame */
	bool CmdFault; /* Flagged when co processor is in fault mode and needs to be reset */
	bool CmdWaiting; /* Flagged while waiting for CMD write (to check during any function that may be called by CbCmdWait) */

} EVE_HalContext;

typedef struct EVE_HalInit
{
	uint32_t TotalChannels;
	uint32_t OpenedContexts;

} EVE_HalPlatform;

/* Initialize HAL platform */
EVE_HalPlatform *EVE_Hal_initialize();

/* Release HAL platform */
void EVE_Hal_release();

/* Get the default configuration parameters */
void EVE_Hal_defaults(EVE_HalParameters *parameters);

/* Opens a new HAL context using the specified parameters */
bool EVE_Hal_open(EVE_HalContext *phost, EVE_HalParameters *parameters);

/* Close a HAL context */
void EVE_Hal_close(EVE_HalContext *phost);

/* Idle. Call regularly to update frequently changing internal state */
void EVE_Hal_idle(EVE_HalContext *phost);

#endif /* #ifndef EVE_HAL__H */

/* end of file */