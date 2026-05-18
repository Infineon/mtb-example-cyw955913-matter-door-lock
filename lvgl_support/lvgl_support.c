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

#if ENABLE_UI

#include <stdlib.h>

#include "cybsp.h"
#include "cyabs_rtos.h"
#include "cyhal_gpio.h"
#include "cyhal_spi.h"
#include "cyhal_pwm.h"
#include "cyhal_i2c.h"

#include "lvgl.h"
#include "lv_ili9341.h"
#include "lv_qrcode.h"

#include "lvgl_support.h"
#include "lv_port_indev_ft6206.h"
#include "ft6206.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define DISPLAY_TASK_NAME               ("display task")
#define DISPLAY_TASK_STACK_SIZE         (8192)

#define DISPLAY_HOR_RES                 (240)
#define DISPLAY_VER_RES                 (320)

#define SPI_CLK_MAX_HZ                  (24*1000*1000)
#define SPI_CLK_DEFAULT_HZ              SPI_CLK_MAX_HZ

#define DISPLAY_DC_PIN                  (CYBSP_D9)
#define DISPLAY_RESET_PIN               (CYBSP_A0)
#define DISPLAY_BACKLIGHT_PIN           (CYBSP_D5)

#define DISPLAY_RESET_DELAY_MS          (1000)

#define DISPLAY_DC_COMMAND              (0)
#define DISPLAY_DC_DATA                 (1)

#define BACKLIGHT_PWM_DUTY_CYCLE_DEFAULT    (50)
#define BACKLIGHT_PWM_FREQ_HZ           (8*1000*1000)
#define BACKLIGHT_PWM_FREQ_MAX_HZ       (24*1000*1000)

#define LVGL_TIMEOUT_MS                 (10)

#define LOCK_DISPLAY_AREA_WIDTH         (100)
#define LOCK_DISPLAY_AREA_HEIGHT        (100)
#define LOCK_DISPLAY_AREA_X             (110)
#define LOCK_DISPLAY_AREA_Y             (60)

#define ANIMATION_OBJ_SIZE              (14)

#define STR_LOCK_STATUS                 "Lock status"
#define STR_LOCK_ACTION                 "Lock"
#define STR_UNLOCK_ACTION               "Unlock"
#define STR_PLEASE_SCAN                 "Please scan the QR code"

#define STR_APP_DESCRIPTION             "CWY955913 Matter Lock Demo"

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

static cy_thread_t display_task_handle;
static uint64_t display_task_stack[DISPLAY_TASK_STACK_SIZE/8];

static cyhal_spi_t display_spi_obj;
static cyhal_pwm_t backlight_pwm_handle;

static cy_timer_t lvgl_timer;

static lv_display_t *display_handle;
static lv_style_t style_btn;
static lv_style_t style_button_pressed;

static lv_obj_t *active_icon;
static lv_obj_t *lock_display_area;

/******************************************************
 *               Function Prototypes
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void lvgl_handler(cy_timer_callback_arg_t arg)
{
    lv_tick_inc(LVGL_TIMEOUT_MS);
}

/* Send short command to the display. This function shall wait until the transaction finishes. */
static void display_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    //printf( "CMD %d bytes [0x%02X] param %d bytes\n", cmd_size, cmd[0], param_size );

    cyhal_gpio_write(DISPLAY_DC_PIN, DISPLAY_DC_COMMAND);
    if (cyhal_spi_transfer(&display_spi_obj, cmd, cmd_size, NULL, 0, 0) == CY_RSLT_SUCCESS)
    {
        if (param && param_size)
        {
            cyhal_gpio_write(DISPLAY_DC_PIN, DISPLAY_DC_DATA);
            if (cyhal_spi_transfer(&display_spi_obj, param, param_size, NULL, 0, 0) != CY_RSLT_SUCCESS)
            {
                printf("Send params failed\n");
            }
        }
    }
    else
    {
        printf("Send CMD failed\n");
    }
}

