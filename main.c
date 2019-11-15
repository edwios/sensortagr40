/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/** @file
 *
 * @defgroup ble_sdk_app_template_main main.c
 * @{
 * @ingroup ble_sdk_app_template
 * @brief Template project main file.
 *
 * This file contains a template for creating a new application. It has the code necessary to wakeup
 * from button, advertise, get a connection restart advertising on disconnect and if no new
 * connection created go back to system-off mode.
 * It can easily be used as a starting point for creating a new application, the comments identified
 * with 'YOUR_JOB' indicates where and how you can customize.
 */

#define USE_MPU                         0
#if defined (BASIC_SENSOR) || defined(SENSORTAG_R40) 
//#if defined (BASIC_SENSOR) 
#define USE_BMP280                      1
#else
#define USE_BMP280                      0   // mixed sensors todo
#endif
#if defined (SENSORTAG_R40)
#if defined(VEML6075)
#define USE_VEML6075                    1
#define USE_AP3216C                     0
#define USE_LTR329                      0
#else
#define USE_VEML6075                    0
#define USE_AP3216C                     1
#define USE_LTR329                      0
#endif
#elif defined(SENSORTAG)
#define USE_VEML6075                    0
#define USE_AP3216C                     0
#define USE_LTR329                      1
#endif

#undef AUTO_DISCONNECT

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "fds.h"
#include "peer_manager.h"
#include "bsp_btn_ble.h"
//#include "sensorsim.h"
#include "ble_conn_state.h"
//#include "ble_nus.h"
#include "ble_dis.h"
#include "nrf_ble_gatt.h"
#include "bsp.h"
#include "boards.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include "app_uart.h"
#include "app_util_platform.h"
#include "nrf_delay.h"

#include "ble_common_items.h"

#if (USE_MPU)
#include "app_mpu.h"
#include "ble_mpu.h"
#endif
#if (USE_VEML6075)
#include "app_veml6075.h"
#include "ble_veml6075.h"
#endif
#if (USE_BMP280)
#include "app_bmp280.h"
#include "ble_bmp280.h"
#endif
#if (USE_AP3216C)
#include "app_ap3216c.h"
#include "ble_ap3216c.h"
#endif
#if (USE_LTR329)
#include "app_ltr329.h"
#include "ble_ltr329.h"
#endif


#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2    /**< Reply when unsupported features are requested. */
#if defined(SENSORTAG_R40)
#if (USE_VEML6075)
#define DEVICE_NAME                     "EnvMultiUV"                         /**< Name of device. Will be included in the advertising data. */
#else
#define DEVICE_NAME                     "EnvMultiIR"                         /**< Name of device. Will be included in the advertising data. */
#endif
#elif defined(BASIC_SENSOR)
#define DEVICE_NAME                     "EvTH"                         /**< Name of device. Will be included in the advertising data. */
#elif defined(SENSORTAG)
#define DEVICE_NAME                     "EvAL"
#else
#define DEVICE_NAME                     "BLE_GeNErIC"
#endif
#define MANUFACTURER_NAME               "ioStation Ltd."                        /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "HYP-AT1A06"                            /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x6c80172535                            /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x10001a                                /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */

#define APP_ADV_INTERVAL                MSEC_TO_UNITS(20, UNIT_0_625_MS)        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 20 ms). */
#define APP_ADV_SLOW_INTERVAL           MSEC_TO_UNITS(1022.5, UNIT_0_625_MS)    /**< The advertising interval (in units of 0.625 ms. This value corresponds to 1022.5 ms). */
//#define APP_ADV_TIMEOUT_IN_SECONDS      BLE_GAP_ADV_TIMEOUT_LIMITED_MAX       /**< The advertising timeout in units of seconds. */
#define APP_ADV_TIMEOUT_IN_SECONDS      1                                       /**< The advertising timeout in units of seconds. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(25, UNIT_1_25_MS)         /**< Minimum acceptable connection interval (25m seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(2000, UNIT_1_25_MS)       /**< Maximum acceptable connection interval (2s second). */
#define SLAVE_LATENCY                   4                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(26000, UNIT_10_MS)        /**< Connection supervisory timeout (26 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    6                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define TEMP_TYPE_AS_CHARACTERISTIC     1                                       /**< Determines if temperature type is given as characteristic (1) or as a field of measurement (0). */
#define MAX_READS                       30                                      /**< Max number of sensor reads, then stop. 0 = Never stop. Disconnect if battery mode BATTERY=1 */
#define READS_UNTIL_UPDATE              5                                       /**< Only update to GATT until after 5 reads */

