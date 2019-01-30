/*

Copyright (c) Bridgetek Pte Ltd

THIS SOFTWARE IS PROVIDED BY BRIDGETEK PTE LTD "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
BRIDGETEK PTE LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

BRIDGETEK DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON BRIDGETEK PARTS.

BRIDGETEK DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Abstract:

Application to demonstrate function of EVE.

Author : Bridgetek

Revision History:
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - added few APIs
1.0 - date 2017.03.24 - Rename and split platforms
*/
#include "Platform.h"
#include "Hal_Config.h"
#ifdef USE_SDCARD
#include "ff.h"
#endif

/*==========================================================================
*
*        COMMON HAL LIBRARIES FUNCTIONS CROSS PLATFORMS
*
*=========================================================================*/
/***************************************************************************
* Interface Description    : API for USB-to-SPI/I2C bridge IC LIB initialize
*                             For MSVC_PLATFORM the following actions are
*                             performed
*                                -libMPSSE/libFT4222 init
*                                -read info about SPI channels on usb-bridge chip
*                                 through lib service
* Implementation           :
* Return Value             : bool_t
*                            TRUE , FALSE
* Author                   :
****************************************************************************/
bool_t Gpu_Hal_Init(Gpu_HalInit_t *halinit)
{
    // Initialize SPIM HW
    sys_enable(sys_device_spi_master);

    /* Initialize the pin functions and directions */
    gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
    gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
    gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
    gpio_function(GPIO_SPIM_MISO, pad_spim_miso);

    gpio_dir(GPIO_SPIM_CLK, pad_dir_output);
    gpio_dir(GPIO_SPIM_SS0, pad_dir_output);
    gpio_dir(GPIO_SPIM_MOSI, pad_dir_output);
    gpio_dir(GPIO_SPIM_MISO, pad_dir_input);

#if (defined(ENABLE_SPI_QUAD))
    /* Initialize IO2 and IO3 pad/pin for quad settings */
    gpio_function(GPIO_SPIM_IO2, pad_spim_io2);
    gpio_function(GPIO_SPIM_IO3, pad_spim_io3);
    gpio_dir(GPIO_SPIM_IO2, pad_dir_output);
    gpio_dir(GPIO_SPIM_IO3, pad_dir_output);
#endif
    /* CS high */
    gpio_write(GPIO_SPIM_SS0, 1);
    spi_init(SPIM, spi_dir_master, spi_mode_0, 16);//SPISysInit(SPIM);

    return TRUE;
}


/***************************************************************************
* Interface Description    : To obtain handle for interface communication
* Implementation           :
* Return Value             : bool_t
*                            TRUE - Configuration successful
*                            FALSE - Configuration failed
* Author                   :
****************************************************************************/
bool_t Gpu_Hal_Open(Gpu_Hal_Context_t *host)
{
    bool_t ret = TRUE;

    gpio_function(host->hal_config.pdn_pin_no, pad_pwd);
    gpio_dir(host->hal_config.pdn_pin_no, pad_dir_output);

    gpio_write(host->hal_config.pdn_pin_no,1);


    /* Initialize the context valriables */
    host->cmd_fifo_wp = host->dl_buff_wp = 0;
    host->spinumdummy = GPU_SPI_ONEDUMMY; //by default ft800/801/810/811 goes with single dummy byte for read
    host->status = GPU_HAL_OPENED;

    return ret;
}

/***************************************************************************
* Interface Description    : Free global wr_buf, release LibFT4222 resources
*                            and release SPI communication handle
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Close(Gpu_Hal_Context_t *host)
{

    host->status = GPU_HAL_CLOSED;
    //spi_close(SPIM,0);

}

/***************************************************************************
* Interface Description    : Uninitialize
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_DeInit()
{

    spi_uninit(SPIM);

}

/***************************************************************************
* Interface Description    : The APIs for reading/writing transfer continuously
*                             only with small buffer system
*                            For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void  Gpu_Hal_StartTransfer(Gpu_Hal_Context_t *host,GPU_TRANSFERDIR_T rw,uint32_t addr)
{
    if (GPU_READ == rw)
    {
        //EVE mem read

        uint8_t spidata[4];
        spidata[0] = (addr >> 16);
        spidata[1] = (addr >> 8);
        spidata[2] = addr & 0xff;
        spi_open(SPIM, host->hal_config.spi_cs_pin_no);

        spi_writen(SPIM, spidata, 3);


        host->status = GPU_HAL_READING;
    }
    else
    {
        //EVE mem write

        uint8_t spidata[4];
        spidata[0] = (0x80 | (addr >> 16));
        spidata[1] = (addr >> 8);
        spidata[2] = addr;

        spi_open(SPIM, host->hal_config.spi_cs_pin_no);
        spi_writen(SPIM, spidata, 3);

        host->status = GPU_HAL_WRITING;
    }
}


/***************************************************************************
* Interface Description    : The APIs for writing transfer continuously to RAM_CMD
*                            For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void  Gpu_Hal_StartCmdTransfer(Gpu_Hal_Context_t *host,GPU_TRANSFERDIR_T rw, uint16_t count)
{
    Gpu_Hal_StartTransfer(host,rw,host->cmd_fifo_wp + RAM_CMD);
}

/***************************************************************************
* Interface Description    :
*                            For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void    Gpu_Hal_TransferString(Gpu_Hal_Context_t *host,const char8_t *string)
{
    uint16_t length = strlen(string);
    while(length --)
    {
        Gpu_Hal_Transfer8(host,*string);
        string ++;
    }
    //Append one null as ending flag
    Gpu_Hal_Transfer8(host,0);
}



/***************************************************************************
* Interface Description    :Function to tranfer byte using SPI_Write()
*                           For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint8_t    Gpu_Hal_Transfer8(Gpu_Hal_Context_t *host,uint8_t value)
{

    uint8_t ReadByte;
    if (host->status == GPU_HAL_WRITING)
    {
        spi_write(SPIM, value);
    }
    else
    {
        spi_read(SPIM, ReadByte);
    }
    return ReadByte;

}

/***************************************************************************
* Interface Description    :
*                            For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint16_t  Gpu_Hal_Transfer16(Gpu_Hal_Context_t *host,uint16_t value)
{
    uint16_t retVal = 0;
    uint8_t v;
    if (host->status == GPU_HAL_WRITING)
    {
        v = (value & 0xFF);
        spi_write(SPIM, v);//LSB first
        v = (value >> 8) & 0xFF;
        spi_write(SPIM, v);
    }
    else
    {
        spi_read(SPIM, v);
        retVal = (uint16_t) v;
        spi_read(SPIM, v) ;
        retVal |= (uint16_t) v<< 8;
    }

    return retVal;
}

/***************************************************************************
* Interface Description    :
*                            For FT4222Lib, this function is obsolete
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint32_t  Gpu_Hal_Transfer32(Gpu_Hal_Context_t *host,uint32_t value)
{
    uint32_t retVal = 0;
    uint8_t v;
    if (host->status == GPU_HAL_WRITING)
    {
        v = value & 0xFF;
        spi_write(SPIM, v);//LSB first
        v = (value >> 8) & 0xFF;
        spi_write(SPIM, v);
        v = (value >> 16) & 0xFF;
        spi_write(SPIM, v);
        v = (value >> 24) & 0xFF;
        spi_write(SPIM, v);
    }
    else
    {
        spi_read(SPIM, v);
        retVal = (uint32_t) v;
        spi_read(SPIM, v);
        retVal |= (uint32_t)v << 8;
        spi_read(SPIM, v);
        retVal |= (uint32_t)v << 16;
        spi_read(SPIM, v);
        retVal |= (uint32_t)v << 24;
    }
    return retVal;
}

/***************************************************************************
* Interface Description    : Inactivate CS
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void   Gpu_Hal_EndTransfer(Gpu_Hal_Context_t *host)
{
    spi_close(SPIM, host->hal_config.spi_cs_pin_no);
    host->status = GPU_HAL_OPENED;
}



/***************************************************************************
* Interface Description    : EVE mem read API for 1 byte
* Implementation           :
* Return Value             : uint8_t
* Author                   :
****************************************************************************/
uint8_t  Gpu_Hal_Rd8(Gpu_Hal_Context_t *host,uint32_t addr)
{
    uint8_t value = 0;
    uint8_t spiData[4] = {0};
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    spi_readn(SPIM,spiData,host->spinumdummy + 1);
    value = spiData[host->spinumdummy];
    Gpu_Hal_EndTransfer(host);
    return value;
}


