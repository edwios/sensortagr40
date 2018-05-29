 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef NRF_DRV_VEML6075__
#define NRF_DRV_VEML6075__


#include <stdbool.h>
#include <stdint.h>

/* Pins to connect VEML6075. Pinout is different for nRF51 DK and nRF52 DK
 * and therefore I have added a conditional statement defining different pins
 * for each board. This is only for my own convenience. 
 */

#if defined(BOARD_CUSTOM)
#define VEML6075_TWI_SCL_PIN 5
#define VEML6075_TWI_SDA_PIN 4
#else
#define VEML6075_TWI_SCL_PIN 5
#define VEML6075_TWI_SDA_PIN 4
#endif


#define VEML6075_TWI_BUFFER_SIZE     	14 // 14 byte buffers will suffice to read all data in one transmission.
#define VEML6075_TWI_TIMEOUT 			10000 
#define VEML6075_ADDRESS     			0x10


/**@brief Function to initiate TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_veml6075_init(void);
	


/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_veml6075_write_single_register(uint8_t reg, uint16_t data);



/**@brief Function for reading arbitrary register(s)
 *
 * @param[in]   reg             Register to read
 * @param[in]   p_data          Pointer to place to store value(s)
 * @param[in]   length          Number of registers to read
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_veml6075_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length);
    

#endif /* NRF_DRV_VEML6075__ */

/**
  @}
*/

