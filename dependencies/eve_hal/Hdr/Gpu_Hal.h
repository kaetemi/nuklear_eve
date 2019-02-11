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

#ifndef GPU_HAL__H
#define GPU_HAL__H

#include "FT_Gpu_Hal.h"

#include <stdlib.h>

#define PROGMEM eve_progmem
#define PROGMEM_CONST eve_progmem_const

#define FIFO_SIZE_MASK EVE_FIFO_SIZE_MASK
#define FIFO_BYTE_ALIGNMENT_MASK EVE_FIFO_BYTE_ALIGNMENT_MASK
#define CMD_SIZE 4

#define GPU_HAL_MODE_E EVE_MODE_T
#define GPU_I2C_MODE EVE_MODE_I2C
#define GPU_SPI_MODE EVE_MODE_SPI

#define GPU_HAL_STATUS_E EVE_STATUS_T
#define GPU_HAL_CLOSED EVE_STATUS_CLOSED
#define GPU_HAL_OPENED EVE_STATUS_OPENED
#define GPU_HAL_READING EVE_STATUS_READING
#define GPU_HAL_WRITING EVE_STATUS_WRITING
#define GPU_HAL_STATUS_ERROR EVE_STATUS_ERROR

#define GPU_TRANSFERDIR_T EVE_TRANSFER_T
#define GPU_READ EVE_TRANSFER_READ
#define GPU_WRITE EVE_TRANSFER_WRITE

#define Gpu_Hal_Callback_t EVE_Callback
#define Gpu_Hal_Config_t EVE_HalParameters

typedef struct
{
	ft_uint32_t TotalDevices; //< Total number channels for libmpsse
} Gpu_HalInit_t;

#define Gpu_Hal_Context_t EVE_HalContext

/*******************************************************************************/
/*******************************************************************************/
/* The basic APIs Level 1 */
static inline bool Gpu_Hal_Init(Gpu_HalInit_t *halinit)
{
	EVE_HalPlatform *platform = EVE_Hal_initialize();
	halinit->TotalDevices = platform->TotalDevices;
	return !!platform;
}

static inline bool Gpu_Hal_Open(EVE_HalContext *phost)
{
	EVE_HalParameters parameters;
	EVE_Hal_defaults(&parameters);
	return EVE_Hal_open(phost, &parameters);
}

#define Gpu_Hal_Close EVE_Hal_close
#define Gpu_Hal_DeInit EVE_Hal_release

#define Gpu_Hal_ESD_Idle EVE_Hal_idle

#define Gpu_Hal_StartTransfer EVE_Hal_startTransfer
#define Gpu_Hal_Transfer8 EVE_Hal_transfer8
#define Gpu_Hal_Transfer16 EVE_Hal_transfer16
#define Gpu_Hal_Transfer32 EVE_Hal_transfer32
#define Gpu_Hal_EndTransfer EVE_Hal_endTransfer

#define Gpu_Hal_Rd8 EVE_Hal_rd8
#define Gpu_Hal_Rd16 EVE_Hal_rd16
#define Gpu_Hal_Rd32 EVE_Hal_rd32
#define Gpu_Hal_Wr8 EVE_Hal_wr8
#define Gpu_Hal_Wr16 EVE_Hal_wr16
#define Gpu_Hal_Wr32 EVE_Hal_wr32

#define Gpu_Hal_WrMem EVE_Hal_wrMem
#define Gpu_Hal_WrMem_ProgMem EVE_Hal_wrProgmem

static inline ft_void_t Gpu_Hal_RdMem(EVE_HalContext *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	EVE_Hal_rdMem(phost, buffer, addr, length);
}

/*******************************************************************************/
/*******************************************************************************/
/* APIs for coprocessor Fifo read/write and space management */
#define Gpu_Hal_WrCmd32 EVE_Cmd_wr32

/// Write a buffer to the command buffer. Waits if there is not enough space in the command buffer. Returns FALSE in case a co processor fault occured
#define Gpu_Hal_WrCmdBuf EVE_Cmd_wrMem
#define Gpu_Hal_WrCmdBuf_ProgMem EVE_Cmd_wrProgmem

/// Wait for the command buffer to fully empty. Returns FALSE in case a co processor fault occured
#define Gpu_Hal_WaitCmdFifoEmpty EVE_Cmd_waitFlush

/// Wait for the command buffer to have at least the requested amount of free space
#define Gpu_Hal_WaitCmdFreespace EVE_Cmd_waitSpace

