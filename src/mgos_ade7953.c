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

#include "mgos_ade7953_internal.h"

struct mgos_ade7953 *mgos_ade7953_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  struct mgos_ade7953 *dev = calloc(1, sizeof(*dev));
  if (!dev) return NULL;
  dev->i2c = i2c;
  dev->i2caddr = i2caddr;
  return dev;
}

bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts) {
  if (!dev || !volts) return false;
  return false;
}

bool mgos_ade7953_get_current(struct mgos_ade7953 *dev, int channel, float *amperes) {
  if (!dev || !amperes) return false;
  if (channel < 0 || channel > 1) return false;
  return false;
}

bool mgos_ade7953_destroy(struct mgos_ade7953 **dev) {
  if (!*dev) return false;
  free(*dev);
  *dev = NULL;
  return true;
}

bool mgos_ade7953_i2c_init(void) {
  return true;
}