#ifdef POWERUP
#define AUTO_DISCONNECT                                                         /**<Automatically disconnect after MAX_READS to save battery consumptions */
#endif

#define FLASH_CS                        30                                      /**< Flash mem /CS pin */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define TIMER_INTERVAL_SLOW_UPDATE      APP_TIMER_TICKS(300000)                 // 5min interval for data reporting
#define TIMER_INTERVAL_FAST_UPDATE      APP_TIMER_TICKS(200)                    // 200ms fast interval for data update
#define TIMER_INTERVAL_LED_FLASH        APP_TIMER_TICKS(5000)                   // 5s intervals
#define LED_BLINK_INTERVAL              1                                       // Non-Battery mode LED blink duration in ms (1 ms)

/*UART buffer size. */
#define UART_TX_BUF_SIZE 32
#define UART_RX_BUF_SIZE 32

#define LED_ADV                         BSP_LED_1_MASK
#define LED_CONN                        BSP_LED_2_MASK

//BLE_NUS_DEF(m_nus);                                                             /**< BLE NUS service instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                             /**< Advertising module instance. */
APP_TIMER_DEF(m_timer_accel_update_id);
#ifndef POWERUP
APP_TIMER_DEF(m_timer_led_id);
#endif
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
static bool m_isAdvertising = false;
//static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;          /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_envsense_t m_ble_envsense;
static uint8_t read_counts = 0;
static bool all_read = false;
#ifdef POWERUP
static int8_t tx_power_level = 0;
#else
static int8_t tx_power_level = 4;
#endif

#if (USE_MPU)
ble_mpu_t m_mpu;
temp_value_t m_temp_value;
#endif
#if (USE_VEML6075)
veml6075_ambient_values_t m_ambient;
#endif
#if (USE_BMP280)
bmp280_ambient_values_t m_bmpambient;
#endif
#if (USE_AP3216C)
ap3216c_ambient_values_t m_ap3216cambient;
#endif
#if (USE_LTR329)
ltr329_ambient_values_t m_ltr329ambient;
#endif

bool start_accel_update_flag = false;
bool fastRead = true;

/* YOUR_JOB: Declare all services structure your application is using
 *  BLE_XYZ_DEF(m_xyz);
 */

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] =                                               /**< Universally unique service identifiers. */
{
//    {BLE_UUID_HEALTH_THERMOMETER_SERVICE, BLE_UUID_TYPE_BLE},
//    {BLE_UUID_MPU_SERVICE_UUID, BLE_UUID_TYPE_BLE},
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
    {BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE, BLE_UUID_TYPE_BLE}
};

void timer_accel_update_handler(void * p_context)
{
    start_accel_update_flag = true;
}

void timer_led_handler(void * p_context)
{
    if (m_isAdvertising) {
        LEDS_ON(BSP_LED_2_MASK);
    }
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
        LEDS_ON(BSP_LED_1_MASK);
    }
    nrf_delay_ms(LED_BLINK_INTERVAL);
    bsp_indication_set(BSP_INDICATE_IDLE);
}

static void advertising_start(bool erase_bonds);
static void application_timers_stop(void);


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    ret_code_t err_code;
    //bool       is_indication_enabled;

    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            //---NRF_LOG_INFO("Connected to a previously bonded device.");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            //---NRF_LOG_INFO("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
            //             ble_conn_state_role(p_evt->conn_handle),
            //             p_evt->conn_handle,
            //             p_evt->params.conn_sec_succeeded.procedure);

            // Send a single temperature measurement if indication is enabled.
            // NOTE: For this to work, make sure ble_hts_on_ble_evt() is called before
            // pm_evt_handler() in ble_evt_dispatch().
            /*
            err_code = ble_hts_is_indication_enabled(&m_hts, &is_indication_enabled);
            APP_ERROR_CHECK(err_code);
            if (is_indication_enabled)
            {
                temperature_measurement_send();
            }
            */
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            err_code = fds_gc();
            if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(err_code);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            advertising_start(false);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
/**@snippet [Handling the data received over BLE] */
/*
static void nus_data_handler(ble_nus_evt_t * p_evt)
{

    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;

        //---NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        //---NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    //---NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }
        if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length-1] == '\r')
        {
            while (app_uart_put('\n') == NRF_ERROR_BUSY);
        }
    }

}
*/
/**@snippet [Handling the data received over BLE] */


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    // Initialize timer module.
    //---NRF_LOG_DEBUG("Initialising timer"); //---NRF_LOG_FLUSH();
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.

    err_code = app_timer_create(&m_timer_accel_update_id, APP_TIMER_MODE_REPEATED, timer_accel_update_handler);
    APP_ERROR_CHECK(err_code);

