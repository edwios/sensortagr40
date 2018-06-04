 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "app_bmp280.h"
#include "nrf_gpio.h"
#include "nrf_drv_bmp280_twi.h"
#include "nrf_error.h"
#include "sdk_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Use floating point
//#define USE_FLOAT

bmp280_part_id_t m_partid;

uint32_t bmp280_config(void)
{
    uint32_t err_code;
    int16_t h4, h5;
    uint8_t ch=0;

    //---//---NRF_LOGDEBUG("Configurating BMP280"); //---NRF_LOGFLUSH();
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_CTRL_HUM, &ch, 1);
    ch &= 0xF8;
    ch |= BMP280_PARAM_HUM_OVS1;
    err_code = nrf_drv_bmp280_write_single_register(BMP280_REGISTER_CTRL_HUM, ch);
    if(err_code != NRF_SUCCESS) return err_code;
    err_code = nrf_drv_bmp280_write_single_register(BMP280_REGISTER_CONTROL, 0x6F);
    if(err_code != NRF_SUCCESS) return err_code;

    err_code = nrf_drv_bmp280_read_register16(BMP280_REGISTER_DIG_T1, &_bmp280_calib.dig_T1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_T2, &_bmp280_calib.dig_T2);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_T3, &_bmp280_calib.dig_T3);
//    //---NRF_LOGDEBUG("dig T1,2,3 %d, %d, %d", _bmp280_calib.dig_T1, _bmp280_calib.dig_T2, _bmp280_calib.dig_T3); //---NRF_LOGFLUSH();

    err_code = nrf_drv_bmp280_read_register16(BMP280_REGISTER_DIG_P1, &_bmp280_calib.dig_P1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P2, &_bmp280_calib.dig_P2);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P3, &_bmp280_calib.dig_P3);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P4, &_bmp280_calib.dig_P4);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P5, &_bmp280_calib.dig_P5);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P6, &_bmp280_calib.dig_P6);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P7, &_bmp280_calib.dig_P7);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P8, &_bmp280_calib.dig_P8);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P9, &_bmp280_calib.dig_P9);

    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_DIG_H1, &_bmp280_calib.dig_H1, 1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_H2, &_bmp280_calib.dig_H2);
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_DIG_H3, &_bmp280_calib.dig_H3, 1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_H4, &h4);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_H5, &h5);
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_DIG_H6, (uint8_t *)&_bmp280_calib.dig_H6, 1);
    _bmp280_calib.dig_H4 = ((h4 & 0x00ff) << 4) | ((h4 & 0x0f00) >> 8);
    _bmp280_calib.dig_H5 = (h5 >> 4);

    return NRF_SUCCESS;
}



uint32_t bmp280_init(void)
{
    uint32_t err_code;
    uint8_t chipid;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
    //---NRF_LOGDEBUG("Initialising BMP280"); //---NRF_LOGFLUSH();
	err_code = nrf_drv_bmp280_init();
    if(err_code != NRF_SUCCESS) return err_code;

    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_CHIPID, &chipid, 1);
    if(err_code != NRF_SUCCESS) return err_code;
    if (chipid != BMP280_CHIPID) {
        //---NRF_LOGDEBUG("Init error: BMP280 not found!"); //---NRF_LOGFLUSH();
    }
    return NRF_SUCCESS;
}



bool bmp280_has_new_data(void)
{
    uint32_t err_code;
    uint8_t status;

    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_STATUS, &status, 1);
    if(err_code != NRF_SUCCESS) return err_code;
    if (status & 0x08)
        return true;
    else
        return false;
}



