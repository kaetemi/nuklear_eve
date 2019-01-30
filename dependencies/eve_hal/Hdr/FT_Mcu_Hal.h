#ifndef FT_MCU_HAL_H
#define FT_MCU_HAL_H
#include "FT_Platform.h"

extern const ft_uint8_t FT_DLCODE_BOOTUP[12];

/* Global variables for display resolution to support various display panels */
/* Default is WQVGA - 480x272 */
extern ft_int16_t FT_DispWidth;
extern ft_int16_t FT_DispHeight;
extern ft_int16_t FT_DispHCycle;
extern ft_int16_t FT_DispHOffset;
extern ft_int16_t FT_DispHSync0;
extern ft_int16_t FT_DispHSync1;
extern ft_int16_t FT_DispVCycle;
extern ft_int16_t FT_DispVOffset;
extern ft_int16_t FT_DispVSync0;
extern ft_int16_t FT_DispVSync1;
extern ft_uint8_t FT_DispPCLK;
extern ft_char8_t FT_DispSwizzle;
extern ft_char8_t FT_DispPCLKPol;
extern ft_char8_t FT_DispCSpread;
extern ft_char8_t FT_DispDither;

ft_void_t Eve_BootupConfig(Ft_Gpu_Hal_Context_t *s_Host);

ft_void_t Ft_Mcu_Init();

#ifdef DISPLAY_RESOLUTION_QVGA
ft_void_t init_DISPLAY_RESOLUTION_QVGA();
#endif

#ifdef DISPLAY_RESOLUTION_WVGA
ft_void_t init_DISPLAY_RESOLUTION_WVGA();
#endif

#ifdef DISPLAY_RESOLUTION_HVGA_PORTRAIT
ft_void_t init_DISPLAY_RESOLUTION_HVGA_PORTRAIT();
#endif

#if (defined(BT_MODULE_PANL)) && !defined(ESD_SIMULATION)
uint8_t ucHeap[PANL_HEAP_SIZE];

void tfp_putc(void *p, char c)
{
	/*Placeholder for user code*/
	//uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}
void bacnet_notification_acked(uint8_t id)
{
	//Placeholder for user code
}
ft_bool_t bacnet_msg_received(uint8_t src, const uint8_t *indata, const size_t inlen, uint8_t *outdata, size_t *outlen)
{
	//Placeholder for user code
}
void bacnet_unconf_msg_received(uint8_t src, const uint8_t *indata, const size_t inlen)
{
	//Placeholder for user code
}
#endif /*(defined(BT_MODULE_PANL)) && !defined(ESD_SIMULATION)*/

#endif
