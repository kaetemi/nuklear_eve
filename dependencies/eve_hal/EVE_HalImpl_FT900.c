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
#if defined(FT900_PLATFORM)

void ticker();

#if defined(EVE_MODULE_PANL)
/* TODO: Pass these as parameters in HAL initialization, instead of here. */

uint8_t ucHeap[PANL_HEAP_SIZE];

/* uart_write((ft900_uart_regs_t*) p, (uint8_t) c); */
void tfp_putc(void *p, char c); /* Placeholder for user code */
void bacnet_notification_acked(uint8_t id); /* Placeholder for user code */
bool bacnet_msg_received(uint8_t src, const uint8_t *indata, const size_t inlen, uint8_t *outdata, size_t *outlen); /* Placeholder for user code */
void bacnet_unconf_msg_received(uint8_t src, const uint8_t *indata, const size_t inlen); /* Placeholder for user code */
#endif /* #if defined(EVE_MODULE_PANL) */

/*********
** INIT **
*********/

EVE_HalPlatform g_HalPlatform;

/* Initialize HAL platform */
void EVE_HalImpl_initialize()
{
	g_HalPlatform.TotalDevices = 1;

	// Initialize SPIM HW
	sys_enable(sys_device_spi_master);

	gpio_function(27, pad_spim_sck); /* GPIO27 to SPIM_CLK */
	gpio_function(28, pad_spim_ss0); /* GPIO28 as CS */
	gpio_function(29, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(30, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_dir(27, pad_dir_output);
	gpio_dir(28, pad_dir_output);
	gpio_dir(29, pad_dir_output);
	gpio_dir(30, pad_dir_input);
#if (defined(ENABLE_SPI_QUAD))
	/* Initialize IO2 and IO3 pad/pin for quad settings */
	gpio_function(31, pad_spim_io2); /* GPIO31 to IO2 */
	gpio_function(32, pad_spim_io3); /* GPIO32 to IO3 */
	gpio_dir(31, pad_dir_output);
	gpio_dir(32, pad_dir_output);
#endif

#if defined(PANL70) || defined(PANL70PLUS)
	gpio_function(GOODIXGPIO, pad_gpio33);
	gpio_dir(GOODIXGPIO, pad_dir_output);
	gpio_write(GOODIXGPIO, 1);
#endif

	gpio_write(28, 1);

#ifdef EVDEMO
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);
#else
	spi_init(SPIM, spi_dir_master, spi_mode_0, 16); //SPISysInit(SPIM);
#endif
}

/* Release HAL platform */
void EVE_HalImpl_release()
{
	spi_uninit(SPIM);
}

/* Get the default configuration parameters */
void EVE_HalImpl_defaults(EVE_HalParameters *parameters)
{
	parameters->PowerDownPin = FT800_PD_N;
	parameters->SpiCsPin = FT800_SEL_PIN;
}

/* Opens a new HAL context using the specified parameters */
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters)
{
	gpio_function(phost->Parameters.SpiCsPin, pad_spim_ss0); /* GPIO28 as CS */
	gpio_write(phost->Parameters.SpiCsPin, 1);

	gpio_function(phost->Parameters.PowerDownPin, pad_gpio43);
	gpio_dir(phost->Parameters.PowerDownPin, pad_dir_output);

	gpio_write(phost->Parameters.PowerDownPin, 1);

	/* Initialize the context valriables */
	phost->SpiDummyBytes = 1; //by default ft800/801/810/811 goes with single dummy byte for read
	phost->SpiChannels = EVE_SPI_SINGLE_CHANNEL;
	phost->Status = EVE_STATUS_OPENED;
	++g_HalPlatform.OpenedDevices;

	return true;
}

/* Close a HAL context */
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
	/* spi_close(SPIM,0); */
}

/* Idle. Call regularly to update frequently changing internal state */
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
#if defined(EVE_MODULE_PANL)
	panl_bacnet_task();
#endif
}

/*************
** TRANSFER **
*************/

void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	if (rw == EVE_TRANSFER_READ)
	{
		uint8_t spidata[4];
		spidata[0] = (addr >> 16);
		spidata[1] = (addr >> 8);
		spidata[2] = addr & 0xff;
		spi_open(SPIM, phost->Parameters.SpiCsPin);
		spi_writen(SPIM, spidata, 3 + phost->SpiDummyBytes);
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		uint8_t spidata[4];
		spidata[0] = (0x80 | (addr >> 16));
		spidata[1] = (addr >> 8);
		spidata[2] = addr;

		spi_open(SPIM, phost->Parameters.SpiCsPin);
		spi_writen(SPIM, spidata, 3);

		phost->Status = EVE_STATUS_WRITING;
	}
}

