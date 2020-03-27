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

struct mgos_ade7953;

struct mgos_ade7953 *mgos_ade7953_create(struct mgos_i2c *i2c, uint8_t i2caddr);

bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts);
bool mgos_ade7953_get_current(struct mgos_ade7953 *dev, int channel, float *amperes);

bool mgos_ade7953_destroy(struct mgos_ade7953 **dev);

#ifdef __cplusplus
}
#endif
