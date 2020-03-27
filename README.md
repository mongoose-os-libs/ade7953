# I2C driver for ADE7953

This is a driver for Analog Devices [ADE7953](docs/ADE7953.pdf).

## Sensor details

The ADE7953 is a high accuracy electrical energy measurement
IC intended for single phase applications. It measures line voltage
and current and calculates active, reactive, and apparent energy,
as well as instantaneous RMS voltage and current.

## Example application

```
#include "mgos.h"
#include "mgos_ade7953.h"


static void ade7953_cb (void *user_data) {
  struct mgos_ade7953 *ade = (struct mgos_ade7953 *) user_data;
  float f, v, ia, ib;
  if (!ade) return;

  mgos_ade7953_get_frequency(ade, &f);
  mgos_ade7953_get_voltage(ade, &v);
  mgos_ade7953_get_current(ade, 0, &ia);
  mgos_ade7953_get_current(ade, 1, &ib);
  LOG(LL_INFO, ("freq=%.2fHz V=%.2fV IA=%.3fA IB=%.3fA",
                f, v, ia, ib));
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_ade7953 *ade = NULL;

  if (!(ade = mgos_ade7953_create(mgos_i2c_get_global(),
                                  MGOS_ADE7953_DEFAULT_I2CADDR))) {
    return false;
  }

  mgos_set_timer(1000, true, ade7953_cb, ade);

  return MGOS_APP_INIT_SUCCESS;
}

```
