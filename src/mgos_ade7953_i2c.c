#include "mgos_ade7953_internal.h"
#include "mgos_i2c.h"

struct mgos_ade7953 *mgos_ade7953_create_i2c(struct mgos_i2c *i2c, const struct mgos_ade7953_config *cfg) {
  struct mgos_ade7953 *dev = NULL;

  if (!i2c) return NULL;
  if (!(dev = calloc(1, sizeof(*dev)))) return NULL;
  dev->i2c = i2c;

  if (mgos_ade7953_create_common(dev, cfg)) {
    return dev;
  }
  return NULL;
}

bool mgos_ade7953_write_reg_i2c(struct mgos_ade7953 *dev, uint16_t reg, int size, int32_t val) {
  uint8_t data[6];
  int i;

  data[0] = (reg >> 8) & 0xff;
  data[1] = reg & 0xff;

  i = 2;
  while (size--) {
    data[i++] = (val >> (8 * size)) & 0xff;
  }

  if (!mgos_i2c_write(dev->i2c, MGOS_ADE7953_I2C_ADDR, data, i, true)) {
    return false;
  }
  mgos_usleep(5);
  return true;
}

bool mgos_ade7953_read_reg_i2c(struct mgos_ade7953 *dev, uint16_t reg, int size, uint8_t *val) {
  uint8_t data[4];

  data[0] = (reg >> 8) & 0xff;
  data[1] = reg & 0xff;
  if (!mgos_i2c_write(dev->i2c, MGOS_ADE7953_I2C_ADDR, data, 2, true)) {
    return false;
  }
  if (!mgos_i2c_read(dev->i2c, MGOS_ADE7953_I2C_ADDR, data, size, true)) {
    return false;
  }

  memcpy(val, data, size);
  return true;
}