#ifndef POWERUP
    err_code = app_timer_create(&m_timer_led_id, APP_TIMER_MODE_REPEATED, timer_led_handler);
    APP_ERROR_CHECK(err_code);
#endif
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
    char *devName;
    int no;

    uint32_t l = strlen(DEVICE_NAME);

    devName = malloc((l+5)*sizeof(char));
    no = NRF_FICR->DEVICEADDR0%10000;
    sprintf(devName, "%s%04d", DEVICE_NAME, no);
    devName[l+5]='\0';
    //---NRF_LOG_DEBUG("Initialising GAP params"); //---NRF_LOG_FLUSH();
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)devName,
                                          strlen(devName));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    //---NRF_LOG_DEBUG("Initialising GATT"); //---NRF_LOG_FLUSH();
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                       ble_yy_service_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. ", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
}
*/

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    /* YOUR_JOB: Add code to initialize the services used by the application.
       ret_code_t                         err_code;
       ble_xxs_init_t                     xxs_init;
       ble_yys_init_t                     yys_init;

       // Initialize XXX Service.
       memset(&xxs_init, 0, sizeof(xxs_init));

       xxs_init.evt_handler                = NULL;
       xxs_init.is_xxx_notify_supported    = true;
       xxs_init.ble_xx_initial_value.level = 100;

       err_code = ble_bas_init(&m_xxs, &xxs_init);
       APP_ERROR_CHECK(err_code);

       // Initialize YYY Service.
       memset(&yys_init, 0, sizeof(yys_init));
       yys_init.evt_handler                  = on_yys_evt;
       yys_init.ble_yy_initial_value.counter = 0;

       err_code = ble_yy_service_init(&yys_init, &yy_init);
       APP_ERROR_CHECK(err_code);
     */

    ret_code_t          err_code;
//    ble_nus_init_t      nus_init;
    ble_dis_init_t      dis_init;
    ble_dis_sys_id_t    sys_id;
    ble_uuid_t          service_uuid;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);


    BLE_UUID_BLE_ASSIGN(service_uuid, BLE_UUID_ENVIRONMENTAL_SENSING_SERVICE);
    m_ble_envsense.conn_handle = BLE_CONN_HANDLE_INVALID;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &m_ble_envsense.service_handle);
    
    APP_ERROR_CHECK(err_code);

    //---NRF_LOG_DEBUG("Initialising Services"); //---NRF_LOG_FLUSH();
    //memset(&nus_init, 0, sizeof(nus_init));

/*
    nus_init.data_handler = nus_data_handler;
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
*/
    // Initialise various devices' services
#if (USE_MPU)
    ble_mpu_service_init(&m_mpu);
#endif
    // Initialise env sensing service

#if (USE_VEML6075)
    ble_veml6075_service_init(&m_ble_envsense);
#endif
#if (USE_AP3216C)
    ble_ap3216c_service_init(&m_ble_envsense);
#endif
#if (USE_BMP280)
    ble_bmp280_service_init(&m_ble_envsense);
#endif
#if (USE_LTR329)
    ble_ltr329_service_init(&m_ble_envsense);
#endif
    
    // Here the sec level for the Health Thermometer Service can be changed/increased.
    /*
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hts_init.hts_meas_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_meas_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_meas_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hts_init.hts_temp_type_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_temp_type_attr_md.write_perm);
    */

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, MODEL_NUM);

    sys_id.manufacturer_id            = MANUFACTURER_ID;
    sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id                 = &sys_id;

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        NRF_LOG_DEBUG("Connection param failed"); NRF_LOG_FLUSH();
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    //---NRF_LOG_DEBUG("Initialising CONN"); //---NRF_LOG_FLUSH();
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting timers.
 */