/*
// Use the provided wait functions!
static inline ft_void_t Gpu_Hal_RdCmdRpWp(EVE_HalContext *phost, ft_uint16_t *rp, ft_uint16_t *wp)
{
	*rp = EVE_Cmd_rp(phost);
	*wp = EVE_Cmd_wp(phost);
}
*/

/*******************************************************************************/
/*******************************************************************************/

#define Gpu_CoCmd_SendCmd EVE_Cmd_wr32
inline static ft_void_t Gpu_CoCmd_SendCmdArr(EVE_HalContext *phost, ft_uint32_t *cmd, ft_size_t nb)
{
	EVE_Cmd_wrMem(phost, (uint8_t *)cmd, (uint32_t)nb * 4);
}
#define Gpu_CoCmd_SendStr(phost, str) EVE_Cmd_wrString(phost, str, EVE_CMD_STRING_MAX)
#define Gpu_CoCmd_SendStr_S EVE_Cmd_wrString
#define Gpu_CoCmd_StartFrame(phost) eve_noop()
#define Gpu_CoCmd_EndFrame(phost) eve_noop()

#define Eve_CoCmd_SendCmd Gpu_CoCmd_SendCmd
#define Eve_CoCmd_SendCmdArr Gpu_CoCmd_SendCmdArr
#define Eve_CoCmd_SendStr Gpu_CoCmd_SendStr
#define Eve_CoCmd_SendStr_S Gpu_CoCmd_SendStr_S
#define Eve_CoCmd_StartFrame Gpu_CoCmd_StartFrame
#define Eve_CoCmd_EndFrame Gpu_CoCmd_EndFrame
#define Gpu_Copro_SendCmd Gpu_CoCmd_SendCmd

#define Gpu_CoCmd_StartFunc(phost, size) EVE_Cmd_startFunc(phost)
#define Gpu_CoCmd_EndFunc(phost, size) EVE_Cmd_endFunc(phost)

#define Gpu_Fonts EVE_Gpu_Fonts
#define Gpu_Fonts_t EVE_Gpu_Fonts
#define Gpu_FontsExt EVE_Gpu_FontsExt
#define Gpu_FontsExt_t EVE_Gpu_FontsExt
#define GPU_NUMCHAR_PERFONT EVE_GPU_NUMCHAR_PERFONT
#define GPU_FONT_TABLE_SIZE EVE_GPU_FONT_TABLE_SIZE

/*******************************************************************************/
/*******************************************************************************/
/* APIs for Host Commands */
#define GPU_INTERNAL_OSC EVE_INTERNAL_OSC
#define GPU_EXTERNAL_OSC EVE_EXTERNAL_OSC
#define GPU_PLL_SOURCE_T EVE_PLL_SOURCE_T

#define GPU_PLL_48M EVE_PLL_48M
#define GPU_PLL_36M EVE_PLL_36M
#define GPU_PLL_24M EVE_PLL_24M
#define GPU_PLL_FREQ_T EVE_PLL_FREQ_T

#define GPU_ACTIVE_M EVE_ACTIVE_M
#define GPU_STANDBY_M EVE_STANDBY_M
#define GPU_SLEEP_M EVE_SLEEP_M
#define GPU_POWERDOWN_M EVE_POWERDOWN_M
#define GPU_POWER_MODE_T EVE_POWER_MODE_T

#if (EVE_MODEL >= EVE_FT810)
#define GPU_SYSCLK_DEFAULT EVE_SYSCLK_DEFAULT
#define GPU_SYSCLK_72M EVE_SYSCLK_72M
#define GPU_SYSCLK_60M EVE_SYSCLK_60M
#define GPU_SYSCLK_48M EVE_SYSCLK_48M
#define GPU_SYSCLK_36M EVE_SYSCLK_36M
#define GPU_SYSCLK_24M EVE_SYSCLK_24M
#define GPU_81X_PLL_FREQ_T EVE_81X_PLL_FREQ_T

#define GPU_MAIN_ROM EVE_MAIN_ROM
#define GPU_RCOSATAN_ROM EVE_RCOSATAN_ROM
#define GPU_SAMPLE_ROM EVE_SAMPLE_ROM
#define GPU_JABOOT_ROM EVE_JABOOT_ROM
#define GPU_J1BOOT_ROM EVE_J1BOOT_ROM
#define GPU_ADC EVE_ADC
#define GPU_POWER_ON_ROM_AND_ADC EVE_POWER_ON_ROM_AND_ADC
#define GPU_81X_ROM_AND_ADC_T EVE_81X_ROM_AND_ADC_T

