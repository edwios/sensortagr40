 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef BLE_VEML6075_SERVICE_H__
#define BLE_VEML6075_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "app_veml6075.h"
#include "ble_common_items.h"
#include "app_mpu.h"

#define BLE_UUID_AMBIENTLIGHT_SERVICE_UUID         0x181A // Environmental sensor
#define BLE_UUID_IRRADIANCE_CHARACTERISTC_UUID     0x2A77 // Irradiance
//#define BLE_UUID_TEMPERATURE_CHARACTERISTC_UUID    0x2A6E // Temperature
#define BLE_UUID_UVA_CHARACTERISTC_UUID            0x815B // Just a random, but recognizable value
#define BLE_UUID_UVB_CHARACTERISTC_UUID            0x815C // Just a random, but recognizable value
#define BLE_UUID_VISIBLE_CHARACTERISTC_UUID        0x815D // Just a random, but recognizable value
#define BLE_UUID_IR_CHARACTERISTC_UUID             0x815E // Just a random, but recognizable value
#define BLE_UUID_UVI_CHARACTERISTC_UUID            0x2A76 // Just a random, but recognizable value
#define BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE     0x181A /**< Environmental Sensing service UUID. */

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of ble Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    uva_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    uvb_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    uvi_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    visible_char_handles;
    ble_gatts_char_handles_t    ir_char_handles;
    ble_gatts_char_handles_t    lux_char_handles;
}ble_veml6075_t;

/**@brief Function for handling BLE Stack events related to VEML6075 service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to veml6075 Service.
 *
 * @param[in]   p_veml6075       veml6075 structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_veml6075_on_ble_evt(ble_envsense_t * p_veml6075, ble_evt_t * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_veml6075       Pointer to ble veml6075 structure.
 */
void ble_veml6075_service_init(ble_envsense_t * p_veml6075);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_veml6075                 veml6075 structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void ble_veml6075_update(ble_envsense_t *p_veml6075, veml6075_ambient_values_t * veml6075_ambient_values);

#endif  /* _ BLE_VEML6075_SERVICE_H__ */