/***************************************************************************
* Interface Description    : EVE mem read API for 2 byte
* Implementation           :
* Return Value             : uint8_t
* Author                   :
****************************************************************************/
uint16_t Gpu_Hal_Rd16(Gpu_Hal_Context_t *host,uint32_t addr)
{
    uint16_t value;
    uint8_t spiData[4] = {0};
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    spi_readn(SPIM,spiData,host->spinumdummy + 2);
    value = spiData[host->spinumdummy] |(spiData[host->spinumdummy+1] << 8) ;
    Gpu_Hal_EndTransfer(host);
    return value;
}

/***************************************************************************
* Interface Description    : EVE mem read API for 4 byte
* Implementation           :
* Return Value             : uint8_t
* Author                   :
****************************************************************************/
uint32_t Gpu_Hal_Rd32(Gpu_Hal_Context_t *host,uint32_t addr)
{
    uint32_t value;
    uint8_t spiData[8] = {0};
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    spi_readn(SPIM,spiData,host->spinumdummy + 4);
    value = (spiData[host->spinumdummy+3] << 24) | (spiData[host->spinumdummy+2] << 16) | (spiData[host->spinumdummy+1] << 8) | spiData[host->spinumdummy];
    Gpu_Hal_EndTransfer(host);
    return value;
}

/***************************************************************************
* Interface Description    : EVE mem write API for 1 byte
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Wr8(Gpu_Hal_Context_t *host,uint32_t addr, uint8_t v)
{
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer8(host,v);
    Gpu_Hal_EndTransfer(host);
}


/***************************************************************************
* Interface Description    : EVE mem write API for 2 byte
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Wr16(Gpu_Hal_Context_t *host,uint32_t addr, uint16_t v)
{

    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer16(host,v);
    Gpu_Hal_EndTransfer(host);
}

/***************************************************************************
* Interface Description    : EVE mem write API for 4 byte
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Wr32(Gpu_Hal_Context_t *host,uint32_t addr, uint32_t v)
{

    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer32(host,v);
    Gpu_Hal_EndTransfer(host);
}

/***************************************************************************
* Interface Description    : Function to transfer the HOST CMD from host to
*                             EVE through lib service
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_HostCommand(Gpu_Hal_Context_t *host,uint8_t cmd)
{
    uint8_t hcmd[4] = {0};
    hcmd[0] = cmd;
    hcmd[1] = 0;
    hcmd[2] = 0;
    hcmd[3] = 0;

    spi_open(SPIM,host->hal_config.spi_cs_pin_no);
    spi_writen(SPIM,hcmd,3);
    spi_close(SPIM,host->hal_config.spi_cs_pin_no);
}


/***************************************************************************
* Interface Description    : This API sends a 3byte command from host to EVE
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_HostCommand_Ext3(Gpu_Hal_Context_t *host,uint32_t cmd)
{
    uint8_t hcmd[4] = {0};
    hcmd[0] = cmd & 0xff;
    hcmd[1] = (cmd>>8) & 0xff;
    hcmd[2] = (cmd>>16) & 0xff;
    hcmd[3] = 0;
    spi_open(SPIM,host->hal_config.spi_cs_pin_no);
    spi_writen(SPIM,hcmd,3);
    spi_close(SPIM,host->hal_config.spi_cs_pin_no);
}



/***************************************************************************
* Interface Description    : Toggle PD_N pin of FT800 board for a power cycle
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Powercycle(Gpu_Hal_Context_t *host, bool_t up)
{

    if (up)
    {
        gpio_write(host->hal_config.pdn_pin_no, 0);
        delay(20);
        gpio_write(host->hal_config.pdn_pin_no, 1);
        delay(20);
    }
    else
    {
        gpio_write(host->hal_config.pdn_pin_no, 1);
        delay(20);
        gpio_write(host->hal_config.pdn_pin_no, 0);
        delay(20);
    }
}

/***************************************************************************
* Interface Description    : Gpu_Hal_WrMemFromFlash and Gpu_Hal_WrMem ideally
*                            perform same operation.Find why was 2 created?
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WrMemFromFlash(Gpu_Hal_Context_t *host,uint32_t addr,const prog_uchar8_t *buffer, uint32_t length)
{
    Gpu_Hal_StartTransfer(host, GPU_WRITE, addr);
    while (length--) {
        Gpu_Hal_Transfer8(host,pgm_read_byte_near(buffer));
        buffer++;
    }
    Gpu_Hal_EndTransfer(host);
}

/***************************************************************************
* Interface Description    :
*
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WrMem(Gpu_Hal_Context_t *host,uint32_t addr,const uint8_t *buffer, uint32_t length)
{

    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    spi_writen(SPIM,buffer,length);
    Gpu_Hal_EndTransfer(host);

}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_RdMem(Gpu_Hal_Context_t *host,uint32_t addr, uint8_t *buffer, uint32_t length)
{

    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    unsigned char spiData[2] = {0};
    spi_readn(SPIM,spiData,host->spinumdummy);
    spi_readn(SPIM,buffer,length);

    Gpu_Hal_EndTransfer(host);
}

/***************************************************************************
* Interface Description    : API to check the status of previous DLSWAP and
                            perform DLSWAP of new DL.
                            Check for the status of previous DLSWAP and if 
                            still not done wait for few ms and check again
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_DLSwap(Gpu_Hal_Context_t *host, uint8_t DL_Swap_Type)
{
    uint8_t Swap_Type = DLSWAP_FRAME,Swap_Done = DLSWAP_FRAME;

    if(DL_Swap_Type == DLSWAP_LINE)
    {
        Swap_Type = DLSWAP_LINE;
    }

    /* Perform a new DL swap */
    Gpu_Hal_Wr8(host,REG_DLSWAP,Swap_Type);

    /* Wait till the swap is done */
    while(Swap_Done)
    {
        Swap_Done = Gpu_Hal_Rd8(host,REG_DLSWAP);
        if(DLSWAP_DONE != Swap_Done)
        {
            Gpu_Hal_Sleep(1);//wait for 1ms
        }
    }
}

