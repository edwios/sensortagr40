 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef APP_VEML6075_H__
#define APP_VEML6075_H__


#include <stdbool.h>
#include <stdint.h>
#include "sdk_config.h"
//#include "veml6075als_register_map.h"

#define VEML6075_POWERON_PIN     (6)   // Pin 6 for Vcc
// Reading the application note on calculation of UV index, the "dummy" channel
// value is actually not a dummy value at all, but the dark current count.
// NAMES ARE IMPORTANT PEOPLE.

#define VEML6075_REG_CONF        (0x00) // Configuration register (options below)
#define VEML6075_REG_UVA         (0x07) // UVA register
#define VEML6075_REG_DARK        (0x08) // Dark current register (NOT DUMMY)
#define VEML6075_REG_UVB         (0x09) // UVB register
#define VEML6075_REG_UVCOMP1     (0x0A) // Visible compensation register
#define VEML6075_REG_UVCOMP2     (0x0B) // IR compensation register
#define VEML6075_REG_DEVID       (0x0C) // Device ID register

#define VEML6075_CONF_IT_50MS    (0x00) // Integration time = 50ms (default)
#define VEML6075_CONF_IT_100MS   (0x10) // Integration time = 100ms
#define VEML6075_CONF_IT_200MS   (0x20) // Integration time = 200ms
#define VEML6075_CONF_IT_400MS   (0x30) // Integration time = 400ms
#define VEML6075_CONF_IT_800MS   (0x40) // Integration time = 800ms
#define VEML6075_CONF_IT_MASK    (0x8F) // Mask off other config bits

#define VEML6075_CONF_HD_NORM    (0x00) // Normal dynamic seetting (default)
#define VEML6075_CONF_HD_HIGH    (0x08) // High dynamic seetting

#define VEML6075_CONF_TRIG       (0x04) // Trigger measurement, clears by itself

#define VEML6075_CONF_AF_OFF     (0x00) // Active force mode disabled (default)
#define VEML6075_CONF_AF_ON      (0x02) // Active force mode enabled (?)

#define VEML6075_CONF_SD_ON      (0x00) // Power up
#define VEML6075_CONF_SD_ON_MASK (0xFE)
#define VEML6075_CONF_SD_OFF     (0x01) // Power down

// To calculate the UV Index, a bunch of empirical/magical coefficients need to
// be applied to UVA and UVB readings to get a proper composite index value.
// Seems pretty hand wavey, though not nearly as annoying as the dark current
// not being subtracted out by default.

#define VEML6075_UVI_UVA_VIS_COEFF (3.33)
#define VEML6075_UVI_UVA_IR_COEFF  (2.5)
#define VEML6075_UVI_UVB_VIS_COEFF (3.66)
#define VEML6075_UVI_UVB_IR_COEFF  (2.75)

// Once the above offsets and crunching is done, there's a last weighting
// function to convert the ADC counts into the UV index values. This handles
// both the conversion into irradiance (W/m^2) and the skin erythema weighting
// by wavelength--UVB is way more dangerous than UVA, due to shorter
// wavelengths and thus more energy per photon. These values convert the compensated values 

#define VEML6075_UVI_UVA_RESPONSE (1.0 / 909.0)
#define VEML6075_UVI_UVB_RESPONSE (1.0 / 800.0)

enum veml6075_int_time {
  VEML6075_IT_50MS,
  VEML6075_IT_100MS,
  VEML6075_IT_200MS,
  VEML6075_IT_400MS,
  VEML6075_IT_800MS
};


/**@brief Simple typedef to hold ambient values */
typedef struct {
    uint16_t ambient_uva_value;
    uint16_t ambient_uvb_value;
    uint16_t ambient_uvi_value;
    uint16_t ambient_visible_value;
    uint16_t ambient_ir_value;
    uint16_t ambient_lux_value;
} veml6075_ambient_values_t;

/**@brief Part ID register structure */
typedef struct {
    uint8_t partid;
    uint8_t company_code    :2;
    uint8_t slave_addr      :2;
    uint8_t version_code    :4;
} veml6075_part_id_t;

/**@brief config register structure */
typedef struct {
    uint8_t reserved        :1;
    uint8_t uv_it           :3;
    uint8_t hd              :1;
    uint8_t uv_trig         :1;
    uint8_t uv_af           :1;
    uint8_t sd              :1;
} veml6075_conf_reg_t;


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
uint32_t veml6075_init(void);

void veml6075_poweron(void);
void veml6075_poweroff(void);

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
uint32_t veml6075_config(void);

/**@brief Function for reading LTR-329ALS part id data.
 *
 * @param[in]   partid_values   Pointer to variable to hold part id data
 * @retval      uint32_t        Error code
 */
uint32_t veml6075_read_partid(uint8_t * partid);

/**@brief Function for reading LTR-329ALS ambient data.
 *
 * @param[in]   ambient_values  Pointer to variable to hold ambient data
 * @retval      uint32_t        Error code
 */
uint32_t veml6075_read_ambient(veml6075_ambient_values_t * veml6075_ambient_values);

/**@brief Function for returning LTR-329ALS lux data.
 *
 * @param[in]   lux_value       Pointer to variable to hold lux data
 * @retval      uint32_t        Error code
 */
// uint32_t veml6075_read_lux(uint32_t * lux);

/**@brief Function for activating LTR-329ALS .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t veml6075_config_activate(void);

/**@brief Function for de-activating LTR-329ALS .
 *
 * @param[in]   
 * @retval      uint32_t        Error code
 */
uint32_t veml6075_config_deactivate(void);

/**@brief Function for detecting LTR-329ALS new data.
 *
 * @param[in]   
 * @retval      bool        True if new data is avaiable
 */
bool veml6075_has_new_data(void);


#endif /* APP_VEML6075_H__ */

/**
  @}
*/