/* Send large array of pixel data to the display. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
static void display_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    //printf("COLOR: CMD %d bytes [0x%02X] param %d bytes\n", cmd_size, cmd[0], param_size);

    cyhal_gpio_write(DISPLAY_DC_PIN, DISPLAY_DC_COMMAND);
    if (cyhal_spi_transfer(&display_spi_obj, cmd, cmd_size, NULL, 0, 0) == CY_RSLT_SUCCESS)
    {
        if (param && param_size)
        {
            cyhal_gpio_write(DISPLAY_DC_PIN, DISPLAY_DC_DATA);
            if (cyhal_spi_transfer(&display_spi_obj, param, param_size, NULL, 0, 0) != CY_RSLT_SUCCESS)
            {
                printf("Send params failed\n");
            }
            lv_display_flush_ready(disp);
        }
    }
    else
    {
        printf("Send CMD failed\n");
    }
}

static lv_obj_t *clear_screen(bool use_background_image)
{
    LV_IMG_DECLARE(screenbackground);
    lv_obj_t *background;

    /* Clear the display */
    lv_obj_t *scr = lv_screen_active();
    lv_obj_clean(scr);

    active_icon       = NULL;
    lock_display_area = NULL;

    if (use_background_image)
    {
        background = lv_image_create(scr);
        lv_image_set_src(background, &screenbackground);
        lv_obj_align(background, LV_ALIGN_CENTER, 0, 0);
    }
    else
    {
        /* set screen background to white */
        background = scr;
        lv_obj_set_style_bg_color(scr, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(scr, LV_OPA_100, 0);
    }

    return background;
}

static lv_color_t darken(const lv_color_filter_dsc_t *dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

static void button_style_init(void)
{
    /* Create a simple button style */
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    /*
     * Create a style for the pressed state.
     * Use a color filter to simply modify all colors in this state
     */

    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_button_pressed);
    lv_style_set_color_filter_dsc(&style_button_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_button_pressed, LV_OPA_20);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    bool locked = (bool)(lv_event_get_user_data(e));

    if (code == LV_EVENT_CLICKED)
    {
        if (0)
        {
            uint16_t x, y;
            ft6206_read_xy(&x, &y);
            printf("*****************I've been clicked (%d,%d)\n", x, y);
        }

        start_lock_action(!locked);
    }
}

static void draw_icon_image(lv_obj_t *parent, bool locked)
{
    LV_IMG_DECLARE(lock_icon);
    LV_IMG_DECLARE(unlock_icon);
    lv_image_dsc_t const * icon;

    if (active_icon)
    {
        lv_obj_delete(active_icon);
        active_icon = NULL;
    }

    icon = (locked ? &lock_icon : &unlock_icon);
    active_icon = lv_image_create(parent);
    lv_image_set_src(active_icon, icon);
    lv_obj_align(active_icon, LV_ALIGN_CENTER, 0, 0);
}

void draw_lock_screen(bool locked)
{
    lv_color_t black = lv_color_black();
    lv_obj_t *base;
    char *button_text;

    /* Clear the display */
    base = clear_screen(true);

    lv_obj_t *txt = lv_label_create(base);
    lv_obj_align(txt, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_height(txt, LV_SIZE_CONTENT);
    lv_obj_set_width(txt, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(txt, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(txt, black, 0);
    lv_label_set_text(txt, (const char *)STR_LOCK_STATUS);

    /*
     * Create a basic object to hold the image icon / animation.
     */

    lock_display_area = lv_obj_create(base);
    lv_obj_set_size(lock_display_area, LOCK_DISPLAY_AREA_WIDTH, LOCK_DISPLAY_AREA_HEIGHT);
    lv_obj_set_pos(lock_display_area, LOCK_DISPLAY_AREA_X, LOCK_DISPLAY_AREA_Y);
    lv_obj_set_style_bg_opa(lock_display_area, LV_OPA_TRANSP, LV_PART_MAIN);    // Transparent
    lv_obj_set_style_border_width(lock_display_area, 0, LV_PART_MAIN);          // No border
    lv_obj_set_style_pad_all(lock_display_area, 0, LV_PART_MAIN);               // No padding
    lv_obj_clear_flag(lock_display_area, LV_OBJ_FLAG_SCROLLABLE);

    draw_icon_image(lock_display_area, locked);

    /* Create a button using our button style */
    lv_obj_t *btn = lv_button_create(base);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_size(btn, 140, 40);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, (void *)locked);

    /* Add a label to the button */
    button_text = (locked ? STR_UNLOCK_ACTION : STR_LOCK_ACTION);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, button_text);
    lv_obj_center(label);
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)var, v);
}