/***************************************************************************
* Interface Description    : Calls platform specific sleep call
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Sleep(uint32_t ms)
{
    delayms(ms);
}


/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint32_t Gpu_CurrentFrequency(Gpu_Hal_Context_t *host)
{
    uint32_t t0, t1;
    //uint32_t addr = REG_CLOCK;
    //uint8_t spidata[4];
    int32_t r = 15625;

    t0 = Gpu_Hal_Rd32(host,REG_CLOCK); /* t0 read */

    __asm__
    (
    "   move.l  $r0,%0"             "\n\t"
    "   mul.l   $r0,$r0,100"                                                  "\n\t"
    "1:"               "\n\t"
    "   sub.l   $r0,$r0,3"          "\n\t" /* Subtract the loop time = 4 cycles */
    "   cmp.l   $r0,0"              "\n\t" /* Check that the counter is equal to 0 */
    "   jmpc    gt, 1b"  "\n\t"
    /* Outputs */ :
    /* Inputs */  : "r"(r)
    /* Using */   : "$r0"

    );

    t1 = Gpu_Hal_Rd32(host,REG_CLOCK); /* t1 read */
    return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}


/***************************************************************************
* Interface Description    : API to select clock source
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_ClockSelect(Gpu_Hal_Context_t *host,GPU_PLL_SOURCE_T pllsource)
{
    Gpu_HostCommand(host,pllsource);
}

/***************************************************************************
* Interface Description    : API to select frequency
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_PLL_FreqSelect(Gpu_Hal_Context_t *host,GPU_PLL_FREQ_T freq)
{
    Gpu_HostCommand(host,freq);
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_PowerModeSwitch(Gpu_Hal_Context_t *host,GPU_POWER_MODE_T pwrmode)
{
    Gpu_HostCommand(host,pwrmode);
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_CoreReset(Gpu_Hal_Context_t *host)
{
    Gpu_HostCommand(host,GPU_CORE_RESET);
}

/***************************************************************************
* Interface Description    : This API can only be called when PLL is stopped
*                            (SLEEP mode).For compatibility, set frequency to
*                            the GPU_12MHZ option in the GPU_SETPLLSP1_T
*                            table.
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
#ifdef FT81X_ENABLE
void Gpu_81X_SelectSysCLK(Gpu_Hal_Context_t *host, GPU_81X_PLL_FREQ_T freq)
{
    if(GPU_SYSCLK_72M == freq)
    Gpu_HostCommand_Ext3(host, (uint32_t)0x61 | (0x40 << 8) | (0x06 << 8));
    else if(GPU_SYSCLK_60M == freq)
    Gpu_HostCommand_Ext3(host, (uint32_t)0x61 | (0x40 << 8) | (0x05 << 8));
    else if(GPU_SYSCLK_48M == freq)
    Gpu_HostCommand_Ext3(host, (uint32_t)0x61 | (0x40 << 8) | (0x04 << 8));
    else if(GPU_SYSCLK_36M == freq)
    Gpu_HostCommand_Ext3(host, (uint32_t)0x61 | (0x03 << 8));
    else if(GPU_SYSCLK_24M == freq)
    Gpu_HostCommand_Ext3(host, (uint32_t)0x61 | (0x02 << 8));
    else if(GPU_SYSCLK_DEFAULT == freq)//default clock
    Gpu_HostCommand_Ext3(host, 0x61);
}

/***************************************************************************
* Interface Description    : Power down or up ROMs and ADCs.  Specified one
*                            or more elements in the GPU_81X_ROM_AND_ADC_T
*                            table to power down, unspecified elements will be
*                            powered up.  The application must retain the state
*                            of the ROMs and ADCs as they're not readable from
*                            the device.
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void GPU_81X_PowerOffComponents(Gpu_Hal_Context_t *host, uint8_t val)
{
    Gpu_HostCommand_Ext3(host, (uint32_t)0x49 | (val<<8));
}

/***************************************************************************
* Interface Description    : this API sets the current strength of supported
*                            GPIO/IO group(s)
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void GPU_81X_PadDriveStrength(Gpu_Hal_Context_t *host, GPU_81X_GPIO_DRIVE_STRENGTH_T strength, GPU_81X_GPIO_GROUP_T group)
{
    Gpu_HostCommand_Ext3(host, (uint32_t)0x70 | (group << 8) | (strength << 8));
}

/***************************************************************************
* Interface Description    : this API will hold the system reset active,
*                            Gpu_81X_ResetRemoval() must be called to
*                            release the system reset.
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_81X_ResetActive(Gpu_Hal_Context_t *host)
{
    Gpu_HostCommand_Ext3(host, GPU_81X_RESET_ACTIVE);
}

/***************************************************************************
* Interface Description    : This API will release the system reset, and the
*                            system will exit reset and behave as after POR,
*                            settings done through SPI commands will not be
*                            affected.
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_81X_ResetRemoval(Gpu_Hal_Context_t *host)
{
    Gpu_HostCommand_Ext3(host, GPU_81X_RESET_REMOVAL);
}
#endif

/***************************************************************************
* Interface Description    : Function to update global HAL context variable
*                             cmd_fifo_wp pointer and write to REG_CMD_WRITE
*                             to indicate GPU to start processing new commands
*                             in RAM_CMD
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Updatecmdfifo(Gpu_Hal_Context_t *host,uint32_t count)
{
    host->cmd_fifo_wp  = (host->cmd_fifo_wp + count) & FIFO_SIZE_MASK;

    //4 byte alignment
    host->cmd_fifo_wp = (host->cmd_fifo_wp + 3) & FIFO_BYTE_ALIGNMENT_MASK;
    Gpu_Hal_Wr16(host,REG_CMD_WRITE,host->cmd_fifo_wp);
}

/***************************************************************************
* Interface Description    : Function to compute available freespace in RAM_CMD.
*                             RAM_CMD is 4K in size.
*                             REG_CMD_READ reg provides command buffer read pointer
* Implementation           :
* Return Value             : uint16_t
* Author                   :
****************************************************************************/
uint16_t Gpu_Cmdfifo_Freespace(Gpu_Hal_Context_t *host)
{
    uint16_t fullness,retval;

    //host->cmd_fifo_wp = Gpu_Hal_Rd16(host,REG_CMD_WRITE);

    fullness = (host->cmd_fifo_wp - Gpu_Hal_Rd16(host,REG_CMD_READ)) & FIFO_SIZE_MASK;
    retval = (CMD_FIFO_SIZE - 4) - fullness;
    return (retval);
}

/***************************************************************************
* Interface Description    : Continuous write to RAM_CMD with wait with start
*                            address as host->cmd_fifo_wp + RAM_CMD.
*                            FT81x RAM_CMD size is 4K (4096 bytes)
*                            Hence one SPI write is adequate.
* Implementation           :
* Return Value             : uint16_t
* Author                   :
****************************************************************************/
void Gpu_Hal_WrCmdBuf(Gpu_Hal_Context_t *host,uint8_t *buffer,uint32_t count)
{
    uint32_t length =0,availablefreesize;


    do
    {
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
        {
            length = availablefreesize;
        }
        Gpu_Hal_CheckCmdBuffer(host,length);

        Gpu_Hal_StartCmdTransfer(host,GPU_WRITE,length);

        spi_writen(SPIM,buffer,length);
        buffer += length;

        Gpu_Hal_EndTransfer(host);

        Gpu_Hal_Updatecmdfifo(host,length);

        Gpu_Hal_WaitCmdfifo_empty(host);

        count -= length;
    }while (count > 0);
}



