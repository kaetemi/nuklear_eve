/**
 * @file EVE_Platform_LINUX.h
 * @brief EVE platform for Linux
 *
 * @author Bridgetek
 *
 * @date 2018
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

#ifndef EVE_PLATFORM_LINUX__H
#define EVE_PLATFORM_LINUX__H

#include "EVE_Config.h"
#if defined(__linux__)

/* Disable legacy C functions under TCC */
#ifndef NO_OLDNAMES
#define NO_OLDNAMES
#endif

/* C library inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>

#define _USE_MATH_DEFINES 1
#include <math.h>

#include <sys/time.h>
#include <time.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#endif
#endif /* #ifndef EVE_PLATFORM_LINUX__H */

/* end of file */