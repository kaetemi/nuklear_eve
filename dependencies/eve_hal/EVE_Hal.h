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

#define EVE_FIFO_SIZE_MASK (0xFFF) // FIFO valid range from 0 to 4095
#define EVE_FIFO_BYTE_ALIGNMENT_MASK (0xFFC)

typedef enum EVE_HalMode
{
	EVE_HAL_MODE_UNKNOWN = 0,
	EVE_HAL_MODE_I2C,
	EVE_HAL_MODE_SPI,

} EVE_HalMode;

typedef enum
{
	EVE_HAL_STATUS_UNKNOWN = 0,
	EVE_HAL_STATUS_OPENED,
	EVE_HAL_STATUS_READING,
	EVE_HAL_STATUS_WRITING,
	EVE_HAL_STATUS_CLOSED,
} EVE_HalStatus;

#endif /* #ifndef EVE_HAL__H */

/* end of file */