static void application_timers_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(m_timer_accel_update_id, TIMER_INTERVAL_FAST_UPDATE, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_slow_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(m_timer_accel_update_id, TIMER_INTERVAL_SLOW_UPDATE, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for starting LED timer.
 */
static void led_timers_start(void)
{
#ifndef POWERUP
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer. */
    ret_code_t err_code;
    err_code = app_timer_start(m_timer_led_id, TIMER_INTERVAL_LED_FLASH, NULL);
    APP_ERROR_CHECK(err_code);
#endif
}

/**@brief Function for stopping timers.
 */
static void application_timers_stop(void)
{
    /* YOUR_JOB: Stop your timers. below is an example of how to stop a timer. */
    ret_code_t err_code;
    err_code = app_timer_stop(m_timer_accel_update_id);
    APP_ERROR_CHECK(err_code);
    NRF_TIMER1->TASKS_SHUTDOWN = 1;
}

/**@brief Function for stopping timers.
 */
static void led_timers_stop(void)
{
#ifndef POWERUP
    /* YOUR_JOB: Stop your timers. below is an example of how to stop a timer. */
    ret_code_t err_code;
    err_code = app_timer_stop(m_timer_led_id);
    APP_ERROR_CHECK(err_code);
#endif
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    ret_code_t err_code;

    read_counts = 0;    // reset read count

    ///err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    ///APP_ERROR_CHECK(err_code);

    // Stop all timers, no matter
    application_timers_stop();
    led_timers_stop();
    app_timer_stop_all();
    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

#if (USE_MPU)
    mpu_sleep(true);
#endif
#if (USE_VEML6075)
    veml6075_config_deactivate();
    veml6075_poweroff();
#endif
#if (USE_AP3216C)
    ap3216c_config_deactivate();
#endif
#if (USE_BMP280)
    bmp280_config_deactivate();
#endif
#if (USE_LTR329)
    ltr329_config_deactivate();
#endif

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            //---NRF_LOG_DEBUG("Fast advertising."); //---NRF_LOG_FLUSH();
            ///err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            ///APP_ERROR_CHECK(err_code);
            m_isAdvertising = true;
            // Refresh LED immediately
            timer_led_handler(0);
            break;

        case BLE_ADV_EVT_IDLE:
            //---NRF_LOG_DEBUG("Advertising Event Idle."); //---NRF_LOG_FLUSH();
            // sleep_mode_enter();  // do not sleep when fast advertise is done (180s)
            err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
            APP_ERROR_CHECK(err_code);
            m_isAdvertising = true;
            // Refresh LED immediately
            timer_led_handler(0);
            break;

        default:
            break;
    }
}

static void on_connected(const ble_gap_evt_t * const p_gap_evt)
{
    ret_code_t  err_code;
    //uint32_t    periph_link_cnt = ble_conn_state_n_peripherals(); // Number of peripheral links.

    //NRF_LOG_INFO("Connection with link 0x%x/%d established.", p_gap_evt->conn_handle, periph_link_cnt);

    m_conn_handle = p_gap_evt->conn_handle;
    m_ble_envsense.conn_handle = p_gap_evt->conn_handle; 
#ifndef POWERUP
    // NORDIC: SET HIGH TX POWER ADV if not runnipng on battery
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, m_ble_envsense.conn_handle, tx_power_level); 
    APP_ERROR_CHECK(err_code); 
#endif
    err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
    APP_ERROR_CHECK(err_code);
    m_isAdvertising = false;

#ifndef POWERUP
    // NORDIC: SET HIGH TX POWER AFTER CONNECTING if not running on battery
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, m_conn_handle, tx_power_level); 
    APP_ERROR_CHECK(err_code); 
#endif
    // Refresh LED immediately
    timer_led_handler(0);
#if (USE_MPU)
    mpu_sleep(false);       // Wake up MPU
    m_mpu.conn_handle = p_gap_evt->conn_handle;
#endif
#if (USE_VEML6075)
    veml6075_config_activate();
#endif
#if (USE_AP3216C)
    ap3216c_config_activate();
    ap3216c_config();
#endif
#if (USE_BMP280)
    bmp280_config_activate();
    bmp280_config();
#endif
#if (USE_LTR329)
    ltr329_config_activate();
    ltr329_config();
#endif

    start_accel_update_flag = true;
    fastRead = true;
    application_timers_start();
    // Optional to re-start advertising if multiple connections (periph_link_cnt < Max allowed)
    // advertising_start();
}

static void on_disconnected(ble_gap_evt_t const * const p_gap_evt)
{
    ret_code_t  err_code;
//    uint32_t    periph_link_cnt = ble_conn_state_n_peripherals(); // Number of peripheral links.

    start_accel_update_flag = false;
    read_counts = 0;
    m_conn_handle = BLE_CONN_HANDLE_INVALID;
    m_ble_envsense.conn_handle = BLE_CONN_HANDLE_INVALID;
    application_timers_stop();

#if (USE_MPU)
    m_mpu.conn_handle = BLE_CONN_HANDLE_INVALID;
    mpu_sleep(true);    // Put MPU to sleep mode
#endif
#if (USE_VEML6075)
    veml6075_config_deactivate();
#endif
#if (USE_AP3216C)
    ap3216c_config_deactivate();
#endif
#if (USE_BMP280)
    bmp280_config_deactivate();
#endif
#if (USE_LTR329)
    ltr329_config_deactivate();
#endif
    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);
