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

Author : BRIDGETEK 

Revision History: 
0.1 - date 2017.03.20 - Initial Version
*/

#include "Platform.h"
#include "Gpu_Hal.h"

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
bool_t  Gpu_Hal_Init(Gpu_HalInit_t *halinit)
{


    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    /* Initialize the libmpsse */
    Init_libMPSSE();
    SPI_GetNumChannels(&halinit->TotalChannelNum);
    /* By default i am assuming only one mpsse cable is connected to PC and channel 0 of that mpsse cable is used for spi transactions */
    if (halinit->TotalChannelNum > 0)
    {
        FT_DEVICE_LIST_INFO_NODE devList;
        SPI_GetChannelInfo(0, &devList);
        printf(" Information on channel number %d:\n", 0);
        /* print the dev info */
        printf(" Flags=0x%x\n", devList.Flags);
        printf(" Type=0x%x\n", devList.Type);
        printf(" ID=0x%x\n", devList.ID);
        printf(" LocId=0x%x\n", devList.LocId);
        printf(" SerialNumber=%s\n", devList.SerialNumber);
        printf(" Description=%s\n", devList.Description);
        printf(" ftHandle=0x%x\n", devList.ftHandle);/*is 0 unless open*/
    }
    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)
    {
        FT_STATUS status;
        //ulong_t numdevs;
        uint32_t numdevs;

        status = FT_CreateDeviceInfoList(&numdevs);
        if (FT_OK == status)
        {
            printf("Number of D2xx devices connected = %d\n", numdevs);
            halinit->TotalChannelNum = numdevs;
        }
        else
        {
            printf("FT_CreateDeviceInfoList failed");
            return FALSE;
        }
    }
    #else
    #error "Usb bridge library configuration missing\n"
    #endif

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
bool_t    Gpu_Hal_Open(Gpu_Hal_Context_t *host)
{
    bool_t ret = TRUE;

    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    ChannelConfig channelConf;            //channel configuration
    FT_STATUS status;

    /* configure the spi settings */
    channelConf.ClockRate = host->hal_config.spi_clockrate_khz * 1000;
    channelConf.LatencyTimer= 2;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x00000000;    /*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    /* Open the first available channel */
    SPI_OpenChannel(host->hal_config.channel_no,(FT_HANDLE *)&host->hal_handle);
    status = SPI_InitChannel((FT_HANDLE)host->hal_handle,&channelConf);
    printf("\nhandle=0x%x status=0x%x\n",host->hal_handle,status);

    host->spichannel = 0;

    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)
    FT_STATUS status;
    //ulong_t numdevs;
    uint32_t numdevs;
    uint32_t index;
    FT_HANDLE fthandle;
    FT4222_Version pversion;
    FT4222_ClockRate ftclk = 0;
    uint16_t max_size = 0;
    FT4222_ClockRate selclk = 0;
    FT4222_SPIClock seldiv = 0;
    /* GPIO0         , GPIO1      , GPIO2       , GPIO3         } */
    GPIO_Dir gpio_dir[4] = { GPIO_OUTPUT , GPIO_INPUT , GPIO_INPUT, GPIO_INPUT };


    host->hal_handle = host->hal_handle2 = NULL;

    status = FT_CreateDeviceInfoList(&numdevs);
    if (FT_OK != status)
    {
        printf("FT_CreateDeviceInfoList failed");
        ret = FALSE;
    }

    status = FT_ListDevices(&numdevs, NULL, FT_LIST_NUMBER_ONLY);
    if (FT_OK != status)
    {
        printf("FT_ListDevices failed");
        ret = FALSE;
    }

    if (ret)
    {
        for (index = 0; (index<numdevs) && ret; index++)
        {
            FT_DEVICE_LIST_INFO_NODE devInfo;
            memset(&devInfo, 0, sizeof(devInfo));

            status = FT_GetDeviceInfoDetail(index,
            &devInfo.Flags, &devInfo.Type, &devInfo.ID, &devInfo.LocId,
            devInfo.SerialNumber, devInfo.Description, &devInfo.ftHandle);
            if (FT_OK == status)
            {
                printf("Dev %d:\n", index);
                printf(" Flags= 0x%x, (%s) (%s) \n", devInfo.Flags,
                ((devInfo.Flags & 0x01) ? "DEVICE_OPEN" : "DEVICE_CLOSED"), ((devInfo.Flags & 0x02) ? "High-speed USB" : "Full-speed USB"));
                printf(" Type= 0x%x\n", devInfo.Type);
                printf(" ID= 0x%x\n", devInfo.ID);
                printf(" LocId= 0x%x\n", devInfo.LocId);
                printf(" SerialNumber= %s\n", devInfo.SerialNumber);
                printf(" Description= %s\n", devInfo.Description);
                printf(" ftHandle= %p\n", devInfo.ftHandle);
            }
            else
            ret = FALSE;

            if(ret && !(devInfo.Flags & 0x01) && ((!strcmp(devInfo.Description, "FT4222 A") && (host->hal_handle == NULL)) || (!strcmp(devInfo.Description, "FT4222 B") && (host->hal_handle2 == NULL))))
            {
                /* obtain handle for the first discovered "FT4222 A" and first "FT4222 B" */
                status = FT_OpenEx(devInfo.Description, FT_OPEN_BY_DESCRIPTION, &fthandle);
                if (status != FT_OK)
                {
                    printf("FT_OpenEx failed %d\n", status);
                    ret = FALSE;
                }
                else
                {
                    if (!strcmp(devInfo.Description, "FT4222 A"))
                    {
                        //is SPI
                        host->hal_handle = fthandle; //SPI communication handle
                        printf("[%d]th of total connected devices is FT4222 A (SPI) : host->hal_hanlde = %p\n", index + 1, host->hal_handle);
                    }
                    else if (!strcmp(devInfo.Description, "FT4222 B"))
                    {
                        //is GPIO
                        host->hal_handle2 = fthandle; //GPIO communication handle
                        printf("[%d]th of total connected devices is FT4222 B (GPIO) : host->hal_hanlde = %p\n", index + 1, host->hal_handle);
                    }
                    else
                    {
                        printf("Error in FT4222 configuration\n");
                    }
                }
            }
            else
            {
                if(
                        (!strcmp(devInfo.Description, "FT4222 A") && host->hal_handle != NULL ) ||
                        (!strcmp(devInfo.Description, "FT4222 B") && host->hal_handle2 != NULL)
                        )
                printf("[%d]th of total connected devices is not the first %s detected. Hence skipping.\n", index + 1, devInfo.Description);
                else if(devInfo.Flags & 0x01)
                printf("[%d]th of total connected devices is already open in another context. Hence skipping.\n", index + 1);
                else
                printf("[%d]th of total connected devices is not FT4222 but is %s. Hence skipping.\n", index+1 , devInfo.Description);
                continue;
            }
        }
    }

    if (ret)
    {
        status = FT4222_GetVersion(host->hal_handle, &pversion);
        if (status != FT4222_OK)
        printf("FT4222_GetVersion failed\n");
        else
        printf("SPI:chipversion = 0x%x\t dllversion = 0x%x\n", pversion.chipVersion, pversion.dllVersion);
    }

    if (ret)
    {
        //Set default Read timeout 5s and Write timeout 5sec
        status = FT_SetTimeouts(host->hal_handle, FT4222_ReadTimeout, FT4222_WriteTimeout);
        if (FT_OK != status)
        {
            printf("FT_SetTimeouts failed!\n");
            ret = FALSE;
        }
    }

    if (ret)
    {
        // no latency to usb
        status = FT_SetLatencyTimer(host->hal_handle, FT4222_LatencyTime);
        if (FT_OK != status)
        {
            printf("FT_SetLatencyTimerfailed!\n");
            ret = FALSE;
        }
    }

    if (ret)
    {
        if (!Gpu_Hal_FT4222_ComputeCLK(host, &selclk, &seldiv))
        {
            printf("Requested clock %d KHz is not supported in FT4222 \n", host->hal_config.spi_clockrate_khz);
            ret = FALSE;
        }
    }

    if(ret)
    {
        status = FT4222_SetClock(host->hal_handle, selclk);
        if (FT_OK != status)
        {
            printf("FT4222_SetClock failed!\n");
            ret = FALSE;
        }

        status = FT4222_GetClock(host->hal_handle, &ftclk);

        if (FT_OK != status)
        printf("FT4222_SetClock failed\n");
        else
        printf("FT4222 clk = %d\n", ftclk);

    }

    if(ret)
    {
        /* Interface 1 is SPI master */
        status = FT4222_SPIMaster_Init(
        host->hal_handle,
        SPI_IO_SINGLE,
        seldiv,
        CLK_IDLE_LOW, //,CLK_IDLE_HIGH
        CLK_LEADING,// CLK_LEADING CLK_TRAILING
        host->hal_config.spi_cs_pin_no
        );    /* slave selection output pins */
        if (FT_OK != status)
        {
            printf("Init FT4222 as SPI master device failed!\n");
            ret = FALSE;
        }
        else
        host->spichannel = GPU_SPI_SINGLE_CHANNEL; //SPI_IO_SINGLE;

        status = FT4222_SPI_SetDrivingStrength(host->hal_handle, DS_4MA, DS_4MA, DS_4MA);
        if (FT4222_OK != status)
        printf("FT4222_SPI_SetDrivingStrength failed!\n");

        Gpu_Hal_Sleep(20);

        status = FT4222_SetSuspendOut(host->hal_handle2, FALSE);
        if (FT_OK != status)
        {
            printf("Disable suspend out function on GPIO2 failed!\n");
            ret = FALSE;
        }

        status = FT4222_SetWakeUpInterrupt(host->hal_handle2, FALSE);
        if (FT_OK != status)
        {
            printf("Disable wakeup/interrupt feature on GPIO3 failed!\n");
            ret = FALSE;
        }
        /* Interface 2 is GPIO */
        status = FT4222_GPIO_Init(host->hal_handle2, gpio_dir);
        if (FT_OK != status)
        {
            printf("Init FT4222 as GPIO interface failed!\n");
            ret = FALSE;
        }
    }

    /* dedicated write buffer used for SPI write. Max size is 2^uint16 */
    if ((host->spiwrbuf_ptr = malloc(FT4222_DYNAMIC_ALLOCATE_SIZE)) == NULL)
    {
        printf("malloc error\n");
        ret = FALSE;
    }

    #else
    #error "USB-to-SPI bridge configuration missing"
    #endif


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
void  Gpu_Hal_Close(Gpu_Hal_Context_t *host)
{
    #if defined (MSVC_PLATFORM_SPI_LIBFT4222)
    FT4222_STATUS status;
    #endif

    host->status = GPU_HAL_CLOSED;
    
    /* Close the channel*/
    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    SPI_CloseChannel(host->hal_handle);
    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)

    free(host->spiwrbuf_ptr);

    if (FT4222_OK != (status = FT4222_UnInitialize(host->hal_handle)))
    printf("FT4222_UnInitialize failed %d \n",status);

    if (FT4222_OK != (status = FT4222_UnInitialize(host->hal_handle2)))
    printf("FT4222_UnInitialize failed %d \n", status);

    if(FT_OK != (status = FT_Close(host->hal_handle)))
    printf("CLOSE failed %d \n", status);

    if (FT_OK != (status = FT_Close(host->hal_handle2)))
    printf("CLOSE failed %d \n", status);
    #else
    #error "Lib configuration missing"
    #endif
    
}

