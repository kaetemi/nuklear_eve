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
#if defined(STM32L476GDISCOVERY_PLATFORM)/* Initialize HAL platform */
// =========================================================================================
#include "main.c"
// =========================================================================================
int sss=0;
#define FT81X_CS_ACTIVE()        { HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);}
#define FT81X_CS_DEACTIVE()      { HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);}
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 0xFFFF);
	return ch;
}
// =========================================================================================

EVE_HalPlatform g_HalPlatform;

uint32_t EVE_millis() {
#if defined(EVE_MODULE_PANL)
	return panl_timer_get_time();
#else
	/* Interrupt implementation */
	return HAL_GetTick();
#endif
}
void EVE_sleep(uint32_t ms) {
	HAL_Delay(ms);
}
void EVE_Mcu_initialize() {
	 HAL_Init();

	  /* USER CODE BEGIN Init */

	  /* USER CODE END Init */

	  /* Configure the system clock */
	  SystemClock_Config();

	  /* USER CODE BEGIN SysInit */

	  /* USER CODE END SysInit */

	  /* Initialize all configured peripherals */
	  MX_GPIO_Init();
	  MX_I2C1_Init();
	  MX_I2C2_Init();
	  MX_SAI1_Init();
	  MX_SPI1_Init();
	  MX_USART2_UART_Init();
	  MX_USB_HOST_Init();

	HAL_Delay(500);
	printf("build time=%s\n\r", __DATE__ " " __TIME__);
}
void EVE_Millis_initialize() {
#if (defined(EVE_MODULE_PANL))
	panl_timer_register_ms_callback(ticker);
#else
	// no imp
#endif
}
void EVE_Millis_release() {
}
void EVE_HalImpl_initialize() {
	g_HalPlatform.TotalDevices = 1;

#if defined(PANL70) || defined(PANL70PLUS)
		gpio_function(GOODIXGPIO, pad_goodixgpio);
		gpio_dir(GOODIXGPIO, pad_dir_output);
		gpio_write(GOODIXGPIO, 1);
	#endif
}
void EVE_HalImpl_release() {
	HAL_SPI_DeInit(&hspi1);
}
void EVE_HalImpl_defaults(EVE_HalParameters *parameters) {
}
void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls,
		uint8_t numdummy) {
	phost->SpiChannels = numchnls;
	phost->SpiDummyBytes = numdummy;
}
bool EVE_HalImpl_open(EVE_HalContext *phost, EVE_HalParameters *parameters) {
	/* Initialize single channel */
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL, 1);

	/* Initialize the context variables */
	phost->Status = EVE_STATUS_OPENED;

	phost->SpiChannels = 0;
	phost->SpiDummyBytes = 1;

	phost->SpiHandle = &hspi1;
	++g_HalPlatform.OpenedDevices;

	return true;
}
void EVE_HalImpl_close(EVE_HalContext *phost) {
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
	phost->SpiHandle = NULL;
}
void EVE_HalImpl_idle(EVE_HalContext *phost) {
#if defined(EVE_MODULE_PANL)
	panl_bacnet_task();
#endif
}
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw,
		uint32_t addr) {
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	FT81X_CS_ACTIVE()
	;
	HAL_StatusTypeDef ret = HAL_TIMEOUT;



	if (rw == EVE_TRANSFER_READ) {
		uint8_t spiRdHdr[4];

		/*STM32L4XX is Little Endian, LSB is in lowest address for addr*/
		spiRdHdr[0] = (addr >> 16) & 0x3F; //MSB is sent first
		spiRdHdr[1] = (addr >> 8) & 0xFF;
		spiRdHdr[2] = addr & 0xFF;
		spiRdHdr[3] = 0; //dummy byte
		ret =HAL_SPI_Transmit(phost->SpiHandle, spiRdHdr, sizeof(spiRdHdr),
		HAL_MAX_DELAY);

		/*The first 3 bytes are dummy bytes and discard it*/
		//uint8_t rxBuff[3] = { 0 };
		//HAL_SPI_Receive(phost->SpiHandle, rxBuff, sizeof(rxBuff),
		//HAL_MAX_DELAY);

		phost->Status = EVE_STATUS_READING;
	} else {
		uint8_t spiWrHdr[3];
		spiWrHdr[0] = 0x80 | ((addr >> 16) & 0x3F);
		spiWrHdr[1] = 0xFF & (addr >> 8);
		spiWrHdr[2] = addr & 0xFF;
ret=		HAL_SPI_Transmit(phost->SpiHandle, spiWrHdr, sizeof(spiWrHdr),
		HAL_MAX_DELAY);
		phost->Status = EVE_STATUS_WRITING;
	}
		if(ret != HAL_OK){
			printf("Error: HAL_SPI_Transmit failed %d\r\n", __LINE__);
		}
}
void EVE_Hal_endTransfer(EVE_HalContext *phost) {
	eve_assert(
			phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);
	FT81X_CS_DEACTIVE();
	phost->Status = EVE_STATUS_OPENED;
}
void EVE_Hal_flush(EVE_HalContext *phost) {
	eve_assert(phost->Status == EVE_STATUS_OPENED);
}
static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer,
		uint32_t size) {
	HAL_SPI_Receive(phost->SpiHandle, buffer, size, HAL_MAX_DELAY);
}
static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer,
		uint32_t size) {
	HAL_SPI_Transmit(phost->SpiHandle, buffer, size, HAL_MAX_DELAY);
}
static inline uint8_t transfer8(EVE_HalContext *phost, uint8_t value) {
	HAL_StatusTypeDef ret = HAL_TIMEOUT;



	if (phost->Status == EVE_STATUS_WRITING) {
		ret =	HAL_SPI_Transmit(phost->SpiHandle, &value, sizeof(value), HAL_MAX_DELAY);
	} else {
		ret = HAL_SPI_Receive(phost->SpiHandle, &value, sizeof(value), HAL_MAX_DELAY);
	}
		if(ret != HAL_OK){
			printf("Error: HAL_SPI_Transmit failed\r\n");
		}
	return value;
}
uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value) {
	return transfer8(phost, value);
}
uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value) {
	uint16_t retVal = 0;
	if (phost->Status == EVE_STATUS_WRITING) {
		EVE_Hal_transfer8(phost, value & 0xFF); //LSB first
		EVE_Hal_transfer8(phost, (value >> 8) & 0xFF);
	} else {
		retVal = EVE_Hal_transfer8(phost, 0);
		retVal |= (uint16_t) EVE_Hal_transfer8(phost, 0) << 8;
	}
	return retVal;
}
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value) {
	uint32_t retVal = 0;
	if (phost->Status == EVE_STATUS_WRITING) {
		EVE_Hal_transfer16(phost, value & 0xFFFF); //LSB first
		EVE_Hal_transfer16(phost, (value >> 16) & 0xFFFF);
	} else {
		retVal = EVE_Hal_transfer16(phost, 0);
		retVal |= (uint32_t) EVE_Hal_transfer16(phost, 0) << 16;
	}
	return retVal;
}
void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result,
		const uint8_t *buffer, uint32_t size) {
	if (!size)
		return;

	if (result && buffer) {
		/* not implemented */
		eve_debug_break();
	} else if (result) {
		rdBuffer(phost, result, size);
	} else if (buffer) {
		wrBuffer(phost, buffer, size);
	}
}
void EVE_Hal_transferProgmem(EVE_HalContext *phost, uint8_t *result,
eve_progmem_const uint8_t *buffer, uint32_t size) {
	if (!size)
		return;

	if (result && buffer) {
		/* not implemented */
		eve_debug_break();
	} else if (result) {
		/* not implemented */
		eve_debug_break();
	} else if (buffer) {
		eve_assert(!((uintptr_t)buffer & 0x3)); // must be 32-bit aligned
		eve_assert(!(size & 0x3)); // must be 32-bit aligned
		eve_progmem_const uint32_t *buf32 =
				(eve_progmem_const uint32_t *) (void eve_progmem_const *) buffer;
		size >>= 2;
		while (size--) {
			uint32_t value = *(buf32++);
			wrBuffer(phost, (uint8_t *) (&value), 4);
		}
	}
}
uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str,
		uint32_t index, uint32_t size, uint32_t padMask) {
	if (!size) {
		/* TODO: Support different padding options */
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}

	eve_assert(size <= EVE_CMD_STRING_MAX);
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING) {
		transferred += (uint32_t) strnlen(str, size) + 1;
		eve_assert(str[transferred - 1] == '\0');
		wrBuffer(phost, str, transferred);
		if (transferred & padMask) {
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = { 0 };
			wrBuffer(phost, padding, pad);
			transferred += pad;
			eve_assert(!(transferred & 0x3));
		}
	} else {
		/* not implemented */
		eve_debug_break();
	}
	return transferred;
}
void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t cmd) {
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	uint8_t hcmd[3] = { 0, 0, 0 };
	hcmd[0] = cmd;

	FT81X_CS_ACTIVE()
	;

	HAL_StatusTypeDef ret = HAL_TIMEOUT;

	ret = HAL_SPI_Transmit(phost->SpiHandle, hcmd, sizeof(hcmd), HAL_MAX_DELAY);

	if(ret != HAL_OK){
		printf("Error: HAL_SPI_Transmit failed\r\n");
	}

	FT81X_CS_DEACTIVE()
	;
}
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd) {
	eve_assert(phost->Status == EVE_STATUS_OPENED);

	uint8_t hcmd[4] = { 0 };
	hcmd[0] = cmd & 0xff;
	hcmd[1] = (cmd >> 8) & 0xff;
	hcmd[2] = (cmd >> 16) & 0xff;
	hcmd[3] = 0;

	FT81X_CS_ACTIVE()
	;
	HAL_SPI_Transmit(phost->SpiHandle, hcmd, sizeof(hcmd), HAL_MAX_DELAY);
	FT81X_CS_DEACTIVE()
	;
}
void EVE_Hal_powerCycle(EVE_HalContext *phost, bool up) {
	if (up) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(500);
	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_Delay(500);
	}
}
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls,
		uint8_t numdummy) {
#if (EVE_MODEL < EVE_FT810)
	return;
#else
	uint8_t writebyte = 0;

	if ((numchnls > EVE_SPI_QUAD_CHANNEL) || (numdummy > 2) || (numdummy < 1))
		return; // error

	// Switch EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == 2)
		writebyte |= EVE_SPI_TWO_DUMMY_BYTES;
	EVE_Hal_wr8(phost, REG_SPI_WIDTH, writebyte);

	// Switch FT9XX to multi channel SPI mode
	setSPI(phost, numchnls, numdummy);
#endif
}
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost) {
	/* no-op */
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
#endif /* #if defined(STM32_PLATFORM) *//* end of file */
