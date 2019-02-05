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
#if defined(BT8XXEMU_PLATFORM)

#include "FT_Gpu_Hal.h"
#include "FT_Emulator.h"

#include "FT_EmulatorMain.h"
#if defined(ESD_SIMULATION)
void Ft_MainReady__ESD(BT8XXEMU_Emulator *emulator);
#endif

/* API to initialize the SPI interface */
ft_bool_t Ft_Gpu_Hal_Init(Ft_Gpu_HalInit_t *halinit)
{
	halinit->total_channel_num = 1;
	return FT_TRUE;
}

ft_void_t Ft_Gpu_Hal_ESD_Idle(Ft_Gpu_Hal_Context_t *phost)
{
	// no-op
}

ft_bool_t Ft_Gpu_Hal_Open(Ft_Gpu_Hal_Context_t *phost)
{
	phost->hal_config.channel_no = 0;
	phost->hal_config.pdn_pin_no = 0;
	phost->hal_config.spi_cs_pin_no = 0;

#if defined(FT_EMULATOR_MAIN)
	phost->emu = Ft_GpuEmu;
	phost->emu_flash = Ft_EmuFlash;
#else
	BT8XXEMU_EmulatorParameters params;

	BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, Ft_Emulator_Mode());

	params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->emu, &params);
#endif

#if defined(ESD_SIMULATION)
	Ft_MainReady__ESD(phost->emu);
#endif

	/* Initialize the context valriables */
	phost->spinumdummy = 1; //by default ft800/801/810/811 goes with single dummy byte for read
	phost->spichannel = 0;
	phost->status = FT_GPU_HAL_OPENED;

	return !!phost->emu;
}

ft_void_t Ft_Gpu_Hal_Close(Ft_Gpu_Hal_Context_t *phost)
{
#if !defined(FT_EMULATOR_MAIN)
	if (phost->emu)
	{
		BT8XXEMU_stop(phost->emu);
		BT8XXEMU_destroy(phost->emu);
	}
	phost->emu = NULL;
	phost->emu_flash = NULL;
#else
	phost->emu = NULL;
	phost->emu_flash = NULL;
#endif

	phost->status = FT_GPU_HAL_CLOSED;
}

ft_void_t Ft_Gpu_Hal_DeInit()
{
	// no-op
}

/*The APIs for reading/writing transfer continuously only with small buffer system*/
ft_void_t Ft_Gpu_Hal_StartTransfer(Ft_Gpu_Hal_Context_t *phost, FT_GPU_TRANSFERDIR_T rw, ft_uint32_t addr)
{
	eve_assert(!(phost->cmd_frame && (addr == REG_CMDB_WRITE)));
	if (FT_GPU_READ == rw)
	{
		BT8XXEMU_chipSelect(phost->emu, 1);
		BT8XXEMU_transfer(phost->emu, (addr >> 16) & 0xFF);
		BT8XXEMU_transfer(phost->emu, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->emu, addr & 0xFF);
		BT8XXEMU_transfer(phost->emu, 0); //Dummy Read Byte
		phost->status = FT_GPU_HAL_READING;
	}
	else
	{
		BT8XXEMU_chipSelect(phost->emu, 1);
		BT8XXEMU_transfer(phost->emu, ((addr >> 16) & 0xFF) | 0x80);
		BT8XXEMU_transfer(phost->emu, (addr >> 8) & 0xFF);
		BT8XXEMU_transfer(phost->emu, addr & 0xFF);
		phost->status = FT_GPU_HAL_WRITING;
	}
}

ft_uint8_t Ft_Gpu_Hal_Transfer8(Ft_Gpu_Hal_Context_t *phost, ft_uint8_t value)
{
	return BT8XXEMU_transfer(phost->emu, value);
}

ft_void_t Ft_Gpu_Hal_EndTransfer(Ft_Gpu_Hal_Context_t *phost)
{
	BT8XXEMU_chipSelect(phost->emu, 0);
	phost->status = FT_GPU_HAL_OPENED;
}

