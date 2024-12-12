/*
 * FreeRTOS V202111.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


// This example uses a common include to avoid repetition
#include "FreeRTOSConfig_examples_common.h"

// #define configNUMBER_OF_CORES 1
// From: https://community.element14.com/products/raspberry-pi/b/blog/posts/raspberry-pico---play-with-multi-core-freertos-smp
// #define configUSE_CORE_AFFINITY 1
// #define configTICK_CORE 0
// #define configRUN_MULTIPLE_PRIORITIES 0
// #define configUSE_PASSIVE_IDLE_HOOK 0
// #define configSUPPORT_PICO_SYNC_INTEROP 1
// #define configSUPPORT_PICO_TIME_INTEROP 1

#define configGENERATE_RUN_TIME_STATS 1
#define configUSE_TRACE_FACILITY 1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
extern uint64_t time_us_64(void);						// "hardware/timer.h"
#define RUN_TIME_STAT_time_us_64Divider 1000			// stat granularity is mS
#define portGET_RUN_TIME_COUNTER_VALUE() (time_us_64()/RUN_TIME_STAT_time_us_64Divider)	// runtime counter in mS

#endif /* FREERTOS_CONFIG_H */