/***************************************************************************
* Interface Description    : Uninitialize
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_DeInit()
{
    //Cleanup the MPSSE Lib
    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    Cleanup_libMPSSE();
    #endif
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
        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
        uint8_t Transfer_Array[4];
        uint32_t SizeTransfered;

        /* Compose the read packet */
        Transfer_Array[0] = addr >> 16;
        Transfer_Array[1] = addr >> 8;
        Transfer_Array[2] = addr;

        Transfer_Array[3] = 0; //Dummy Read byte
        SPI_Write(
        (FT_HANDLE)host->hal_handle,
        Transfer_Array,
        sizeof(Transfer_Array),
        &SizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
        );
        #endif
        host->status = GPU_HAL_READING;
    }
    else
    {
        //EVE mem write
        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
        uint8_t Transfer_Array[3];
        uint32_t SizeTransfered;

        /* Compose the read packet */
        Transfer_Array[0] = (0x80 | (addr >> 16));
        Transfer_Array[1] = addr >> 8;
        Transfer_Array[2] = addr;
        SPI_Write(
        (FT_HANDLE)host->hal_handle,
        Transfer_Array,
        3,
        &SizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE
        );
        #endif
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
void  Gpu_Hal_TransferString(Gpu_Hal_Context_t *host,const char8_t *string)
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
#if defined(MSVC_PLATFORM_SPI_LIBMPSSE)        
    uint32_t SizeTransfered;
    if (host->status == GPU_HAL_WRITING) {
        SPI_Write(host->hal_handle, &value, sizeof(value), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
    }
    else {
        SPI_Read(host->hal_handle, &value, sizeof(value), &SizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
    }

    if (SizeTransfered != sizeof(value))
    host->status = GPU_HAL_STATUS_ERROR;
    return value;
#else
    return 0; // MSVC_PLATFORM_SPI_LIBFT4222 not support this 
#endif    
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

    if (host->status == GPU_HAL_WRITING)
    {
        Gpu_Hal_Transfer8(host,value & 0xFF);//LSB first
        Gpu_Hal_Transfer8(host,(value >> 8) & 0xFF);
    }
    else
    {
        retVal = Gpu_Hal_Transfer8(host,0);
        retVal |= (uint16_t)Gpu_Hal_Transfer8(host,0) << 8;
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
    
    if (host->status == GPU_HAL_WRITING)
    {
        Gpu_Hal_Transfer16(host,value & 0xFFFF);//LSB first
        Gpu_Hal_Transfer16(host,(value >> 16) & 0xFFFF);
    }
    else
    {
        retVal = Gpu_Hal_Transfer16(host,0);
        retVal |= (uint32_t)Gpu_Hal_Transfer16(host,0) << 16;
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
    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)        
    //just disbale the CS - send 0 bytes with CS disable
    SPI_ToggleCS((FT_HANDLE)host->hal_handle,FALSE);        
    #endif
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
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Rd(host, addr, &value, sizeof(value)))
    {
        printf("Gpu_Hal_FT4222_Rd failed\n");
    }           
    #else
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer8(host,0);
    Gpu_Hal_EndTransfer(host);
    #endif        
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

    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Rd(host, addr,(uint8_t *) &value, sizeof(value)))
    {
        printf("Gpu_Hal_FT4222_Rd failed\n");
    }            
    #else
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer16(host,0);
    Gpu_Hal_EndTransfer(host);
    #endif    
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

    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)           
    if (!Gpu_Hal_FT4222_Rd(host, addr,(uint8_t *) &value, sizeof(value)))
    {
        printf("Gpu_Hal_FT4222_Rd failed\n");
    }           
    #else
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    value = Gpu_Hal_Transfer32(host,0);
    Gpu_Hal_EndTransfer(host);
    #endif

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
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Wr(host, addr, &v, sizeof(v)))
    {
        printf("Gpu_Hal_FT4222_Wr failed\n");
    }
    #else
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer8(host,v);
    Gpu_Hal_EndTransfer(host);
    #endif
}