ft_uint8_t Ft_Gpu_Hal_Rd8(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr)
{
	ft_uint8_t value;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer8(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}

ft_uint16_t Ft_Gpu_Hal_Rd16(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr)
{
	ft_uint16_t value;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer16(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}
ft_uint32_t Ft_Gpu_Hal_Rd32(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr)
{
	ft_uint32_t value;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	value = Ft_Gpu_Hal_Transfer32(phost, 0);
	Ft_Gpu_Hal_EndTransfer(phost);
	return value;
}

ft_void_t Ft_Gpu_Hal_Wr8(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, ft_uint8_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer8(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Wr16(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, ft_uint16_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer16(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Wr32(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, ft_uint32_t v)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	Ft_Gpu_Hal_Transfer32(phost, v);
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_HostCommand(Ft_Gpu_Hal_Context_t *phost, ft_uint8_t cmd)
{
	;
}
//This API sends a 3byte command to the phost
ft_void_t Ft_Gpu_HostCommand_Ext3(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t cmd)
{
	;
}

ft_bool_t Ft_Gpu_Hal_WrCmdBuf(Ft_Gpu_Hal_Context_t *phost, ft_uint8_t *buffer, ft_uint32_t count)
{
	eve_assert(!phost->cmd_frame);
	const ft_uint32_t *buf32 = (const ft_uint32_t *)(void *)(buffer);
	ft_int32_t length = 0, availablefreesize = 0;
	count = (count + 3) & ~0x3;
	do
	{
		length = count;
		availablefreesize = Ft_Gpu_Hal_Rd16(phost, REG_CMDB_SPACE) & FIFO_SIZE_MASK;
		if (FT_COCMD_FAULT(availablefreesize))
			return FT_FALSE; // Co processor fault
		if (length > availablefreesize)
			length = availablefreesize;
		if (length)
		{
			count -= length;
			length >>= 2;
			Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, REG_CMDB_WRITE);
			for (int i = 0; i < length; ++i)
				Ft_Gpu_Hal_Transfer32(phost, *(buf32++));
			Ft_Gpu_Hal_EndTransfer(phost);
		}
	} while (count > 0);
	return FT_TRUE;
}

ft_bool_t Ft_Gpu_Hal_WrCmdBuf_ProgMem(Ft_Gpu_Hal_Context_t *phost, ft_prog_uchar8_t *buffer, ft_uint32_t count)
{
	eve_assert(!((uintptr_t)buffer & 0x3UL)); // must have 32-bit alignment
	return Ft_Gpu_Hal_WrCmdBuf(phost, buffer, count);
}

ft_void_t Ft_Gpu_Hal_WrCmd32(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t cmd)
{
	eve_assert_ex(!phost->cmd_frame, "Did you mean 'Ft_Gpu_CoCmd_SendCmd'?");
	Ft_Gpu_Hal_Wr32(phost, REG_CMDB_WRITE, cmd);
}

/* Toggle PD_N pin of FT800 board for a power cycle*/
ft_void_t Ft_Gpu_Hal_Powercycle(Ft_Gpu_Hal_Context_t *phost, ft_bool_t up)
{
#if !defined(FT_EMULATOR_MAIN)
	// ESD would need to call MainReady__ESD again...
	// TODO: Implement powercycle in BT8XXEMU
	if (up)
	{
		if (phost->emu)
		{
			BT8XXEMU_stop(phost->emu);
			BT8XXEMU_destroy(phost->emu);
			phost->emu = NULL;
		}

		BT8XXEMU_EmulatorParameters params;

		BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, Ft_Emulator_Mode());

		params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
		BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->emu, &params);
	}
	else
	{
		if (!phost->emu)
		{
			BT8XXEMU_EmulatorParameters params;

			BT8XXEMU_defaults(BT8XXEMU_VERSION_API, &params, Ft_Emulator_Mode());

			params.Flags &= (~BT8XXEMU_EmulatorEnableDynamicDegrade & ~BT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
			BT8XXEMU_run(BT8XXEMU_VERSION_API, &phost->emu, &params);
		}

		BT8XXEMU_stop(phost->emu);
		BT8XXEMU_destroy(phost->emu);
		phost->emu = NULL;
	}
#endif
}

ft_void_t Ft_Gpu_Hal_WrMem(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, const ft_uint8_t *buffer, ft_uint32_t length)
{
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	while (length--)
	{
		Ft_Gpu_Hal_Transfer8(phost, *buffer);
		buffer++;
	}
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_WrMem_ProgMem(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, const ft_prog_uchar8_t *buffer, ft_uint32_t length)
{
	eve_assert(!((uintptr_t)buffer & 0x3)); // must be 32-bit aligned
	eve_assert(!(length & 0x3)); // must be 32-bit aligned
	uint32_t FT_PROGMEM_CONST *buf32 = (uint32_t FT_PROGMEM_CONST *)(void FT_PROGMEM_CONST *)buffer;
	length >>= 2;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_WRITE, addr);
	while (length--)
		Ft_Gpu_Hal_Transfer32(phost, *(buf32++));
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_RdMem(Ft_Gpu_Hal_Context_t *phost, ft_uint32_t addr, ft_uint8_t *buffer, ft_uint32_t length)
{
	ft_uint32_t SizeTransfered = 0;
	Ft_Gpu_Hal_StartTransfer(phost, FT_GPU_READ, addr);
	while (length--)
	{
		*buffer = Ft_Gpu_Hal_Transfer8(phost, 0);
		buffer++;
	}
	Ft_Gpu_Hal_EndTransfer(phost);
}

ft_void_t Ft_Gpu_Hal_Sleep(ft_uint32_t ms)
{
#if defined(ESD_SIMULATION)
	Ft_Sleep__ESD(ms);
#else
	Sleep(ms);
#endif
}

ft_int16_t Ft_Gpu_Hal_SetSPI(Ft_Gpu_Hal_Context_t *phost, FT_GPU_SPI_NUMCHANNELS_T numchnls, FT_GPU_SPI_NUMDUMMYBYTES numdummy)
{
	ft_uint8_t writebyte = 0;

	if ((numchnls > FT_GPU_SPI_QUAD_CHANNEL) || (numdummy > FT_GPU_SPI_TWODUMMY) || (numdummy < FT_GPU_SPI_ONEDUMMY))
	{
		return -1; //error
	}

	//swicth EVE to multi channel SPI mode
	writebyte = numchnls;
	if (numdummy == FT_GPU_SPI_TWODUMMY)
		writebyte |= FT_SPI_TWO_DUMMY_BYTE;
	Ft_Gpu_Hal_Wr8(phost, REG_SPI_WIDTH, writebyte);
	//FT81x swicthed to dual/quad mode, now update global HAL context
	phost->spichannel = numchnls;
	phost->spinumdummy = numdummy;
	return 0;
}

ft_uint32_t Ft_Gpu_CurrentFrequency(Ft_Gpu_Hal_Context_t *phost)
{
	ft_uint32_t t0, t1;
	ft_uint32_t addr = REG_CLOCK;
	// ft_uint8_t spidata[4];
	ft_int32_t r = 15625;

	t0 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t0 read */
	//may not be precise
	Sleep(15625 / 1000);

	t1 = Ft_Gpu_Hal_Rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

static DWORD s_Millis_Start;

ft_void_t ft_millis_init()
{
	s_Millis_Start = GetTickCount();
}

/* global counter to loopback after ~49.71 days */
ft_uint32_t ft_millis()
{
	return GetTickCount() - s_Millis_Start;
}

ft_void_t ft_millis_exit()
{
	// no-op
}

#endif

/* end of file */
