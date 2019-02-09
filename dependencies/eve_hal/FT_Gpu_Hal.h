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
#include "EVE_Cmd.h"

#define FIFO_SIZE_MASK EVE_FIFO_SIZE_MASK
#define FIFO_BYTE_ALIGNMENT_MASK EVE_FIFO_BYTE_ALIGNMENT_MASK

#define FT_GPU_HAL_MODE_E EVE_MODE_T
#define FT_GPU_I2C_MODE EVE_MODE_I2C
#define FT_GPU_SPI_MODE EVE_MODE_SPI

#define FT_GPU_HAL_STATUS_E EVE_STATUS_T
#define FT_GPU_HAL_CLOSED EVE_STATUS_CLOSED
#define FT_GPU_HAL_OPENED EVE_STATUS_OPENED
#define FT_GPU_HAL_READING EVE_STATUS_READING
#define FT_GPU_HAL_WRITING EVE_STATUS_WRITING
#define FT_GPU_HAL_STATUS_ERROR EVE_STATUS_ERROR

#define FT_GPU_TRANSFERDIR_T EVE_TRANSFER_T
#define FT_GPU_READ EVE_TRANSFER_READ
#define FT_GPU_WRITE EVE_TRANSFER_WRITE

#define Ft_Gpu_Hal_Callback_t EVE_Callback
#define Ft_Gpu_Hal_Config_t EVE_HalParameters

typedef struct
{
	ft_uint32_t TotalDevices; //< Total number channels for libmpsse
} Ft_Gpu_HalInit_t;

#define Ft_Gpu_Hal_Context_t EVE_HalContext

/*******************************************************************************/
/*******************************************************************************/
/* The basic APIs Level 1 */
static inline bool Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit)
{
	EVE_HalPlatform *platform = EVE_Hal_initialize();
	halinit->TotalDevices = platform->TotalDevices;
	return !!platform;
}

static inline bool Ft_Gpu_Hal_Open(EVE_HalContext *phost)
{
	EVE_HalParameters parameters;
	EVE_Hal_defaults(&parameters);
	return EVE_Hal_open(phost, &parameters);
}

#define Ft_Gpu_Hal_Close EVE_Hal_close
#define Ft_Gpu_Hal_DeInit EVE_Hal_release

#define Ft_Gpu_Hal_ESD_Idle EVE_Hal_idle

#define Ft_Gpu_Hal_StartTransfer EVE_Hal_startTransfer
#define Ft_Gpu_Hal_Transfer8 EVE_Hal_transfer8
#define Ft_Gpu_Hal_Transfer16 EVE_Hal_transfer16
#define Ft_Gpu_Hal_Transfer32 EVE_Hal_transfer32
#define Ft_Gpu_Hal_EndTransfer EVE_Hal_endTransfer

#define Ft_Gpu_Hal_Rd8 EVE_Hal_rd8
#define Ft_Gpu_Hal_Rd16 EVE_Hal_rd16
#define Ft_Gpu_Hal_Rd32 EVE_Hal_rd32
#define Ft_Gpu_Hal_Wr8 EVE_Hal_wr8
#define Ft_Gpu_Hal_Wr16 EVE_Hal_wr16
#define Ft_Gpu_Hal_Wr32 EVE_Hal_wr32

#define Ft_Gpu_Hal_WrMem EVE_Hal_wrBuffer
#define Ft_Gpu_Hal_WrMem_ProgMem EVE_Hal_wrProgmem

static inline ft_void_t Ft_Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	EVE_Hal_rdBuffer(phost, buffer, addr, length);
}

/*******************************************************************************/
/*******************************************************************************/
/* APIs for coprocessor Fifo read/write and space management */
#define Ft_Gpu_Hal_WrCmd32 EVE_Cmd_wr32

/// Write a buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns FT_FALSE in case a co processor fault occured
#define Ft_Gpu_Hal_WrCmdBuf EVE_Cmd_wrBuffer
#define Ft_Gpu_Hal_WrCmdBuf_ProgMem EVE_Cmd_wrProgmem

/// Wait for the command buffer to fully empty. Returns FT_FALSE in case a co processor fault occured
#define Ft_Gpu_Hal_WaitCmdFifoEmpty EVE_Cmd_waitFlush

/// Wait for the command buffer to have at least the requested amount of free space
#define Ft_Gpu_Hal_WaitCmdFreespace EVE_Cmd_waitSpace

static inline ft_void_t Ft_Gpu_Hal_RdCmdRpWp(EVE_HalContext *phost, ft_uint16_t *rp, ft_uint16_t *wp)
{
	*rp = EVE_Cmd_rp(phost);
	*wp = EVE_Cmd_wp(phost);
}

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

#define FT_GPU_SPI_NUMCHANNELS_T EVE_SPI_CHANNELS_T
#define FT_GPU_SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define FT_GPU_SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define FT_GPU_SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define FT_GPU_SPI_NUMDUMMYBYTES uint8_t
// #define FT_GPU_SPI_ONEDUMMY 1
// #define FT_GPU_SPI_TWODUMMY 2

#define FT_SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define FT_SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define FT_SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define FT_SPI_ONE_DUMMY_BYTE EVE_SPI_ONE_DUMMY_BYTE
#define FT_SPI_TWO_DUMMY_BYTE EVE_SPI_TWO_DUMMY_BYTES

ft_void_t Ft_Gpu_Hal_Sleep(ft_uint32_t ms);
ft_void_t Ft_Gpu_ClockSelect(EVE_HalContext *phost, FT_GPU_PLL_SOURCE_T pllsource);
ft_void_t Ft_Gpu_PLL_FreqSelect(EVE_HalContext *phost, FT_GPU_PLL_FREQ_T freq);
ft_void_t Ft_Gpu_PowerModeSwitch(EVE_HalContext *phost, FT_GPU_POWER_MODE_T pwrmode);
ft_void_t Ft_Gpu_CoreReset(EVE_HalContext *phost);

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
