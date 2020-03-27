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

static int mgos_ade7953_regsize(uint16_t reg) {
  int size = 1;
  if (reg >= 0x300) size++;
  if (reg >= 0x200) size++;
  if (reg >= 0x100) size++;
  return size;
}

static bool mgos_ade7953_write(struct mgos_ade7953 *dev, uint16_t reg, int32_t val) {
  uint8_t data[6];
  int size = mgos_ade7953_regsize(reg);
  int i;

  if (!dev || size > 4 || size < 0) return false;
  data[0] = (reg >> 8) & 0xff;
  data[1] = reg & 0xff;

  i = 2;
  while (size--) {
    data[i++] = (val >> (8 * size)) & 0xff;
  }

  if (!mgos_i2c_write(dev->i2c, dev->i2caddr, data, i, true)) {
    LOG(LL_ERROR, ("ADE7953 I2C write error (%d bytes)", i));
    return false;
  }
  mgos_usleep(5);
  return true;
}

static int32_t mgos_ade7953_read(struct mgos_ade7953 *dev, uint16_t reg) {
  uint8_t data[4];
  int32_t val;
  int size = mgos_ade7953_regsize(reg);
  if (!dev || size > 4 || size < 0) return false;

  data[0] = (reg >> 8) & 0xff;
  data[1] = reg & 0xff;
  if (!mgos_i2c_write(dev->i2c, dev->i2caddr, data, 2, true)) {
    LOG(LL_ERROR, ("ADE7953 I2C write error (2 bytes)"));
    return 0xff;
  }
  if (!mgos_i2c_read(dev->i2c, dev->i2caddr, data, size, true)) {
    LOG(LL_ERROR, ("ADE7953 I2C read8 error (%d bytes)", size));
    return 0xff;
  }
  val = 0;
  for (int i = 0; i < size; i++) val = (val << 8) | data[i];
  return val;
}

struct mgos_ade7953 *mgos_ade7953_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  struct mgos_ade7953 *dev;
  uint8_t version;

  if (!i2c) return NULL;
  if (!(dev = calloc(1, sizeof(*dev)))) return NULL;
  dev->i2c = i2c;
  dev->i2caddr = i2caddr;

  // Empirical observation, use API calls to override.
  dev->voltage_scale = (1. / 26000);
  dev->current_scale[0] = (1. / 100000);
  dev->current_scale[1] = (1. / 100000);

  version = mgos_ade7953_read(dev, MGOS_ADE7953_REG_VERSION);
  LOG(LL_INFO, ("ADE7953 silicon version: 0x%02x (%d)", version, version));

  // Lock comms interface, enable high pass filter
  mgos_ade7953_write(dev, MGOS_ADE7953_REG_CONFIG, 0x04);

  // Unlock unnamed (!) register 0x120 (see datasheet, page 18)
  mgos_ade7953_write(dev, MGOS_ADE7953_REG_UNNAMED, 0xAD);

  // Set "optimal setting" (see datasheet, page 18)
  mgos_ade7953_write(dev, MGOS_ADE7953_REG_RESERVED, 0x30);

  return dev;
}

bool mgos_ade7953_get_frequency(struct mgos_ade7953 *dev, float *hertz) {
  int32_t val;
  if (!dev || !hertz) return false;
  val = mgos_ade7953_read(dev, MGOS_ADE7953_REG_PERIOD);
  *hertz = 223750.0f / ((float) val + 1);
  return true;
}

bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts) {
  int32_t val;
  if (!dev || !volts) return false;
  val = mgos_ade7953_read(dev, MGOS_ADE7953_REG_V);
  *volts = val * dev->voltage_scale;
  return true;
}

bool mgos_ade7953_get_current(struct mgos_ade7953 *dev, int channel, float *amperes) {
  uint16_t reg;
  int32_t val;
  if (!dev || !amperes) return false;
  if (channel == 0)
    reg = MGOS_ADE7953_REG_IA;
  else if (channel == 1)
    reg = MGOS_ADE7953_REG_IB;
  else
    return false;
  val = mgos_ade7953_read(dev, reg);
  *amperes = val * dev->current_scale[channel];
  return true;
}

bool mgos_ade7953_set_scale_voltage(struct mgos_ade7953 *dev, float scale) {
  if (!dev) return false;
  dev->voltage_scale = scale;
  return true;
}

bool mgos_ade7953_set_scale_current(struct mgos_ade7953 *dev, int channel, float scale) {
  if (!dev) return false;
  if (channel == 0 || channel == 1) {
    dev->current_scale[channel] = scale;
    return true;
  }
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
