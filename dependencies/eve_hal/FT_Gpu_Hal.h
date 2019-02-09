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

#ifndef FT_GPU_HAL_H
#define FT_GPU_HAL_H

#include "EVE_Hal.h"

#define FIFO_SIZE_MASK EVE_FIFO_SIZE_MASK
#define FIFO_BYTE_ALIGNMENT_MASK EVE_FIFO_BYTE_ALIGNMENT_MASK

#define FT_GPU_HAL_MODE_E EVE_HalMode
#define FT_GPU_I2C_MODE EVE_HAL_MODE_I2C
#define FT_GPU_SPI_MODE EVE_HAL_MODE_SPI

#define FT_GPU_HAL_STATUS_E EVE_HalStatus
#define FT_GPU_HAL_STATUS_ERROR EVE_HAL_STATUS_UNKNOWN
#define FT_GPU_HAL_OPENED EVE_HAL_STATUS_OPENED
#define FT_GPU_HAL_READING EVE_HAL_STATUS_READING
#define FT_GPU_HAL_WRITING EVE_HAL_STATUS_WRITING
#define FT_GPU_HAL_CLOSED EVE_HAL_STATUS_CLOSED

#define FT_GPU_TRANSFERDIR_T EVE_HalTransfer
#define FT_GPU_READ EVE_HAL_TRANSFER_READ
#define FT_GPU_WRITE EVE_HAL_TRANSFER_WRITE

typedef struct
{
	union
	{
		ft_uint8_t spi_cs_pin_no; //< SPI chip select number of FT8XX chip
		ft_uint8_t i2c_addr; //< I2C address of FT8XX chip
	};
	union
	{
		ft_uint16_t spi_clockrate_khz; //< In kHz
		ft_uint16_t i2c_clockrate_khz; //< In kHz
	};
	ft_uint8_t channel_no; //< MPSSE channel number
	ft_uint8_t pdn_pin_no; //< FT8XX power down pin number
} Ft_Gpu_Hal_Config_t;

typedef struct
{
	ft_uint32_t total_channel_num; //< Total number channels for libmpsse
} Ft_Gpu_HalInit_t;

typedef ft_bool_t (*Ft_Gpu_Hal_Callback_t)(void *phost);

typedef struct EVE_HalContext
{
	void *AppContext;
	Ft_Gpu_Hal_Config_t HalConfig;
	FT_GPU_HAL_STATUS_E Status; //< OUT

	// uint16_t CmdFifoWp; //< OUT. coprocessor fifo write pointer

	union
	{
		ft_void_t *SpiHandle; //< IN/OUT
		ft_void_t *Emulator; //< OUT. FT8XXEMU_Emulator
	};
	union
	{
		ft_void_t *GpioHandle; //< IN/OUT. LibFT4222 uses this member to store GPIO handle
		ft_void_t *EmulatorFlash; //< OUT. FT8XXEMU_Flash
	};

	/* Additions specific to FT81X */
	ft_uint8_t SpiChannel; //< Variable to contain single/dual/quad channels
	ft_uint8_t SpiNumDummy; //< Number of dummy bytes as 1 or 2 for SPI read
	ft_uint8_t *SpiWriBufPtr;

	ft_bool_t CmdFrame; //< OUT. Set while inside a co cmd software buffering frame
	ft_bool_t CmdFault; //< OUT. Set when co processor is in fault mode and needs to be reset
	ft_bool_t CmdWaiting; //< OUT. Set to FT_TRUE while waiting for CMD write (check during any function that may be called by cb_cmd_wait)
	Ft_Gpu_Hal_Callback_t CbCmdWait; //< IN. Called anytime the code is waiting during CMD write. Return FT_FALSE to abort wait

} EVE_HalContext;
#define Ft_Gpu_Hal_Context_t EVE_HalContext

/* Display parameters */
typedef struct EVE_DisplayParameters
{
	ft_int16_t Width;
	ft_int16_t Height;
	ft_int16_t HCycle;
	ft_int16_t HOffset;
	ft_int16_t HSync0;
	ft_int16_t HSync1;
	ft_int16_t VCycle;
	ft_int16_t VOffset;
	ft_int16_t VSync0;
	ft_int16_t VSync1;
	ft_uint8_t PCLK;
	ft_char8_t Swizzle;
	ft_char8_t PCLKPol;
	ft_char8_t CSpread;
	ft_bool_t Dither;
} Eve_DisplayParameters;

