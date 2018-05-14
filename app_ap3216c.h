 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef APP_AP3216C_H__
#define APP_AP3216C_H__


#include <stdbool.h>
#include <stdint.h>
#include "sdk_config.h"
//#include "ap3216cals_register_map.h"

#define ALS_MODE_STANDBY 0
#define ALS_MODE_ACTIVE 1

#define AP3216C_REG_FIXED_NV_SIZE_128       128
#define AP3216C_ALS_AUTO_GAIN_EN            0
#define AP3216C_ALS_AUTO_GAIN_USDELAY       250000  // mirco sec
#define AP3216C_ALS_AUTO_GAIN_MAX_DATA      65535
#define AP3216C_ALS_AUTO_GAIN_HTHRES        ((3*AP3216C_ALS_AUTO_GAIN_MAX_DATA)/4)
#define AP3216C_ALS_AUTO_GAIN_LTHRES        ((2*AP3216C_ALS_AUTO_GAIN_MAX_DATA)/16)

#define AP3216C_DD_HANDLE_ALIGN     3
#define AP3216C_LSC_MAGIC_NUM       50
#define AP3216C_LSC_DRIVER_ID       1
#define AP3216C_INVAILD_GPIO        0xffff

#define AP3216C_STA_PRX_INTR            0x02
#define AP3216C_STA_ALS_INTR            0x01
#define AP3216C_CLS_PS_INTR             ~AP3216C_STA_PRX_INTR
#define AP3216C_CLS_ALS_INTR            ~AP3216C_STA_ALS_INTR
#define AP3216Cx_ENABLE_MSK_PEN         0x02
#define AP3216Cx_ENABLE_MSK_AEN         0x01
#define AP3216Cx_ALSPRX_PWR_DOWN        0x00

#define AP3216C_RANGE_23360         0x00
#define AP3216C_RANGE_5840          0x01
#define AP3216C_RANGE_1460          0x02
#define AP3216C_RANGE_365           0x03

#define AP3216C_RANGE_23360_RATION      0.36    //  23360 / 65535
#define AP3216C_RANGE_5840_RATION       0.089   //  5480  / 65535
#define AP3216C_RANGE_1460_RATION       0.022   //  1460  / 65535
#define AP3216C_RANGE_365_RATION        0.0056  //  365   / 65535


//system registers
#define AP3216C_MODE_COMMAND     0x00
#define AP3216C_INT_STATUS       0x01
#define AP3216C_INT_CLEAR        0x01
#define AP3216C_INT_CTRL         0x02
#define AP3216C_WAITE_TIME       0x06
#define AP3216C_INT_CLEAR_MASK   0x7c
#define AP3216C_PX_ADC_LSB       0x0e
#define AP3216C_PX_ADC_LSB_MASK  0x0f
#define AP3216C_PX_ADC_MSB       0x0f
#define AP3216C_PX_ADC_MSB_SHIFT 4
#define AP3216C_PX_ADC_MSB_MASK  0x3f

//als registers
#define AP3216C_ALS_CONF        0x10
#define AP3216C_ALS_GAIN_MASK   0x30
#define AP3216C_ALS_GAIN_SHIFT  4
#define AP3216C_INT_ON_1_CONV   0x00

#define AP3216C_ALS_IRDATA_L    0x0a
#define AP3216C_ALS_IRDATA_H    0x0b
#define AP3216C_ALS_LDATA_L     0x0c
#define AP3216C_ALS_LDATA_H     0x0d
#define AP3216C_ALS_LTHL        0x1a
#define AP3216C_ALS_LTHH        0x1b
#define AP3216C_ALS_HTHL        0x1c
#define AP3216C_ALS_HTHH        0x1d

#define AP3216C_ALS_PERSIS      0x14

//ps registers
#define AP3216C_PX_LED_CTRL           0x10
#define AP3216C_PX_INT_FORM           0x02
#define AP3216C_PX_LED_WTIME          0x06
#define AP3216C_PX_LDATA              0x0E
#define AP3216C_PX_HDATA              0x0F
#define AP3216C_PX_DATA_MASK          0x03ff
#define AP3216C_PX_CONF               0x20
#define AP3216C_PX_GAIN_SHIFT         2
#define AP3216C_PX_GAIN_MASK          0x0c
#define AP3216C_PX_LED_CONTRL         0x21
#define AP3216C_PX_LED_PULSE_SHIFT    4
#define AP3216C_PX_LED_PULSE_MASK     0x30
#define AP3216C_PX_INT_MODE           0x22
#define AP3216C_PX_MEAN_TIME          0x23
#define AP3216C_PX_INTEG_TIME         0x25
#define AP3216C_PX_INTEG_SHIFT        4
#define AP3216C_PX_INTEG_MASK         0xf0
#define AP3216C_PX_PERSIS             0x26
#define AP3216C_PX_CAL_L              0x28
#define AP3216C_PX_CAL_H              0x29
#define AP3216C_PX_LTHL               0x2a
#define AP3216C_PX_LTHH               0x2b
#define AP3216C_PX_HTHL               0x2c
#define AP3216C_PX_HTHH               0x2d

