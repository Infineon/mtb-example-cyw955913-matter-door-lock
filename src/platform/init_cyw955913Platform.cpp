/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
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

#include <lib/support/CHIPPlatformMemory.h>
#include <platform/CHIPDeviceLayer.h>

#include <assert.h>
#include <string.h>
#include <cy_retarget_io.h>
#include <cybsp.h>
#include <cyhal.h>

void init_cyw955913Platform(void)
{
    /* Initialize the board support package */
    cybsp_init();

    /* Initialize retarget-io to use the debug UART port */
#if ENABLE_UI
    /* Use BT UART to free up SPI pins */
    cy_retarget_io_init(BT_UART_TXD, BT_UART_RXD, CY_RETARGET_IO_BAUDRATE);
#else
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
#endif
}
