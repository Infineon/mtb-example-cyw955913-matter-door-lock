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
#include <stdio.h>

#include "lvgl.h"
#include "ft6206.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

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

/******************************************************
 *               Variable Definitions
 ******************************************************/

extern volatile uint32_t touch_count;

static lv_indev_t *indev_touchpad;

/******************************************************
 *               Function Definitions
 ******************************************************/

/* Return true is the touchpad is pressed */
static bool touchpad_is_pressed(void)
{
    if (touch_count > 0)
    {
        touch_count = 0;
        return true;
    }
    else
        return false;
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_t *indev_drv, lv_indev_data_t *data)
{
    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    /* Save the pressed coordinates and the state */
    if (touchpad_is_pressed())
    {
        ft6206_read_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PRESSED;
        //printf("touchpad_read  touch_count = %ld last_x = %d  last_y = %d\n", last_x, last_y);
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    /*Set the last pressed coordinates*/
    data->point.x = (int32_t)last_x;
    data->point.y = (int32_t)last_y;
}

cy_rslt_t lv_port_indev_ft6202_init(void)
{
    cy_rslt_t result;

    /*
     * Make sure the touchscreen has been initialized.
     */

    result = ft6206_init();
    if (result != CY_RSLT_SUCCESS)
    {
        return result;
    }

    /*
     * Register a pointer input device
     */

    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);

    return CY_RSLT_SUCCESS;
}