/* Hal paramters */
typedef struct EVE_Paramaters
{
	void *AppContext;
	Eve_DisplayParameters Display;
} EVE_Paramaters;

/*******************************************************************************/
/*******************************************************************************/
/* The basic APIs Level 1 */
ft_bool_t Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit);
ft_bool_t Ft_Gpu_Hal_Open(EVE_HalContext *phost);
ft_void_t Ft_Gpu_Hal_ESD_Idle(EVE_HalContext *phost);

/* The APIs for reading/writing transfer continuously only with small buffer system */
ft_void_t Ft_Gpu_Hal_StartTransfer(EVE_HalContext *phost, FT_GPU_TRANSFERDIR_T rw, ft_uint32_t addr);
ft_uint8_t Ft_Gpu_Hal_Transfer8(EVE_HalContext *phost, ft_uint8_t value);
ft_uint16_t Ft_Gpu_Hal_Transfer16(EVE_HalContext *phost, ft_uint16_t value);
ft_uint32_t Ft_Gpu_Hal_Transfer32(EVE_HalContext *phost, ft_uint32_t value);
ft_void_t Ft_Gpu_Hal_EndTransfer(EVE_HalContext *phost);

/* Close and de-init APIs */
ft_void_t Ft_Gpu_Hal_Close(EVE_HalContext *phost);
ft_void_t Ft_Gpu_Hal_DeInit();

/* Helper function APIs Read */
ft_uint8_t Ft_Gpu_Hal_Rd8(EVE_HalContext *phost, ft_uint32_t addr);
ft_uint16_t Ft_Gpu_Hal_Rd16(EVE_HalContext *phost, ft_uint32_t addr);
ft_uint32_t Ft_Gpu_Hal_Rd32(EVE_HalContext *phost, ft_uint32_t addr);

/* Helper function APIs Write */
ft_void_t Ft_Gpu_Hal_Wr8(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t v);
ft_void_t Ft_Gpu_Hal_Wr16(EVE_HalContext *phost, ft_uint32_t addr, ft_uint16_t v);
ft_void_t Ft_Gpu_Hal_Wr32(EVE_HalContext *phost, ft_uint32_t addr, ft_uint32_t v);

/*******************************************************************************/
/*******************************************************************************/
/* APIs for coprocessor Fifo read/write and space management */
ft_void_t Ft_Gpu_Hal_WrCmd32(EVE_HalContext *phost, ft_uint32_t cmd);

/// Write a buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns FT_FALSE in case a co processor fault occured
ft_bool_t Ft_Gpu_Hal_WrCmdBuf(EVE_HalContext *phost, ft_uint8_t *buffer, ft_uint32_t count);
ft_bool_t Ft_Gpu_Hal_WrCmdBuf_ProgMem(EVE_HalContext *phost, ft_prog_uchar8_t *buffer, ft_uint32_t count);

/// Wait for the command buffer to fully empty. Returns FT_FALSE in case a co processor fault occured
ft_bool_t Ft_Gpu_Hal_WaitCmdFifoEmpty(EVE_HalContext *phost);

/// Wait for the command buffer to have at least the requested amount of free space
ft_bool_t Ft_Gpu_Hal_WaitCmdFreespace(EVE_HalContext *phost, ft_uint32_t bytes);

/// Reads co processor read pointer and write pointer
ft_void_t Ft_Gpu_Hal_RdCmdRpWp(EVE_HalContext *phost, ft_uint16_t *rp, ft_uint16_t *wp);

/*******************************************************************************/
/*******************************************************************************/
/* Power cycle */
ft_void_t Ft_Gpu_Hal_Powercycle(EVE_HalContext *phost, ft_bool_t up);

/*******************************************************************************/
/*******************************************************************************/
/* APIs for Host Commands */
typedef enum
{
	FT_GPU_INTERNAL_OSC = 0x48, // default
	FT_GPU_EXTERNAL_OSC = 0x44,
} FT_GPU_PLL_SOURCE_T;

