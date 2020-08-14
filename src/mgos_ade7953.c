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

#include "mgos_ade7953.h"

#include "mgos_i2c.h"

#include "mgos_ade7953_internal.h"

int mgos_ade7953_regsize(uint16_t reg) {
  int size = 1;
  if (reg != MGOS_ADE7953_REG_VERSION && reg != MGOS_ADE7953_REG_EX_REF) {
    if (reg >= 0x300) size++;
    if (reg >= 0x200) size++;
    if (reg >= 0x100) size++;
  }
  return size;
}

bool mgos_ade7953_write_reg(struct mgos_ade7953 *dev, uint16_t reg, int32_t val) {
  int size = mgos_ade7953_regsize(reg);
  if (!dev || size > 4 || size < 0) return false;

#if MGOS_ADE7953_ENABLE_I2C
  if (dev->i2c) return mgos_ade7953_write_reg_i2c(dev, reg, size, val);
#endif
#if MGOS_ADE7953_ENABLE_SPI
  if (dev->spi) return mgos_ade7953_write_reg_spi(dev, reg, size, val);
#endif
    return false;
}

bool mgos_ade7953_read_reg(struct mgos_ade7953 *dev, uint16_t reg, bool is_signed, int32_t *val) {
  int32_t v = 0;
  uint8_t data[4];
  int size = mgos_ade7953_regsize(reg);

  if (!dev || size > 4 || size < 0) return false;

  do {
#if MGOS_ADE7953_ENABLE_I2C
    if (dev->i2c) {
      if (mgos_ade7953_read_reg_i2c(dev, reg, size, &data[0])) break;
      return false;
#endif
#if MGOS_ADE7953_ENABLE_SPI
    if (dev->spi) {
      if (mgos_ade7953_read_reg_spi(dev, reg, size, &data[0])) break;
      return false;
    }
#endif
    return false;
  } while (false);

  for (int i = 0; i < size; i++) {
    v = (v << 8) | data[i];
  }
  if (is_signed) {
    uint32_t sign_mask = 0;
    if (size == 1) sign_mask = (1 << 7);
    if (size == 2) sign_mask = (1 << 15);
    if (size == 3) sign_mask = (1 << 23);
    if ((v & sign_mask) != 0) {
      v &= ~sign_mask;
      v |= (1 << 31);
    }
  }
  *val = (int32_t) v;

  return true;
}

bool mgos_ade7953_create_common(struct mgos_ade7953 *dev, const struct mgos_ade7953_config *cfg) {
  int32_t version;

  dev->voltage_scale = cfg->voltage_scale;
  for (int i = 0; i < 2; i++) {
    dev->current_scale[i] = cfg->current_scale[i];
    dev->apower_scale[i] = cfg->apower_scale[i];
    dev->aenergy_scale[i] = cfg->aenergy_scale[i];
  }

  if (mgos_ade7953_read_reg(dev, MGOS_ADE7953_REG_VERSION, false, &version)) {
    LOG(LL_INFO, ("ADE7953 silicon version: 0x%02x (%d)", (int) version, (int) version));

    // Lock comms interface, enable high pass filter
    mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_CONFIG, 0x04);

    // Unlock unnamed (!) register 0x120 (see datasheet, page 18)
    mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_UNNAMED, 0xAD);

    // Set "optimal setting" (see datasheet, page 18)
    mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_RESERVED, 0x30);

    // Program measurement offsets.
    if (cfg->voltage_offset != 0) {
      mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_VRMSOS, (int32_t)(cfg->voltage_offset / cfg->voltage_scale));
    }
    if (cfg->current_offset[0] != 0) {
      mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_AIRMSOS, (int32_t)(cfg->current_offset[0] / cfg->current_scale[0]));
    }
    if (cfg->current_offset[1] != 0) {
      mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_BIRMSOS, (int32_t)(cfg->current_offset[1] / cfg->current_scale[1]));
    }

    mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_LCYCMODE, 0x40);

  } else {
    LOG(LL_ERROR, ("Failed to communicate with ADE7953"));
    return false;
  }
  return true;
}

bool mgos_ade7953_get_frequency(struct mgos_ade7953 *dev, float *hertz) {
  int32_t val;
  if (!dev || !hertz) return false;
  if (!mgos_ade7953_read_reg(dev, MGOS_ADE7953_REG_PERIOD, false, &val)) {
    return false;
  }
  *hertz = 223750.0f / ((float) val + 1);
  return true;
}

bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts) {
  int32_t val;
  if (!dev || !volts) return false;
  if (!mgos_ade7953_read_reg(dev, MGOS_ADE7953_REG_V, false, &val)) {
    return false;
  }
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
  if (!mgos_ade7953_read_reg(dev, reg, true, &val)) {
    return false;
  }
  *amperes = val * dev->current_scale[channel];
  return true;
}

bool mgos_ade7953_get_apower(struct mgos_ade7953 *dev, int channel, float *watts) {
  uint16_t reg;
  int32_t val;
  if (!dev || !watts) return false;
  if (channel == 0)
    reg = MGOS_ADE7953_REG_AWATT;
  else if (channel == 1)
    reg = MGOS_ADE7953_REG_BWATT;
  else
    return false;
  if (!mgos_ade7953_read_reg(dev, reg, true, &val)) {
    return false;
  }
  *watts = val * dev->apower_scale[channel];
  return true;
}

bool mgos_ade7953_get_aenergy(struct mgos_ade7953 *dev, int channel, bool reset, float *wh) {
  uint16_t reg;
  int32_t val;
  if (!dev || !wh) return false;
  if (channel == 0)
    reg = MGOS_ADE7953_REG_ANENERGYA;
  else if (channel == 1)
    reg = MGOS_ADE7953_REG_ANENERGYB;
  else
    return false;
  // Ensure LCYCMODE[6] (RSTREAD) is set correctly.
  if (!mgos_ade7953_read_reg(dev, MGOS_ADE7953_REG_LCYCMODE, false, &val)) {
    return false;
  }
  if (((bool) (val & (1 << 6))) != reset) {
    if (reset) {
      val |= (1 << 6);
    } else {
      val &= ~(1 << 6);
    }
    if (!mgos_ade7953_write_reg(dev, MGOS_ADE7953_REG_LCYCMODE, val)) {
      return false;
    }
  }

  if (!mgos_ade7953_read_reg(dev, reg, true, &val)) {
    return false;
  }
  *wh = val * dev->aenergy_scale[channel];
  return true;
}

bool mgos_ade7953_destroy(struct mgos_ade7953 **dev) {
  if (!*dev) return false;
  free(*dev);
  *dev = NULL;
  return true;
}

bool mgos_ade7953_init(void) {
  return true;
}
