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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_result.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define FT6206_REG_DEV_MODE             (0x00)
#define FT6206_REG_GEST_ID              (0x01)
#define FT6206_REG_TD_STATUS            (0x02)
#define FT6206_REG_P1_XH                (0x03)
#define FT6206_REG_P1_XL                (0x04)
#define FT6206_REG_P1_YH                (0x05)
#define FT6206_REG_P1_YL                (0x06)
#define FT6206_REG_P1_WEIGHT            (0x07)
#define FT6206_REG_P1_MISC              (0x08)
#define FT6206_REG_P2_XH                (0x09)
#define FT6206_REG_P2_XL                (0x0A)
#define FT6206_REG_P2_YH                (0x0B)
#define FT6206_REG_P2_YL                (0x0C)
#define FT6206_REG_P2_WEIGHT            (0x0D)
#define FT6206_REG_P2_MISC              (0x0E)
#define FT6206_REG_TH_GROUP             (0x80)
#define FT6206_REG_TH_DIFF              (0x85)
#define FT6206_REG_CTRL                 (0x86)
#define FT6206_REG_TIMEENTERMONITOR     (0x87)
#define FT6206_REG_PERIODACTIVE         (0x88)
#define FT6206_REG_PERIODMONITOR        (0x89)
#define FT6206_REG_RADIAN_VALUE         (0x91)
#define FT6206_REG_OFFSET_LEFT_RIGHT    (0x92)
#define FT6206_REG_OFFSET_UP_DOWN       (0x93)
#define FT6206_REG_DISTANCE_LEFT_RIGHT  (0x94)
#define FT6206_REG_DISTANCE_UP_DOWN     (0x95)
#define FT6206_REG_DISTANCE_ZOOM        (0x96)
#define FT6206_REG_LIB_VER_H            (0xA1)
#define FT6206_REG_LIB_VER_L            (0xA2)
#define FT6206_REG_CIPHER               (0xA3)
#define FT6206_REG_G_MODE               (0xA4)
#define FT6206_REG_PWR_MODE             (0xA5)
#define FT6206_REG_FIRMID               (0xA6)
#define FT6206_REG_FOCALTECH_ID         (0xA8)
#define FT6206_REG_RELEASE_CODE         (0xAF)
#define FT6206_REG_STATE                (0xBC)

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

cy_rslt_t ft6206_init(void);
cy_rslt_t ft6206_read_byte(uint8_t reg_addr, uint8_t *data);
cy_rslt_t ft6206_write_byte(uint8_t reg_addr, uint8_t data);

cy_rslt_t ft6206_read_xy(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
} /* extern C */
#endif
