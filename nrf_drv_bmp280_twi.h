 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef NRF_DRV_BMP280__
#define NRF_DRV_BMP280__

// Define one of the followings in Makefile
//  BASIC_SENSOR
//  SENSORTAG_R40

#include <stdbool.h>
#include <stdint.h>

/* Pins to connect BMP280. Pinout is different for nRF51 DK and nRF52 DK
 * and therefore I have added a conditional statement defining different pins
 * for each board. This is only for my own convenience. 
 */
#if defined(BOARD_CUSTOM)
#if defined(SENSORTAG_R40)
// For SensorTag R40
#define BMP280_TWI_SCL_PIN 12
#define BMP280_TWI_SDA_PIN 11
#elif defined(BASIC_SENSOR)
// For Basic Sensor board
#define BMP280_TWI_SCL_PIN 3
#define BMP280_TWI_SDA_PIN 5
#endif
#else
#define BMP280_TWI_SCL_PIN 12
#define BMP280_TWI_SDA_PIN 11
#endif


#define BMP280_TWI_BUFFER_SIZE     	14 // 14 byte buffers will suffice to read all data in one transmission.
#define BMP280_TWI_TIMEOUT 			10000 
#define BMP280_ADDRESS     			0x76

#define BMA255_ADDRESS				0x18

/**@brief Function to initiate TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_bmp280_init(void);

/**@brief Function to start TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_bmp280_start(void);

/**@brief Function to stop TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_bmp280_stop(void);
	

/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_bmp280_write_single_register(uint8_t reg, uint8_t data);

#if defined(BASIC_SENSOR)
uint32_t nrf_drv_bma255_write_single_register(uint8_t reg, uint8_t data);
#endif



/**@brief Function for reading arbitrary register(s)
 *
 * @param[in]   reg             Register to read
 * @param[in]   p_data          Pointer to place to store value(s)
 * @param[in]   length          Number of registers to read
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_bmp280_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length);
uint32_t nrf_drv_bmp280_read_register16(uint8_t reg, uint16_t * value);
uint32_t nrf_drv_bmp280_read_register16S(uint8_t reg, int16_t * value);


#endif /* NRF_DRV_BMP280__ */

/**
  @}
*/

