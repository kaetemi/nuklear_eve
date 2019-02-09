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

#ifndef EVE_CMD__H
#define EVE_CMD__H
#include "EVE_Hal.h"

/********************
** COPROCESSOR CMD **
********************/

#define EVE_CMD_STRING_MAX 511

uint16_t EVE_Cmd_rp(EVE_HalContext *phost);
uint16_t EVE_Cmd_wp(EVE_HalContext *phost);
uint16_t EVE_Cmd_space(EVE_HalContext *phost);

/* Begin writing a function, keeps the transfer open */
void EVE_Cmd_beginFunc(EVE_HalContext *phost);

/* End writing a function, closes the transfer */
void EVE_Cmd_endFunc(EVE_HalContext *phost);

bool EVE_Cmd_wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size);
bool EVE_Cmd_wrProgmem(EVE_HalContext *phost, eve_progmem_const uint8_t *buffer, uint32_t size);
uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t size);

bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value);
bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value);
bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value);

/* Move the write pointer forward by the specified number of bytes. Returns the previous write pointer */
uint16_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes);

/* Wait for the command buffer to fully empty. Returns FT_FALSE in case a co processor fault occured */
bool EVE_Cmd_waitFlush(EVE_HalContext *phost);

/* Wait for the command buffer to have at least the requested amount of free space. Returns FT_FALSE in case a co processor fault occured */
bool EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size);

/************
** UTILITY **
************/

#if defined(PLATFORM_FT900) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// #define EVE_CMD_PLATFORM_LE
#endif

/* When using EVE_Cmd_wrBuffer and EVE_Cmd_wrProgmem, commands must be in the correct endianness. */
/* EVE_Cmd_wr16 and EVE_Cmd_wr32 already apply the correct endianness. */
/* Convert to little endian. */
static inline uint32_t EVE_Cmd_le32(uint32_t value)
{
#if defined(EVE_CMD_PLATFORM_LE)
	return value;
#else
	uint8_t res[4];
	res[0] = value & 0xFF;
	res[1] = (value >> 8) & 0xFF;
	res[2] = (value >> 16) & 0xFF;
	res[3] = value >> 24;
	return *(uint32_t *)res;
#endif
}

#endif /* #ifndef EVE_HAL__H */

/* end of file */