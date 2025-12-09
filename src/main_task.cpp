/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    Copyright (c) 2024 Infineon Technologies, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <lib/support/CHIPMem.h>
#include <lib/support/CHIPPlatformMemory.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>

#include <AppTask.h>

#include "AppConfig.h"
#include <app/server/Server.h>
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include <platform/Infineon/cat5/CYW955913Utils.h>
#include <malloc.h>
#define TO_KB(size_bytes)  ((float)(size_bytes)/1024)

using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::DeviceLayer;

volatile int apperror_cnt;

// ================================================================================
// App Error
//=================================================================================
void appError(int err)
{
    INF_LOG("!!!!!!!!!!!! App Critical Error: %d !!!!!!!!!!!", err);
    while (true)
        ;
}

void appError(CHIP_ERROR error)
{
    appError(static_cast<int>(error.AsInteger()));
}

void print_max_sram_heap_usage(void)
{
    struct mallinfo mall_info = mallinfo();
 
    extern uint8_t __HeapBase;  /* Symbol exported by the linker. */
    extern uint8_t __HeapLimit; /* Symbol exported by the linker. */
    uint8_t* heap_base = (uint8_t *)&__HeapBase;
    uint8_t* heap_limit = (uint8_t *)&__HeapLimit;
    uint32_t heap_size = (uint32_t)(heap_limit - heap_base);
 
    printf("\r\n\n********** Heap Usage **********\r\n");
    printf("Total available heap        : %lu bytes/%.2f KB\r\n", heap_size, TO_KB(heap_size));
 
    printf("Maximum heap utilized so far: %u bytes/%.2f KB, %.2f%% of available heap\r\n",
            mall_info.arena, TO_KB(mall_info.arena), ((float) mall_info.arena * 100u)/heap_size);
 
    printf("Heap in use at this point   : %u bytes/%.2f KB, %.2f%% of available heap\r\n",
            mall_info.uordblks, TO_KB(mall_info.uordblks), ((float) mall_info.uordblks * 100u)/heap_size);
 
    printf("********************************\r\n\n");
}

void main_task(void)
{
    CHIP_ERROR ret = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init();
    if (ret != CHIP_NO_ERROR)
    {
        INF_LOG("PersistedStorage::KeyValueStoreMgrImpl().Init() failed");
        appError(ret);
    }

    ret = PlatformMgr().InitChipStack();
    if (ret != CHIP_NO_ERROR)
    {
        INF_LOG("PlatformMgr().InitChipStack() failed ret:");
        appError(ret);
    }

    ret = chip::DeviceLayer::ConnectivityMgr().SetBLEDeviceName("H1CP_LOCK");
    if (ret != CHIP_NO_ERROR)
    {
        INF_LOG("ConnectivityMgr().SetBLEDeviceName() failed");
        appError(ret);
    }

    printf("Starting Platform Manager Event Loop");
    ret = PlatformMgr().StartEventLoopTask();
    if (ret != CHIP_NO_ERROR)
    {
        printf("PlatformMgr().StartEventLoopTask() failed");
        appError(ret);
    }
    ret = GetAppTask().StartAppTask();
    if (ret != CHIP_NO_ERROR)
    {
        printf("GetAppTask().Init() failed");
        appError(ret);
    }
}
