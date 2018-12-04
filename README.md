# SensorTag R40 and Basic Sensor
Firmware for the new SensorTag R40 and Basic Sensor from Infor-link.
2018-06-08: Adopted to SDK15.0

Both boards are based on nRF52832 and has 1 RGB LED. The Basic Sensor has two buttons and one BME280 THP sensor. The SensorTag R40 has one button and 3 sensors (BMP280, BMI160, AP3216C) as well as on-board flash.

The LOG is by default disable. Re-enable by setting `NRF_LOG_ENABLED` to 1 in `app_config.h`

This board uses P0.21 as one of the I2C bus, however, P0.21 is set to /RESET by nRF SDK default. Therefore, extra step must be taken care of to use it for GPIO:

Both
    CFLAGS += -DCONFIG_GPIO_AS_PINRESET
    ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
were commented out so that P0.21 is no longer used for RESET

Perform at least once on a board with
    nrfjprog --recover
The above is required so that P.21 is no longer tied to /RESET

##Logging
make sdk_config to launch the GUI for maintaining sdk_config.h
Enable NRF_LOG and compile, flash.
Launch JLink logger:
JLinkExe -device NRF52832_XXAA -if SWD -speed 4000 -autoconnect 1
JLinkRTTClient


##Note
_NRF_SDH_BLE_VS_UUID_COUNT_ is very important. It *MUST* match the VS UUID count. If not, some Bluetooth client may not be able to read reliably the Services. If there is no VS UUID, set it to 0.

## Board variations

### Basic Sensor
make BOARD=BASIC_SENSOR
Or define by adding CFLAGS += -DBASIC_SENSOR in Makefile

### SensorTag R40
make BOARD=SENSORTAG_R40
Or define by adding CFLAGS += -DSENSORTAG_R40 in Makefile

### AC Powered
Define by adding CFLASG += -DPOWERUP in Makefile


##Other board variations

### Board with external VEML6075 UV sensor
make SENSOR=VME6075
Or, in main.c
Change `USE_VME6075` to 1
Change `USE_AP3216C` to 0

### Board without external sensor

In main.c
Change `USE_AP3216C` to 1
Change `USE_VME6075` to 0

## Compile and flash
```cd pca10040/s132/armgcc
make [BOARD=...] [SENSOR=...]
nrfjprog --recover
make flash_softdevice
make flash
```


