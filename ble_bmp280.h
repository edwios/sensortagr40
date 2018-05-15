 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef BLE_BMP280_SERVICE_H__
#define BLE_BMP280_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "app_bmp280.h"
#include "ble_common_items.h"
#include "app_mpu.h"

#define BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE      0x181A /**< Environmental Sensing service UUID. */
#define BLE_UUID_ALTITUDE_CHARACTERISTC_UUID        0x2AB3 // Altitude
#define BLE_UUID_TEMPERATURE_CHARACTERISTC_UUID     0x2A6E // Temperature
#define BLE_UUID_PRESSURE_CHARACTERISTC_UUID        0x2A6D // Pressure
#define BLE_UUID_HUMIDITY_CHARACTERISTC_UUID        0x2A6F // Humidity

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of ble Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    temperature_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    humidity_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    pressure_char_handles;
    ble_gatts_char_handles_t    altitude_char_handles;
}ble_bmp280_t;

/**@brief Function for handling BLE Stack events related to BMP280 service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to bmp280 Service.
 *
 * @param[in]   p_bmp280       bmp280 structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_bmp280_on_ble_evt(ble_bmp280_t * p_bmp280, ble_evt_t * p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_bmp280       Pointer to ble bmp280 structure.
 */
void ble_bmp280_service_init(ble_bmp280_t * p_bmp280);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_bmp280                 bmp280 structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void ble_bmp280_update(ble_bmp280_t *p_bmp280, bmp280_ambient_values_t * bmp280_ambient_values);

#endif  /* _ BLE_BMP280_SERVICE_H__ */
