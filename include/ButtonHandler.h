/*
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

#pragma once

#include <stdint.h>

#include "cyhal.h"
#include "cyabs_rtos.h"

#define GPIO_INTERRUPT_PRIORITY (5)

class ButtonHandler
{
public:
    static void Init(void);

private:
    static void GpioInit(void);
    static void LockButtonCallback(void * handler_arg, cyhal_gpio_event_t event);
    static void FuncButtonCallback(void * handler_arg, cyhal_gpio_event_t event);
    static void TimerCallback(cy_timer_callback_arg_t arg);
};
