 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_ap3216c.h"
#include "nrf_gpio.h"
#include "nrf_drv_ap3216c_twi.h"
#include "nrf_error.h"
#include "nrf_delay.h"
#include "sdk_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


uint32_t ap3216c_config(void)
{
    uint32_t err_code;
    uint8_t raw_value = 0;
    uint8_t control_config = 0;

    NRF_LOG_DEBUG("Configurating AP3216CALS"); NRF_LOG_FLUSH();
    control_config |= (AP3216C_RANGE_23360 << 4);
    control_config |= AP3216C_INT_ON_1_CONV;
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_ALS_CONF, control_config);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Configurating AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    // Set Low Threshold to 0xFFFF to generate interrupt on every ALS read
    raw_value = 0xFF;
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_ALS_LTHL, raw_value);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Configurating Low Threshold to AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_ALS_LTHH, raw_value);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Configurating Low Threshold to AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    return NRF_SUCCESS;
}



uint32_t ap3216c_init(void)
{
    uint32_t err_code;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
    NRF_LOG_DEBUG("Initialising AP3216CALS"); NRF_LOG_FLUSH();
	err_code = nrf_drv_ap3216c_init();
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Initialising AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    uint8_t reset_value = 0; // Resets sensor signal paths.
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_MODE_COMMAND, reset_value);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Resetting AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }
    nrf_delay_us(10000); // must not send command within 10ms
    return NRF_SUCCESS;
}



bool ap3216c_has_new_data(void)
{
    uint32_t err_code;
    uint8_t raw_value;
    err_code = nrf_drv_ap3216c_read_registers(AP3216C_INT_STATUS, &raw_value, 1);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR reading Status from AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }
    NRF_LOG_DEBUG("AP3216CALS Status %02x", raw_value); NRF_LOG_FLUSH();
    if (raw_value & 0x01) {
        return true;
    }
    return false;
}



uint32_t ap3216c_read_ambient(ap3216c_ambient_values_t * ap3216c_ambient_values)
{
    uint32_t err_code;
    uint8_t visible_values[2];
    uint8_t ir_values[2];

    err_code = nrf_drv_ap3216c_read_registers(AP3216C_ALS_LDATA_L, visible_values, 2);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR reading ALS data from AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    uint16_t visible = (visible_values[1] << 8) + visible_values[0];

    err_code = nrf_drv_ap3216c_read_registers(AP3216C_ALS_IRDATA_L, ir_values, 2);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR readin IR data from AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    uint16_t ir = (ir_values[1] << 8) + (ir_values[0] & 0x7F);

    ap3216c_ambient_values->ambient_visible_value = visible;
    ap3216c_ambient_values->ambient_ir_value = ir;
    // Todo: cal lux with multiple range
    ap3216c_ambient_values->ambient_lux_value = (uint16_t)((float)visible * AP3216C_RANGE_23360_RATION);

    return NRF_SUCCESS;
}

/*
uint32_t ap3216c_read_lux(uint32_t * lux)
{
    uint32_t err_code;
    ap3216c_ambient_values_t m_ambient;
    uint16_t t;
    uint32_t l=0, ratio;

    err_code = ap3216c_read_ambient(&m_ambient);
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

uint32_t ap3216c_read_partid(ap3216c_part_id_t * partid)
{
    return NRF_SUCCESS;
}



uint32_t ap3216c_config_activate(void)
{
    uint32_t err_code;
    uint8_t raw_value = 0;

    NRF_LOG_DEBUG("Activating AP3216CALS"); NRF_LOG_FLUSH();
    raw_value |= ALS_MODE_ACTIVE;
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_MODE_COMMAND, raw_value);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Activating AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t ap3216c_config_deactivate(void)
{
    uint32_t err_code;
    uint8_t raw_value=0;

    NRF_LOG_DEBUG("De-activating AP3216CALS"); NRF_LOG_FLUSH();
    raw_value |= AP3216Cx_ALSPRX_PWR_DOWN;
    err_code = nrf_drv_ap3216c_write_single_register(AP3216C_MODE_COMMAND, raw_value);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR De-activating AP3216C"); NRF_LOG_FLUSH();
        return err_code;
    }

    return NRF_SUCCESS;
}


/**
  @}
*/