typedef enum
{
	FT_GPU_PLL_48M = 0x62, // default
	FT_GPU_PLL_36M = 0x61,
	FT_GPU_PLL_24M = 0x64,
} FT_GPU_PLL_FREQ_T;

typedef enum
{
	FT_GPU_ACTIVE_M = 0x00,
	FT_GPU_STANDBY_M = 0x41, // default
	FT_GPU_SLEEP_M = 0x42,
	FT_GPU_POWERDOWN_M = 0x50,
} FT_GPU_POWER_MODE_T;

#if (EVE_MODEL >= EVE_FT810)
typedef enum
{
	FT_GPU_SYSCLK_DEFAULT = 0x00, // default 60mhz
	FT_GPU_SYSCLK_72M = 0x06,
	FT_GPU_SYSCLK_60M = 0x05,
	FT_GPU_SYSCLK_48M = 0x04,
	FT_GPU_SYSCLK_36M = 0x03,
	FT_GPU_SYSCLK_24M = 0x02,
} FT_GPU_81X_PLL_FREQ_T;

typedef enum
{
	FT_GPU_MAIN_ROM = 0x80, // main graphicas ROM used
	FT_GPU_RCOSATAN_ROM = 0x40, // line slope table used for
	FT_GPU_SAMPLE_ROM = 0x20, // JA samples
	FT_GPU_JABOOT_ROM = 0x10, // JA microcode
	FT_GPU_J1BOOT_ROM = 0x08, // J1 microcode
	FT_GPU_ADC = 0x01,
	FT_GPU_POWER_ON_ROM_AND_ADC = 0x00, // specify this element to power on all ROMs and ADCs
} FT_GPU_81X_ROM_AND_ADC_T;

typedef enum
{
	FT_GPU_5MA = 0x00, //default current
	FT_GPU_10MA = 0x01,
	FT_GPU_15MA = 0x02,
	FT_GPU_20MA = 0x03,
} FT_GPU_81X_GPIO_DRIVE_STRENGTH_T;

typedef enum
{
	FT_GPU_GPIO0 = 0x00,
	FT_GPU_GPIO1 = 0x04,
	FT_GPU_GPIO2 = 0x08,
	FT_GPU_GPIO3 = 0x0C,
	FT_GPU_GPIO4 = 0x10,
	FT_GPU_DISP = 0x20,
	FT_GPU_DE = 0x24,
	FT_GPU_VSYNC_HSYNC = 0x28,
	FT_GPU_PCLK = 0x2C,
	FT_GPU_BACKLIGHT = 0x30,
	FT_GPU_R_G_B = 0x34,
	FT_GPU_AUDIO_L = 0x38,
	FT_GPU_INT_N = 0x3C,
	FT_GPU_TOUCHWAKE = 0x40,
	FT_GPU_SCL = 0x44,
	FT_GPU_SDA = 0x48,
	FT_GPU_SPI_MISO_MOSI_IO2_IO3 = 0x4C,
} FT_GPU_81X_GPIO_GROUP_T;

#define FT_GPU_81X_RESET_ACTIVE 0x000268
#define FT_GPU_81X_RESET_REMOVAL 0x002068
#endif

#define FT_GPU_CORE_RESET (0x68)

#define FT_COCMD_FAULT(rp) (rp & 0x3)

/* Enums for number of SPI dummy bytes and number of channels */
typedef enum
{
	FT_GPU_SPI_SINGLE_CHANNEL = 0,
	FT_GPU_SPI_DUAL_CHANNEL = 1,
	FT_GPU_SPI_QUAD_CHANNEL = 2,
} FT_GPU_SPI_NUMCHANNELS_T;

typedef enum
{
	FT_GPU_SPI_ONEDUMMY = 1,
	FT_GPU_SPI_TWODUMMY = 2,
} FT_GPU_SPI_NUMDUMMYBYTES;

#define FT_SPI_ONE_DUMMY_BYTE (0x00)
#define FT_SPI_TWO_DUMMY_BYTE (0x04)
#define FT_SPI_SINGLE_CHANNEL (0x00)
#define FT_SPI_DUAL_CHANNEL (0x01)
#define FT_SPI_QUAD_CHANNEL (0x02)

