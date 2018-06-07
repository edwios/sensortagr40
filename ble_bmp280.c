 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "ble_bmp280.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "app_bmp280.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


void ble_bmp280_on_ble_evt(ble_envsense_t * p_bmp280, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            p_bmp280->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            p_bmp280->conn_handle = BLE_CONN_HANDLE_INVALID;
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_bmp280        bmp280 structure.
 *
 */
static uint32_t ble_char_temperature_add(ble_envsense_t * p_bmp280)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    ble_uuid_t          char_uuid;   
    BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_TEMPERATURE_CHARACTERISTC_UUID);
    
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    ble_gatts_attr_md_t cccd_md;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
        
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;    
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.max_len     = sizeof(int16_t);
    attr_char_value.init_len    = sizeof(int16_t);
    uint8_t value[2]            = {0};
    attr_char_value.p_value     = value;

//    //---NRF_LOG_DEBUG("ble_char_accel_add: add gatt char"); //---NRF_LOG_FLUSH();
    err_code = sd_ble_gatts_characteristic_add(p_bmp280->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_bmp280->temperature_char_handles);
    APP_ERROR_CHECK(err_code);   

    return NRF_SUCCESS;
}

static uint32_t ble_char_humidity_add(ble_envsense_t * p_bmp280)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    ble_uuid_t          char_uuid;   
    BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_HUMIDITY_CHARACTERISTC_UUID);
    
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    ble_gatts_attr_md_t cccd_md;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
        
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;    
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.max_len     = sizeof(int16_t);
    attr_char_value.init_len    = sizeof(int16_t);
    uint8_t value[2]            = {0};
    attr_char_value.p_value     = value;

//    //---NRF_LOG_DEBUG("ble_char_accel_add: add gatt char"); //---NRF_LOG_FLUSH();
    err_code = sd_ble_gatts_characteristic_add(p_bmp280->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_bmp280->humidity_char_handles);
    APP_ERROR_CHECK(err_code);   

    return NRF_SUCCESS;
}

static uint32_t ble_char_pressure_add(ble_envsense_t * p_bmp280)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    ble_uuid_t          char_uuid;   
    BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_PRESSURE_CHARACTERISTC_UUID);
    
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    ble_gatts_attr_md_t cccd_md;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
        
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;    
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.max_len     = sizeof(int16_t);
    attr_char_value.init_len    = sizeof(int16_t);
    uint8_t value[2]            = {0};
    attr_char_value.p_value     = value;

//    //---NRF_LOG_DEBUG("ble_char_accel_add: add gatt char"); //---NRF_LOG_FLUSH();
    err_code = sd_ble_gatts_characteristic_add(p_bmp280->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_bmp280->pressure_char_handles);
    APP_ERROR_CHECK(err_code);   

    return NRF_SUCCESS;
}

#if defined(REPORT_ALTITUDE)
static uint32_t ble_char_altitide_add(ble_envsense_t * p_bmp280)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    ble_uuid_t          char_uuid;   
    BLE_UUID_BLE_ASSIGN(char_uuid, BLE_UUID_ALTITUDE_CHARACTERISTC_UUID);
    
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    ble_gatts_attr_md_t cccd_md;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
        
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;    
    
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.max_len     = sizeof(int16_t);
    attr_char_value.init_len    = sizeof(int16_t);
    uint8_t value[2]            = {0};
    attr_char_value.p_value     = value;

//    //---NRF_LOG_DEBUG("ble_char_accel_add: add gatt char"); //---NRF_LOG_FLUSH();
    err_code = sd_ble_gatts_characteristic_add(p_bmp280->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_bmp280->altitude_char_handles);
    APP_ERROR_CHECK(err_code);   

    return NRF_SUCCESS;
}
#endif

/**@brief Function for initiating our new service.
 *
 * @param[in]   p_bmp280        Our Service structure.
 *
 */
void ble_bmp280_service_init(ble_envsense_t * p_bmp280)
{    
    ble_char_temperature_add(p_bmp280);
    ble_char_humidity_add(p_bmp280);
    ble_char_pressure_add(p_bmp280);
#if defined(REPORT_ALTITUDE)
    ble_char_altitide_add(p_bmp280);
#endif
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void ble_bmp280_update(ble_envsense_t *p_bmp280, bmp280_ambient_values_t * bmp280_ambient_values)
{
    // Send value if connected and notifying
    if (p_bmp280->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = sizeof(uint16_t);
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        uint16_t temperature_value = bmp280_ambient_values->ambient_temperature_value;
        uint16_t humidity_value = bmp280_ambient_values->ambient_humidity_value;
        uint16_t pressure_value = bmp280_ambient_values->ambient_pressure_value;
#if defined(REPORT_ALTITUDE)
        uint16_t altitude_value = bmp280_ambient_values->ambient_altitude_value;
#endif        

        hvx_params.handle = p_bmp280->temperature_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)&temperature_value;  
//---NRF_LOG_DEBUG("BLE BMP280 temp %d", temperature_value); //---NRF_LOG_FLUSH();

        sd_ble_gatts_hvx(p_bmp280->conn_handle, &hvx_params);
 
        memset(&hvx_params, 0, sizeof(hvx_params));
        hvx_params.handle = p_bmp280->humidity_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)&humidity_value;  

        sd_ble_gatts_hvx(p_bmp280->conn_handle, &hvx_params);

        memset(&hvx_params, 0, sizeof(hvx_params));
        hvx_params.handle = p_bmp280->pressure_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)&pressure_value;  

        sd_ble_gatts_hvx(p_bmp280->conn_handle, &hvx_params);

#if defined(REPORT_ALTITUDE)
        memset(&hvx_params, 0, sizeof(hvx_params));
        hvx_params.handle = p_bmp280->altitude_char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)altitude_value;  
        sd_ble_gatts_hvx(p_bmp280->conn_handle, &hvx_params);
#endif
    } 
}