/***************************************************************************
* Interface Description    : EVE mem write API for 2 byte
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Wr16(Gpu_Hal_Context_t *host,uint32_t addr, uint16_t v)
{
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)        
    if (!Gpu_Hal_FT4222_Wr(host, addr, &v, sizeof(v)))
    {
        printf("Gpu_Hal_FT4222_Wr failed\n");
    }
    #else
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer16(host,v);
    Gpu_Hal_EndTransfer(host);
    #endif
}

/***************************************************************************
* Interface Description    : EVE mem write API for 4 byte
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_Wr32(Gpu_Hal_Context_t *host,uint32_t addr, uint32_t v)
{
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Wr(host, addr, &v, sizeof(v)))
    {
        printf("Gpu_Hal_FT4222_Wr failed\n");
    }
    #else
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    Gpu_Hal_Transfer32(host,v);
    Gpu_Hal_EndTransfer(host);
    #endif
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
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    FT4222_STATUS status;
    #endif

    
    uint8_t Transfer_Array[3];
    uint32_t SizeTransfered;    
    uint8_t dummy_read;

    Transfer_Array[0] = cmd;
    Transfer_Array[1] = 0;
    Transfer_Array[2] = 0;

    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    SPI_Write(
    host->hal_handle,Transfer_Array,
    sizeof(Transfer_Array),
    &SizeTransfered,
    SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE
    );
    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)            
    switch(host->spichannel)
    {
    case GPU_SPI_SINGLE_CHANNEL:
        /* FYI : All HOST CMDs should only be executed in single channel mode
                    */
        status = FT4222_SPIMaster_SingleWrite(
        host->hal_handle,
        Transfer_Array,
        sizeof(Transfer_Array),
        (uint16_t *)&SizeTransfered,
        TRUE
        );
        if (FT4222_OK != status)
        printf("SPI write failed = %d\n",status);
        break;
    case GPU_SPI_DUAL_CHANNEL:
    case GPU_SPI_QUAD_CHANNEL:
        /* only reset command among host commands can be executed in multi channel mode*/                    
        status = FT4222_SPIMaster_MultiReadWrite(
        host->hal_handle,
        &dummy_read,
        Transfer_Array,
        0,
        sizeof(Transfer_Array),
        0,
        &SizeTransfered
        );
        if (FT4222_OK != status)
        printf("SPI write failed = %d\n", status);
        break;
    default:
        printf("No transfer\n");
    }            
    #else
    #error "LIB configuration missing "
    #endif

}

