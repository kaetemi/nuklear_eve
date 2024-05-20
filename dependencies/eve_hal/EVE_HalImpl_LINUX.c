/**
 * @file EVE_HalImpl_WIN32.c
 * @brief Eve_Hal framework APIs for WIN32 host platform
 *
 * @author Bridgetek
 *
 * @date 2023
 *
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "EVE_HalImpl.h"
#include "EVE_Platform.h"
#if defined(__linux__)

/************
** UTILITY **
************/

/**
 * @brief Get current system clock of Coprocessor
 *
 * @param phost Pointer to Hal context
 * @return uint32_t Frequency of Coprocessor
 */
uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost)
{
	uint32_t t0;
	uint32_t t1;

	t0 = EVE_Hal_rd32(phost, REG_CLOCK); /* t0 read */
	/* may not be precise */
	EVE_sleep(15625 / 1000);

	t1 = EVE_Hal_rd32(phost, REG_CLOCK); /* t1 read */
	return ((t1 - t0) * 64); /* bitshift 6 places is the same as multiplying 64 */
}

/*********
** MISC **
*********/

/**
 * @brief Init host MCU
 *
 */
void EVE_Mcu_initialize()
{
	/* no-op */
}

/**
 * @brief Release host MCU
 *
 */
void EVE_Mcu_release()
{
	/* no-op */
}

/*********
** MISC **
*********/

static struct timeval s_Millis_Start;

/**
 * @brief Init timer
 *
 */
void EVE_Millis_initialize()
{
	gettimeofday(&s_Millis_Start, NULL);
}

/**
 * @brief Release timer
 *
 */
void EVE_Millis_release()
{
	/* no-op */
}

/**
 * @brief Get clock in millisecond
 *
 * global counter to loopback after ~49.71 days
 *
 * @return uint32_t Clock number
 */
uint32_t EVE_millis()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (now.tv_sec - s_Millis_Start.tv_sec) * 1000 + (now.tv_usec - s_Millis_Start.tv_usec) / 1000;
}

/**
 * @brief Get clock in millisecond
 *
 * @return uint64_t Clock number
 */
uint64_t EVE_millis64()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (uint64_t)(now.tv_sec - s_Millis_Start.tv_sec) * 1000 + (now.tv_usec - s_Millis_Start.tv_usec) / 1000;
}

#if defined(ESD_SIMULATION)
int Ft_Sleep__ESD(int ms);
#endif

/**
 * @brief Sleep in millisecond
 *
 * @param ms Millisecond
 */
void EVE_sleep(uint32_t ms)
{
#if defined(ESD_SIMULATION)
	Ft_Sleep__ESD(ms);
#else
	struct timespec req, rem;
	req.tv_sec = ms / 1000; // Convert milliseconds to seconds
	req.tv_nsec = (ms % 1000) * 1000000; // Convert remainder to nanoseconds

	while (nanosleep(&req, &rem) == -1) // Handle interrupted sleep
	{
		req = rem; // Set remaining time as new request
	}
#endif
}

#endif

/* end of file */