/***************************************************************************
* Interface Description    : Blocking function call
*                             Blocks until "count" number of bytes gets available
*                             in RAM_CMD
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_CheckCmdBuffer(Gpu_Hal_Context_t *host,uint32_t count)
{
    uint16_t getfreespace;
    do{
        getfreespace = Gpu_Cmdfifo_Freespace(host);
    }while(getfreespace < count);
}

/***************************************************************************
* Interface Description    : Blocking function call
*                             Blocks until all commands in RAM_CMD are executed and
*                             it is fully empty
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WaitCmdfifo_empty(Gpu_Hal_Context_t *host)
{
    while(Gpu_Hal_Rd16(host,REG_CMD_READ) != Gpu_Hal_Rd16(host,REG_CMD_WRITE));

    host->cmd_fifo_wp = Gpu_Hal_Rd16(host,REG_CMD_WRITE);
}

/***************************************************************************
* Interface Description    : Continuous write to RAM_CMD with no wait
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WrCmdBuf_nowait(Gpu_Hal_Context_t *host,uint8_t *buffer,uint32_t count)
{
    uint32_t length =0 , availablefreesize;

    do {
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
        {
            length = availablefreesize;
        }
        Gpu_Hal_CheckCmdBuffer(host,length);


        Gpu_Hal_StartCmdTransfer(host,GPU_WRITE,length);

        spi_writen(SPIM,buffer,length);
        buffer += length;

        Gpu_Hal_EndTransfer(host);

        Gpu_Hal_Updatecmdfifo(host,length);

        //    Gpu_Hal_WaitCmdfifo_empty(host);

        count -= length;
    }while (count > 0);
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint8_t Gpu_Hal_WaitCmdfifo_empty_status(Gpu_Hal_Context_t *host)
{
    if(Gpu_Hal_Rd16(host,REG_CMD_READ) != Gpu_Hal_Rd16(host,REG_CMD_WRITE))
    {
        return 0;
    }
    else
    {
        host->cmd_fifo_wp = Gpu_Hal_Rd16(host,REG_CMD_WRITE);
        return 1;
    }
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WaitLogo_Finish(Gpu_Hal_Context_t *host)
{
    int16_t cmdrdptr,cmdwrptr;

    do{
        cmdrdptr = Gpu_Hal_Rd16(host,REG_CMD_READ);
        cmdwrptr = Gpu_Hal_Rd16(host,REG_CMD_WRITE);
    }while ((cmdwrptr != cmdrdptr) || (cmdrdptr != 0));
    host->cmd_fifo_wp = 0;
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_ResetCmdFifo(Gpu_Hal_Context_t *host)
{
    host->cmd_fifo_wp = 0;
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_WrCmd32(Gpu_Hal_Context_t *host,uint32_t cmd)
{
    Gpu_Hal_CheckCmdBuffer(host,sizeof(cmd));

    Gpu_Hal_Wr32(host,RAM_CMD + host->cmd_fifo_wp,cmd);

    Gpu_Hal_Updatecmdfifo(host,sizeof(cmd));
}

/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_ResetDLBuffer(Gpu_Hal_Context_t *host)
{
    host->dl_buff_wp = 0;
}



/***************************************************************************
* Interface Description    : Helper api for dec to ascii
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
int32_t Gpu_Hal_Dec2Ascii(char8_t *pSrc,int32_t value)
{
    int16_t Length;
    char8_t *pdst,charval;
    int32_t CurrVal = value,tmpval,i;
    char8_t tmparray[16];
    uchar8_t idx = 0;

    Length = strlen(pSrc);
    pdst = pSrc + Length;

    if(0 == value)
    {
        *pdst++ = '0';
        *pdst++ = '\0';
        return 0;
    }

    if(CurrVal < 0)
    {
        *pdst++ = '-';
        CurrVal = - CurrVal;
    }
    /* insert the value */
    while(CurrVal > 0){
        tmpval = CurrVal;
        CurrVal /= 10;
        tmpval = tmpval - CurrVal*10;
        charval = '0' + tmpval;
        tmparray[idx++] = charval;
    }

    for(i=0;i<idx;i++)
    {
        *pdst++ = tmparray[idx - i - 1];
    }
    *pdst++ = '\0';

    return 0;
}



/***************************************************************************
* Interface Description    : Set EVE spi communication mode
*                            Set USB bridge communication mode
*                            Update global variable
* Implementation           :
* Return Value             : void
*                            -1 - Error, 0 - Success
* Author                   :
****************************************************************************/
#ifdef FT81X_ENABLE
int16_t Gpu_Hal_SetSPI(Gpu_Hal_Context_t *host,GPU_SPI_NUMCHANNELS_T numchnls,GPU_SPI_NUMDUMMYBYTES numdummy)
{
    uint8_t writebyte = 0;

    if((numchnls > GPU_SPI_QUAD_CHANNEL) || (numdummy > GPU_SPI_TWODUMMY) || (numdummy < GPU_SPI_ONEDUMMY))
    {
        return -1;//error
    }

    //swicth EVE to multi channel SPI mode
    writebyte = numchnls;
    if(numdummy == GPU_SPI_TWODUMMY)
    writebyte |= SPI_TWO_DUMMY_BYTE;
    Gpu_Hal_Wr8(host, REG_SPI_WIDTH, writebyte);

    //FT81x swicthed to dual/quad mode, now update global HAL context
    host->spichannel = numchnls;
    host->spinumdummy = numdummy;

    return 0;
}
#endif



/***************************************************************************
* Interface Description    : FIFO related apis
*                            Init all the parameters of fifo buffer
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Fifo_Init(Fifo_t *pFifo,uint32_t StartAddress,uint32_t Length,uint32_t HWReadRegAddress,uint32_t HWWriteRegAddress)
{
    /* update the context parameters */
    pFifo->fifo_buff = StartAddress;
    pFifo->fifo_len = Length;
    pFifo->fifo_rp = pFifo->fifo_wp = 0;

    /* update the hardware register addresses - specific to FT800 series chips */
    pFifo->HW_Read_Reg = HWReadRegAddress;
    pFifo->HW_Write_Reg = HWWriteRegAddress;
}

