/*
 * Copyright 2020 Pim van Pelt <pim@ipng.nl>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "mgos_i2c.h"

#define MGOS_ADE7953_DEFAULT_I2CADDR (0x38)

struct mgos_ade7953;

// Create an instance of the driver at the given I2C bus and address.
// Returns a pointer to the object upon success, NULL otherwise.
struct mgos_ade7953 *mgos_ade7953_create(struct mgos_i2c *i2c, uint8_t i2caddr);

// Write the detected voltage in Volts RMS in the *volts pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts);

// Set scaling factor for the current shunt on the given channel (0 or 1).
// Returns true on success, false otherwise.
bool mgos_ade7953_set_scale_current(struct mgos_ade7953 *dev, int channel, float scale);

// Set scaling factor for the volotage divider over the VP/VN pins of the chip.
// Returns true on success, false otherwise.
bool mgos_ade7953_set_scale_voltage(struct mgos_ade7953 *dev, float scale);

// Write the detected line frequency in Hertz to the *hertz pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_frequency(struct mgos_ade7953 *dev, float *hertz);

// Write the measured RMS current for the given channel (0 or 1), in Amperes
// to the *amperes pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_current(struct mgos_ade7953 *dev, int channel, float *amperes);

// CLean up the driver and return memory used for it.
bool mgos_ade7953_destroy(struct mgos_ade7953 **dev);

#ifdef __cplusplus
}
#endif
