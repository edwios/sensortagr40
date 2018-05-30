# SensorTag R40
Firmware for the new SensorTag R40 from Infor-link.

This board is based on nRF52832 and has 1 RGB LED, two buttons, 3 sensors (BMP280, BMI160, AP3216C) and on-board flash.

The LOG is by default disable. Re-enable by setting `NRF_LOG_ENABLED` to 1 in `app_config.h`

This board uses P0.21 as one of the I2C bus, however, P0.21 is set to /RESET by nRF SDK default. Therefore, extra step must be taken care of to use it for GPIO:

Both
    CFLAGS += -DCONFIG_GPIO_AS_PINRESET
    ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
were commented out so that P0.21 is no longer used for RESET

Perform at least once on a board with
    nrfjprog --recover
The above is required so that P.21 is no longer tied to /RESET

## Board variations
### Board with external VEML6075 UV sensor

In main.c
Change `USE_VME6075` to 1
Change `USE_AP3216C` to 0

### Board without external sensor

In main.c
Change `USE_AP3216C` to 1
Change `USE_VME6075` to 0

## Compile and flash
```cd pca10040/s132/armgcc
make
nrfjprog --recover
make flash_softdevice
make flash
```