/***************************************************************************
* Interface Description    : FIFO related apis
*                            update both the read and write pointers
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Fifo_Update(Gpu_Hal_Context_t *host,Fifo_t *pFifo)
{
    pFifo->fifo_rp = Gpu_Hal_Rd32(host,pFifo->HW_Read_Reg);
    //Gpu_Hal_Wr32(host,pFifo->HW_Write_Reg,pFifo->fifo_wp);
}

/***************************************************************************
* Interface Description    : FIFO related apis
*                            just write and update the write register
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint32_t Fifo_Write(Gpu_Hal_Context_t *host,Fifo_t *pFifo,uint8_t *buffer,uint32_t NumbytetoWrite)
{
    uint32_t FreeSpace = Fifo_GetFreeSpace(host,pFifo),TotalBytes = NumbytetoWrite;

    if(NumbytetoWrite > FreeSpace)
    {
        /* update the read pointer and get the free space */
        Fifo_Update(host,pFifo);
        FreeSpace = Fifo_GetFreeSpace(host,pFifo);

        if(NumbytetoWrite > FreeSpace)
        {
            TotalBytes = FreeSpace;
        }
    }

    /* sanity check */
    if(TotalBytes <= 0)
    {
        //printf("no space in fifo write %d %d %d %d\n",TotalBytes,FreeSpace,pFifo->fifo_wp,pFifo->fifo_rp);
        return 0;//error condition
    }
    /* check for the loopback conditions */
    if(pFifo->fifo_wp + TotalBytes >= pFifo->fifo_len)
    {
        uint32_t partialchunk = pFifo->fifo_len - pFifo->fifo_wp,secpartialchunk = TotalBytes - partialchunk;

        Gpu_Hal_WrMem(host,pFifo->fifo_buff + pFifo->fifo_wp,buffer,partialchunk);
        if(secpartialchunk > 0)
        {
            Gpu_Hal_WrMem(host,pFifo->fifo_buff,buffer + partialchunk,secpartialchunk);
        }
        pFifo->fifo_wp = secpartialchunk;
        //printf("partial chunks %d %d %d %d\n",partialchunk,secpartialchunk,pFifo->fifo_wp,pFifo->fifo_rp);

    }
    else
    {
        Gpu_Hal_WrMem(host,pFifo->fifo_buff + pFifo->fifo_wp,buffer,TotalBytes);
        pFifo->fifo_wp += TotalBytes;
    }

    /* update the write pointer address in write register */
    Gpu_Hal_Wr32(host,pFifo->HW_Write_Reg,pFifo->fifo_wp);

    return TotalBytes;
}

/***************************************************************************
* Interface Description    : FIFO related apis
*                            just write one word and update the write register
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Fifo_Write32(Gpu_Hal_Context_t *host,Fifo_t *pFifo,uint32_t WriteWord)
{
    Fifo_WriteWait(host,pFifo,(uint8_t *)&WriteWord,4);
}

/***************************************************************************
* Interface Description    : FIFO related apis
*                            write and wait for the fifo to be empty. handle cases even if
*                            the Numbytes are more than freespace
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Fifo_WriteWait(Gpu_Hal_Context_t *host,Fifo_t *pFifo,uint8_t *buffer,uint32_t Numbyte)
{
    uint32_t TotalBytes = Numbyte,currchunk = 0,FreeSpace;
    uint8_t *pbuff = buffer;
    /* blocking call, manage to check for the error case and break in case of error */
    while(TotalBytes > 0)
    {
        currchunk = TotalBytes;
        FreeSpace = Fifo_GetFreeSpace(host,pFifo);
        if(currchunk > FreeSpace)
        {
            currchunk = FreeSpace;
        }

        Fifo_Write(host,pFifo,pbuff,currchunk);
        pbuff += currchunk;
        TotalBytes -= currchunk;


    }
}



/***************************************************************************
* Interface Description    : FIFO related apis
*                            get the free space in the fifo - make sure the
*                            return value is maximum of (LENGTH - 4)
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
uint32_t Fifo_GetFreeSpace(Gpu_Hal_Context_t *host,Fifo_t *pFifo)
{
    uint32_t FreeSpace = 0;

    Fifo_Update(host,pFifo);

    if(pFifo->fifo_wp >= pFifo->fifo_rp)
    {
        FreeSpace = pFifo->fifo_len - pFifo->fifo_wp + pFifo->fifo_rp;
    }
    else
    {
        FreeSpace = pFifo->fifo_rp - pFifo->fifo_wp;
    }

    if(FreeSpace >= 4)
    {
        FreeSpace -= 4;//make sure 1 word space is maintained between rd and wr pointers
    }
    return FreeSpace;
}


/***************************************************************************
* Interface Description    :
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
int32_t Gpu_ClockTrimming(Gpu_Hal_Context_t *host,int32_t LowFreq)
{
    uint32_t f;
    uint8_t i;

    /* Trim the internal clock by increase the REG_TRIM register till the measured frequency is within the acceptable range.*/
    for (i=0; (i < 31) && ((f= Gpu_CurrentFrequency(host)) < LowFreq); i++)
    {
        Gpu_Hal_Wr8(host,REG_TRIM, i);  /* increase the REG_TRIM register value automatically increases the internal clock */

    }

    Gpu_Hal_Wr32(host,REG_FREQUENCY,f);  /* Set the final frequency to be used for internal operations */

    return f;
}