/***************************************************************************
* Interface Description    : This API sends a 3byte command from host to EVE
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_HostCommand_Ext3(Gpu_Hal_Context_t *host,uint32_t cmd)
{
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    FT4222_STATUS status;
    uint8_t dummy_read;
    #endif

    uint8_t Transfer_Array[3];
    uint32_t SizeTransfered;
    
    Transfer_Array[0] = cmd;
    Transfer_Array[1] = (cmd>>8) & 0xff;
    Transfer_Array[2] = (cmd>>16) & 0xff;
    
    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
    SPI_Write(
    host->hal_handle,
    Transfer_Array,
    sizeof(Transfer_Array),
    &SizeTransfered,
    SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE
    );
    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)            
    switch (host->spichannel)
    {
    case GPU_SPI_SINGLE_CHANNEL:
        status = FT4222_SPIMaster_SingleWrite(
        host->hal_handle,
        Transfer_Array,
        sizeof(Transfer_Array),
        (uint16_t *) &SizeTransfered,
        TRUE
        );
        if (FT4222_OK != status)
        printf("SPI write failed = %d\n", status);
        break;
    case GPU_SPI_DUAL_CHANNEL:
    case GPU_SPI_QUAD_CHANNEL:
        /* FYI : Mostly all HOST CMDs can be executed in single channel mode
                * except system reset cmd */                  
        status = FT4222_SPIMaster_MultiReadWrite(
        host->hal_handle,
        &dummy_read,
        Transfer_Array,
        0,
        sizeof(Transfer_Array),
        0,
        &SizeTransfered
        );
        if (FT4222_OK != status)
        printf("SPI write failed = %d\n", status);
        break;
    default:
        printf("No transfer\n");
    }
    #else
    #error "LIB configuration missing "
    #endif          
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

        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
        //FT_WriteGPIO(host->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        FT_WriteGPIO(host->hal_handle, (1 << host->hal_config.pdn_pin_no) | 0x3B, (0<<host->hal_config.pdn_pin_no)|0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        
        Gpu_Hal_Sleep(20);

        //FT_WriteGPIO(host->hal_handle, 0xBB, 0x88);//PDN set to 1
        FT_WriteGPIO(host->hal_handle, (1 << host->hal_config.pdn_pin_no) | 0x3B, (1<<host->hal_config.pdn_pin_no)|0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        Gpu_Hal_Sleep(20);
        #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)
        FT4222_STATUS status = FT4222_OTHER_ERROR;

        if (FT4222_OK != (status = FT4222_GPIO_Write(host->hal_handle2, host->hal_config.pdn_pin_no, 0)))
        printf("FT4222_GPIO_Write error = %d\n",status);
        Gpu_Hal_Sleep(20);
        

        if (FT4222_OK != (status = FT4222_GPIO_Write(host->hal_handle2, host->hal_config.pdn_pin_no, 1)))
        printf("FT4222_GPIO_Write error = %d\n", status);
        Gpu_Hal_Sleep(20);
        #else
        #error "LIB configuration missing for MSVC platform"
        #endif
    }
    else
    {
        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)
        //FT_WriteGPIO(host->hal_handle, 0xBB, 0x88);//PDN set to 1
        FT_WriteGPIO(host->hal_handle, (1 << host->hal_config.pdn_pin_no) | 0x3B, (1<<host->hal_config.pdn_pin_no)|0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        Gpu_Hal_Sleep(20);
        
        //FT_WriteGPIO(host->hal_handle, 0xBB, 0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        FT_WriteGPIO(host->hal_handle, (1 << host->hal_config.pdn_pin_no) | 0x3B, (0<<host->hal_config.pdn_pin_no)|0x08);//PDN set to 0 ,connect BLUE wire of MPSSE to PDN# of FT800 board
        
        Gpu_Hal_Sleep(20);
        #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)
        FT4222_STATUS status = FT4222_OTHER_ERROR;

        if (FT4222_OK != (status = FT4222_GPIO_Write(host->hal_handle2, host->hal_config.pdn_pin_no, 1)))
        printf("FT4222_GPIO_Write error = %d\n", status);
        Gpu_Hal_Sleep(20);                

        if (FT4222_OK != (status = FT4222_GPIO_Write(host->hal_handle2, host->hal_config.pdn_pin_no, 0)))
        printf("FT4222_GPIO_Write error = %d\n", status);
        Gpu_Hal_Sleep(20);                
        #else
        #error "LIB configuration missing for MSVC platform"
        #endif
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
    uint32_t SizeTransfered = 0;      

    #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)    
    Gpu_Hal_StartTransfer(host, GPU_WRITE, addr);
	Gpu_Hal_LIBMPSSE_Wr(host,(uint8_t*)buffer,length,&SizeTransfered);
    Gpu_Hal_EndTransfer(host);
    #elif defined(MSVC_PLATFORM_SPI_LIBFT4222)
    Gpu_Hal_FT4222_Wr(host, addr, buffer, length);
    #endif
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
    uint32_t SizeTransfered = 0;      
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Wr(host, addr, buffer, length))
    {
        printf("Gpu_Hal_FT4222_Wr failed\n");
    }
    #else
    Gpu_Hal_StartTransfer(host,GPU_WRITE,addr);
    {
		Gpu_Hal_LIBMPSSE_Wr(host,(uint8_t*)buffer,length,&SizeTransfered);
    }
    Gpu_Hal_EndTransfer(host);
    #endif    
}

