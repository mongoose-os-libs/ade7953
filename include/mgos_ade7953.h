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


struct mgos_ade7953;

enum ade7953_pga_gain {
  MGOS_ADE7953_PGA_GAIN_1 = 0x00,
  MGOS_ADE7953_PGA_GAIN_2 = 0x01,
  MGOS_ADE7953_PGA_GAIN_4 = 0x02,
  MGOS_ADE7953_PGA_GAIN_8 = 0x03,
  MGOS_ADE7953_PGA_GAIN_16 = 0x04,
  MGOS_ADE7953_PGA_GAIN_22 = 0x05
};

struct mgos_ade7953_config {
  // Scaling factor to convert voltage channel ADC readings to voltage.
  // It depends on the parameters of the voltage divider used on the input.
  float voltage_scale;
  // Voltage measurement offset, volts.
  float voltage_offset;

  // Scaling factoris to convert current channel ADC readings to amperes.
  // Depends on the shunt parameters.
  float current_scale[2];
  // Current measurement offsets, in amps.
  float current_offset[2];

  // Scaling factors to convert active power to watts.
  float apower_scale[2];

  // Scaling factors to convert active energy to watt-hours.
  float aenergy_scale[2];

  // PGA selectable gains. Default is 0 (MGOS_ADE7953_PGA_GAIN_1).
  enum ade7953_pga_gain voltage_pga_gain;
  enum ade7953_pga_gain current_pga_gain[2];
};

#if MGOS_ADE7953_ENABLE_I2C
// Create an instance of the driver at the given I2C bus and address.
// Returns a pointer to the object upon success, NULL otherwise.
#include "mgos_i2c.h"
struct mgos_ade7953 *mgos_ade7953_create_i2c(struct mgos_i2c *i2c, const struct mgos_ade7953_config *cfg);
#define mgos_ade7953_create mgos_ade7953_create_i2c
#endif

#if MGOS_ADE7953_ENABLE_SPI
#include "mgos_spi.h"
struct mgos_ade7953 *mgos_ade7953_create_spi(struct mgos_spi *spi, int cs, const struct mgos_ade7953_config *cfg);
#endif

// Write the detected voltage in Volts RMS in the *volts pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_voltage(struct mgos_ade7953 *dev, float *volts);

// Write the detected line frequency in Hertz to the *hertz pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_frequency(struct mgos_ade7953 *dev, float *hertz);

// Write the measured RMS current for the given channel (0 or 1), in Amperes
// to the *amperes pointer.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_current(struct mgos_ade7953 *dev, int channel, float *amperes);

// Write the instantaneous active power value, in Watts, to *watts.
bool mgos_ade7953_get_apower(struct mgos_ade7953 *dev, int channel, float *watts);

// Write the accumulated active energy, in watt-hours, to *wh.
// If reset is true, resets the accumulator.
bool mgos_ade7953_get_aenergy(struct mgos_ade7953 *dev, int channel, bool reset, float *wh);

// Write the measured power factor to the *pf pointer.
// Power factor is a dimensionless number in the closed interval of −1 to 1.
// Returns true on success, false otherwise.
bool mgos_ade7953_get_pf(struct mgos_ade7953 *dev, int channel, float *pf);

// Advanced usage: functions to read/write ADE7953 registers.
bool mgos_ade7953_read_reg(struct mgos_ade7953 *dev, uint16_t reg, bool is_signed, int32_t *val);
bool mgos_ade7953_write_reg(struct mgos_ade7953 *dev, uint16_t reg, int32_t val);

// Clean up the driver and return memory used for it.
bool mgos_ade7953_destroy(struct mgos_ade7953 **dev);

#ifdef __cplusplus
}
#endif