/* Boot up for FT800 followed by graphics primitive sample cases */
/* Initial boot up DL - make the back ground green color */
const uint8_t DLCODE_BOOTUP[12] =
{
    0,0,0,2,//GPU instruction CLEAR_COLOR_RGB
    7,0,0,38, //GPU instruction CLEAR

    0,0,0,0,  //GPU instruction DISPLAY
};
/***************************************************************************
* Interface Description    : Clear the screen
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_ClearScreen(Gpu_Hal_Context_t *host)
{
    Gpu_Hal_WrMem(host, RAM_DL,(uint8_t *)DLCODE_BOOTUP,sizeof(DLCODE_BOOTUP));
    Gpu_Hal_Wr8(host, REG_DLSWAP,DLSWAP_FRAME);
}

#if defined(FT811_ENABLE) || defined(FT813_ENABLE)
#define TOUCH_DATA_LEN 1172

static uint8_t TOUCH_DATA_U8 [TOUCH_DATA_LEN]={
 26,255,255,255,32,32,48,0,4,0,0,0,2,0,0,0,34,255,255,255,0,176,48,0,120,218,237,84,255,107,92,69,16,159,125,155,107,141,201,121,247,106,130,9,225,244,238,37,246,146,52,63,53,98,172,53,48,243,8,36,166,182,63,136,216,246,7,205,219,75,122,119,185,139,196,128,34,33,136,123,65,240,7,31,44,105,41,69,72,12,210,166,6,17,12,104,64,165,73,133,134,98,80,74,43,148,22,20,133,40,20,34,233,15,82,250,131,10,113,246,229,106,197,191,161,111,217,55,179,59,59,59,243,153,47,251,135,15,58,204,11,109,114,89,149,84,169,242,172,2,109,10,73,53,167,92,158,89,21,6,89,230,236,126,86,89,206,242,169,178,61,187,115,62,85,190,199,133,129,44,132,249,90,12,243,124,67,49,169,76,81,22,195,60,211,113,40,133,249,253,104,114,113,148,197,131,105,169,158,3,19,132,65,28,187,240,25,112,3,147,99,235,163,80,98,63,10,123,181,208,73,213,29,24,246,226,121,112,199,146,74,98,56,22,195,135,245,32,14,194,17,180,182,100,46,169,220,114,244,103,171,114,252,41,222,143,97,150,53,64,167,202,110,240,86,186,45,16,90,14,191,158,110,196,163,216,130,241,232,159,42,251,88,4,169,90,80,42,163,226,248,168,206,97,132,136,169,69,113,20,27,245,203,40,131,12,54,87,37,143,253,43,105,97,137,9,172,103,181,26,172,79,163,102,184,142,57,83,0,61,139,144,159,79,135,193,62,61,0,157,250,5,20,186,67,91,255,133,238,140,104,167,30,192,197,72,2,85,137,221,89,134,101,140,177,191,238,137,75,105,137,23,97,18,226,31,175,98,31,174,165,141,90,193,21,88,194,117,60,207,243,115,248,202,254,171,171,37,248,20,87,112,17,59,180,59,246,35,199,177,83,175,35,140,93,224,187,172,45,169,172,108,29,47,192,59,186,70,159,1,208,155,136,40,71,190,195,101,222,117,131,221,90,142,110,48,221,85,165,177,42,173,169,82,89,165,78,149,138,136,110,224,13,108,200,0,132,1,50,103,114,55,176,145,254,51,132,208,183,177,137,64,111,128,192,13,104,162,86,145,165,198,255,141,86,209,68,79,146,84,87,33,163,95,132,39,120,182,242,76,115,78,123,40,142,7,200,227,21,103,50,103,171,109,123,27,242,115,193,0,197,176,70,63,141,125,212,35,14,8,95,28,20,246,116,155,30,132,152,118,3,40,29,162,62,60,158,9,131,8,197,120,49,99,115,35,39,228,200,168,48,81,197,2,175,221,224,205,140,173,148,4,87,13,148,94,19,235,216,205,62,212,179,14,199,221,234,21,223,207,112,254,163,90,169,197,247,50,110,249,16,249,156,113,137,83,98,146,163,23,215,115,16,146,173,102,142,158,74,160,205,181,28,217,67,93,156,121,40,156,202,76,130,169,184,193,12,9,125,82,132,149,121,209,65,219,198,12,47,80,7,15,182,163,97,52,171,206,211,20,186,229,195,148,42,15,137,45,116,244,29,188,138,105,177,76,139,149,18,113,141,160,192,1,218,222,14,131,101,90,224,17,195,239,51,113,156,194,111,245,21,49,137,171,100,61,238,21,166,50,67,95,83,63,154,202,71,116,141,45,205,208,37,94,133,188,186,201,177,134,82,86,117,84,109,46,137,52,237,214,102,248,22,253,82,41,137,91,148,37,29,97,147,195,86,126,89,156,165,45,214,188,43,54,43,160,63,36,139,169,142,17,73,175,31,255,230,232,164,175,244,161,197,38,71,118,121,70,109,226,43,252,14,100,249,142,135,180,156,48,185,45,172,176,247,71,244,29,104,240,25,3,37,216,78,179,159,101,171,9,63,225,95,166,6,126,71,24,29,156,165,199,253,126,108,119,88,51,146,216,19,191,137,102,191,107,186,68,109,78,247,244,95,130,99,94,134,49,183,128,158,84,207,58,245,129,9,220,145,78,16,218,85,102,226,94,102,142,121,247,51,243,146,119,63,51,3,14,71,157,94,245,86,169,151,134,156,99,212,206,189,148,208,9,109,239,43,242,125,57,135,249,19,111,120,245,182,79,121,218,62,110,231,222,203,57,220,245,52,237,217,126,246,253,183,89,95,59,59,145,238,165,15,28,91,45,86,239,140,215,231,199,56,34,59,239,154,239,159,244,38,57,63,167,156,41,126,95,14,83,82,189,203,183,244,51,186,33,39,12,234,244,105,156,79,111,161,100,186,232,37,25,187,195,253,252,137,35,245,2,243,54,207,243,8,176,143,109,175,225,57,92,131,71,248,220,151,222,113,255,28,26,114,131,139,17,130,111,24,65,63,235,157,198,90,188,238,213,251,63,120,130,59,151,187,130,95,193,207,252,26,214,221,175,215,240,186,211,85,217,197,107,67,123,113,15,217,42,252,201,155,245,133,94,131,122,255,87,207,80,143,215,238,91,47,126,247,106,248,204,159,153,187,116,219,49,116,45,115,147,182,48,242,4,190,240,127,118,224,193,247,224,3,89,247,15,148,99,211,103,26,255,255,255,20,33,48,0,4,0,0,0,15,0,0,0,26,255,255,255,32,32,48,0,4,0,0,0,0,0,0,0};
#endif

void BootupConfig(Gpu_Hal_Context_t *host)
{
    Gpu_Hal_Powercycle(host,TRUE);

    /* FT81x will be in SPI Single channel after POR
    If we are here with FT4222 in multi channel, then
    an explicit switch to single channel is essential
    */
    #ifdef FT81X_ENABLE
    Gpu_Hal_SetSPI(host, GPU_SPI_SINGLE_CHANNEL, GPU_SPI_ONEDUMMY);
    #endif

    /* Set the clk to external clock */
    #if (!defined(ME800A_HV35R) && !defined(ME810A_HV35R) && !defined(ME812AU_WH50R) && !defined(ME813AU_WH50C) && !defined(ME810AU_WH70R) && !defined(ME811AU_WH70C))
        Gpu_HostCommand(host,GPU_EXTERNAL_OSC);
        Gpu_Hal_Sleep(10);
    #endif

    /* Access address 0 to wake up the FT800 */
    Gpu_HostCommand(host,GPU_ACTIVE_M);
    Gpu_Hal_Sleep(300);

	#if defined(FT811_ENABLE) || defined(FT813_ENABLE)
    /* Download new firmware to JTouch to fix bug pen up */
    Gpu_Hal_WrCmdBuf(host, TOUCH_DATA_U8, TOUCH_DATA_LEN);
    Gpu_Hal_WaitCmdfifo_empty(host);
	#endif

    {
        uint8_t chipid;
        //Read Register ID to check if FT800 is ready.
        chipid = Gpu_Hal_Rd8(host, REG_ID);
        while(chipid != 0x7C)
        {
            chipid = Gpu_Hal_Rd8(host, REG_ID);
            delay(100);
        }

        printf("VC1 register ID after wake up %x\n",chipid);

    }
    /* Configuration of LCD display */



#if defined(ME800A_HV35R)
    /* After recognizing the type of chip, perform the trimming if necessary */
    Gpu_ClockTrimming(host,LOW_FREQ_BOUND);
#endif

    Gpu_Hal_Wr16(host, REG_HCYCLE, DispHCycle);
    Gpu_Hal_Wr16(host, REG_HOFFSET, DispHOffset);
    Gpu_Hal_Wr16(host, REG_HSYNC0, DispHSync0);
    Gpu_Hal_Wr16(host, REG_HSYNC1, DispHSync1);
    Gpu_Hal_Wr16(host, REG_VCYCLE, DispVCycle);
    Gpu_Hal_Wr16(host, REG_VOFFSET, DispVOffset);
    Gpu_Hal_Wr16(host, REG_VSYNC0, DispVSync0);
    Gpu_Hal_Wr16(host, REG_VSYNC1, DispVSync1);
    Gpu_Hal_Wr8(host, REG_SWIZZLE, DispSwizzle);
    Gpu_Hal_Wr8(host, REG_PCLK_POL, DispPCLKPol);
    Gpu_Hal_Wr16(host, REG_HSIZE, DispWidth);
    Gpu_Hal_Wr16(host, REG_VSIZE, DispHeight);
    Gpu_Hal_Wr16(host, REG_CSPREAD, DispCSpread);
    Gpu_Hal_Wr16(host, REG_DITHER, DispDither);