uint32_t bmp280_read_ambient(bmp280_ambient_values_t * bmp280_ambient_values)
{
    uint32_t err_code;
    uint8_t env_values[8];
    uint8_t *temperature_values;
    uint8_t *pressure_values;
    int32_t var1, var2, t_fine;
    int64_t p;
    bool    bme280 = false;
/** BME280 only */
    uint8_t *humidity_values;

    bmp280_read_partid(&m_partid.chip_id);
    if (m_partid.chip_id == BME280_CHIPID) bme280 = true;


    // As recommended in datasheet, read all values in one go. Data started with Pressure
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_PRESSUREDATA, env_values, 8);
    if(err_code != NRF_SUCCESS) return err_code;
    pressure_values = (uint8_t *)env_values;
    temperature_values = (uint8_t *)(env_values+3*sizeof(uint8_t));
    if (bme280) {
        humidity_values = (uint8_t *)(env_values+6*sizeof(uint8_t));
    }
    int32_t adc_T = ((temperature_values[0] << 16) + (temperature_values[1] << 8) + (temperature_values[2] & 0xF0))/16;
    int32_t adc_P = (((pressure_values[0] << 16) + (pressure_values[1] << 8) + (pressure_values[2] & 0xF0))/16)>>4;

    var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
        ((int32_t)_bmp280_calib.dig_T2)) >> 11;
    var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
        ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
        ((int32_t)_bmp280_calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    bmp280_ambient_values->ambient_temperature_value = (t_fine * 5 + 128) >> 8;

#ifdef USE_FLOAT
    double fvar1, fvar2, fp;
    fvar1 = ((double)t_fine/2.0) - 64000.0;
    fvar2 = fvar1 * fvar1 * ((double)_bmp280_calib.dig_P6) / 32768.0;
    fvar2 = fvar2 + fvar1 * ((double)_bmp280_calib.dig_P5) * 2.0;
    fvar2 = (fvar2/4.0)+(((double)_bmp280_calib.dig_P4) * 65536.0);
    fvar1 = (((double)_bmp280_calib.dig_P3) * fvar1 * fvar1 / 524288.0 + ((double)_bmp280_calib.dig_P2) * fvar1) / 524288.0; fvar1 = (1.0 + fvar1 / 32768.0)*((double)_bmp280_calib.dig_P1);
    if (fvar1 == 0.0) {
        return 0; // avoid exception caused by division by zero
    }
    fp = 1048576.0 - (double)adc_P;
    fp = (fp - (fvar2 / 4096.0)) * 6250.0 / fvar1;
    fvar1 = ((double)_bmp280_calib.dig_P9) * fp * fp / 2147483648.0;
    fvar2 = fp * ((double)_bmp280_calib.dig_P8) / 32768.0;
    p = round(fp + (fvar1 + fvar2 + ((double)_bmp280_calib.dig_P7)) / 16.0); 
#else
    int64_t v1, v2;
    v1 = ((int64_t)t_fine) - 128000;
    v2 = v1 * v1 * (int64_t)_bmp280_calib.dig_P6;
    v2 = v2 + ((v1*(int64_t)_bmp280_calib.dig_P5)<<17);
    v2 = v2 + (((int64_t)_bmp280_calib.dig_P4)<<35);
    v1 = ((v1 * v1 * (int64_t)_bmp280_calib.dig_P3)>>8) +
        ((v1 * (int64_t)_bmp280_calib.dig_P2)<<12);
    v1 = (((((int64_t)1)<<47)+v1))*((int64_t)_bmp280_calib.dig_P1)>>33;

    if (v1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p<<31) - v2)*3125) / v1;
    v1 = (((int64_t)_bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
    v2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + v1 + v2) >> 8) + (((int64_t)_bmp280_calib.dig_P7)<<4);
    p = (uint32_t)(p/256); // Has to /2, because of filter??
    /*
    v1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
    v2 = (((v1>>2) * (v1>>2)) >> 11 ) * ((int32_t)_bmp280_calib.dig_P6);
    v2 = v2 + ((v1*((int32_t)_bmp280_calib.dig_P5))<<1);
    v2 = (v2>>2)+(((int32_t)_bmp280_calib.dig_P4)<<16);
    v1 = (((_bmp280_calib.dig_P3 * (((v1>>2) * (v1>>2)) >> 13 )) >> 3) + ((((int32_t)_bmp280_calib.dig_P2) * v1)>>1))>>18; v1 =((((32768+v1))*((int32_t)_bmp280_calib.dig_P1))>>15);
    if (v1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t)1048576)-adc_P)-(v2>>12)))*3125;
    if (p < 0x80000000) {
        p = (p << 1) / ((uint32_t)v1);
    } else {
        p = (p / (uint32_t)v1) * 2;
    }
    v1 = (((int32_t)_bmp280_calib.dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12; v2 = (((int32_t)(p>>2)) * ((int32_t)_bmp280_calib.dig_P8))>>13;
    p = (uint32_t)((int32_t)p + ((v1 + v2 + _bmp280_calib.dig_P7) >> 4));
    */
