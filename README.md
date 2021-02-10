# Mongoose OS driver for ADE7953

This is a driver for Analog Devices [ADE7953](docs/ADE7953.pdf).

## Bus selection

Communication over I2C and SPI is supported, with I2C enabled by default. Bus
support can be selected/deselected using defines:

```
cdefs:
  MGOS_ADE7953_ENABLE_SPI: 1
  MGOS_ADE7953_ENABLE_I2C: 0
```

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
  float f = 0, v = 0, ia = 0, ib = 0, aea = 0, aeb = 0, apa = 0, apb = 0;
  if (!ade) return;

  mgos_ade7953_get_frequency(ade, &f);
  mgos_ade7953_get_voltage(ade, &v);
  mgos_ade7953_get_current(ade, 0, &ia);
  mgos_ade7953_get_current(ade, 1, &ib);
  mgos_ade7953_get_apower(ade, 0, &apa);
  mgos_ade7953_get_apower(ade, 1, &apb);
  mgos_ade7953_get_aenergy(ade, 0, false /* reset */, &aea);
  mgos_ade7953_get_aenergy(ade, 1, false /* reset */, &aeb);
  LOG(LL_INFO, ("V=%.3fV f=%.2fHz | IA=%.3fA VIA=%.3f APA=%.3f AEA=%.3f | IB=%.3fA VIB=%.3f APB=%.3f AEB=%.3f",
                v, f, ia, (v * ia), apa, aea, ib, (v * ib), apb, aeb));
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_ade7953 *ade;

  // These constants are specific to the specific application circuit.
  const struct mgos_config_ade7953 ade_cfg = {
      .voltage_scale = .0000382602,
      .voltage_offset = -0.068,
      .current_scale_0 = 0.00000949523,
      .current_scale_1 = 0.00000949523,
      .current_offset_0 = -0.017,
      .current_offset_1 = -0.017,
      .apower_scale_0 = (1 / 164.0),
      .apower_scale_1 = (1 / 164.0),
      .aenergy_scale_0 = (1 / 25240.0),
      .aenergy_scale_1 = (1 / 25240.0),
      .voltage_pga_gain = MGOS_ADE7953_PGA_GAIN_1,
      .current_pga_gain_0 = MGOS_ADE7953_PGA_GAIN_8,
      .current_pga_gain_1 = MGOS_ADE7953_PGA_GAIN_8,
  };
  if (!(ade = mgos_ade7953_create(mgos_i2c_get_global(), &ade_cfg))) {
    return false;
  }

  mgos_set_timer(1000, true, ade7953_cb, ade);

  return MGOS_APP_INIT_SUCCESS;
}

```