void start_lock_screen_animation(void)
{
    /*
     * Make sure we're on the right display.
     */

    if (lock_display_area == NULL)
    {
        return;
    }

    if (active_icon)
    {
        lv_obj_delete(active_icon);
        active_icon = NULL;
    }

    /*
     * Create the animation object.
     */

    lv_obj_t *obj = lv_obj_create(lock_display_area);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(obj, ANIMATION_OBJ_SIZE, ANIMATION_OBJ_SIZE);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_BLUE), 0);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);

    /*
     * Create the animation.
     */

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_values(&a, 10, LOCK_DISPLAY_AREA_WIDTH - 10 - ANIMATION_OBJ_SIZE);
    lv_anim_start(&a);
}

static void draw_qrcode_screen(char *qrcode)
{
    lv_color_t bg_color = lv_color_white();
    lv_color_t fg_color = lv_color_black();
    lv_obj_t *base;


    /* Clear the display */
    base = clear_screen(true);

    /*
     * Create some text labels
     */

    lv_obj_t *txt = lv_label_create(base);
    lv_obj_align(txt, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_height(txt, LV_SIZE_CONTENT);
    lv_obj_set_width(txt, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(txt, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(txt, fg_color, 0);
    lv_label_set_text(txt, (const char *)STR_PLEASE_SCAN);

    lv_obj_t *txt2 = lv_label_create(base);
    lv_obj_align(txt2, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_height(txt2, LV_SIZE_CONTENT);
    lv_obj_set_width(txt2, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(txt2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(txt2, fg_color, 0);
    lv_label_set_text(txt2, (const char *)STR_APP_DESCRIPTION);

    /*
     * Create the QR code object.
     */

    lv_obj_t *qr = lv_qrcode_create(base);
    lv_qrcode_set_size(qr, 140);
    lv_qrcode_set_dark_color(qr, fg_color);
    lv_qrcode_set_light_color(qr, bg_color);

    /* Generate and display the QR code */
    lv_qrcode_update(qr, qrcode, strlen(qrcode));
    lv_obj_center(qr);
}

static void draw_splash_screen(void)
{
    LV_IMG_DECLARE(ifx_logo);
    lv_obj_t *base;

    /* Clear the display */
    base = clear_screen(false);

    lv_obj_t * img1 = lv_image_create(base);
    lv_image_set_src(img1, &ifx_logo);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, -10);

    lv_obj_t *txt2 = lv_label_create(base);
    lv_obj_align(txt2, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_height(txt2, LV_SIZE_CONTENT);
    lv_obj_set_width(txt2, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(txt2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(txt2, lv_color_black(), 0);
    lv_label_set_text(txt2, (const char *)STR_APP_DESCRIPTION);
}

static void lvgl_init(void)
{
    lv_init();

    cy_rtos_init_timer(&lvgl_timer, CY_TIMER_TYPE_PERIODIC, lvgl_handler, (cy_timer_callback_arg_t)NULL);
    cy_rtos_start_timer(&lvgl_timer, LVGL_TIMEOUT_MS);

    display_handle = lv_ili9341_create(DISPLAY_HOR_RES, DISPLAY_VER_RES, LV_LCD_FLAG_NONE, display_send_cmd, display_send_color);
    lv_display_set_rotation(display_handle, LV_DISPLAY_ROTATION_270);

    uint32_t buf_size = DISPLAY_HOR_RES * DISPLAY_VER_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(display_handle));

    static lv_color_t* buf1;
    buf1 = lv_malloc(buf_size);
    if (buf1 == NULL)
    {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }
    memset(buf1, 0xFF, buf_size);
    lv_display_set_buffers(display_handle, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*
     * Initialize the LVGL input device.
     */

    if (lv_port_indev_ft6202_init() != CY_RSLT_SUCCESS)
    {
        printf("Touchscreen driver init failed\n");
    }

    /*
     * Initialize the button styles
     */

    button_style_init();

    /*
     * Display the splash screen.
     */

    draw_splash_screen();
}

static void display_task(cy_thread_arg_t arg)
{
    /*
     * Reset the display
     */

    cyhal_gpio_write(DISPLAY_RESET_PIN, 1);
    cy_rtos_delay_milliseconds(DISPLAY_RESET_DELAY_MS);
    cyhal_gpio_write(DISPLAY_RESET_PIN, 0);

    lvgl_init();

    lv_timer_handler();

    /*
     * Start the backlight now since we know that the display
     * has been updated and will start cleanly.
     */

    cyhal_pwm_start(&backlight_pwm_handle);
    cy_rtos_delay_milliseconds(10);

    while (1)
    {
//      cy_rtos_mutex_get(&lvgl_mutex,CY_RTOS_NEVER_TIMEOUT);
        lv_timer_handler(); /* run lv_timer_handler() every 10ms */
//      cy_rtos_mutex_set(&lvgl_mutex);
        cy_rtos_delay_milliseconds(10);
    }
}

void display_init(void)
{
    cy_rslt_t result;

    /*
     * SPI initialization
     */

    result = cyhal_spi_init(&display_spi_obj, CYBSP_SPI_MOSI, CYBSP_SPI_MISO, CYBSP_SPI_CLK, CYBSP_SPI_CS, NULL, 8, CYHAL_SPI_MODE_00_MSB, false);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("ERROR: cyhal_spi_init failed\n");
        return;
    }

    if (cyhal_spi_set_frequency(&display_spi_obj, SPI_CLK_DEFAULT_HZ) != CY_RSLT_SUCCESS)
    {
        printf("ERROR: cyhal_spi_set_frequency failed\n");
        return;
    }

    /*
     * Set up the GPIOs we need.
     */

    if (cyhal_gpio_init(DISPLAY_DC_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0) != CY_RSLT_SUCCESS)
    {
        printf("ERROR: cyhal_gpio_init DISPLAY_DC_PIN failed\n");
    }

    if (cyhal_gpio_init(DISPLAY_RESET_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1) != CY_RSLT_SUCCESS)
    {
        printf("ERROR: cyhal_gpio_init DISPLAY_RESET_PIN failed\n");
    }

    /*
     * Set up the PWM for the backlight. Don't start the backlight until we've rendered the first
     * screen to prevent old images from displaying.
     */

    cyhal_pwm_init(&backlight_pwm_handle, DISPLAY_BACKLIGHT_PIN, NULL);
    cyhal_pwm_set_duty_cycle(&backlight_pwm_handle, BACKLIGHT_PWM_DUTY_CYCLE_DEFAULT, BACKLIGHT_PWM_FREQ_HZ);
    //cyhal_pwm_start(&backlight_pwm_handle);

    /*
     * Initialize the touchscreen.
     */

    if (ft6206_init() != CY_RSLT_SUCCESS)
    {
        printf("Error initializing touchscreen\n");
    }

    /*
     * Kick off the LCD task.
     */

    result = cy_rtos_thread_create(&display_task_handle, display_task, DISPLAY_TASK_NAME, display_task_stack,
                                   DISPLAY_TASK_STACK_SIZE, CY_RTOS_PRIORITY_NORMAL, 0);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Display thread creation failed\n");
    }
}

void display_qrcode(char *qrcode)
{
    if (qrcode != NULL)
    {
        draw_qrcode_screen(qrcode);
    }
}

#endif /* ENABLE_UI */