#endif

    bmp280_ambient_values->ambient_pressure_value = p;
    bmp280_ambient_values->ambient_humidity_value = 0;

    /** For BME280 only **/
    if (bme280) {
        int32_t adc_H = (humidity_values[0] << 8) + humidity_values[1];

/** floating point math **/
#ifdef USE_FLOAT

        double var_H;
        uint32_t uh=0;
          
        var_H = (((double)t_fine) - 76800.0);
        var_H = (adc_H - (((double)_bmp280_calib.dig_H4) * 64.0 + ((double)_bmp280_calib.dig_H5) / 16384.0 * var_H)) *
        (((double)_bmp280_calib.dig_H2) / 65536.0 * (1.0 + ((double)_bmp280_calib.dig_H6) / 67108864.0 * var_H *
        (1.0 + ((double)_bmp280_calib.dig_H3) / 67108864.0 * var_H)));
        var_H = var_H * (1.0 - ((double)_bmp280_calib.dig_H1) * var_H / 524288.0);
        if (var_H > 100.0)
            var_H = 100.0;
        else if (var_H < 0.0)
            var_H = 0.0;
        uh = round((var_H * 100));
#else

/** Integer math **/

        int32_t h=0;
        uint64_t uh=0;

        h = (t_fine - ((int32_t)76800));
        h = (((((adc_H << 14) - (((int32_t)_bmp280_calib.dig_H4) << 20) - (((int32_t)_bmp280_calib.dig_H5) * h)) +
        ((int32_t)16384)) >> 15) * (((((((h * ((int32_t)_bmp280_calib.dig_H6)) >> 10) * (((h *
        ((int32_t)_bmp280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
        ((int32_t)_bmp280_calib.dig_H2) + 8192) >> 14));
        h = (h - (((((h >> 15) * (h >> 15)) >> 7) *
                                  ((int32_t)_bmp280_calib.dig_H1)) >> 4));
        h = (h < 0 ? 0 : h);
        h = (h > 419430400 ? 419430400 : h);
        uh = (uint32_t)(h >> 12);
        uh = uh * 100 / 1024;
#endif

        bmp280_ambient_values->ambient_humidity_value = uh;
    }

    return NRF_SUCCESS;
}

/*
uint32_t bmp280_read_lux(uint32_t * lux)
{
    uint32_t err_code;
    bmp280_ambient_values_t m_ambient;
    uint16_t t;
    uint32_t l=0, ratio;

    err_code = bmp280_read_ambient(&m_ambient);
    if (err_code != NRF_SUCCESS) return err_code;
    t = m_ambient.ambient_ir_value + m_ambient.ambient_visible_value;
    if (t > 0) {
        ratio = (m_ambient.ambient_visible_value * 1000) / t;
        if (ratio < 450) {
            l = (m_ambient.ambient_ir_value * 17743 + m_ambient.ambient_visible_value * 11059);
        } else if (ratio < 640 && ratio >= 450) {
            l = (m_ambient.ambient_ir_value * 42785 - m_ambient.ambient_visible_value * 19548);
        } else if (ratio < 850 && ratio >= 640) {
            l = (m_ambient.ambient_ir_value * 5926 + m_ambient.ambient_visible_value * 1185);
        }
        *lux = l / 50;
    } else {
        *lux = 0;
    }
    return NRF_SUCCESS;
}
*/

uint32_t bmp280_read_partid(uint8_t * partid)
{
    uint32_t err_code;
    uint8_t raw_value=0;
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_CHIPID, &raw_value, 1);
    if(err_code != NRF_SUCCESS) return err_code;
    *partid = raw_value;
    return NRF_SUCCESS;
}



uint32_t bmp280_config_activate(void)
{
    uint32_t err_code;

    err_code = nrf_drv_bmp280_write_single_register(BMP280_REGISTER_CONTROL, 0x6F);
    if(err_code != NRF_SUCCESS) return err_code;
    return NRF_SUCCESS;
}

uint32_t bmp280_config_deactivate(void)
{
    uint32_t err_code;

    err_code = nrf_drv_bmp280_write_single_register(BMP280_REGISTER_CONTROL, 0x6C);
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}


/**
  @}
*/
