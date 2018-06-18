 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef APP_BMP280_H__
#define APP_BMP280_H__


#include <stdbool.h>
#include <stdint.h>
#include "sdk_config.h"

#define BMP280_REG_CONTROL 0xF4
#define BMP280_REG_RESULT_PRESSURE 0xF7         // 0xF7(msb) , 0xF8(lsb) , 0xF9(xlsb) : stores the pressure data.
#define BMP280_REG_RESULT_TEMPRERATURE 0xFA     // 0xFA(msb) , 0xFB(lsb) , 0xFC(xlsb) : stores the temperature data.

#define BMP280_COMMAND_TEMPERATURE 0x2E
#define BMP280_COMMAND_PRESSURE0 0x25            
#define BMP280_COMMAND_PRESSURE1 0x29           
#define BMP280_COMMAND_PRESSURE2 0x2D    
#define BMP280_COMMAND_PRESSURE3 0x31    
#define BMP280_COMMAND_PRESSURE4 0x5D 

#define BMP280_PARAM_HUM_OVS1    0x01
#define BMP280_PARAM_HUM_OVS2    0x02
#define BMP280_PARAM_HUM_OVS4    0x03
#define BMP280_PARAM_HUM_OVS8    0x04

#define BME280_CHIPID            0x60
#define BMP280_CHIPID            0x58

#define BMA255_PMU_LPW           0x11
#define BMA255_REGISTER_CHIPID   0x00
#define BMA255_CHIPID            0xFA

/**@BMP280 Registers */
enum
{
    BMP280_REGISTER_DIG_T1              = 0x88,
    BMP280_REGISTER_DIG_T2              = 0x8A,
    BMP280_REGISTER_DIG_T3              = 0x8C,

    BMP280_REGISTER_DIG_P1              = 0x8E,
    BMP280_REGISTER_DIG_P2              = 0x90,
    BMP280_REGISTER_DIG_P3              = 0x92,
    BMP280_REGISTER_DIG_P4              = 0x94,
    BMP280_REGISTER_DIG_P5              = 0x96,
    BMP280_REGISTER_DIG_P6              = 0x98,
    BMP280_REGISTER_DIG_P7              = 0x9A,
    BMP280_REGISTER_DIG_P8              = 0x9C,
    BMP280_REGISTER_DIG_P9              = 0x9E,

    BMP280_REGISTER_DIG_H1              = 0xA1,
    BMP280_REGISTER_DIG_H2              = 0xE1,
    BMP280_REGISTER_DIG_H3              = 0xE3,
    BMP280_REGISTER_DIG_H4              = 0xE4,
    BMP280_REGISTER_DIG_H5              = 0xE5,
    BMP280_REGISTER_DIG_H6              = 0xE7,

    BMP280_REGISTER_CHIPID             = 0xD0,
    BMP280_REGISTER_VERSION            = 0xD1,
    BMP280_REGISTER_SOFTRESET          = 0xE0,

    BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

    BMP280_REGISTER_CTRL_HUM           = 0xF2,
    BMP280_REGISTER_STATUS             = 0XF3,
    BMP280_REGISTER_CONTROL            = 0xF4,
    BMP280_REGISTER_CONFIG             = 0xF5,
    BMP280_REGISTER_PRESSUREDATA       = 0xF7,
    BMP280_REGISTER_TEMPDATA           = 0xFA,
    BMP280_REGISTER_HUMIDATA           = 0xFD,
};

/**@BMP280 Calibration data */
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bmp280_calib_data;


/**@brief Simple typedef to hold ambient values */
typedef struct {
    uint16_t ambient_temperature_value;
    uint16_t ambient_humidity_value;
    uint16_t ambient_pressure_value;
} bmp280_ambient_values_t;

/**@brief Part ID register structure */
typedef struct {
    uint8_t chip_id;
} bmp280_part_id_t;

bmp280_calib_data _bmp280_calib;
//bmp280_ambient_values_t bmp280_ambient_values;


/**@brief Function for initiating BMP280 library
 * 
 * Resets sensor signal paths.
 * Function resets the analog and digital signal paths of the light sensor,
 * and temperature sensors.
 * The reset will revert the signal path analog to digital converters and filters to their power up
 * configurations.
 *
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_init(void);

    

/**@brief Function for basic configuring of the BMP280
 * 
 * Register 80  Control Register. This register specifies 
 * the ALS operation mode and software reset.
 * 
 * Register 85  Configuration CONFIG. This register configures 
 * the BMP280 for different measurement rates 
 *
 * @param[in]   config          Pointer to configuration structure
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_config(void);

#if defined(BASIC_SENSOR)
uint32_t bma255_read_partid(uint8_t * partid);
uint32_t bma255_config_deactivate(void);
#endif

/**@brief Function for reading BMP280 part id data.
 *
 * @param[in]   partid_values   Pointer to variable to hold part id data
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_read_partid(uint8_t * partid);

/**@brief Function for reading BMP280 ambient data.
 *
 * @param[in]   ambient_values  Pointer to variable to hold ambient data
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_read_ambient(bmp280_ambient_values_t * bmp280_ambient_values);

/**@brief Function for returning BMP280 lux data.
 *
 * @param[in]   lux_value       Pointer to variable to hold lux data
 * @retval      uint32_t        Error code
 */
// uint32_t bmp280_read_lux(uint32_t * lux);

/**@brief Function for activating BMP280 .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_config_activate(void);

/**@brief Function for de-activating BMP280 .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t bmp280_config_deactivate(void);

/**@brief Function for detecting BMP280 new data.
 *
 * @param[in]   
 * @retval      bool        True if new data is avaiable
 */
bool bmp280_has_new_data(void);


#endif /* APP_BMP280_H__ */

/**
  @}
*/

