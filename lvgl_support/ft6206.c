/*
 * (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 */

#include <inttypes.h>

#include "cybsp.h"
#include "cyhal_i2c.h"
#include "cyhal_gpio.h"

#include "ft6206.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define FT6206_I2C_ADDRESS          (0x38)

#define FT6206_I2C_FREQUENCY        (100000u)
#define FT6206_INTERRPUT_PIN        (CYBSP_D7)

#define X_RESOLUTION                (240)
#define Y_RESOLUTION                (320)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Prototypes
 ******************************************************/

static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);

/******************************************************
 *               Variable Definitions
 ******************************************************/

static bool ft6206_initialized;
static cyhal_i2c_t ft6206_i2c_handle;
static uint32_t i2c_timeout;
volatile uint32_t touch_count;

static cyhal_gpio_callback_data_t cb_data =
        { .callback     = gpio_interrupt_handler,
          .callback_arg = (void*) &touch_count };

/******************************************************
 *               Function Definitions
 ******************************************************/

// Interrupt handler callback function
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    CY_UNUSED_PARAMETER(event);

    // Increment global_count (passed as handler_arg) using a pointer
    uint32_t *count = (uint32_t *)handler_arg;
    *count = *count + 1;
}

cy_rslt_t ft6206_init(void)
{
    cyhal_i2c_cfg_t i2c_master_config;
    cy_rslt_t result;

    if (ft6206_initialized)
    {
        return CY_RSLT_SUCCESS;
    }

    /*
     * Initialize the I2C interface.
     */

    i2c_master_config.is_slave = CYHAL_I2C_MODE_MASTER;
    i2c_master_config.address  = 0;
    i2c_master_config.frequencyhal_hz = FT6206_I2C_FREQUENCY;

    result = cyhal_i2c_init(&ft6206_i2c_handle, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Unable to initialize I2C\n");
        return result;
    }

    result = cyhal_i2c_configure(&ft6206_i2c_handle, &i2c_master_config);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Unable to configure I2C\n");
        return result;
    }

    /*
     * Configure the touchscreen interrupt pin.
     */

    result = cyhal_gpio_init(FT6206_INTERRPUT_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
    if (CY_RSLT_SUCCESS != result)
    {
        printf("FT6206_INTERRPUT_PIN init error\n");
    }

    // Register callback function - gpio_interrupt_handler and pass the value global_count
    cyhal_gpio_register_callback(FT6206_INTERRPUT_PIN, &cb_data);

    // Enable falling edge interrupt event with interrupt priority set to 3
    cyhal_gpio_enable_event(FT6206_INTERRPUT_PIN, CYHAL_GPIO_IRQ_FALL, 3, true);

    if (result == CY_RSLT_SUCCESS)
    {
        ft6206_initialized = true;
    }

    return result;
}

cy_rslt_t ft6206_read_byte(uint8_t reg_addr, uint8_t *data)
{
    cy_rslt_t result;

    result = cyhal_i2c_master_write(&ft6206_i2c_handle, FT6206_I2C_ADDRESS, &reg_addr, 1, i2c_timeout, true);
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_i2c_master_read(&ft6206_i2c_handle, FT6206_I2C_ADDRESS, data, 1, i2c_timeout, true);
    }

    return result;
}

cy_rslt_t ft6206_write_byte(uint8_t reg_addr, uint8_t data)
{
    cy_rslt_t result;
    uint8_t buffer[2];

    buffer[0] = reg_addr;
    buffer[1] = data;

    result = cyhal_i2c_master_write(&ft6206_i2c_handle, FT6206_I2C_ADDRESS, buffer, 2, i2c_timeout, true);

    return result;
}

cy_rslt_t ft6206_read_xy(uint16_t *x, uint16_t *y)
{
    uint8_t reg_addr;
    uint8_t xy_data[4];
    uint16 tmp_x, tmp_y;
    cy_rslt_t result;

    *x = 0;
    *y = 0;

    reg_addr = FT6206_REG_P1_XH;
    result   = cyhal_i2c_master_write(&ft6206_i2c_handle, FT6206_I2C_ADDRESS, &reg_addr, 1, i2c_timeout, true);
    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_i2c_master_read(&ft6206_i2c_handle, FT6206_I2C_ADDRESS, xy_data, 4, i2c_timeout, true);
        if (result == CY_RSLT_SUCCESS)
        {
            /*
             * When the display isn't rotated, the bottom right corner is 0,0 and x increases as you move left.
             * Y increases as you move up. We want to map the touchscreen values so that they match the display
             * environment.
             */

            tmp_x = ((xy_data[0] & 0x0F) << 8) | xy_data[1];
            tmp_y = ((xy_data[2] & 0x0F) << 8) | xy_data[3];

            *x = X_RESOLUTION - tmp_x - 1;
            *y = Y_RESOLUTION - tmp_y - 1;
        }
    }

    return result;
}
