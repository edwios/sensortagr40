 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_twi.h"
#include "nrf_drv_bmp280_twi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static const nrf_drv_twi_t m_twi_instance = NRF_DRV_TWI_INSTANCE(0);
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;

uint8_t twi_tx_buffer[BMP280_TWI_BUFFER_SIZE];


static void nrf_drv_bmp280_twi_event_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch(p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            switch(p_event->xfer_desc.type)
            {
                case NRF_DRV_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
    }
}



/**
 * @brief TWI initialization.
 * Just the usual way. Nothing special here
 */
uint32_t nrf_drv_bmp280_init(void)
{
    uint32_t err_code;

    err_code = nrf_drv_bmp280_stop();
    if(err_code != NRF_SUCCESS) return err_code;
    err_code = nrf_drv_bmp280_start();
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}

uint32_t nrf_drv_bmp280_start(void)
{
    uint32_t err_code;
    
    const nrf_drv_twi_config_t twi_bmp280_config = {
       .scl                = BMP280_TWI_SCL_PIN,
       .sda                = BMP280_TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init      = true
    };
    
    //---NRF_LOG_DEBUG("TWI init begin ..."); //---NRF_LOG_FLUSH();
    err_code = nrf_drv_twi_init(&m_twi_instance, &twi_bmp280_config, nrf_drv_bmp280_twi_event_handler, NULL);
    if(err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
	{
        //---NRF_LOG_DEBUG("TWI init error %d", err_code); //---NRF_LOG_FLUSH();
		return err_code;
	}    
    nrf_drv_twi_enable(&m_twi_instance);
	
	return NRF_SUCCESS;
}

uint32_t nrf_drv_bmp280_stop(void)
{
    nrf_drv_twi_uninit(&m_twi_instance);
    *(volatile uint32_t *)0x40003FFC = 0;
    *(volatile uint32_t *)0x40003FFC;
    *(volatile uint32_t *)0x40003FFC = 1;

    return NRF_SUCCESS;

}

#if defined(BASIC_SENSOR)
uint32_t nrf_drv_bma255_write_single_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = BMP280_TWI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    err_code = nrf_drv_twi_tx(&m_twi_instance, BMA255_ADDRESS, packet, 2, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    twi_tx_done = false;

    return err_code;
}
#endif

uint32_t nrf_drv_bmp280_write_single_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = BMP280_TWI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    err_code = nrf_drv_twi_tx(&m_twi_instance, BMP280_ADDRESS, packet, 2, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    twi_tx_done = false;

    return err_code;
}


uint32_t nrf_drv_bmp280_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = BMP280_TWI_TIMEOUT;

    err_code = nrf_drv_twi_tx(&m_twi_instance, BMP280_ADDRESS, &reg, 1, true);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;

    err_code = nrf_drv_twi_rx(&m_twi_instance, BMP280_ADDRESS, p_data, length);
    if(err_code != NRF_SUCCESS) return err_code;

    timeout = BMP280_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_rx_done = false;

    return err_code;
}

uint32_t nrf_drv_bmp280_read_register16(uint8_t reg, uint16_t * value)
{
    uint32_t err_code;
    uint16_t udata;

    err_code = nrf_drv_bmp280_read_registers(reg, (uint8_t *)&udata, 2);
    if(err_code != NRF_SUCCESS) return err_code;
    *value = udata;
    return NRF_SUCCESS;
}

uint32_t nrf_drv_bmp280_read_register16S(uint8_t reg, int16_t * value)
{
    uint32_t err_code;
    uint16_t udata;

    err_code = nrf_drv_bmp280_read_registers(reg, (uint8_t *)&udata, 2);
    if(err_code != NRF_SUCCESS) return err_code;
    *value = (int16_t)udata;

    return NRF_SUCCESS;
}

/**
  @}
*/
