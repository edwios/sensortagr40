
#define BSP_BTN_BLE_ENABLED 1
#define BLE_ADVERTISING_ENABLED 1
#define NRF_BLE_CONN_PARAMS_ENABLED 1
#define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT 1
#define NRF_BLE_GATT_ENABLED 1
#define GPIOTE_ENABLED 1
#define FDS_ENABLED 1
#define PEER_MANAGER_ENABLED 1
#define BLE_NUS_ENABLED 0
#define NRF_FSTORAGE_ENABLED 1
//#define BLE_HTS_ENABLED 1
#define BLE_DIS_ENABLED 1

#define TWI_ENABLED 1
#define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 1

#define TWI0_ENABLED 1
#define TWI0_USE_EASY_DMA 1

#define TWI1_ENABLED 1
#define TWI1_USE_EASY_DMA 1

#define SPI_ENABLED 0
//#define SPI0_ENABLED 1
#define SPI2_ENABLED 0
#define NRF_SPI_DRV_MISO_PULLUP_CFG 3		// Need to configure from 14.1

#define TIMER_ENABLED 1
#define TIMER0_ENABLED 1
#define APP_TIMER_ENABLED 1

#define APP_UART_ENABLED 0
#define UART_ENABLED 0
#define APP_FIFO_ENABLED 1

#define BUTTON_ENABLED 1

#define NRF_LOG_ENABLED 1					// Use Log
#define NRF_LOG_DEFERRED 1					// Buffered, may need to increase buffer size
// <0=> SKIP 
// <1=> TRIM 
// <2=> BLOCK_IF_FIFO_FULL 
#define SEGGER_RTT_CONFIG_DEFAULT_MODE 0
#define NRF_LOG_BACKEND_SERIAL_USES_RTT 1	// 1 if RTT Log
#define NRF_LOG_BACKEND_RTT_ENABLED 1		// 1 if RTT Log
#define NRF_LOG_BACKEND_UART_ENABLED 0		// Must be disabled or uart_init() will fail with err_code 8

#define NRF_SDH_BLE_LOG_ENABLED 0
#define NRF_SDH_BLE_VS_UUID_COUNT 0			// Must observe the number of vendor specific services
#define NRF_SDH_LOG_ENABLED 0
#define NRF_SDH_SOC_LOG_ENABLED 0
#define NRF_LOG_DEFAULT_LEVEL 4
#define NRF_LOG_FILTERS_ENABLED 0

#define NRF_SDH_BLE_ENABLED 1
#define NRF_SDH_ENABLED 1
#define NRF_SDH_SOC_ENABLED 1

#define NRF_BALLOC_ENABLED 1
#define NRF_FPRINTF_ENABLED 1
#define NRF_MEMOBJ_ENABLED 1
#define NRF_SECTION_ITER_ENABLED 1
#define NRF_STRERROR_ENABLED 1

#define PERIPHERAL_RESOURCE_SHARING_ENABLED  1
