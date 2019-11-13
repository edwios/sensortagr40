#ifndef BLE_COMMON_ITEMS_H__
#define BLE_COMMON_ITEMS_H__

#define BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE     0x181A /**< Environmental Sensing service UUID. */

// EnvMulti
#if defined (SENSORTAG_R40) && defined(VEML6075)
// EnvMultiUV
#define BLE_UUID_BASE_UUID                {0x3D, 0x72, 0x1B, 0xF8, 0x6F, 0x56, 0x66, 0x1E, 0x9B, 0x6B, 0x79, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#endif
#if defined (SENSORTAG_R40) && !defined(VEML6075)
// EnvMultiIR
#define BLE_UUID_BASE_UUID                {0x2D, 0x72, 0x8B, 0x84, 0x64, 0x56, 0x2D, 0x15, 0x7F, 0x00, 0x91, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#endif

#if defined(SENSORTAG)
// EnvMulti
#define BLE_UUID_BASE_UUID              {0x2A, 0x72, 0x1B, 0x84, 0x59, 0x56, 0x2D, 0x15, 0x6C, 0x6B, 0x8E, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#endif

#if defined(BASIC_SENSOR)
#ifdef SEQ1
// EnvE280_01 (Balcony)
#define BLE_UUID_BASE_UUID             	{0x28, 0x42, 0xBB, 0x85, 0x66, 0x56, 0x6D, 0x15, 0x7F, 0x00, 0x91, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#elif SEQ2
// EnvE280_02 (Bed room)
#define BLE_UUID_BASE_UUID             	{0x23, 0x42, 0xBB, 0x84, 0x63, 0x56, 0x6D, 0x15, 0x7D, 0x00, 0x90, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#elif SEQ3
// EnvE280_03 (Living room)
#define BLE_UUID_BASE_UUID             	{0x24, 0x42, 0xBB, 0x84, 0x62, 0x56, 0x6D, 0x15, 0x7C, 0x00, 0x8F, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#else
// EnvE280_04 (Some other)
#define BLE_UUID_BASE_UUID             	{0x25, 0x42, 0xBB, 0x84, 0x61, 0x56, 0x6D, 0x15, 0x7B, 0x00, 0x8F, 0x06, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID
#endif
#endif

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of ble Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    visible_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    ir_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    lux_char_handles;
    ble_gatts_char_handles_t    temperature_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    humidity_char_handles;   /**< Handles related to the our new characteristic. */
    ble_gatts_char_handles_t    pressure_char_handles;
    ble_gatts_char_handles_t    altitude_char_handles;
    ble_gatts_char_handles_t    uva_char_handles;   /**< Handles of UVA characteristic. */
    ble_gatts_char_handles_t    uvb_char_handles;   /**< Handles of UVB characteristic. */
    ble_gatts_char_handles_t    uvc_char_handles;   /**< Handles of UVC characteristic. */
    ble_gatts_char_handles_t    uvi_char_handles;   /**< Handles of UVI characteristic. */
} ble_envsense_t;

#endif /* BLE_COMMON_ITEMS_H__ */
