 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_twi.h"
#include "nrf_drv_veml6075_twi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


static const nrf_drv_twi_t m_twi_instance = NRF_DRV_TWI_INSTANCE(1);
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;

uint8_t twi_tx_buffer[VEML6075_TWI_BUFFER_SIZE];


static void nrf_drv_veml6075_twi_event_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
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
uint32_t nrf_drv_veml6075_init(void)
{
    uint32_t err_code;
    
    const nrf_drv_twi_config_t twi_veml6075_config = {
       .scl                = VEML6075_TWI_SCL_PIN,
       .sda                = VEML6075_TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init      = true
    };
    
    //---NRF_LOG_DEBUG("TWI init begin ..."); //---NRF_LOG_FLUSH();
    err_code = nrf_drv_twi_init(&m_twi_instance, &twi_veml6075_config, nrf_drv_veml6075_twi_event_handler, NULL);
    if(err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE)
	{
        NRF_LOG_DEBUG("TWI init error %d", err_code); //---NRF_LOG_FLUSH();
		return err_code;
	}    
    nrf_drv_twi_enable(&m_twi_instance);
	
	return NRF_SUCCESS;
}




uint32_t nrf_drv_veml6075_write_single_register(uint8_t reg, uint16_t data)
{
    uint32_t err_code;
    uint32_t timeout = VEML6075_TWI_TIMEOUT;

    uint8_t packet[3] = {reg, data};

    err_code = nrf_drv_twi_tx(&m_twi_instance, VEML6075_ADDRESS, packet, 3, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    twi_tx_done = false;

    return err_code;
}


uint32_t nrf_drv_veml6075_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = VEML6075_TWI_TIMEOUT;

    err_code = nrf_drv_twi_tx(&m_twi_instance, VEML6075_ADDRESS, &reg, 1, true);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;

    err_code = nrf_drv_twi_rx(&m_twi_instance, VEML6075_ADDRESS, p_data, length);
    if(err_code != NRF_SUCCESS) return err_code;

    timeout = VEML6075_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_rx_done = false;

    return err_code;
}




/**
  @}
*/
