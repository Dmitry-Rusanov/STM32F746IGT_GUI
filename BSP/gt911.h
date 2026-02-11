/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GT911_H_
#define __GT911_H_

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Exported macro ------------------------------------------------------------*/

// I2C address of GT911 (check your hardware wiring)
#define GOODIX_ADDRESS              0x14    // Alternative: 0x5D

// Write only registers
#define GOODIX_REG_COMMAND          0x8040
#define GOODIX_REG_ESD_CHECK        0x8041
#define GOODIX_REG_PROXIMITY_EN     0x8042

// Read/write configuration registers
#define GOODIX_REG_CONFIG_DATA      0x8047  // Configuration data (184 bytes)
#define GOODIX_REG_MAX_X            0x8048  // X output max value (2 bytes LSB)
#define GOODIX_REG_MAX_Y            0x804A  // Y output max value (2 bytes LSB)
#define GOODIX_REG_MAX_TOUCH        0x804C  // Max touch points (4 bits, 1-5)
#define GOODIX_REG_MOD_SW1          0x804D  // Module switch 1
#define GOODIX_REG_MOD_SW2          0x804E  // Module switch 2
#define GOODIX_REG_SHAKE_CNT        0x804F  // Debounce count
#define GOODIX_REG_X_THRESHOLD      0x8057  // X threshold
#define GOODIX_REG_CONFIG_FRESH     0x8100  // Config update flag

// Read-only registers
#define GOODIX_REG_ID               0x8140  // Product ID (4 bytes)
#define GOODIX_REG_FW_VER           0x8144  // Firmware version (2 bytes)
#define GOODIX_READ_X_RES           0x8146  // Current X resolution (2 bytes)
#define GOODIX_READ_Y_RES           0x8148  // Current Y resolution (2 bytes)
#define GOODIX_READ_VENDOR_ID       0x814A  // Vendor ID

// Coordinate registers
#define GOODIX_READ_COORD_ADDR      0x814E  // Coordinate status register
#define GOODIX_POINT1_X_ADDR        0x8150  // First touch point X coordinate
#define GOODIX_POINT1_Y_ADDR        0x8152  // First touch point Y coordinate

// Each touch point occupies 8 bytes in memory
#define GT911_TOUCH_POINT_SIZE      8

// Maximum number of supported touch points
#define GT911_MAX_TOUCH_POINTS      5

// Command definitions for GOODIX_REG_COMMAND
#define GOODIX_CMD_READ             0x00    // Read coordinate state
#define GOODIX_CMD_DIFFVAL          0x01    // Read difference value
#define GOODIX_CMD_SOFTRESET        0x02    // Software reset
#define GOODIX_CMD_BASEUPDATE       0x03    // Baseline update
#define GOODIX_CMD_CALIBRATE        0x04    // Benchmark calibration
#define GOODIX_CMD_SCREEN_OFF       0x05    // Screen off

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Status codes for GT911 operations
 */
typedef enum {
    GT911_OK = 0,           // Operation successful
    GT911_Error = 1,        // General error
    GT911_NotResponse = 2,  // Device not responding
    GT911_I2C_Error = 3,    // I2C communication error
    GT911_Config_Error = 4  // Configuration error
} GT911_Status_t;

/**
 * @brief GT911 configuration structure
 */
typedef struct {
    uint16_t X_Resolution;              // X resolution (e.g., 1024)
    uint16_t Y_Resolution;              // Y resolution (e.g., 600)
    uint8_t Number_Of_Touch_Support;    // Max touch points (1-5)
    bool ReverseX;                      // Reverse X axis
    bool ReverseY;                      // Reverse Y axis
    bool SwithX2Y;                      // Swap X and Y axes
    bool SoftwareNoiseReduction;        // Enable software noise reduction
} GT911_Config_t;

/**
 * @brief Touch coordinate structure
 */
typedef struct {
    uint16_t x;     // X coordinate
    uint16_t y;     // Y coordinate
} TouchCoordinate_t;

/* Exported constants --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/**
 * @brief Initialize GT911 with default configuration
 * @note Uses predefined configuration without sending it to device
 * @return GT911_Status_t Status of initialization
 */
GT911_Status_t GT911_Init(void);

/**
 * @brief Initialize GT911 with custom configuration
 * @param config Custom configuration structure
 * @return GT911_Status_t Status of initialization
 */
GT911_Status_t GT911_InitWithConfig(GT911_Config_t config);

/**
 * @brief Read touch coordinates from GT911
 * @param cordinate Array to store touch coordinates
 * @param number_of_cordinate Pointer to store number of detected touches
 * @return GT911_Status_t Status of read operation
 */
GT911_Status_t GT911_ReadTouch(TouchCoordinate_t* cordinate, uint8_t* number_of_cordinate);

/**
 * @brief Set device mode (working, sleep, etc.)
 * @param mode Device mode (0x00 = working, 0x01 = sleep, 0x04 = factory)
 * @return GT911_Status_t Status of operation
 */
GT911_Status_t GT911_SetDeviceMode(uint8_t mode);

/**
 * @brief Get current device mode
 * @param mode Pointer to store device mode
 * @return GT911_Status_t Status of operation
 */
GT911_Status_t GT911_GetDeviceMode(uint8_t *mode);

/**
 * @brief Read configuration from GT911
 * @param config_buffer Buffer to store configuration (min 190 bytes)
 * @param max_length Maximum buffer length
 * @return GT911_Status_t Status of read operation
 */
GT911_Status_t GT911_ReadConfig(uint8_t *config_buffer, uint16_t max_length);

/**
 * @brief Send configuration to GT911
 * @return GT911_Status_t Status of write operation
 */
GT911_Status_t GT911_SendConfig(void);

/**
 * @brief Perform hardware calibration
 * @return GT911_Status_t Status of calibration
 */
GT911_Status_t GT911_Calibrate(void);

/**
 * @brief Perform software reset of GT911
 * @return GT911_Status_t Status of reset
 */
GT911_Status_t GT911_SoftReset(void);

// Platform-specific function prototypes (must be implemented by user)
void GT911_INT_Input(void);     // Configure INT pin as input
void GT911_INT_Output(void);    // Configure INT pin as output
void GT911_RST_Control(bool state);     // Control RST pin (true = high, false = low)
void GT911_INT_Control(bool state);     // Control INT pin (true = high, false = low)
void GT911_Delay(uint16_t ms);          // Delay function
GT911_Status_t GT911_I2C_Write(uint8_t Addr, uint8_t* write_data, uint16_t write_length);
GT911_Status_t GT911_I2C_Read(uint8_t Addr, uint8_t* read_data, uint16_t read_length);

#endif /* __GT911_H_ */