#ifdef POWERUP
    bsp_board_led_off(BSP_BOARD_LED_1);
#endif

//    NRF_LOG_INFO("Connection 0x%x/%d has been disconnected. Reason: 0x%X",
//                 p_gap_evt->conn_handle, periph_link_cnt,
//                 p_gap_evt->params.disconnected.reason);

    // Optional to re-start advertising if multiple connections (periph_link_cnt < Max allowed)
    // advertising_start();
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnected(&p_ble_evt->evt.gap_evt);
            break;

        case BLE_GAP_EVT_CONNECTED:
            on_connected(&p_ble_evt->evt.gap_evt);

            break;

#if defined(S132)
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;
#endif

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout."); NRF_LOG_FLUSH();
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout."); NRF_LOG_FLUSH();
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
//            if (req.type == BLE_GATTS_AUTHORIZE_TYPE_READ) {
                NRF_LOG_DEBUG("Reading uuid %x", req.request.read.uuid.uuid); NRF_LOG_FLUSH();
//            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    //---NRF_LOG_DEBUG("Initialising BLE Stack"); //---NRF_LOG_FLUSH();
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    //---NRF_LOG_DEBUG("Initialising Peer Manager"); //---NRF_LOG_FLUSH();
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    //---NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
static void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            NRF_LOG_DEBUG("BSP event disconnect"); NRF_LOG_FLUSH();
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break; // BSP_EVENT_KEY_0

        default:
            break;
    }
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    ret_code_t              err_code;
    ble_advertising_init_t  init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
    init.advdata.p_tx_power_level        = &tx_power_level;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    init.config.ble_adv_slow_enabled  = true;
    init.config.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout  = 0;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    //---NRF_LOG_DEBUG("Initialising bsp"); //---NRF_LOG_FLUSH();
//    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    // Disable buttons if not used
    err_code = bsp_init(BSP_INIT_LEDS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Power manager.
 
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}
*/

/**@brief Function for starting advertising.
 */
