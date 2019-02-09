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

#include "FT_Platform.h"
#if defined(FT900_PLATFORM)

#include "FT_MCU_Hal.h"

static ft_void_t initSdHost()
{
	sys_enable(sys_device_sd_card);
	sdhost_init();

#define GPIO_SD_CLK (19)
#define GPIO_SD_CMD (20)
#define GPIO_SD_DAT3 (21)
#define GPIO_SD_DAT2 (22)
#define GPIO_SD_DAT1 (23)
#define GPIO_SD_DAT0 (24)
#define GPIO_SD_CD (25)
#define GPIO_SD_WP (26)

	gpio_function(GPIO_SD_CLK, pad_sd_clk);
	gpio_pull(GPIO_SD_CLK, pad_pull_none); // pad_pull_none
	gpio_function(GPIO_SD_CMD, pad_sd_cmd);
	gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT3, pad_sd_data3);
	gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT2, pad_sd_data2);
	gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT1, pad_sd_data1);
	gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
	gpio_function(GPIO_SD_DAT0, pad_sd_data0);
	gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
	gpio_function(GPIO_SD_CD, pad_sd_cd);
	gpio_pull(GPIO_SD_CD, pad_pull_pullup);
	gpio_function(GPIO_SD_WP, pad_sd_wp);
	gpio_pull(GPIO_SD_WP, pad_pull_pullup);
}

ft_void_t Ft_Mcu_Init()
{
#if (defined(EVE_MODULE_PANL))
#ifndef EVDEMO
	init_printf(UART0, tfp_putc);
#endif
	panl_board_cfg bcfg;
#if defined(PANL35)
	panl_board_cfg_set_defaults(PANL35_GENERIC_V2, &bcfg);
#elif defined(PANL70) || defined(PANL70PLUS)
	panl_board_cfg_set_defaults(PANL70_PLUS_V2, &bcfg);
#endif

	panl_result res = panl_init(&bcfg);
	panl_bacnet_cfg_s cfg = { .msg_rxed_cb = bacnet_msg_received, .notif_ack_cb = bacnet_notification_acked, .unconf_msg_rxed_cb = bacnet_unconf_msg_received, .mac = 13, .config = config_default };
	if (!panl_bacnet_init(&cfg))
	{
		eve_printf_debug("Error init bacnet!\n");
	}

	panl_led_init();

	uint8_t panlID = 0;
	res = panl_get_panl_id(&panlID);

	eve_printf_debug("\r\nPanL ID :  %04x\r\n", panlID);
#endif

	interrupt_enable_globally();

#ifndef EVDEMO
	sys_enable(sys_device_uart0);
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
	uart_open(UART0, /* Device */
	    1, /* Prescaler = 1 */
	    UART_DIVIDER_115200_BAUD, /* Divider = 1302 */
	    uart_data_bits_8, /* No. Data Bits */
	    uart_parity_none, /* Parity */
	    uart_stop_bits_1); /* No. Stop Bits */
#endif

#if (defined(ENABLE_ILI9488_HVGA_PORTRAIT) || defined(ENABLE_KD2401_HVGA_PORTRAIT))
	/* asign all the respective pins to gpio and set them to default values */
	gpio_function(34, pad_gpio34);
	gpio_dir(34, pad_dir_output);
	gpio_write(34, 1);

	gpio_function(27, pad_gpio27);
	gpio_dir(27, pad_dir_output);
	gpio_write(27, 1);

	gpio_function(29, pad_gpio29);
	gpio_dir(29, pad_dir_output);
	gpio_write(29, 1);

	gpio_function(33, pad_gpio33);
	gpio_dir(33, pad_dir_output);
	gpio_write(33, 1);

	gpio_function(30, pad_gpio30);
	gpio_dir(30, pad_dir_output);
	gpio_write(30, 1);

	gpio_function(28, pad_gpio28);
	gpio_dir(28, pad_dir_output);
	gpio_write(28, 1);

	gpio_function(43, pad_gpio43);
	gpio_dir(43, pad_dir_output);
	gpio_write(43, 1);
	gpio_write(34, 1);
	gpio_write(28, 1);
	gpio_write(43, 1);
	gpio_write(33, 1);
	gpio_write(33, 1);
#endif

	initSdHost();
	EVE_Millis_initialize();
}

#endif

/* end of file */
