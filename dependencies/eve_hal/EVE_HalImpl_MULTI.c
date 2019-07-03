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
#if defined(EVE_MULTI_TARGET)

static size_t s_DeviceCount;
static EVE_DeviceInfo s_DeviceList[256];

void EVE_HalImpl_BT8XXEMU_list(EVE_DeviceInfo *deviceList, size_t *deviceCount);

EVE_DeviceInfo *EVE_Hal_list(size_t *deviceCount)
{
	memset(s_DeviceList, 0, sizeof(s_DeviceList));
	s_DeviceCount = 0;
	EVE_HalImpl_BT8XXEMU_list(&s_DeviceCount);
	*deviceCount = s_DeviceCount;
	return s_DeviceList;
}

/*
#if defined(EVE_MULTI_TARGET)
#define EVE_HalImpl_initialize EVE_HalImpl_BT8XXEMU_initialize
#define EVE_HalImpl_release EVE_HalImpl_BT8XXEMU_release
#define EVE_HalImpl_defaults EVE_HalImpl_BT8XXEMU_defaults
#define EVE_HalImpl_open EVE_HalImpl_BT8XXEMU_open
#define EVE_HalImpl_close EVE_HalImpl_BT8XXEMU_close
#define EVE_HalImpl_idle EVE_HalImpl_BT8XXEMU_idle
#define EVE_Hal_flush EVE_Hal_BT8XXEMU_flush
#define EVE_Hal_startTransfer EVE_Hal_BT8XXEMU_startTransfer
#define EVE_Hal_endTransfer EVE_Hal_BT8XXEMU_endTransfer
#define EVE_Hal_transfer8 EVE_Hal_BT8XXEMU_transfer8
#define EVE_Hal_transfer16 EVE_Hal_BT8XXEMU_transfer16
#define EVE_Hal_transfer32 EVE_Hal_BT8XXEMU_transfer32
#define EVE_Hal_transferMem EVE_Hal_BT8XXEMU_transferMem
#define EVE_Hal_transferProgmem EVE_Hal_BT8XXEMU_transferProgmem
#define EVE_Hal_transferString EVE_Hal_BT8XXEMU_transferString
#define EVE_Hal_hostCommand EVE_Hal_BT8XXEMU_hostCommand
#define EVE_Hal_hostCommandExt3 EVE_Hal_BT8XXEMU_hostCommandExt3
#define EVE_Hal_powerCycle EVE_Hal_BT8XXEMU_powerCycle
#define EVE_UtilImpl_bootupDisplayGpio EVE_UtilImpl_BT8XXEMU_bootupDisplayGpio
#endif
*/


#endif /* #if defined(BT8XXEMU_PLATFORM) */

/* end of file */