#define GPU_5MA EVE_5MA
#define GPU_10MA EVE_10MA
#define GPU_15MA EVE_15MA
#define GPU_20MA EVE_20MA
#define GPU_81X_GPIO_DRIVE_STRENGTH_T EVE_81X_GPIO_DRIVE_STRENGTH_T

#define GPU_GPIO0 EVE_GPIO0
#define GPU_GPIO1 EVE_GPIO1
#define GPU_GPIO2 EVE_GPIO2
#define GPU_GPIO3 EVE_GPIO3
#define GPU_GPIO4 EVE_GPIO4
#define GPU_DISP EVE_DISP
#define GPU_DE EVE_DE
#define GPU_VSYNC_HSYNC EVE_VSYNC_HSYNC
#define GPU_PCLK EVE_PCLK
#define GPU_BACKLIGHT EVE_BACKLIGHT
#define GPU_R_G_B EVE_R_G_B
#define GPU_AUDIO_L EVE_AUDIO_L
#define GPU_INT_N EVE_INT_N
#define GPU_TOUCHWAKE EVE_TOUCHWAKE
#define GPU_SCL EVE_SCL
#define GPU_SDAEVE_SDA
#define GPU_SPI_MISO_MOSI_IO2_IO3 EVE_SPI_MISO_MOSI_IO2_IO3
#define GPU_81X_GPIO_GROUP_T EVE_81X_GPIO_GROUP_T

#define GPU_81X_RESET_ACTIVE EVE_81X_RESET_ACTIVE
#define GPU_81X_RESET_REMOVAL EVE_81X_RESET_REMOVAL
#endif

#define GPU_CORE_RESET EVE_CORE_RESET

#define COCMD_FAULT(rp) EVE_CMD_FAULT(rp)

#define GPU_SPI_NUMCHANNELS_T EVE_SPI_CHANNELS_T
#define GPU_SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define GPU_SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define GPU_SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define GPU_SPI_NUMDUMMYBYTES uint8_t
#define GPU_SPI_ONEDUMMY 1
#define GPU_SPI_TWODUMMY 2

#define SPI_SINGLE_CHANNEL EVE_SPI_SINGLE_CHANNEL
#define SPI_DUAL_CHANNEL EVE_SPI_DUAL_CHANNEL
#define SPI_QUAD_CHANNEL EVE_SPI_QUAD_CHANNEL

#define SPI_ONE_DUMMY_BYTE EVE_SPI_ONE_DUMMY_BYTE
#define SPI_TWO_DUMMY_BYTE EVE_SPI_TWO_DUMMY_BYTES

#define ft_delay EVE_sleep

#define Gpu_Hal_WaitLogo_Finish EVE_Cmd_waitLogo

inline static ft_int16_t Gpu_Hal_TransferString(EVE_HalContext *phost, const ft_char8_t *str)
{
	return EVE_Hal_transferString(phost, str, 0, EVE_CMD_STRING_MAX, 0) - 1;
}

inline static ft_int16_t Gpu_Hal_TransferString_S(EVE_HalContext *phost, const ft_char8_t *str, int length)
{
	return EVE_Hal_transferString(phost, str, 0, length, 0) - 1;
}
#define Gpu_Hal_Sleep EVE_sleep

#define Gpu_HostCommand EVE_Hal_hostCommand
#define Gpu_HostCommand_Ext3 EVE_Hal_hostCommandExt3
#define Gpu_Hal_Powercycle EVE_Hal_powerCycle
#define Gpu_Hal_SetSPI EVE_Hal_setSPI
#define Gpu_CurrentFrequency EVE_Hal_currentFrequency

#define Gpu_ClockTrimming EVE_Hal_clockTrimming

#define Gpu_ClockSelect EVE_Host_clockSelect
#define Gpu_PLL_FreqSelect EVE_Host_pllFreqSelect
#define Gpu_PowerModeSwitch EVE_Host_powerModeSwitch
#define Gpu_CoreReset EVE_Host_coreReset

#if (EVE_MODEL >= EVE_FT810)
#define Gpu_81X_SelectSysCLK EVE_Host_selectSysClk
#define GPU_81X_PowerOffComponents EVE_Host_powerOffComponents
#define GPU_81X_PadDriveStrength EVE_Host_padDriveStrength
#define Gpu_81X_ResetActive EVE_Host_resetActive
#define Gpu_81X_ResetRemoval EVE_Host_resetRemoval
#endif

#define Hal_LoadSDCard() EVE_Util_loadSdCard(NULL)

#endif /* GPU_HAL__H */