#if (defined(FT800_ENABLE) || defined(FT810_ENABLE) ||defined(FT812_ENABLE))
    /* Touch configuration - configure the resistance value to 1200 - this value is specific to customer requirement and derived by experiment */
    Gpu_Hal_Wr16(host, REG_TOUCH_RZTHRESH,RESISTANCE_THRESHOLD);
#endif
#if defined(FT81X_ENABLE)
    Gpu_Hal_Wr16(host, REG_GPIOX_DIR, 0xffff);
    Gpu_Hal_Wr16(host, REG_GPIOX, 0xffff);
#else
    Gpu_Hal_Wr8(host, REG_GPIO_DIR,0xff);
    Gpu_Hal_Wr8(host, REG_GPIO,0xff);
#endif

    Gpu_ClearScreen(host);
    Gpu_Hal_Wr8(host, REG_PCLK,DispPCLK);//after this display is visible on the LCD


#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
    /* to cross check reset pin */
    Gpu_Hal_Wr8(host, REG_GPIO,0xff);
    delay(120);
    Gpu_Hal_Wr8(host, REG_GPIO,0x7f);
    delay(120);
    Gpu_Hal_Wr8(host, REG_GPIO,0xff);

    /* Boot ILI9488 */
    ILI9488_Bootup();

    /* Reconfigure the SPI */

    printf("after ILI9488 bootup \n");
    //spi
    // Initialize SPIM HW
    sys_enable(sys_device_spi_master);
    gpio_function(GPIO_SPIM_CLK, pad_spim_sck); /* GPIO27 to SPIM_CLK */
    gpio_function(GPIO_SPIM_SS0, pad_spim_ss0); /* GPIO28 as CS */
    gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi); /* GPIO29 to SPIM_MOSI */
    gpio_function(GPIO_SPIM_MISO, pad_spim_miso); /* GPIO30 to SPIM_MISO */

    gpio_write(GPIO_SPIM_SS0, 1);
    spi_init(SPIM, spi_dir_master, spi_mode_0, 4);


#endif



    /* make the spi to quad mode - addition 2 bytes for silicon */
#ifdef FT81X_ENABLE
    /* api to set quad and numbe of dummy bytes */
#ifdef ENABLE_SPI_QUAD
    Gpu_Hal_SetSPI(host,GPU_SPI_QUAD_CHANNEL,GPU_SPI_TWODUMMY);
#elif ENABLE_SPI_DUAL
    Gpu_Hal_SetSPI(host,GPU_SPI_DUAL_CHANNEL,GPU_SPI_TWODUMMY);
#else
    Gpu_Hal_SetSPI(host,GPU_SPI_SINGLE_CHANNEL,GPU_SPI_ONEDUMMY);
#endif

#endif


    /* Change clock frequency to 25mhz */
    spi_init(SPIM, spi_dir_master, spi_mode_0, 4);

#if (defined(ENABLE_SPI_QUAD))
    /* Initialize IO2 and IO3 pad/pin for dual and quad settings */
    gpio_function(GPIO_SPIM_IO2, pad_spim_io2);
    gpio_function(GPIO_SPIM_IO3, pad_spim_io3);
    gpio_write(GPIO_SPIM_IO2, 1);
    gpio_write(GPIO_SPIM_IO3, 1);
#endif
    /* Enable FIFO of QSPI */
    spi_option(SPIM,spi_option_fifo_size,64);
    spi_option(SPIM,spi_option_fifo,1);
    spi_option(SPIM,spi_option_fifo_receive_trigger,1);


#ifdef ENABLE_SPI_QUAD

    //SPIMConfig.format         = DQSPI_TRANSFER_QUAD;            // Single data transfer
    spi_option(SPIM,spi_option_bus_width,4);

    //printf("QUAD SPI Configured \n");
#elif ENABLE_SPI_DUAL

    //SPIMConfig.format         = DQSPI_TRANSFER_DUAL;            // Single data transfer
    spi_option(SPIM,spi_option_bus_width,2);

    //printf("Dual SPI Configured \n");
#else

    spi_option(SPIM,spi_option_bus_width,1);

    //printf("Single channel SPI Configured %x \n",Gpu_Hal_Rd8(host, REG_ID));
#endif

    host->cmd_fifo_wp = Gpu_Hal_Rd16(host,REG_CMD_WRITE);

#if defined(FT9XX_PLATFORM) && defined(USE_SDCARD)
    sdcard_init(host);
#endif

}
/*==========================================================================
*
*                      FOR FT9XX PLATFORMS ONLY
*
*=========================================================================*/

/* Helper api for millis */
/* api to return the time in ms. 0 after reset */

/* Globals for polling implementation */
uint32_t millis_curr = 0,millis_prev = 0;

/* Globals for interrupt implementation */
uint32_t TotalMilliseconds = 0;

void millis_ticker()
{
    timer_disable_interrupt(FT900_MILLIS_TIMER);
    /* Clear the interrupt and increment the counter */
    timer_is_interrupted(FT900_MILLIS_TIMER);

    TotalMilliseconds += 1;
    timer_enable_interrupt(FT900_MILLIS_TIMER);
}
void millis_init()
{

    millis_curr = 0;
    millis_prev = 0;

    sys_enable(sys_device_timer_wdt);
    timer_prescaler(FT900_TIMER_PRESCALE_VALUE);
    timer_init(FT900_MILLIS_TIMER,FT900_TIMER_OVERFLOW_VALUE,timer_direction_up,timer_prescaler_select_on,timer_mode_continuous);

    interrupt_attach(interrupt_timers, 17, millis_ticker);


    /* enabling the interrupts for timer */
    timer_enable_interrupt(FT900_MILLIS_TIMER);

    timer_start(FT900_MILLIS_TIMER);

}
/* Need to ensure that below api is called at least once in 6.5 seconds duration for FT900 platform as this module doesnt use timer for context update */
/* global counter to loopback after ~49.71 days */
uint32_t get_millis()
{
    /* Interrupt implementation */
    return (TotalMilliseconds);
}

void millis_exit()
{
    timer_stop(FT900_MILLIS_TIMER);
    timer_disable_interrupt(FT900_MILLIS_TIMER);

}

void getFlashTextString(char __flash__ *str, uchar8_t *destArray, uint16_t numOfChars)
{
    uint16_t i;
    for(i=0;i<numOfChars;i++)
    destArray[i] = str[i];
}

void Gpu_Hal_WrCmdBufFromFlash(Gpu_Hal_Context_t *host, PROGMEM prog_uchar8_t *buffer,uint32_t count)
{
    uint32_t length =0, SizeTransfered = 0;

#define MAX_CMD_FIFO_TRANSFER   Gpu_Cmdfifo_Freespace(host)
    do {
        length = count;
        if (length > MAX_CMD_FIFO_TRANSFER){
            length = MAX_CMD_FIFO_TRANSFER;
        }
        Gpu_Hal_CheckCmdBuffer(host,length);

        Gpu_Hal_StartCmdTransfer(host,GPU_WRITE,length);


        SizeTransfered = 0;
        while (length--) {
            Gpu_Hal_Transfer8(host,pgm_read_byte_near(buffer));
            buffer++;
            SizeTransfered ++;
        }
        length = SizeTransfered;

        Gpu_Hal_EndTransfer(host);
        Gpu_Hal_Updatecmdfifo(host,length);

        Gpu_Hal_WaitCmdfifo_empty(host);

        count -= length;
    }while (count > 0);
}



