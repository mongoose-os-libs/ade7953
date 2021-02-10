#include "mgos_ade7953_internal.h"
#include "mgos_spi.h"

struct mgos_ade7953 *mgos_ade7953_create_spi(struct mgos_spi *spi, int cs, const struct mgos_config_ade7953 *cfg) {
  struct mgos_ade7953 *dev = NULL;

  if (!spi) return NULL;
  if (!(dev = calloc(1, sizeof(*dev)))) return NULL;
  dev->spi = spi;

  if (mgos_ade7953_create_common(dev, cfg)) {
    return dev;
  }
  return NULL;
}

bool mgos_ade7953_write_reg_spi(struct mgos_ade7953 *dev, uint16_t reg, int size, int32_t val) {
  uint8_t data_out[7];
  int i;

  if (!dev || size > 4 || size < 0) return false;

  data_out[0] = (reg >> 8) & 0xff;
  data_out[1] = reg & 0xff;
  data_out[2] = 0x00;

  i = 3;
  while (size--) {
    data_out[i++] = (val >> (8 * size)) & 0xff;
  }

  struct mgos_spi_txn txn = {
      .cs = dev->spi_cs,
      .mode = 0,
      .freq = MGOS_ADE7953_SPI_FREQ,
      .hd =
          {
              .tx_data = data_out,
              .tx_len = 7,
              .dummy_len = 0,
              .rx_data = NULL,
              .rx_len = 0,
          },
  };

  bool res = mgos_spi_run_txn(dev->spi, false /* full-duplex */, &txn);
  if (!res) {
    return false;
  }

  return true;
}

bool mgos_ade7953_read_reg_spi(struct mgos_ade7953 *dev, uint16_t reg, int size, uint8_t *val) {
  uint8_t data_out[3];

  data_out[0] = (reg >> 8) & 0xff;
  data_out[1] = reg & 0xff;
  data_out[2] = 0x80;

  struct mgos_spi_txn txn = {
      .cs = dev->spi_cs,
      .mode = 0,
      .freq = MGOS_ADE7953_SPI_FREQ,
      .hd =
          {
              .tx_data = data_out,
              .tx_len = 3,
              .dummy_len = 0,
              .rx_data = val,
              .rx_len = size,
          },
  };

  bool res = mgos_spi_run_txn(dev->spi, false /* full-duplex */, &txn);
  if (!res) {
    LOG(LL_ERROR, ("read_reg failed!"));
    return false;
  }

  return true;
}
