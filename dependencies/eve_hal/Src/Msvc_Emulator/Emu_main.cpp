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
0.1 - date 2013.04.24 - Initial version
0.2 - date 2013.08.19 - few minor edits
0.3 - date 2015.04.29 - New version for multi-emulation mode

*/

#include "Platform.h"

#ifdef MSVC_FT800EMU
#include <Emulator.h>


FT8XXEMU_EmulatorMode GpuEmu_Mode();

extern "C" void setup();
extern "C" void loop();

int32_t main(int32_t argc,char8_t *argv[])
{
	FT8XXEMU_EmulatorParameters params;
	

	FT8XXEMU_defaults(FT8XXEMU_VERSION_API, &params, GpuEmu_Mode());

	params.Flags &= (~FT8XXEMU_EmulatorEnableDynamicDegrade & ~FT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	params.Setup = setup;
	params.Loop = loop;
	// params.Graphics = graphics;
	FT8XXEMU_run(FT8XXEMU_VERSION_API, &params);
	return 0;
}

void GpuEmu_SPII2C_csLow()
{
	FT8XXEMU_cs(1);
}

void GpuEmu_SPII2C_csHigh()
{
	FT8XXEMU_cs(0);
}

void GpuEmu_SPII2C_begin()
{

}

void GpuEmu_SPII2C_end()
{

}


uint8_t GpuEmu_SPII2C_transfer(uint8_t data)
{
	return FT8XXEMU_transfer(data);
}


void  GpuEmu_SPII2C_StartRead(uint32_t addr)
{
	GpuEmu_SPII2C_csLow();
	GpuEmu_SPII2C_transfer((addr >> 16) & 0xFF);
	GpuEmu_SPII2C_transfer((addr >> 8) & 0xFF);
	GpuEmu_SPII2C_transfer(addr & 0xFF);

	GpuEmu_SPII2C_transfer(0); //Dummy Read Byte
}

void  GpuEmu_SPII2C_StartWrite(uint32_t addr)
{
	GpuEmu_SPII2C_csLow();
	GpuEmu_SPII2C_transfer(((addr >> 16) & 0xFF) | 0x80);
	GpuEmu_SPII2C_transfer((addr >> 8) & 0xFF);
	GpuEmu_SPII2C_transfer(addr & 0xFF);
	
}


FT8XXEMU_EmulatorMode GpuEmu_Mode(){
#if defined(FT800_ENABLE)
		return FT8XXEMU_EmulatorFT800;
#elif defined(FT801_ENABLE)
		return FT8XXEMU_EmulatorFT801;
#elif defined(FT810_ENABLE)
		return FT8XXEMU_EmulatorFT810;
#elif defined(FT811_ENABLE)
		return FT8XXEMU_EmulatorFT811;
#elif defined(FT812_ENABLE)
		return FT8XXEMU_EmulatorFT812;
#elif defined(FT813_ENABLE)
		return FT8XXEMU_EmulatorFT813;
#endif
		return FT8XXEMU_EmulatorFT800;
}


#endif