void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);

	spi_close(SPIM, phost->Parameters.SpiCsPin);
	phost->Status = EVE_STATUS_OPENED;
}

static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	spi_readn(SPIM, buffer, size);
}

static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	spi_writen(SPIM, buffer, size);
}

static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value)
{
	if (phost->Status == EVE_STATUS_READING)
	{
		spi_read(SPIM, value);
		return value;
	}
	else
	{
		spi_write(SPIM, value);
		return 0;
	}
}

uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value)
{
	return transfer8(phost, value);
}

uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value)
{
	uint8_t buffer[2];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 2);
		return (uint16_t)buffer[0]
			| (uint16_t)buffer[1] << 8;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = value >> 8;
		wrBuffer(phost, buffer, 2);
		return 0;
	}
}

uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint8_t buffer[4];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 4);
		return (uint32_t)buffer[0]
			| (uint32_t)buffer[1] << 8
			| (uint32_t)buffer[2] << 16
			| (uint32_t)buffer[3] << 24;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		wrBuffer(phost, buffer, 4);
		return 0;
	}
}

void EVE_Hal_transferBuffer(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result, eve_progmem_const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;

	if (result && buffer)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (result)
	{
		/* not implemented */
		eve_debug_break();
	}
	else if (buffer)
	{
		eve_assert(!((uintptr_t)buffer & 0x3)); // must be 32-bit aligned
		eve_assert(!(size & 0x3)); // must be 32-bit aligned
		eve_progmem_const uint32_t *buf32 = (eve_progmem_const uint32_t *)(void eve_progmem_const *)buffer;
		size >>= 2;
		while (size--)
		{
			uint32_t value = *(buf32++);
			wrBuffer(phost, (uint8_t *)(&value), 4);
		}
	}
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask)
{
	uint32_t transferred;
	for (transferred = 0; transferred < size;)
	{
		char c = str[index + (transferred++)];
		// putchar(c ? c : '_');
		transfer8(phost, c);
		if (!c)
			break;
	}
	while (transferred & padMask)
	{
		++transferred;
		// putchar('_');
		transfer8(phost, 0);
	}
	return transferred;
}

/************
** UTILITY **
************/

void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd)
{
	uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd;
	hcmd[1] = 0;
	hcmd[2] = 0;
	hcmd[3] = 0;

	spi_open(SPIM, phost->Parameters.SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->Parameters.SpiCsPin);
}

void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd)
{
	uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd & 0xff;
	hcmd[1] = (cmd >> 8) & 0xff;
	hcmd[2] = (cmd >> 16) & 0xff;
	hcmd[3] = 0;
	spi_open(SPIM, phost->Parameters.SpiCsPin);
	spi_writen(SPIM, hcmd, 3);
	spi_close(SPIM, phost->Parameters.SpiCsPin);
}

void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
	if (up)
	{
		gpio_write(phost->Parameters.PowerDownPin, 0);
		EVE_sleep(20);
		gpio_write(phost->Parameters.PowerDownPin, 1);
		EVE_sleep(20);
	}
	else
	{
		gpio_write(phost->Parameters.PowerDownPin, 1);
		EVE_sleep(20);
		gpio_write(phost->Parameters.PowerDownPin, 0);
		EVE_sleep(20);
	}
}