/**
 * Controls the range and resolution of illuminance values
 */
typedef enum {
    AP3216C_GAIN_1X= 0,   ///< Illuminance range between [1, 64k] lux (default)
    AP3216C_GAIN_2X,      ///< Illuminance range between [0.5, 32k] lux
    AP3216C_GAIN_4X,      ///< Illuminance range between [0.25, 16k] lux
    AP3216C_GAIN_8X,      ///< Illuminance range between [0.125, 8k] lux
    AP3216C_GAIN_48X,     ///< Illuminance range between [0.02, 1.3k] lux
    AP3216C_GAIN_96X      ///< Illuminance range between [0.01, 600] lux
} ap3216cGain;

/**
 * Measurement time for each cycle
 */
typedef enum {
    AP3216C_TIME_100ms= 0,    ///< Default setting
    AP3216C_TIME_50ms,
    AP3216C_TIME_200ms,
    AP3216C_TIME_400ms,
    AP3216C_TIME_150ms,
    AP3216C_TIME_250ms,
    AP3216C_TIME_300ms,
    AP3216C_TIME_350ms
} ap3216cIntegrationTime;

/**
 * How frequently to update the illuminance data.
 */
typedef enum {
    AP3216C_RATE_50ms= 0,
    AP3216C_RATE_100ms,
    AP3216C_RATE_200ms,
    AP3216C_RATE_500ms,       ///< Default setting
    AP3216C_RATE_1000ms,
    AP3216C_RATE_2000ms
} ap3216cMeasurementRate;

/**@brief Simple typedef to hold ambient values */
typedef struct {
    uint16_t ambient_visible_value;
    uint16_t ambient_ir_value;
    uint16_t ambient_lux_value;
} ap3216c_ambient_values_t;

/**@brief Control register structure */
typedef struct {
    uint8_t als_mode    :1;
    uint8_t sw_reset    :1;
    uint8_t als_gain    :3;
    uint8_t reserved    :3;
} ap3216c_als_contr_t;

/**@brief Measurement rate register structure */
typedef struct {
    uint8_t als_measurement_repeat_rate :3;
    uint8_t als_integration_time        :3;
    uint8_t reserved                    :2;
} ap3216c_als_meas_rate_t;

/**@brief Part ID register structure */
typedef struct {
    uint8_t revision_id     :4;
    uint8_t part_number_id  :4;
} ap3216c_part_id_t;

/**@brief Status register structure */
typedef struct {
    uint8_t reserved        :2;
    uint8_t als_data_status :1;
    uint8_t reserved2       :1;
    uint8_t als_gain        :3;
    uint8_t als_data_valid  :1;
} ap3216c_als_status_t;


/**@brief Function for initiating LTR-329ALS library
 * 
 * Resets sensor signal paths.
 * Function resets the analog and digital signal paths of the light sensor,
 * and temperature sensors.
 * The reset will revert the signal path analog to digital converters and filters to their power up
 * configurations.
 *
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_init(void);

    

/**@brief Function for basic configuring of the LTR-329ALS
 * 
 * Register 80  Control Register. This register specifies 
 * the ALS operation mode and software reset.
 * 
 * Register 85  Configuration CONFIG. This register configures 
 * the LTR-329ALS for different measurement rates 
 *
 * @param[in]   config          Pointer to configuration structure
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_config(void);

/**@brief Function for reading LTR-329ALS part id data.
 *
 * @param[in]   partid_values   Pointer to variable to hold part id data
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_read_partid(ap3216c_part_id_t * partid);

/**@brief Function for reading LTR-329ALS ambient data.
 *
 * @param[in]   ambient_values  Pointer to variable to hold ambient data
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_read_ambient(ap3216c_ambient_values_t * ap3216c_ambient_values);

/**@brief Function for returning LTR-329ALS lux data.
 *
 * @param[in]   lux_value       Pointer to variable to hold lux data
 * @retval      uint32_t        Error code
 */
// uint32_t ap3216c_read_lux(uint32_t * lux);

/**@brief Function for activating LTR-329ALS .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_config_activate(void);

/**@brief Function for de-activating LTR-329ALS .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t ap3216c_config_deactivate(void);

/**@brief Function for detecting LTR-329ALS new data.
 *
 * @param[in]   
 * @retval      bool        True if new data is avaiable
 */
bool ap3216c_has_new_data(void);


#endif /* APP_AP3216C_H__ */

/**
  @}
*/

