/******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for Hello World Example using HAL APIs.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * Copyright 2022-2023, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

/*******************************************************************************
 * Header Files
 *******************************************************************************/
extern   "C" {
#include <cy_tx_thread.h>
}
#include "CHIPProjectConfig.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include <lib/support/CHIPMem.h>
#include <lib/support/CHIPPlatformMemory.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>
#include <AppTask.h>

#include "AppConfig.h"
#include "init_cyw955913Platform.h"
#include "tx_api.h"

#include "lvgl_support.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
#ifndef MAIN_TASK_STACK_SIZE
#define MAIN_TASK_STACK_SIZE        (4096)
#endif

#ifndef MAIN_TASK_PRIORITY
#define MAIN_TASK_PRIORITY          (CY_RTOS_PRIORITY_NORMAL)
#endif

#define WDT_TIMEOUT_SECS            (5)
#define WDT_TIMER_MSECS             (4 * 1000)
cy_timer_t wdt_timer;

typedef struct
{
    UINT32 *sp;
    UINT32 pc;
    UINT32 lr;
    UINT32 r[7];     /* R0-R6 */
    UINT32 PSR;
} sec_exc_t;

static sec_exc_t app_regs;

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
static void start_main_task(ULONG thread_input);
static void application_thread_cleanup(TX_THREAD *thread_ptr, UINT condition);
extern void main_task(void);
extern "C" void wiced_set_coredump_callback(BOOL32 (*callback)(void *info));
using namespace ::chip;

static BOOL32 app_coredump_callback(void *info)
{
    sec_exc_t *p_exec = &app_regs;

    memcpy(p_exec, info, sizeof(sec_exc_t));

    printf("Core dump:sp %p\n", p_exec->sp);
    printf("Core dump:pc 0x%08lx\n", p_exec->pc);
    printf("Core dump:lr 0x%08lx\n", p_exec->lr);
    printf("Core dump:r[0] 0x%08lx\n", p_exec->r[0]);
    printf("Core dump:r[1] 0x%08lx\n", p_exec->r[1]);
    printf("Core dump:r[2] 0x%08lx\n", p_exec->r[2]);
    printf("Core dump:r[3] 0x%08lx\n", p_exec->r[3]);
    printf("Core dump:r[4] 0x%08lx\n", p_exec->r[4]);
    printf("Core dump:r[5] 0x%08lx\n", p_exec->r[5]);
    printf("Core dump:r[6] 0x%08lx\n", p_exec->r[6]);
    printf("Core dump:PSR 0x%08lx\n", p_exec->PSR);

    return FALSE;
}

static void wdt_timer_callback(cy_timer_callback_arg_t arg)
{
    thread_ap_watchdog_ConfigureTime(WDT_TIMEOUT_SECS);
}

static void start_main_task(ULONG thread_input)
{
    main_task();
}

// ================================================================================
// Main Code
// ================================================================================
int main(void)
{
    cy_rslt_t result;
    TX_THREAD *main_thread_handle;
    char      *main_thread_stack;
    UINT      status;

    init_cyw955913Platform();

    /* Enable global interrupts */
    __enable_irq();

    // Initialize WDT handling.
    thread_ap_watchdog_ConfigureTime(WDT_TIMEOUT_SECS);
    result = cy_rtos_timer_init(&wdt_timer, CY_TIMER_TYPE_PERIODIC, wdt_timer_callback, (cy_timer_callback_arg_t)NULL);
    if (result == CY_RSLT_SUCCESS)
    {
        cy_rtos_timer_start(&wdt_timer, WDT_TIMER_MSECS);
    }
    else
    {
        INF_LOG("ERROR creating WDT timer");
    }

    /* Register coredump callback */
    wiced_set_coredump_callback(app_coredump_callback);

    chip::Platform::MemoryInit();

#if ENABLE_UI
    display_init();
    cy_rtos_delay_milliseconds(5 * 1000);	// Pause for logo display and to allow serial terminal to attach
#endif

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    INF_LOG("====================================================");
    INF_LOG("chip-CYW955913-lock-app starting Version %d", CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION);
#if ENABLE_UI
    INF_LOG("             UI enabled");
#endif
    INF_LOG("====================================================\r\n");

    /*
     * Create the application thread.
     * We dynamically allocate the thread and stack since the main task only runs for
     * initialization and then exits. This lets us reclaim the memory after the task exits.
     */
    main_thread_handle = (TX_THREAD *)malloc(sizeof(TX_THREAD));
    main_thread_stack  = (char *)malloc(MAIN_TASK_STACK_SIZE);

    status = tx_thread_create(main_thread_handle, (char *)"main_task", start_main_task, 0, main_thread_stack,
                              MAIN_TASK_STACK_SIZE, MAIN_TASK_PRIORITY, MAIN_TASK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    if (TX_SUCCESS != status)
    {
        INF_LOG("Error creating main task thread\r\n");
        free(main_thread_handle);
        free(main_thread_stack);
        main_thread_handle = NULL;
        main_thread_stack  = NULL;
    }
    else
    {
        (void)tx_thread_entry_exit_notify(main_thread_handle, application_thread_cleanup);
    }
}

static void application_thread_cleanup(TX_THREAD *thread_ptr, UINT condition)
{
    /* Determine if the thread was exited. */
    if (thread_ptr && condition == TX_THREAD_EXIT)
    {
        tx_thread_terminate(thread_ptr);
        tx_thread_delete(thread_ptr);
        free(thread_ptr->tx_thread_stack_start);
        free(thread_ptr);
    }
}
/* [] END OF FILE */
