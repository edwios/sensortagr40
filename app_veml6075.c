 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "app_veml6075.h"
#include "nrf_gpio.h"
#include "nrf_drv_veml6075_twi.h"
#include "nrf_error.h"
#include "nrf_delay.h"
#include "sdk_config.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

uint16_t veml6075_conf_reg = 0;

void veml6075_poweron()
{
    //---NRF_LOG_DEBUG("Power on VEML6075ALS"); //---NRF_LOG_FLUSH();
    nrf_gpio_cfg_output(VEML6075_POWERON_PIN);
    nrf_gpio_pin_set(VEML6075_POWERON_PIN);
}

void veml6075_poweroff()
{
    nrf_gpio_cfg_output(VEML6075_POWERON_PIN);
    nrf_gpio_pin_clear(VEML6075_POWERON_PIN);
}

uint32_t veml6075_config(void)
{
    uint32_t err_code;

    //---NRF_LOG_DEBUG("Configurating VEML6075ALS"); //---NRF_LOG_FLUSH();
    veml6075_conf_reg |= VEML6075_CONF_IT_400MS;
    err_code = nrf_drv_veml6075_write_single_register(VEML6075_REG_CONF, veml6075_conf_reg);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR Configurating VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    return NRF_SUCCESS;
}



uint32_t veml6075_init(void)
{
    uint32_t err_code;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
    //---NRF_LOG_DEBUG("Initialising VEML6075ALS"); //---NRF_LOG_FLUSH();
	err_code = nrf_drv_veml6075_init();
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Initialising VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }

    veml6075_conf_reg |= VEML6075_CONF_SD_ON;
    err_code = nrf_drv_veml6075_write_single_register(VEML6075_REG_CONF, veml6075_conf_reg);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Resetting VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    return NRF_SUCCESS;
}



bool veml6075_has_new_data(void)
{
    return true;
}



uint32_t veml6075_read_ambient(veml6075_ambient_values_t * veml6075_ambient_values)
{
    uint32_t err_code;
    uint8_t _uva[2],_uvb[2],_uv_comp1[2],_uv_comp2[2];
    uint8_t _dark[2];
    uint16_t uva, uvb, uv_comp1, uv_comp2, dark, uvi;

    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_UVA, _uva, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    uva = (uint16_t)((_uva[1] << 8) + _uva[0]);
    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_UVB, _uvb, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    uvb = (uint16_t)((_uvb[1] << 8) + _uvb[0]);
    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_UVCOMP1, _uv_comp1, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    uv_comp1 = (uint16_t)((_uv_comp1[1] << 8) + _uv_comp1[0]);
    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_UVCOMP2, _uv_comp2, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    uv_comp2 = (uint16_t)((_uv_comp2[1] << 8) + _uv_comp2[0]);
    /** Dark current for compensation, not mentioned in VEML6075 datasheet!! **/
    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_DARK, _dark, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    dark = (uint16_t)((_dark[1] << 8) + _dark[0]);

    NRF_LOG_DEBUG(">> UVA %d, UVB %d, C1 %d, C2 %d, DK %d", uva, uvb, uv_comp1, uv_comp2, dark);
    NRF_LOG_FLUSH();

    float uvacal = (uva - dark) - VEML6075_UVI_UVA_VIS_COEFF * (uv_comp1 - dark) 
                      - VEML6075_UVI_UVA_IR_COEFF * (uv_comp2 - dark);
    float uvbcal = (uvb - dark) - VEML6075_UVI_UVB_VIS_COEFF * (uv_comp1 - dark) 
                      - VEML6075_UVI_UVB_IR_COEFF * (uv_comp2 - dark);

    float uvia = uvacal * VEML6075_UVI_UVA_RESPONSE;
    float uvib = uvbcal * VEML6075_UVI_UVB_RESPONSE;
    uvi = round((uvia + uvib) * 100 / 2);

    veml6075_ambient_values->ambient_uva_value = round(uvacal);
    veml6075_ambient_values->ambient_uvb_value = round(uvbcal);
    veml6075_ambient_values->ambient_uvi_value = uvi;
    veml6075_ambient_values->ambient_visible_value = round(uv_comp1 - dark);
    veml6075_ambient_values->ambient_ir_value = round(uv_comp2 - dark);

    // Todo: cal lux with multiple range
    veml6075_ambient_values->ambient_lux_value = (uint16_t)(VEML6075_UVI_UVA_VIS_COEFF * (uv_comp1 - dark));

    return NRF_SUCCESS;
}

/*
uint32_t veml6075_read_lux(uint32_t * lux)
{
    uint32_t err_code;
    veml6075_ambient_values_t m_ambient;
    uint16_t t;
    uint32_t l=0, ratio;

    err_code = veml6075_read_ambient(&m_ambient);
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

uint32_t veml6075_read_partid(uint8_t * partid)
{
    uint32_t err_code;
    uint8_t raw_value[2];

    err_code = nrf_drv_veml6075_read_registers(VEML6075_REG_DEVID, raw_value, 2);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR reading ALS data from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    *partid = raw_value[0];
    return NRF_SUCCESS;
}



uint32_t veml6075_config_activate(void)
{
    uint32_t err_code;

    veml6075_conf_reg &= VEML6075_CONF_SD_ON_MASK;
    veml6075_conf_reg |= VEML6075_CONF_SD_ON;
    //---NRF_LOG_DEBUG("Activating VEML6075ALS %d", veml6075_conf_reg); //---NRF_LOG_FLUSH();
    err_code = nrf_drv_veml6075_write_single_register(VEML6075_REG_CONF, veml6075_conf_reg);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR Activating VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    uint8_t partid;
    err_code = veml6075_read_partid(&partid);
    if(err_code != NRF_SUCCESS) {
        //---NRF_LOG_DEBUG("ERROR cannot read from VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }
    //nrf_delay_us(100000); // must not send command within 100ms
    return NRF_SUCCESS;
}

uint32_t veml6075_config_deactivate(void)
{
    uint32_t err_code;

    //---NRF_LOG_DEBUG("De-activating VEML6075ALS"); //---NRF_LOG_FLUSH();
    veml6075_conf_reg &= VEML6075_CONF_SD_ON_MASK;
    veml6075_conf_reg |= VEML6075_CONF_SD_OFF;
    err_code = nrf_drv_veml6075_write_single_register(VEML6075_REG_CONF, veml6075_conf_reg);
    if(err_code != NRF_SUCCESS) {
        NRF_LOG_DEBUG("ERROR De-activating VEML6075"); //---NRF_LOG_FLUSH();
        return err_code;
    }

    return NRF_SUCCESS;
}


/**
  @}
*/
