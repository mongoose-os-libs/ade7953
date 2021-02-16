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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mgos.h"
#include "mgos_ade7953.h"

#define MGOS_ADE7953_I2C_ADDR 0x38

// Registers
//
#define MGOS_ADE7953_REG_LCYCMODE 0x004
#define MGOS_ADE7953_REG_PGA_V 0x007
#define MGOS_ADE7953_REG_PGA_IA 0x008
#define MGOS_ADE7953_REG_PGA_IB 0x009
#define MGOS_ADE7953_REG_UNNAMED 0x0FE

#define MGOS_ADE7953_REG_LINECYC 0x101
#define MGOS_ADE7953_REG_CONFIG 0x102
#define MGOS_ADE7953_REG_PERIOD 0x10E
#define MGOS_ADE7953_REG_RESERVED 0x120

#define MGOS_ADE7953_REG_AIRMSOS 0x386
#define MGOS_ADE7953_REG_VRMSOS 0x388
#define MGOS_ADE7953_REG_BIRMSOS 0x392
#define MGOS_ADE7953_REG_AVA 0x310
#define MGOS_ADE7953_REG_BVA 0x311
#define MGOS_ADE7953_REG_AWATT 0x312
#define MGOS_ADE7953_REG_BWATT 0x313
#define MGOS_ADE7953_REG_AVAR 0x314
#define MGOS_ADE7953_REG_BVAR 0x315
#define MGOS_ADE7953_REG_IA 0x31A
#define MGOS_ADE7953_REG_IB 0x31B
#define MGOS_ADE7953_REG_V 0x31C
#define MGOS_ADE7953_REG_ANENERGYA 0x31E
#define MGOS_ADE7953_REG_ANENERGYB 0x31F
#define MGOS_ADE7953_REG_PFA 0x10A
#define MGOS_ADE7953_REG_PFB 0x10B

#define MGOS_ADE7953_REG_IRQSTATA 0x32D
#define MGOS_ADE7953_REG_IRQSTATA_RESET (1 << 20)

#define MGOS_ADE7953_REG_VERSION 0x702
#define MGOS_ADE7953_REG_EX_REF 0x800

#define MGOS_ADE7953_REG_CONFIG_HPFEN (1 << 2)
#define MGOS_ADE7953_REG_CONFIG_SWRST (1 << 7)

struct mgos_ade7953 {
#if MGOS_ADE7953_ENABLE_I2C
  struct mgos_i2c *i2c;
#endif
#if MGOS_ADE7953_ENABLE_SPI
  struct mgos_spi *spi;
  int spi_cs;
#endif

  float voltage_scale;
  float current_scale[2];
  float apower_scale[2];
  float aenergy_scale[2];
};

bool mgos_ade7953_create_common(struct mgos_ade7953 *dev, const struct mgos_config_ade7953 *cfg);

#if MGOS_ADE7953_ENABLE_I2C
bool mgos_ade7953_write_reg_i2c(struct mgos_ade7953 *dev, uint16_t reg, int size, int32_t val);
bool mgos_ade7953_read_reg_i2c(struct mgos_ade7953 *dev, uint16_t reg, int size, uint8_t *val);
#endif

#if MGOS_ADE7953_ENABLE_SPI
bool mgos_ade7953_write_reg_spi(struct mgos_ade7953 *dev, uint16_t reg, int size, int32_t val);
bool mgos_ade7953_read_reg_spi(struct mgos_ade7953 *dev, uint16_t reg, int size, uint8_t *val);
#endif
