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

#ifndef EVE_EMULATOR_MAIN__H
#define EVE_EMULATOR_MAIN__H

#include "EVE_Config.h"
#if defined(BT8XXEMU_PLATFORM)

#include "EVE_Platform_WIN32.h"
#include "bt8xxemu.h"

// FIXME (2018 OCT 13): Currently the ESD simulation requires the main function override mechanism
#if !defined(EVE_MULTI_TARGET) // defined(ESD_SIMULATION)
#define EVE_EMULATOR_MAIN (1)

// Override main function with wrapper for initializing the emulator
#define main EVE_emuMain

extern BT8XXEMU_Emulator *EVE_GpuEmu;
extern BT8XXEMU_Flash *EVE_EmuFlash;

#endif /* #if defined(ESD_SIMULATION) */
#endif /* #if defined(BT8XXEMU_PLATFORM) || defined(ESD_SIMULATION) */
#endif /* #ifndef EVE_EMULATOR_MAIN__H */

/* end of file */