ft_void_t Ft_Gpu_Hal_Sleep(ft_uint32_t ms);
ft_void_t Ft_Gpu_ClockSelect(EVE_HalContext *phost, FT_GPU_PLL_SOURCE_T pllsource);
ft_void_t Ft_Gpu_PLL_FreqSelect(EVE_HalContext *phost, FT_GPU_PLL_FREQ_T freq);
ft_void_t Ft_Gpu_PowerModeSwitch(EVE_HalContext *phost, FT_GPU_POWER_MODE_T pwrmode);
ft_void_t Ft_Gpu_CoreReset(EVE_HalContext *phost);
ft_void_t Ft_Gpu_Hal_StartTransfer(EVE_HalContext *phost, FT_GPU_TRANSFERDIR_T rw, ft_uint32_t addr);
ft_void_t Ft_Gpu_Hal_WrMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_uint8_t *buffer, ft_uint32_t length);
ft_void_t Ft_Gpu_Hal_WrMem_ProgMem(EVE_HalContext *phost, ft_uint32_t addr, const ft_prog_uchar8_t *buffer, ft_uint32_t length);
ft_void_t Ft_Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length);
ft_bool_t Ft_Gpu_Hal_WaitLogo_Finish(EVE_HalContext *phost);
ft_int16_t Ft_Gpu_Hal_TransferString(EVE_HalContext *phost, const ft_char8_t *str);
ft_int16_t Ft_Gpu_Hal_TransferString_S(EVE_HalContext *phost, const ft_char8_t *str, int length);
ft_void_t Ft_Gpu_HostCommand(EVE_HalContext *phost, ft_uint8_t cmd);
ft_void_t Ft_Gpu_HostCommand_Ext3(EVE_HalContext *phost, ft_uint32_t cmd);
ft_int32_t Ft_Gpu_Hal_Dec2Ascii(ft_char8_t *pSrc, ft_int32_t value);

#if (EVE_MODEL >= EVE_FT810)
ft_int16_t Ft_Gpu_Hal_SetSPI(EVE_HalContext *phost, FT_GPU_SPI_NUMCHANNELS_T numchnls, FT_GPU_SPI_NUMDUMMYBYTES numdummy);
ft_void_t Ft_Gpu_81X_SelectSysCLK(EVE_HalContext *phost, FT_GPU_81X_PLL_FREQ_T freq);
ft_void_t Ft_GPU_81X_PowerOffComponents(EVE_HalContext *phost, ft_uint8_t val);
ft_void_t Ft_GPU_81X_PadDriveStrength(EVE_HalContext *phost, FT_GPU_81X_GPIO_DRIVE_STRENGTH_T strength, FT_GPU_81X_GPIO_GROUP_T group);
ft_void_t Ft_Gpu_81X_ResetActive(EVE_HalContext *phost);
ft_void_t Ft_Gpu_81X_ResetRemoval(EVE_HalContext *phost);
#endif

ft_uint32_t Ft_Gpu_CurrentFrequency(EVE_HalContext *phost);
ft_int32_t Ft_Gpu_ClockTrimming(EVE_HalContext *phost, ft_uint32_t lowFreq);
ft_void_t Ft_Gpu_DownloadJ1Firmware(EVE_HalContext *phost);

/* Globals for polling implementation */
extern ft_uint32_t ft_millis_curr;
extern ft_uint32_t ft_millis_prev;

ft_void_t ft_millis_ticker();
ft_void_t ft_millis_init();
ft_uint32_t ft_millis();
ft_void_t ft_millis_exit();

#if defined(PANL70) || defined(PANL70PLUS)
ft_void_t Ft_Gpu_Panl70_GOODIXGPIO(EVE_HalContext *phost);
#endif

#if defined(BT8XXEMU_PLATFORM) || defined(ESD_SIMULATION)
#include <FT_Gpu_Hal_Simulation.h>
#elif defined(MSVC_PLATFORM)
#include <FT_Gpu_Hal_MSVC.h>
#elif defined(FT900_PLATFORM)
#include <FT_Gpu_Hal_FT900.h>
#endif

ft_void_t Ft_DisplayPanel_Init();

#endif /* FT_GPU_HAL_H */
