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

#ifndef LIBMPSSE_SPI_H
#define LIBMPSSE_SPI_H

#include "ftd2xx.h"

#define __x86_64__

/******************************************************************************/
/*								Macro defines								  */
/******************************************************************************/

#ifdef _MSC_VER
#define FTDI_API
#else
#define FTDI_API
#endif

/* Bit defination of the transferOptions parameter in SPI_Read, SPI_Write & SPI_Transfer  */

/* transferOptions-Bit0: If this bit is 0 then it means that the transfer size provided is in bytes */
#define SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES 0x00000000
/* transferOptions-Bit0: If this bit is 1 then it means that the transfer size provided is in bytes */
#define SPI_TRANSFER_OPTIONS_SIZE_IN_BITS 0x00000001
/* transferOptions-Bit1: if BIT1 is 1 then CHIP_SELECT line will be enables at start of transfer */
#define SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE 0x00000002
/* transferOptions-Bit2: if BIT2 is 1 then CHIP_SELECT line will be disabled at end of transfer */
#define SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE 0x00000004

/* Bit defination of the Options member of configOptions structure*/
#define SPI_CONFIG_OPTION_MODE_MASK 0x00000003
#define SPI_CONFIG_OPTION_MODE0 0x00000000
#define SPI_CONFIG_OPTION_MODE1 0x00000001
#define SPI_CONFIG_OPTION_MODE2 0x00000002
#define SPI_CONFIG_OPTION_MODE3 0x00000003

#define SPI_CONFIG_OPTION_CS_MASK 0x0000001C /*111 00*/
#define SPI_CONFIG_OPTION_CS_DBUS3 0x00000000 /*000 00*/
#define SPI_CONFIG_OPTION_CS_DBUS4 0x00000004 /*001 00*/
#define SPI_CONFIG_OPTION_CS_DBUS5 0x00000008 /*010 00*/
#define SPI_CONFIG_OPTION_CS_DBUS6 0x0000000C /*011 00*/
#define SPI_CONFIG_OPTION_CS_DBUS7 0x00000010 /*100 00*/

#define SPI_CONFIG_OPTION_CS_ACTIVELOW 0x00000020

/******************************************************************************/
/*								Type defines								  */
/******************************************************************************/

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed long long int64;

#ifndef _MSC_VER
//typedef unsigned char bool;
#endif

#ifdef __x86_64__
/*20111025: 64bit linux doesn't work is uint32 is unsigned long*/
typedef unsigned int uint32;
typedef signed int int32;
#else
typedef unsigned long uint32;
typedef signed long int32;
#endif

typedef enum I2C_ClockRate_t
{
	I2C_CLOCK_STANDARD_MODE = 100000, // 100kb/sec
	I2C_CLOCK_FAST_MODE = 400000, // 400kb/sec
	I2C_CLOCK_FAST_MODE_PLUS = 1000000, // 1000kb/sec
	I2C_CLOCK_HIGH_SPEED_MODE = 3400000 // 3.4Mb/sec
} I2C_CLOCKRATE;

typedef struct ChannelConfig_t
{
	uint32 ClockRate;

	uint8 LatencyTimer;

	uint32 configOptions; /*This member provides a way to enable/disable features
	specific to the protocol that are implemented in the chip
	BIT1-0=CPOL-CPHA:	00 - MODE0 - data captured on rising edge, propagated on falling
						01 - MODE1 - data captured on falling edge, propagated on rising
						10 - MODE2 - data captured on falling edge, propagated on rising
						11 - MODE3 - data captured on rising edge, propagated on falling
	BIT4-BIT2: 000 - A/B/C/D_DBUS3=ChipSelect
			 : 001 - A/B/C/D_DBUS4=ChipSelect
			 : 010 - A/B/C/D_DBUS5=ChipSelect
			 : 011 - A/B/C/D_DBUS6=ChipSelect
			 : 100 - A/B/C/D_DBUS7=ChipSelect
	BIT5: ChipSelect is active high if this bit is 0
	BIT6 -BIT31		: Reserved
	*/

	uint32 Pin; /*BIT7   -BIT0:   Initial direction of the pins	*/
	/*BIT15 -BIT8:   Initial values of the pins		*/
	/*BIT23 -BIT16: Final direction of the pins		*/
	/*BIT31 -BIT24: Final values of the pins		*/
	uint16 reserved;
} ChannelConfig;

/******************************************************************************/
/*								External variables							  */
/******************************************************************************/

/******************************************************************************/
/*								Function declarations						  */
/******************************************************************************/
FTDI_API FT_STATUS SPI_GetNumChannels(uint32 *numChannels);
FTDI_API FT_STATUS SPI_GetChannelInfo(uint32 index,
    FT_DEVICE_LIST_INFO_NODE *chanInfo);
FTDI_API FT_STATUS SPI_OpenChannel(uint32 index, FT_HANDLE *handle);
FTDI_API FT_STATUS SPI_InitChannel(FT_HANDLE handle, ChannelConfig *config);
FTDI_API FT_STATUS SPI_CloseChannel(FT_HANDLE handle);
FTDI_API FT_STATUS SPI_ToggleCS(FT_HANDLE handle, uint8 high);
FTDI_API FT_STATUS SPI_Read(FT_HANDLE handle, uint8 *buffer,
    uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
FTDI_API FT_STATUS SPI_Write(FT_HANDLE handle, uint8 *buffer,
    uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
FTDI_API FT_STATUS SPI_ReadWrite(FT_HANDLE handle, uint8 *inBuffer,
    uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred,
    uint32 transferOptions);
FTDI_API FT_STATUS SPI_IsBusy(FT_HANDLE handle, bool *state);
FTDI_API void Init_libMPSSE(void);
FTDI_API void Cleanup_libMPSSE(void);
FTDI_API FT_STATUS SPI_ChangeCS(FT_HANDLE handle, uint32 configOptions);
FTDI_API FT_STATUS FT_WriteGPIO(FT_HANDLE handle, uint8 dir, uint8 value);
FTDI_API FT_STATUS FT_ReadGPIO(FT_HANDLE handle, uint8 *value);

/******************************************************************************/

#endif /*LIBMPSSE_SPI_H*/