/***************************************************************************
* Interface Description    : 
* Implementation           :
* Return Value             : void
* Author                   :
****************************************************************************/
void Gpu_Hal_RdMem(Gpu_Hal_Context_t *host,uint32_t addr, uint8_t *buffer, uint32_t length)
{
    uint32_t SizeTransfered = 0;      
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    if (!Gpu_Hal_FT4222_Rd(host, addr, buffer, length))
    {
        printf("Gpu_Hal_FT4222_Rd failed\n");
    }
    #else
    Gpu_Hal_StartTransfer(host,GPU_READ,addr);
    {
		Gpu_Hal_LIBMPSSE_Rd(host,buffer,length,&SizeTransfered);
    }
    Gpu_Hal_EndTransfer(host);
    #endif    
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
    Sleep(ms);
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
    uint32_t addr = REG_CLOCK;
    uint8_t spidata[4];
    int32_t r = 15625;

    t0 = Gpu_Hal_Rd32(host,REG_CLOCK); /* t0 read */
    //may not be precise
    Sleep(15625/1000);

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
    uint32_t length =0, SizeTransfered = 0,availablefreesize;
    
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)  
    FT4222_STATUS status;
    uint8_t * wrpktptr;
    uint8_t dummy_read;
    wrpktptr = host->spiwrbuf_ptr;  //Using global buf , FT4222_DYNAMIC_ALLOCATE_SIZE
    #endif

    do 
    {                
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
        {
            length = availablefreesize;
        }
        Gpu_Hal_CheckCmdBuffer(host,length);

        #if defined(MSVC_PLATFORM_SPI_LIBFT4222)  
        *(wrpktptr + 0) = (uint8_t) ((host->cmd_fifo_wp + RAM_CMD) >> 16) | 0x80;
        *(wrpktptr + 1) = (uint8_t) ((host->cmd_fifo_wp + RAM_CMD) >> 8);
        *(wrpktptr + 2) = (uint8_t) (host->cmd_fifo_wp + RAM_CMD) & 0xff;
        memcpy((wrpktptr + 3), buffer, length);

        if (host->spichannel == GPU_SPI_SINGLE_CHANNEL)
        {
            status = FT4222_SPIMaster_SingleWrite(
            host->hal_handle,
            wrpktptr,
            (length + 3), //3 for RAM_CMD address
            (uint16_t *) &SizeTransfered,
            TRUE
            );
            if ((FT4222_OK != status) || (SizeTransfered != (length + 3)))
            {
                printf("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, SizeTransfered, status);
                break;
            }
        }
        else
        {    /* DUAL and QAUD */
            status = FT4222_SPIMaster_MultiReadWrite(
            host->hal_handle,
            &dummy_read,
            wrpktptr,
            0,
            (length + 3),
            0,
            &SizeTransfered
            );
        }
        buffer += length;
        #else
        Gpu_Hal_StartCmdTransfer(host,GPU_WRITE,length); 
        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)            
        {   
            SPI_Write(host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
            length = SizeTransfered;
            buffer += SizeTransfered;
        }        
        #endif
        Gpu_Hal_EndTransfer(host);
        #endif
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
	int rdptr, wrptr;
	char buffer[256] = { '\0' };
	do {
		rdptr = Gpu_Hal_Rd16(host, REG_CMD_READ);
		wrptr = Gpu_Hal_Rd16(host, REG_CMD_WRITE);
		if ((rdptr & 3) || (wrptr & 3))
		{
			Gpu_Hal_RdMem(host, 0x309800, buffer, 128);
			printf("Error in co processor %x %x %s\n", rdptr, wrptr, buffer);
		}
	} while (rdptr != wrptr);

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
    uint32_t length =0, SizeTransfered = 0,availablefreesize;
    
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)  
    FT4222_STATUS status;
    uint8_t * wrpktptr;
    uint8_t dummy_read;
    wrpktptr = host->spiwrbuf_ptr;  //Using global buf , FT4222_DYNAMIC_ALLOCATE_SIZE
    #endif

    do 
    {                
        length = count;
        availablefreesize = Gpu_Cmdfifo_Freespace(host);

        if (length > availablefreesize)
        {
            length = availablefreesize;
        }
        Gpu_Hal_CheckCmdBuffer(host,length);

        #if defined(MSVC_PLATFORM_SPI_LIBFT4222)  
        *(wrpktptr + 0) = (uint8_t) ((host->cmd_fifo_wp + RAM_CMD) >> 16) | 0x80;
        *(wrpktptr + 1) = (uint8_t) ((host->cmd_fifo_wp + RAM_CMD) >> 8);
        *(wrpktptr + 2) = (uint8_t) (host->cmd_fifo_wp + RAM_CMD) & 0xff;
        memcpy((wrpktptr + 3), buffer, length);

        if (host->spichannel == GPU_SPI_SINGLE_CHANNEL)
        {
            status = FT4222_SPIMaster_SingleWrite(
            host->hal_handle,
            wrpktptr,
            (length + 3), //3 for RAM_CMD address
            (uint16_t *) &SizeTransfered,
            TRUE
            );
            if ((FT4222_OK != status) || (SizeTransfered != (length + 3)))
            {
                printf("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, SizeTransfered, status);
                break;
            }
        }
        else
        {    /* DUAL and QAUD */
            status = FT4222_SPIMaster_MultiReadWrite(
            host->hal_handle,
            &dummy_read,
            wrpktptr,
            0,
            (length + 3),
            0,
            &SizeTransfered
            );
        }
        buffer += length;
        #else
        Gpu_Hal_StartCmdTransfer(host,GPU_WRITE,length); 
        #if defined(MSVC_PLATFORM_SPI_LIBMPSSE)            
        {   
            SPI_Write(host->hal_handle,buffer,length,&SizeTransfered,SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
            length = SizeTransfered;
            buffer += SizeTransfered;
        }        
        #endif
        Gpu_Hal_EndTransfer(host);
        #endif
        Gpu_Hal_Updatecmdfifo(host,length);

        //Gpu_Hal_WaitCmdfifo_empty(host);

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
    char8_t tmparray[16],idx = 0;

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
    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    FT4222_STATUS status;
    FT4222_SPIMode spimode;
    #endif
    
    if((numchnls > GPU_SPI_QUAD_CHANNEL) || (numdummy > GPU_SPI_TWODUMMY) || (numdummy < GPU_SPI_ONEDUMMY))
    {
        return -1;//error
    }

    //swicth EVE to multi channel SPI mode
    writebyte = numchnls;
    if(numdummy == GPU_SPI_TWODUMMY)
    writebyte |= SPI_TWO_DUMMY_BYTE;
    Gpu_Hal_Wr8(host, REG_SPI_WIDTH, writebyte);

    #if defined(MSVC_PLATFORM_SPI_LIBFT4222)
    //swicth FT4222 to relevant multi channel SPI communication mode
    if (numchnls == GPU_SPI_DUAL_CHANNEL)
    spimode = SPI_IO_DUAL;
    else if (numchnls == GPU_SPI_QUAD_CHANNEL)
    spimode = SPI_IO_QUAD;
    else
    spimode = SPI_IO_SINGLE;

    status = FT4222_SPIMaster_SetLines(host->hal_handle, spimode);
    if (FT4222_OK != status)
    printf("FT4222_SPIMaster_SetLines failed with status %d\n", status);
    #endif
    
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


    host->cmd_fifo_wp = Gpu_Hal_Rd16(host,REG_CMD_WRITE);



}

void Gpu_Hal_LoadImageToMemory(Gpu_Hal_Context_t *host, char8_t* fileName, uint32_t destination, uint8_t type){

    FILE *fp;
    uint32_t fileLen;
    uint8_t pBuff[8192];
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
    fp = fopen(fileName, "rb+");

    if(fp){

        fseek(fp,0,SEEK_END);
        fileLen = ftell(fp);
        fseek(fp,0,SEEK_SET);
        while(fileLen > 0)
        {
            uint32_t blocklen = fileLen>8192?8192:fileLen;
            fread(pBuff,1,blocklen,fp);
            fileLen -= blocklen;
            if(LOAD== type)
            {
                Gpu_Hal_WrMem(host,destination,pBuff, blocklen);//alignment is already taken care by this api
            }
			else
			{
				Gpu_Hal_WrCmdBuf_nowait(host,pBuff, blocklen);//transfer data completely and check/wait for the last chunk only
			}

            destination += blocklen;
        }
        fclose(fp);
    }else{
        printf("Unable to open file: %s\n",fileName);
        //exit(1);
    }



}
/*==========================================================================
*
*                      FOR MSVC_PLATFORM PLATFORMS ONLY
*
*=========================================================================*/

#if defined(MSVC_PLATFORM_SPI_LIBFT4222)
/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY READ command followed 
*                             by read of data bytes from GPU
* Implementation           : Using FT4222_SPIMaster_SingleRead,
*                             FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite
*                             
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
uint8_t    Gpu_Hal_FT4222_Rd(Gpu_Hal_Context_t *host, uint32_t hrdcmd, uint8_t * rdbufptr, uint32_t exprdbytes)
{
    uint32_t SizeTransfered;    
    FT4222_STATUS status;
    uint8_t hrdpkt[8] = {0,0,0,0,0,0,0,0}; //3 byte addr + 2 or1 byte dummy
    uint8_t retcode = 1;        /* assume successful operation */    
    uint32_t bytes_per_read;    

    if (rdbufptr == NULL || exprdbytes == 0)
    retcode = 0; //fail                

    if (retcode)
    {
        if (host->spichannel == GPU_SPI_SINGLE_CHANNEL)
        {
            /* Compose the HOST MEMORY READ packet */
            hrdpkt[0] = (uint8_t)(hrdcmd >> 16) & 0xFF;
            hrdpkt[1] = (uint8_t)(hrdcmd >> 8) & 0xFF;
            hrdpkt[2] = (uint8_t)(hrdcmd & 0xFF);

            status = FT4222_SPIMaster_SingleWrite(
            host->hal_handle,
            hrdpkt,
            3 + host->spinumdummy, /* 3 address and chosen dummy bytes */
            (uint16_t *)&SizeTransfered,
            FALSE            /* continue transaction */
            );
            if ((FT4222_OK != status) || ((uint16_t)SizeTransfered != (3 + host->spinumdummy)))
            {
                printf("FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", (uint16_t)SizeTransfered, status);
                retcode = 0;
                if ((uint16_t)SizeTransfered != (3 + host->spinumdummy))
                host->status = GPU_HAL_STATUS_ERROR;
            }
            else
            {
                /* continue reading data bytes only if HOST MEMORY READ command sent successfully */
                if (rdbufptr != NULL)
                {
                    BOOL disable_cs = FALSE; //assume multi SPI read calls                
                    bytes_per_read = exprdbytes;

                    while (retcode && exprdbytes)
                    {
                        if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH)
                        {
                            bytes_per_read = exprdbytes;
                            disable_cs = TRUE; //1 iteration of SPI read adequate
                        }
                        else
                        {
                            bytes_per_read = FT4222_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH;
                            disable_cs = FALSE;
                        }

                        status = FT4222_SPIMaster_SingleRead(
                        host->hal_handle,
                        rdbufptr,
                        bytes_per_read,
                        (uint16_t *)&SizeTransfered,
                        disable_cs
                        );
                        if ((FT4222_OK != status) || ((uint16_t)SizeTransfered != bytes_per_read))
                        {
                            printf("FT4222_SPIMaster_SingleRead failed,SizeTransfered is %d with status %d\n", (uint16_t)SizeTransfered, status);
                            retcode = 0;
                            if ((uint16_t)SizeTransfered != bytes_per_read)
                            host->status = GPU_HAL_STATUS_ERROR;
                        }

                        //multiple iterations of SPI read needed
                        bytes_per_read = (uint16_t)SizeTransfered;

                        exprdbytes -= bytes_per_read;
                        rdbufptr += bytes_per_read;

                    }
                }
            }
        }
        else
        {
            /* Multi channel SPI communication */
            uint32_t t_hrdcmd = hrdcmd;
            uint32_t read_data_index = 0;

            while (retcode && exprdbytes)
            {
                /* Compose the HOST MEMORY READ ADDR packet */
                hrdpkt[0] = (uint8_t)(t_hrdcmd >> 16) & 0xFF;
                hrdpkt[1] = (uint8_t)(t_hrdcmd >> 8) & 0xFF;
                hrdpkt[2] = (uint8_t)(t_hrdcmd & 0xff);

                if (exprdbytes <= FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH)
                bytes_per_read = exprdbytes;
                else
                bytes_per_read = FT4222_MAX_RD_BYTES_PER_CALL_IN_MULTI_CH;

                status = FT4222_SPIMaster_MultiReadWrite(
                host->hal_handle,
                rdbufptr + read_data_index,
                hrdpkt,
                0,
                3 + host->spinumdummy, // 3 addr + dummy bytes
                bytes_per_read,
                &SizeTransfered
                );
                if ((FT4222_OK != status) || ((uint16_t)SizeTransfered != bytes_per_read))
                {
                    printf("FT4222_SPIMaster_MultiReadWrite failed, SizeTransfered is %d with status %d\n", SizeTransfered, status);
                    retcode = 0;
                    if ((uint16_t)SizeTransfered != bytes_per_read)
                    host->status = GPU_HAL_STATUS_ERROR;
                }

                //its multi SPI read calls
                bytes_per_read = (uint16_t)SizeTransfered;

                exprdbytes -= bytes_per_read;
                read_data_index += bytes_per_read;
                t_hrdcmd += bytes_per_read;
            }
        }
    }
    return retcode;
}

/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY WRITE command
*
* Implementation           : Uisng FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite    
*
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
uint8_t    Gpu_Hal_FT4222_Wr(Gpu_Hal_Context_t *host, uint32_t hwraddr, const uint8_t * wrbufptr, uint32_t bytestowr)
{
    uint32_t SizeTransfered;    
    FT4222_STATUS status;
    uint8_t * temp_wrpktptr;
    uint16_t per_write = 0;
    BOOL disable_cs = FALSE; //assume multi SPI write calls    
    uint8_t dummy_read;
    uint8_t retcode = 1;        /* assume successful operation */        

    if (wrbufptr == NULL || bytestowr == 0)
    retcode = 0;

    if (retcode)
    {

        temp_wrpktptr = host->spiwrbuf_ptr; //global host write buffer of size FT4222_MAX_BYTES_PER_CALL
        
        if (host->spichannel == GPU_SPI_SINGLE_CHANNEL)
        {
            *(temp_wrpktptr + 0) = (hwraddr >> 16) | 0x80; //MSB bits 10 for WRITE
            *(temp_wrpktptr + 1) = (hwraddr >> 8) & 0xFF;
            *(temp_wrpktptr + 2) = hwraddr & 0xff;

            status = FT4222_SPIMaster_SingleWrite(
            host->hal_handle,
            temp_wrpktptr,
            3, //3 address bytes
            (uint16_t *)&SizeTransfered,
            FALSE
            );

            if ((FT4222_OK != status) || ((uint16_t)SizeTransfered != 3))
            {
                printf("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, (uint16_t)SizeTransfered, status);
                retcode = 0;
            }

            if (retcode)
            {
                while (retcode && bytestowr)
                {
                    if (bytestowr <= FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH)
                    {
                        per_write = bytestowr;
                        disable_cs = TRUE;
                    }
                    else
                    {
                        per_write = FT4222_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH;
                        disable_cs = FALSE;
                    }
                    
                    status = FT4222_SPIMaster_SingleWrite(
                    host->hal_handle,
                    wrbufptr,
                    per_write,
                    (uint16_t *)&SizeTransfered,
                    disable_cs
                    );

                    if ((FT4222_OK != status) || ((uint16_t)SizeTransfered != per_write))
                    {
                        printf("%d FT4222_SPIMaster_SingleWrite failed, SizeTransfered is %d with status %d\n", __LINE__, (uint16_t)SizeTransfered, status);
                        retcode = 0;
                        if ((uint16_t)SizeTransfered != per_write)
                        host->status = GPU_HAL_STATUS_ERROR;
                    }

                    //continue writing more bytes
                    per_write = (uint16_t)SizeTransfered;
                    wrbufptr += per_write;
                    bytestowr -= per_write;
                }
            }
        }
        else
        {
            //multi channel SPI communication
            while (bytestowr && retcode)
            {
                *(temp_wrpktptr + 0) = (hwraddr >> 16) | 0x80; //MSB bits 10 for WRITE
                *(temp_wrpktptr + 1) = (hwraddr >> 8) & 0xFF;
                *(temp_wrpktptr + 2) = hwraddr & 0xff;

                if (bytestowr <= FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH) //3 for address            
                per_write = bytestowr;
                else
                per_write = FT4222_MAX_WR_BYTES_PER_CALL_IN_MULTI_CH;

                memcpy((temp_wrpktptr + 3), wrbufptr, per_write);

                status = FT4222_SPIMaster_MultiReadWrite(
                host->hal_handle,
                &dummy_read,
                temp_wrpktptr,
                0,
                per_write + 3, // 3 byte of mem address
                0,
                &SizeTransfered
                );
                if (FT4222_OK != status)
                {
                    printf("FT4222_SPIMaster_MultiReadWrite failed, status %d\n", status);
                    retcode = 0;
                    host->status = GPU_HAL_STATUS_ERROR;
                }

                hwraddr += per_write;
                bytestowr -= per_write;
                wrbufptr += per_write;
            }
        }
    }
    return retcode;
}

/***************************************************************************
* Interface Description    : Function to compute FT4222 sys clock and divisor
*                            to obtain user requested SPI communication clock
*                            Available FT4222_ClockRate (FT4222 system clock):
*                               SYS_CLK_60,
*                               SYS_CLK_24,
*                               SYS_CLK_48,
*                               SYS_CLK_80 
*                            Divisors available (FT4222_SPIClock):
*                               CLK_NONE,
*                               CLK_DIV_2,
*                               CLK_DIV_4,
*                               CLK_DIV_8,
*                               CLK_DIV_16,
*                               CLK_DIV_32,
*                               CLK_DIV_64,
*                               CLK_DIV_128,
*                               CLK_DIV_256,
*                               CLK_DIV_512 
* Implementation           : Good performance is observed with divisors other than CLK_DIV_2
*                            and CLK_DIV_4 from test report by firmware developers.
*                            Hence supporting the following clocks for SPI communication
*                               5000KHz
*                               10000KHz
*                               15000KHz
*                               20000KHz
*                               25000KHz 
*                               30000KHz
*                            Global variable host->hal_config.spi_clockrate_khz is
*                            updated accodingly   
* Return Value             : bool_t
*                               TRUE : Supported by FT4222
*                               FALSE : Not supported by FT4222
*
* Author                   :
****************************************************************************/
bool_t Gpu_Hal_FT4222_ComputeCLK(Gpu_Hal_Context_t *host, FT4222_ClockRate *sysclk, FT4222_SPIClock *sysdivisor)
{
    //host->hal_config.spi_clockrate_khz is the user requested SPI communication clock      

    if (host->hal_config.spi_clockrate_khz <= 5000)
    {  //set to 5000 KHz              
        *sysclk = SYS_CLK_80;
        *sysdivisor = CLK_DIV_16;
    }
    else if (host->hal_config.spi_clockrate_khz > 5000 && host->hal_config.spi_clockrate_khz <= 10000)
    {
        //set to 10000 KHz
        *sysclk = SYS_CLK_80;
        *sysdivisor = CLK_DIV_8;
    }
    else if (host->hal_config.spi_clockrate_khz > 10000 && host->hal_config.spi_clockrate_khz <= 15000)
    {
        //set to 15000 KHz
        *sysclk = SYS_CLK_60;
        *sysdivisor = CLK_DIV_4; 
    }
    else
    {
        //set to 20000 KHz : Maximum throughput is obeserved with this clock combination
        *sysclk = SYS_CLK_80;
        *sysdivisor = CLK_DIV_4; 
    }
    printf("User Selected SPI clk : %d KHz \n", host->hal_config.spi_clockrate_khz);
    printf("Configured clk :  Ft4222 sys clk enum = %d , divisor enum = %d \n",*sysclk, *sysdivisor);        
    return(TRUE);     
}
#elif defined(MSVC_PLATFORM_SPI_LIBMPSSE)
/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY READ command followed
*                             by read of data bytes from GPU
* Implementation           : Using FT4222_SPIMaster_SingleRead,
*                             FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite
*
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
uint8_t    Gpu_Hal_LIBMPSSE_Rd(Gpu_Hal_Context_t *host, uint8 *buffer,uint32 sizeToTransfer, uint32 *sizeTransfered)
{
	uint32_t SizeRead;
	FT_STATUS status;
	uint8_t retcode = 1;        /* assume successful operation */
	uint32_t bytes_per_read;
	uint32_t options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES; //assume multi SPI read calls  
	if (buffer == NULL || sizeToTransfer == 0)
		retcode = 0; //fail                

	if (retcode)
	{
		bytes_per_read = sizeToTransfer;

		while (retcode && sizeToTransfer)
		{
			if (sizeToTransfer <= LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH)
			{
				bytes_per_read = sizeToTransfer;
				//options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE; //1 iteration of SPI read adequate
			}
			else
			{
				bytes_per_read = LIBMPSSE_MAX_RD_BYTES_PER_CALL_IN_SINGLE_CH;
				//options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
			}

			status = SPI_Read(
				host->hal_handle,
				buffer,
				bytes_per_read,
				&SizeRead,
				options
			);
			if ((FT_OK != status) || (SizeRead != bytes_per_read))
			{
				printf("SPI_Read failed,SizeRead is %d with status %d\n", SizeRead, status);
				retcode = 0;
				if (SizeRead != bytes_per_read)
					host->status = GPU_HAL_STATUS_ERROR;
			}

			//multiple iterations of SPI read needed
			bytes_per_read = SizeRead;

			sizeToTransfer -= bytes_per_read;
			buffer += bytes_per_read;
			*sizeTransfered += bytes_per_read;
		}
	}
	
	return retcode;
}

/***************************************************************************
* Interface Description    : Function to tranfer HOST MEMORY WRITE command
*
* Implementation           : Uisng FT4222_SPIMaster_SingleWrite
*                             FT4222_SPIMaster_MultiReadWrite
*
* Return Value             : uint8_t
*                             1 - Success
*                             0 - Failure
* Author                   :
****************************************************************************/
uint8_t    Gpu_Hal_LIBMPSSE_Wr(Gpu_Hal_Context_t *host, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered)
{
	uint32_t SizeWrite;
	FT_STATUS status;
	uint16_t per_write = 0;
	uint32 options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES; //assume multi SPI write calls    
	uint8_t retcode = 1;        /* assume successful operation */

	if (buffer == NULL || sizeToTransfer == 0)
		retcode = 0;

	if (retcode)
	{
		while (retcode && sizeToTransfer)
		{
			if (sizeToTransfer <= LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH)
			{
				per_write = sizeToTransfer;
				//options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE;
			}
			else
			{
				per_write = LIBMPSSE_MAX_WR_BYTES_PER_CALL_IN_SINGLE_CH;
				//options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;
			}

			status = SPI_Write(
				host->hal_handle,
				buffer,
				per_write,
				&SizeWrite,
				options
			);

			if ((FT_OK != status) || (SizeWrite != per_write))
			{
				printf("%d SPI_Write failed, SizeWrite is %d with status %d\n", __LINE__, SizeWrite, status);
				retcode = 0;
				if (SizeWrite != per_write)
					host->status = GPU_HAL_STATUS_ERROR;
			}

			//continue writing more bytes
			per_write = SizeWrite;
			buffer += per_write;
			sizeToTransfer -= per_write;
			*sizeTransfered += per_write;
		}
	}
	return retcode;
}

#endif


