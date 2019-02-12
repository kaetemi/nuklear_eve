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
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - fixed bugs
1.0 - date 2015.03.16 - Updated with new APIs for FT81X
1.1 - date 2017.03.24 - Rename APIs
*/

#ifndef CO_PRO_CMDS__H
#define CO_PRO_CMDS__H
#include "FT_CoPro_Cmds.h"
#include "Gpu_Hal.h"

void Gpu_CoCmd_FillWidth(Gpu_Hal_Context_t *phost, uint32_t s);
void Gpu_CoCmd_Nop(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_Inflate2(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t options);
void Gpu_CoCmd_RotateAround(Gpu_Hal_Context_t *phost, int32_t x, int32_t y, int32_t a, int32_t s);

void Gpu_CoCmd_FlashErase(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_FlashWrite(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t num);
void Gpu_CoCmd_FlashWriteExt(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t num, uint8_t *data);
void Gpu_CoCmd_FlashUpdate(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t src, uint32_t num);
void Gpu_CoCmd_FlashRead(Gpu_Hal_Context_t *phost, uint32_t dest, uint32_t src, uint32_t num);
void Gpu_CoCmd_FlashSource(Gpu_Hal_Context_t *phost, uint32_t ptr);
void Gpu_CoCmd_FlashSpiTx(Gpu_Hal_Context_t *phost, uint32_t num);
void Gpu_CoCmd_FlashFast(Gpu_Hal_Context_t *phost, uint32_t result);
void Gpu_CoCmd_FlashSpiRx(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t num);
void Gpu_CoCmd_FlashAttach(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_FlashDetach(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_FlashSpiDesel(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_ClearCache(Gpu_Hal_Context_t *phost);

void Gpu_CoCmd_Int_RamShared(Gpu_Hal_Context_t *phost, uint32_t ptr);
void Gpu_CoCmd_Sha1(Gpu_Hal_Context_t *phost, uint32_t src, uint32_t num, uint32_t hash);
void Gpu_CoCmd_Hmac(Gpu_Hal_Context_t *phost, uint32_t src, uint32_t num, uint32_t hash); //not yet available

void Gpu_CoCmd_ResetFonts(Gpu_Hal_Context_t *phost);

void Gpu_CoCmd_GradientA(Gpu_Hal_Context_t *phost, int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1);
void Gpu_CoCmd_AppendF(Gpu_Hal_Context_t *phost, uint32_t ptr, uint32_t num);
void Gpu_CoCmd_GetPoint(Gpu_Hal_Context_t *phost, int16_t x, int16_t y, uint32_t sx, uint32_t sy);

/* Flash section */
void Gpu_CoCmd_FlashHelper_Init(Gpu_Hal_Context_t *phost);
uint32_t Gpu_CoCmd_FlashHelper_SwitchState(Gpu_Hal_Context_t *phost, uint8_t nextState);
uint32_t Gpu_CoCmd_FlashHelper_SwitchFullMode(Gpu_Hal_Context_t *phost);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Write(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t num, uint8_t *write_data);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Update(Gpu_Hal_Context_t *phost, uint32_t dest_flash, uint32_t src_ram, uint32_t num);
Flash_Cmd_Status_t Gpu_CoCmd_FlashHelper_Read(Gpu_Hal_Context_t *phost, uint32_t dest_ram, uint32_t src_flash, uint32_t num, uint8_t *read_data);
void Gpu_CoCmd_FlashHelper_Erase(Gpu_Hal_Context_t *phost);
void Gpu_CoCmd_FlashHelper_ClearCache(Gpu_Hal_Context_t *phost);
uint8_t Gpu_CoCmd_FlashHelper_GetState(Gpu_Hal_Context_t *phost);

void Gpu_CoCmd_AnimStart(Gpu_Hal_Context_t *phost, int32_t ch, uint32_t aoptr, uint32_t loop);

#endif