void FT9XX_Config()
{
    sys_enable(sys_device_uart0);
    gpio_function(GPIO_UART0_TX, pad_uart0_txd); /* UART0 TXD */
    gpio_function(GPIO_UART0_RX, pad_uart0_rxd); /* UART0 RXD */
    uart_open(UART0,                    /* Device */
    1,                        /* Prescaler = 1 */
    UART_DIVIDER_115200_BAUD,  /* Divider = 1302 */
    uart_data_bits_8,         /* No. Data Bits */
    uart_parity_none,         /* Parity */
    uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,

    "(C) Copyright Bridgetek Pte Ltd\r\n"
    "--------------------------------------------------------------------- \r\n"
    "Welcome to FT8XX Example ... \r\n"
    "\r\n"
    "--------------------------------------------------------------------- \r\n"
    );

#ifdef ENABLE_ILI9488_HVGA_PORTRAIT
    /* asign all the respective pins to GPIO and set them to default values */
    gpio_function(GPIO_ILI9488_DCX, pad_ili9488_dcx);
    gpio_dir(GPIO_ILI9488_DCX, pad_dir_output);
    gpio_write(GPIO_ILI9488_DCX,1);

    gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
    gpio_dir(GPIO_SPIM_CLK, pad_dir_output);
    gpio_write(GPIO_SPIM_CLK,1);

    gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
    gpio_dir(GPIO_SPIM_MOSI, pad_dir_output);
    gpio_write(GPIO_SPIM_MOSI,1);

    gpio_function(GPIO_ILI9488_CS1, pad_ili9488_cs1);
    gpio_dir(GPIO_ILI9488_CS1, pad_dir_output);
    gpio_write(GPIO_ILI9488_CS1,1);


    gpio_function(GPIO_SPIM_MISO, pad_spim_miso);
    gpio_dir(GPIO_SPIM_MISO, pad_dir_output);
    gpio_write(GPIO_SPIM_MISO,1);

    gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
    gpio_dir(GPIO_SPIM_SS0, pad_dir_output);
    gpio_write(GPIO_SPIM_SS0,1);


    gpio_function(GPIO_PWD, pad_pwd);
    gpio_dir(GPIO_PWD, pad_dir_output);
    gpio_write(GPIO_PWD,1);

    gpio_write(GPIO_ILI9488_DCX,1);
    gpio_write(GPIO_SPIM_SS0,1);
    gpio_write(GPIO_PWD,1);
    gpio_write(GPIO_ILI9488_CS1,1);

#endif


    /* useful for timer */
#ifdef USE_FT9XX_TIMER
    millis_init();
    interrupt_enable_globally();
    //printf("FT900 config done \n");
#endif
}

#ifdef USE_SDCARD

FATFS            FatFs;                // FatFs work area needed for each volume
FIL              CurFile;
FRESULT          fResult;
SDHOST_STATUS    SDHostStatus;

void sdcard_init(Gpu_Hal_Context_t *host)
{
    uint8_t sd_status;
    sys_enable(sys_device_sd_card);
    /*use for sd card*/
    sdhost_sys_init();
    sdhost_init();
    gpio_function(GPIO_SD_CLK, pad_sd_clk); gpio_pull(GPIO_SD_CLK, pad_pull_none);//pad_pull_none
    gpio_function(GPIO_SD_CMD, pad_sd_cmd); gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT3, pad_sd_data3); gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT2, pad_sd_data2); gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT1, pad_sd_data1); gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT0, pad_sd_data0); gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
    gpio_function(GPIO_SD_CD, pad_sd_cd); gpio_pull(GPIO_SD_CD, pad_pull_pullup);
    gpio_function(GPIO_SD_WP, pad_sd_wp); gpio_pull(GPIO_SD_WP, pad_pull_pullup);

    sd_status = sdhost_card_detect();
    printf("\n sd_status=%x\n",sd_status);

    if(sd_status == SDHOST_CARD_INSERTED)
    {
        printf("-------------1---------------");
    }

    if (sd_status != SDHOST_CARD_INSERTED)
    {
        printf("Please Insert SD Card\r\n");
    }

    while (sd_status != SDHOST_CARD_INSERTED)
    {
        sd_status = sdhost_card_detect();
        Gpu_CoCmd_Dlstart(host);        // start
        Gpu_Hal_WrCmd32(host,CLEAR(1,1,1));
        Gpu_Hal_WrCmd32(host,COLOR_RGB(255,255,255));
        Gpu_CoCmd_Text(host,DispWidth>>1,DispHeight>>1,29,OPT_CENTER,"Storage Device Not Found");
        Gpu_Hal_WrCmd32(host,DISPLAY());
        Gpu_CoCmd_Swap(host);
        Gpu_Hal_WaitCmdfifo_empty(host);
    }

    printf("\n SD Card inserted!");

    if (f_mount(&FatFs, "", 0) != FR_OK)
    {
        printf("\n Mounted failed.");;
    }

    printf("\n Mounted succesfully.\n");
}

void Gpu_Hal_LoadImageToMemory(Gpu_Hal_Context_t *host, char8_t* fileName, uint32_t destination, uint8_t type){

    FIL InfSrc;
    int32_t blocklen,filesize;
    uint32_t g_random_buffer[512L];
    char tempName[250]="0:";

    strcat(tempName,fileName);
    fResult = f_open(&InfSrc, tempName, FA_READ | FA_OPEN_EXISTING);
    //fResult = f_open(&InfSrc, "0:\\overlayH.raw", FA_READ | FA_OPEN_EXISTING);
    if(fResult==FR_OK)
    {
        printf("Loading: %s\n",tempName);
        if(type==LOADIMAGE){  //loadimage command takes destination address and options before the actual bitmap data
            Gpu_Hal_WrCmd32(host,  CMD_LOADIMAGE);
            Gpu_Hal_WrCmd32(host,  destination);
            Gpu_Hal_WrCmd32(host,  OPT_NODL);
        }
        else if(type==INFLATE){ //inflate command takes destination address before the actual bitmap
            Gpu_Hal_WrCmd32(host,  CMD_INFLATE);
            Gpu_Hal_WrCmd32(host,  destination);
        }
        else if(type==LOAD){ //load bitmaps directly
        }

        filesize = f_size(&InfSrc);
        //printf("   Size: %d",filesize);
        while(filesize>0)
        {
            fResult = f_read(&InfSrc, g_random_buffer, 512, &blocklen);    // read a chunk of src file
            filesize -= blocklen;
            if(type==LOAD){
                Gpu_Hal_WrMem(host,destination,g_random_buffer, blocklen);
                destination += blocklen;
            }
            else
            {
                Gpu_Hal_WrCmdBuf_nowait(host,g_random_buffer, blocklen);//transfer data completely and check/wait for the last chunk only
            }


        }
        f_close(&InfSrc);

    }
    else{
        printf("Unable to open: %s error code: %d\n", fileName, fResult);
    }

}
#endif /* USE_SDCARD */