int16_t EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls, uint8_t numdummy)
{
	uint8_t writebyte = 0;

	if ((numchnls > EVE_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
	{
		return -1; //error
	}

	//swicth EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= EVE_SPI_TWO_DUMMY_BYTES;
	EVE_Hal_wr8(phost, REG_SPI_WIDTH, writebyte);
	//FT81x swicthed to dual/quad mode, now update global HAL context
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
	return 0;
}

uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0, t1;
	uint32_t addr = REG_CLOCK;
	uint8_t spidata[4];
	int32_t r = 15625;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */

	__asm__(
		"   move.l  $r0,%0"
		"\n\t"
		"   mul.l   $r0,$r0,100"
		"\n\t"
		"1:"
		"\n\t"
		"   sub.l   $r0,$r0,3"
		"\n\t" /* Subtract the loop time = 4 cycles */
		"   cmp.l   $r0,0"
		"\n\t" /* Check that the counter is equal to 0 */
		"   jmpc    gt, 1b"
		"\n\t"
		/* Outputs */
		:
	/* Inputs */
	: "r"(r)
		/* Using */
		: "$r0"

		);

	//usleep(15625);
	//EVE_sleep(15625);

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

/*********
** MISC **
*********/

static void initSdHost()
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

void EVE_Mcu_initialize()
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
}

void EVE_Mcu_release()
{
	/* no-op */
}

/*********
** MISC **
*********/

/* Globals for interrupt implementation */
static uint32_t s_TotalMilliseconds = 0;

void EVE_Millis_initialize()
{
	s_TotalMilliseconds = 0;
#if (defined(EVE_MODULE_PANL))
	panl_timer_register_ms_callback(ticker);
#else

	sys_enable(sys_device_timer_wdt);
	timer_prescaler(FT900_TIMER_PRESCALE_VALUE);
	timer_init(FT900_FT_MILLIS_TIMER, FT900_TIMER_OVERFLOW_VALUE, timer_direction_up, timer_prescaler_select_on, timer_mode_continuous);

	interrupt_attach(interrupt_timers, 17, ticker);
	/* enabling the interrupts for timer */
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);

	timer_start(FT900_FT_MILLIS_TIMER);
#endif
}

void EVE_Millis_release()
{
	timer_stop(FT900_FT_MILLIS_TIMER);
	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
}

/* Need to ensure that below api is called at least once in 6.5 seconds duration for FT900 platform as this module doesnt use timer for context update */
/* global counter to loopback after ~49.71 days */
uint32_t EVE_millis()
{
#if (defined(EVE_MODULE_PANL))
	return panl_timer_get_time();
#else
	/* Interrupt implementation */
	return (s_TotalMilliseconds);
#endif
}

void ticker()
{
	s_TotalMilliseconds += 1;

	timer_disable_interrupt(FT900_FT_MILLIS_TIMER);
	/* Clear the interrupt and increment the counter */
	timer_is_interrupted(FT900_FT_MILLIS_TIMER);
	timer_enable_interrupt(FT900_FT_MILLIS_TIMER);
}

void EVE_sleep(uint32_t ms)
{
	delayms(ms);
}

/*********
** MISC **
*********/

bool EVE_UtilImpl_prepareSpiMaster(EVE_HalContext *phost)
{
	/* Reconfigure the SPI */
	// Initialize SPIM HW
	sys_enable(sys_device_spi_master);
	gpio_function(27, pad_spim_sck); /* GPIO27 to SPIM_CLK */
	gpio_function(28, pad_spim_ss0); /* GPIO28 as CS */
	gpio_function(29, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
	gpio_function(30, pad_spim_miso); /* GPIO30 to SPIM_MISO */

	gpio_write(28, 1);
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);

	return true;
}

bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
#if defined(PANL70) || defined(PANL70PLUS)
	gpio_function(GOODIXGPIO, pad_gpio33);
	gpio_dir(GOODIXGPIO, pad_dir_output);
	gpio_write(GOODIXGPIO, 0);
	EVE_sleep(1);
	Ft_Gpu_Hal_Wr8(phost, REG_CPURESET, 0);
	EVE_sleep(100);
	gpio_dir(GOODIXGPIO, pad_dir_input);
#endif

	return true;
}

bool EVE_UtilImpl_postBootupConfig(EVE_HalContext *phost)
{
	/* Change clock frequency to 25mhz */
	spi_init(SPIM, spi_dir_master, spi_mode_0, 4);

#if (defined(ENABLE_SPI_QUAD))
	/* Initialize IO2 and IO3 pad/pin for dual and quad settings */
	gpio_function(31, pad_spim_io2);
	gpio_function(32, pad_spim_io3);
	gpio_write(31, 1);
	gpio_write(32, 1);
#endif
	/* Enable FIFO of QSPI */
	spi_option(SPIM, spi_option_fifo_size, 64);
	spi_option(SPIM, spi_option_fifo, 1);
	spi_option(SPIM, spi_option_fifo_receive_trigger, 1);

#ifdef ENABLE_SPI_QUAD
	spi_option(SPIM, spi_option_bus_width, 4);
#elif ENABLE_SPI_DUAL
	spi_option(SPIM, spi_option_bus_width, 2);
#else
	spi_option(SPIM, spi_option_bus_width, 1);
#endif

	return true;
}

#endif /* #if defined(FT900_PLATFORM) */

/* end of file */