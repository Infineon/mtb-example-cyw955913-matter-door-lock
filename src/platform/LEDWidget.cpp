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

#include "LEDWidget.h"

#include "cybsp.h"
#include "cyhal.h"
#include <platform/CHIPDeviceLayer.h>

/*******************************************************************************
 * Macro Definitions
 *******************************************************************************/
/* Allowed TCPWM compare value for maximum brightness */
#define LED_MAX_BRIGHTNESS (100u)

/* Allowed TCPWM compare value for minimum brightness*/
#define LED_MIN_BRIGHTNESS (1u)

#define PWM_LED_FREQ_HZ (1000000u) /* in Hz */

/* subtracting from 100 since the LED is connected in active low configuration */
#define GET_DUTY_CYCLE(x) (100 - x)

void LEDWidget::Init(int ledNum)
{
    mLastChangeTimeMS = 0;
    mBlinkOnTimeMS    = 0;
    mBlinkOffTimeMS   = 0;
    mLedNum           = ledNum;
    mState            = 0;
    mbrightness       = LED_MAX_BRIGHTNESS;

    if (CY_RSLT_SUCCESS !=
        cyhal_gpio_init((cyhal_gpio_t) ledNum, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF))
    {
        printf("GPIO Init failed for Led %d \r\n", ledNum);
    }
}

void LEDWidget::Invert(void)
{
    Set(!mState);
}

void LEDWidget::Set(bool state)
{
    mLastChangeTimeMS = mBlinkOnTimeMS = mBlinkOffTimeMS = 0;
    DoSet(state);
}

bool LEDWidget::Get()
{
    return mState;
}

void LEDWidget::Blink(uint32_t changeRateMS)
{
    Blink(changeRateMS, changeRateMS);
}

void LEDWidget::Blink(uint32_t onTimeMS, uint32_t offTimeMS)
{
    mBlinkOnTimeMS  = onTimeMS;
    mBlinkOffTimeMS = offTimeMS;
    Animate();
}

void LEDWidget::Animate()
{
    if (mBlinkOnTimeMS != 0 && mBlinkOffTimeMS != 0)
    {
        uint64_t nowMS            = chip::System::SystemClock().GetMonotonicMilliseconds64().count();
        uint64_t stateDurMS       = mState ? mBlinkOnTimeMS : mBlinkOffTimeMS;
        uint64_t nextChangeTimeMS = mLastChangeTimeMS + stateDurMS;

        if (nextChangeTimeMS < nowMS)
        {
            DoSet(!mState);
            mLastChangeTimeMS = nowMS;
        }
    }
}

void LEDWidget::DoSet(bool state)
{
    if (mState != state)
    {
        cyhal_gpio_write((cyhal_gpio_t) mLedNum, ((state) ? CYBSP_LED_STATE_ON : CYBSP_LED_STATE_OFF));
    }
    mState = state;
}

void LEDWidget::PWM_start()
{
    if (!mState)
    {
        /* Start PWM to turn the LED on */
        if (CY_RSLT_SUCCESS != cyhal_pwm_start(&pwm_led))
        {
            printf("PWM failed to start!");
        }
        mState = 1;
    }
}

void LEDWidget::PWM_stop()
{
    if (mState)
    {
        /* Stop PWM to turn the LED off */
        if (CY_RSLT_SUCCESS != cyhal_pwm_stop(&pwm_led))
        {
            printf("PWM failed to stop!");
        }
        mState = 0;
    }
}

void LEDWidget::SetColor(uint8_t Hue, uint8_t Saturation)
{

}

void LEDWidget::RGB_init()
{

}


void LEDWidget::SetBrightness(uint32_t led_brightness)
{

}

void LEDWidget::RGB_set(bool state)
{

} 
