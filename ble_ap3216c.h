 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef BLE_AP3216C_SERVICE_H__
#define BLE_AP3216C_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "app_ap3216c.h"
#include "ble_common_items.h"
#include "app_mpu.h"

#define BLE_UUID_AMBIENTLIGHT_SERVICE_UUID         0x181A // Environmental sensor
#define BLE_UUID_IRRADIANCE_CHARACTERISTC_UUID     0x2A77 // Irradiance
//#define BLE_UUID_TEMPERATURE_CHARACTERISTC_UUID    0x2A6E // Temperature
#define BLE_UUID_VISIBLE_CHARACTERISTC_UUID        0x815B // Just a random, but recognizable value
#define BLE_UUID_IR_CHARACTERISTC_UUID             0x815C // Just a random, but recognizable value
#define BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE     0x181A /**< Environmental Sensing service UUID. */

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of ble Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    visible_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    ir_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    lux_char_handles;
//    ble_gatts_char_handles_t    temp_char_handles;
}ble_ap3216c_t;

/**@brief Function for handling BLE Stack events related to AP3216C service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to ap3216c Service.
 *
 * @param[in]   p_ap3216c       ap3216c structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_ap3216c_on_ble_evt(ble_ap3216c_t * p_ap3216c, ble_evt_t * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_ap3216c       Pointer to ble ap3216c structure.
 */
void ble_ap3216c_service_init(ble_ap3216c_t * p_ap3216c);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_ap3216c                 ap3216c structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void ble_ap3216c_update(ble_ap3216c_t *p_ap3216c, ap3216c_ambient_values_t * ap3216c_ambient_values);
//void ble_ap3216c_temperature_update(ble_ap3216c_t *p_ap3216c, temp_value_t * temperature_value);

#endif  /* _ BLE_AP3216C_SERVICE_H__ */
