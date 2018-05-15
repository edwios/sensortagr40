 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_bmp280.h"
#include "nrf_gpio.h"
#include "nrf_drv_bmp280_twi.h"
#include "nrf_error.h"
#include "sdk_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


uint32_t bmp280_config(void)
{
    uint32_t err_code;

    NRF_LOG_DEBUG("Configurating BMP280"); NRF_LOG_FLUSH();
    err_code = nrf_drv_bmp280_write_single_register(BMP280_REGISTER_CONTROL, 0x3F);
    if(err_code != NRF_SUCCESS) return err_code;

    err_code = nrf_drv_bmp280_read_register16(BMP280_REGISTER_DIG_T1, &_bmp280_calib.dig_T1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_T2, &_bmp280_calib.dig_T2);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_T3, &_bmp280_calib.dig_T3);

    err_code = nrf_drv_bmp280_read_register16(BMP280_REGISTER_DIG_P1, &_bmp280_calib.dig_P1);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P2, &_bmp280_calib.dig_P2);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P3, &_bmp280_calib.dig_P3);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P4, &_bmp280_calib.dig_P4);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P5, &_bmp280_calib.dig_P5);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P6, &_bmp280_calib.dig_P6);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P7, &_bmp280_calib.dig_P7);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P8, &_bmp280_calib.dig_P8);
    err_code = nrf_drv_bmp280_read_register16S(BMP280_REGISTER_DIG_P9, &_bmp280_calib.dig_P9);

    return NRF_SUCCESS;
}



uint32_t bmp280_init(void)
{
    uint32_t err_code;
    uint8_t chipid;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
    NRF_LOG_DEBUG("Initialising BMP280"); NRF_LOG_FLUSH();
	err_code = nrf_drv_bmp280_init();
    if(err_code != NRF_SUCCESS) return err_code;

    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_CHIPID, &chipid, 1);
    if(err_code != NRF_SUCCESS) return err_code;
    if (chipid != BMP280_CHIPID) {
        NRF_LOG_DEBUG("Init error: BMP280 not found!"); NRF_LOG_FLUSH();
    }
    return NRF_SUCCESS;
}



bool bmp280_has_new_data(void)
{
    return true;
}



uint32_t bmp280_read_ambient(bmp280_ambient_values_t * bmp280_ambient_values)
{
    uint32_t err_code;
    uint8_t temperature_values[3];
    uint8_t humidity_values[3];
    uint8_t pressure_values[3];
    int32_t var1, var2, t_fine, h;
    int64_t v1, v2, p;


    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_TEMPDATA, temperature_values, 3);
    if(err_code != NRF_SUCCESS) return err_code;

    int32_t adc_T = (temperature_values[2] << 16) + (temperature_values[1] << 8) + temperature_values[0];
    var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
        ((int32_t)_bmp280_calib.dig_T2)) >> 11;
    var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
        ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
        ((int32_t)_bmp280_calib.dig_T3)) >> 14;
    t_fine = var1 + var2;
    bmp280_ambient_values->ambient_temperature_value = (t_fine * 5 + 128) >> 8;

    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_PRESSUREDATA, pressure_values, 3);
    if(err_code != NRF_SUCCESS) return err_code;

    int32_t adc_P = (pressure_values[2] << 16) + (pressure_values[1] << 8) + pressure_values[0];
    adc_P >>= 4;
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
    bmp280_ambient_values->ambient_pressure_value = p/256;

    humidity_values[2] = 0; humidity_values[1] = 0; humidity_values[0]=0;
    h = (humidity_values[2] << 16) + (humidity_values[1] << 8) + humidity_values[0];
    bmp280_ambient_values->ambient_humidity_value = h;

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

uint32_t bmp280_read_partid(bmp280_part_id_t * partid)
{
    uint32_t err_code;
    uint8_t raw_values;
    err_code = nrf_drv_bmp280_read_registers(BMP280_REGISTER_CHIPID, &raw_values, 1);
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}



uint32_t bmp280_config_activate(void)
{

    return NRF_SUCCESS;
}

uint32_t bmp280_config_deactivate(void)
{

    return NRF_SUCCESS;
}


/**
  @}
*/