static void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETED_SUCEEDED evetnt
    }
    else
    {
        ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        m_isAdvertising = true;
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
/**
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') || (index >= (m_ble_nus_max_data_len)))
            {
                //---NRF_LOG_DEBUG("Ready to send data over BLE NUS");
                //---NRF_LOG_HEXDUMP_DEBUG(data_array, index);

                do
                {
                    uint16_t length = (uint16_t)index;
                    err_code = ble_nus_string_send(&m_nus, data_array, &length);
                    if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_BUSY) )
                    {
                        APP_ERROR_CHECK(err_code);
                    }
                } while (err_code == NRF_ERROR_BUSY);

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

**/

/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
/*
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        .baud_rate    = UART_BAUDRATE_BAUDRATE_Baud115200
    };
//    nrf_gpio_cfg_input(RX_PIN_NUMBER, NRF_GPIO_PIN_PULLUP);
//    nrf_gpio_cfg_output(TX_PIN_NUMBER);
    //---NRF_LOG_DEBUG("Initialising UART"); //---NRF_LOG_FLUSH();
    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}
*/
/**@snippet [UART Initialization] */

#if (USE_MPU)
void mpu_setup(void)
{
    ret_code_t ret_code;
    // Initiate MPU driver
    //---NRF_LOG_DEBUG("Initialising MPU9255"); //---NRF_LOG_FLUSH();
    ret_code = mpu_init();
    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 19;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    ret_code = mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
}
#endif

/**@brief Function for application main entry.
 */
int main(void)
{
    bool erase_bonds;
    uint8_t err_code;
    bool error = false;
    int fastReadCount = 0;

#if (USE_VEML6075)
    uint8_t veml6057_partid=0;
    bool veml6075_read=false;
#endif
#if (USE_BMP280)
    bool bmp280_read=false;
    uint8_t bmp280_partid=0;
#endif
#if (USE_AP3216C)
    bool ap3216c_read=false;
#endif
#if (USE_LTR329)
    bool ltr329_read=false;
    ltr329_part_id_t ltr329_partid;
#endif

//    NRF_POWER->DCDCEN = 1;  

    // Initialize.
#if (USE_VEML6075)
    veml6075_poweroff();
#endif
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
#if defined(SENSORTAG_R40)
    nrf_gpio_cfg_output(FLASH_CS);
    nrf_gpio_pin_set(FLASH_CS);
#endif
    log_init();
    NRF_LOG_INFO("\033[2J\033[;BASIC_SENSOR. Compiled @ %s.\r\n", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
//    uart_init();  NRF_UART0->ENABLE = 1;
    timers_init();
    buttons_leds_init(&erase_bonds);
    NRF_LOG_INFO("LED done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    power_management_init();
    NRF_LOG_INFO("PWM done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    bsp_indication_set(BSP_INDICATE_CONNECTED);
    NRF_LOG_INFO("BSP done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    ble_stack_init();
    NRF_LOG_INFO("BLE done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    gap_params_init();
    NRF_LOG_INFO("GAP done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    gatt_init();
    NRF_LOG_INFO("GATT done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    advertising_init();
    NRF_LOG_INFO("ADV done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    services_init();
    NRF_LOG_INFO("SVC done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();
    conn_params_init();
    peer_manager_init();
    NRF_LOG_INFO("Init done", nrf_log_push(__TIME__));
    NRF_LOG_FLUSH();

#ifndef POWERUP
    // NORDIC: SET HIGH TX POWER ADV if not runnipng on battery
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_advertising.adv_handle, tx_power_level); 
    APP_ERROR_CHECK(err_code); 
#endif

    /** Comment the following if without L4 and L5 on custom board */
#if defined(BASIC_SENSOR) || defined(SENSORTAG)
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
#else
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_DISABLE);
#endif

#if (USE_VEML6075)
    veml6075_poweron();
    nrf_delay_us(300000);
    err_code = veml6075_init();
    if (err_code == NRF_SUCCESS)
    {
        veml6075_config();
        veml6075_config_deactivate();
    } else {
        error = true;
    }
#endif
#if (USE_BMP280)
    err_code = bmp280_init();
    if (err_code == NRF_SUCCESS)
    {
        bmp280_config();
#if defined(BASIC_SENSOR)
        // Disable the MPU on board
        // Must done before bmp280_config_deactivate otherwise TWI will be disabled too early
        bma255_config_deactivate();
#endif
        bmp280_config_deactivate(); // bmp280_config_deactivate() will also stop TWI
    } else {
        error = true;
    }
#endif
#if (USE_AP3216C)
    err_code = ap3216c_init();
    if (err_code == NRF_SUCCESS)
    {
        ap3216c_config();
        ap3216c_config_deactivate();
    } else {
        error = true;
    }
#endif
#if (USE_LTR329)
    err_code = ltr329_init();
    if (err_code == NRF_SUCCESS)
    {
        ltr329_config();
        ltr329_config_deactivate();
    } else {
        error = true;
    }
#endif
#if (USE_MPU)
    accel_values_t accel_values, last_accel_values;
    gyro_values_t gyro_values;
    unsigned long delta_accel=0;
    mpu_sleep(true);
    last_accel_values.x=last_accel_values.y=last_accel_values.z=0;
#endif

    //nrf_gpio_pin_set(LED_3);
//    nrf_gpio_cfg_sense_input(BUTTON1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    led_timers_start();
    advertising_start(erase_bonds);
    if (!error) bsp_indication_set(BSP_INDICATE_IDLE);

    // Start execution.
    NRF_LOG_DEBUG("%s", nrf_log_push("BASIC_SENSOR started."));
    NRF_LOG_FLUSH();
//    start_accel_update_flag = true;

    fastReadCount = 0;
    // Enter main loop.
    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            if(start_accel_update_flag)
            {
                if (fastRead || fastReadCount == 5)
                {
#if (USE_MPU)
                    mpu_read_accel(&accel_values);
                    mpu_read_gyro(&gyro_values);
                    mpu_read_temp(&m_temp_value);
#endif
#if (USE_VEML6075)
                    if (veml6075_has_new_data()) {
                        veml6075_read_partid(&veml6057_partid);
                        veml6075_read_ambient(&m_ambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_DEBUG("[%d] Ambient: Vis %d, IR %d, Lux %d", veml6057_partid, m_ambient.ambient_visible_value, m_ambient.ambient_ir_value, m_ambient.ambient_lux_value);
                            NRF_LOG_DEBUG("[%d] UV: UVA %d, UVB %d, UVI %d", veml6057_partid, m_ambient.ambient_uva_value, m_ambient.ambient_uvb_value, m_ambient.ambient_uvi_value);
                        }
                        veml6075_read = true;
                    }
#endif
#if (USE_AP3216C)
                    if (ap3216c_has_new_data()) {
                        ap3216c_read_ambient(&m_ap3216cambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_DEBUG("AP3216c Ambient: Vis %d, IR %d, Lux %d", m_ap3216cambient.ambient_visible_value, m_ap3216cambient.ambient_ir_value, m_ap3216cambient.ambient_lux_value);
                        }
                        ap3216c_read = true;
                    }
#endif
#if (USE_BMP280)
                    if (bmp280_has_new_data()) {
                        bmp280_read_partid(&bmp280_partid);
                        bmp280_read_ambient(&m_bmpambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_DEBUG("[%d] Ambient: Temp %d, Humi %d, Pres %d", bmp280_partid, m_bmpambient.ambient_temperature_value, m_bmpambient.ambient_humidity_value, m_bmpambient.ambient_pressure_value);
                        }
                        bmp280_read = true;
                    }
                
                    
#endif
#if (USE_LTR329)
                    if (ltr329_has_new_data()) {
                        ltr329_read_partid(&ltr329_partid);
                        ltr329_read_ambient(&m_ltr329ambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_DEBUG("LTR329 Ambient: Lux %d, Vis %d, IR %d", m_ltr329ambient.ambient_lux_value, m_ltr329ambient.ambient_visible_value, m_ltr329ambient.ambient_ir_value);
                        }
                        ltr329_read = true;
                    }
#endif
#if (USE_MPU)
                    delta_accel = ((accel_values.x-last_accel_values.x)*(accel_values.x-last_accel_values.x))+((accel_values.y-last_accel_values.y)*(accel_values.y-last_accel_values.y))+((accel_values.z-last_accel_values.z)*(accel_values.z-last_accel_values.z));
                    last_accel_values = accel_values;
                    //---NRF_LOG_INFO("\033[2J\033[;HAccel: %05d, %05d, %05d\r\n", accel_values.x, accel_values.y, accel_values.z);
                    //---NRF_LOG_INFO("Accel: %05d, %05d, %05d", accel_values.x, accel_values.y, accel_values.z);
                    //---NRF_LOG_INFO("Delta: %d", delta_accel); 
                    //---NRF_LOG_INFO("Gyro: %05d, %05d, %05d", gyro_values.x, gyro_values.y, gyro_values.z);
                    //---NRF_LOG_INFO("Temperature: %05d\n", m_temp_value);
                    //---NRF_LOG_INFO("Accel: %02x, %02x, %02x, %02x, %02x, %02x\r\n", (uint8_t)(accel_values.x >> 8), (uint8_t)accel_values.x, (uint8_t)(accel_values.y >> 8), (uint8_t)accel_values.y, (uint8_t)(accel_values.z >> 8), (uint8_t)accel_values.z);
                    //---NRF_LOG_FLUSH();
                    if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_mpu_update(&m_mpu, &accel_values);
#endif
#if (USE_BMP280) && (USE_VEML6075)
                    if (veml6075_read && bmp280_read) {
                        veml6075_read = false;
                        bmp280_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_bmp280_update(&m_ble_envsense, &m_bmpambient);
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_veml6075_update(&m_ble_envsense, &m_ambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("[%d] Ambient: Temp %d, Humi %d, Pres %d", bmp280_partid, m_bmpambient.ambient_temperature_value, m_bmpambient.ambient_humidity_value, m_bmpambient.ambient_pressure_value);
                        }
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("[%d] Ambient: Vis %d, IR %d, Lux %d", veml6057_partid, m_ambient.ambient_visible_value, m_ambient.ambient_ir_value, m_ambient.ambient_lux_value);
                            NRF_LOG_INFO("[%d] UV: UVA %d, UVB %d, UVI %d", veml6057_partid, m_ambient.ambient_uva_value, m_ambient.ambient_uvb_value, m_ambient.ambient_uvi_value);
                        }
                    }
#elif (USE_BMP280) && (USE_AP3216C)               
                    if (ap3216c_read && bmp280_read) {
                        ap3216c_read = false;
                        bmp280_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_bmp280_update(&m_ble_envsense, &m_bmpambient);
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_ap3216c_update(&m_ble_envsense, &m_ap3216cambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("[%d] Ambient: Temp %d, Humi %d, Pres %d", bmp280_partid, m_bmpambient.ambient_temperature_value, m_bmpambient.ambient_humidity_value, m_bmpambient.ambient_pressure_value);
                        }
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("AP3216c Ambient: Vis %d, IR %d, Lux %d", m_ap3216cambient.ambient_visible_value, m_ap3216cambient.ambient_ir_value, m_ap3216cambient.ambient_lux_value);
                        }
                    }
#elif (USE_BMP280)
                    if (bmp280_read) {
                        bmp280_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_bmp280_update(&m_ble_envsense, &m_bmpambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("[%d],%03d Ambient: Temp %d, Humi %d, Pres %d", bmp280_partid, read_counts, m_bmpambient.ambient_temperature_value, m_bmpambient.ambient_humidity_value, m_bmpambient.ambient_pressure_value);
                        }
                    }
#elif (USE_VEML6075)
                    if (veml6075_read) {
                        veml6075_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_veml6075_update(&m_ble_envsense, &m_ambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("[%d] Ambient: Vis %d, IR %d, Lux %d", veml6057_partid, m_ambient.ambient_visible_value, m_ambient.ambient_ir_value, m_ambient.ambient_lux_value);
                            NRF_LOG_INFO("[%d] UV: UVA %d, UVB %d, UVI %d", veml6057_partid, m_ambient.ambient_uva_value, m_ambient.ambient_uvb_value, m_ambient.ambient_uvi_value);
                        }
                    }
#elif (USE_AP3216C)
                    if (ap3216c_read) {
                        ap3216c_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_ap3216c_update(&m_ble_envsense, &m_ap3216cambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("AP3216c Ambient: Vis %d, IR %d, Lux %d", m_ap3216cambient.ambient_visible_value, m_ap3216cambient.ambient_ir_value, m_ap3216cambient.ambient_lux_value);
                        }
                    }
#elif (USE_LTR329)
                    if (ltr329_read) {
                        ltr329_read = false;
                        all_read = true;
                        if ((read_counts > READS_UNTIL_UPDATE) && (m_conn_handle != BLE_CONN_HANDLE_INVALID)) ble_ltr329_update(&m_ble_envsense, &m_ltr329ambient);
                        if (read_counts > READS_UNTIL_UPDATE) {
                            NRF_LOG_INFO("LTR329 Ambient: Lux %d, Vis %d, IR %d", m_ltr329ambient.ambient_lux_value, m_ltr329ambient.ambient_visible_value, m_ltr329ambient.ambient_ir_value);
                        }
                    }                
#endif

                    //mpu_sleep(true); // Somehow enable this will cause the mpu not reporting readings
                    if (all_read) {
                        all_read = false;
                        read_counts++;
                    }


#ifdef AUTO_DISCONNECT
                    if (MAX_READS > 0) {    // Auto-disconnect enabled
                        ret_code_t err_code;
                        if (read_counts > MAX_READS) {
                            read_counts = 0;
                            start_accel_update_flag = false;
                            NRF_LOG_INFO("Enough sensor reads, disconnecting"); NRF_LOG_FLUSH();
                            if (m_ble_envsense.conn_handle != BLE_CONN_HANDLE_INVALID) {

                                err_code = sd_ble_gap_disconnect(m_ble_envsense.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                                APP_ERROR_CHECK(err_code);
                            }
                        }
                    }
#else
                    if (MAX_READS > 0) {    // Auto-disconnect enabled
                        if (read_counts > MAX_READS) {
                            read_counts = 0;
                            fastReadCount = 10; // anything > 5 (see #1718)
                            fastRead = true;
                            start_accel_update_flag = false;
                            NRF_LOG_INFO("Enough sensor reads, change to slow mode"); NRF_LOG_FLUSH();
                            application_timers_stop();
                            application_timers_slow_start();
                        }
                    }
#endif
                } else {
#if (USE_AP3216C)
                    ap3216c_read = false;
#endif
#if (USE_VEML6075)
                    veml6075_read = false;
#endif
#if (USE_BMP280)
                    bmp280_read = false;
#endif
#if (USE_LTR329)
                    ltr329_read = false;
#endif
                }
                start_accel_update_flag = false;
                if (fastRead && (fastReadCount == 0)) {
                    NRF_LOG_INFO("Fast sensor reads, change to fast mode"); NRF_LOG_FLUSH();
                    application_timers_stop();
                    application_timers_start();
                }
                if (fastReadCount++ > 5)
                {
                    fastReadCount = 0;
                }
            }
            nrf_pwr_mgmt_run();
        }
    }
}


/**
 * @}
 */
