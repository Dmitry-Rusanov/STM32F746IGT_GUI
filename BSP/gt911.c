/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "GT911.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define CONFIG_SIZE 186            // Total configuration size (184 + checksum + fresh flag)
#define CONFIG_DATA_SIZE 184       // Configuration data size
#define CONFIG_CHECKSUM_POS 184    // Checksum position in config array
#define CONFIG_FRESH_FLAG 0x01     // Fresh flag value for config update

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/**
 * @brief Default configuration for GT911 (WaveShare 7-inch configuration)
 *
 * Byte offsets:
 * 0-1: X Resolution (LSB first) - 0x045B = 1115
 * 2-3: Y Resolution - 0x0258 = 600
 * 4: Touch points - 0x05 = 5 points
 * 5: Module switch 1
 * 6-... Other configuration parameters
 */
static uint8_t GT911_Config[CONFIG_SIZE] = {
    // Basic parameters (first 7 bytes are most important)
    0x5B, 0x04,         // X Resolution LSB: 0x045B = 1115
    0x58, 0x02,         // Y Resolution LSB: 0x0258 = 600
    0x05,               // Touch points: 5
    0x3D,               // Module switch 1

    // Rest of configuration...
    0x20, 0x22, 0x08, 0x28, 0x08, 0x64, 0x46, 0x03, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0x18, 0x1A, 0x1E, 0x14,
    0x8C, 0x2E, 0x0E, 0xB1, 0xB3, 0xB8, 0x08, 0x00, 0x00, 0x00,
    0xD9, 0x02, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9B, 0xD0, 0x9E, 0xD5, 0xF4, 0x08,
    0x00, 0x00, 0x04, 0x83, 0x9F, 0x00, 0x81, 0xA9, 0x00, 0x7F,
    0xB3, 0x00, 0x7E, 0xBE, 0x00, 0x7E, 0xCA, 0x00, 0x7E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C,
    0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x2A, 0x29, 0x28, 0x26, 0x24, 0x22,
    0x21, 0x20, 0x1F, 0x1E, 0x1D, 0x1C, 0x18, 0x16, 0x14, 0x13,
    0x12, 0x10, 0x0F, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,          // Config data (184 bytes)
    0x00                                              // Checksum placeholder
};

/* Communication buffers */
static uint8_t TxBuffer[200];
static uint8_t RxBuffer[200];

/* Extern variables */
extern I2C_HandleTypeDef hi2c4;  // I2C handler for GT911

/* Private function prototypes -----------------------------------------------*/
static void GT911_Reset(void);
static void GT911_CalculateCheckSum(void);
static GT911_Status_t GT911_SetCommandRegister(uint8_t command);
static GT911_Status_t GT911_GetProductID(uint32_t *id);
static GT911_Status_t GT911_GetStatus(uint8_t *status);
static GT911_Status_t GT911_SetStatus(uint8_t status);
static GT911_Status_t GT911_UpdateConfigResolution(uint16_t x_res, uint16_t y_res);

/* API Implementation --------------------------------------------------------*/

/**
 * @brief Initialize GT911 with default configuration
 *
 * This function performs hardware reset, checks device communication,
 * but does NOT send configuration to the device. It assumes the device
 * already has valid configuration stored in its flash memory.
 *
 * @return GT911_Status_t Status of initialization
 */
GT911_Status_t GT911_Init(void)
{
    GT911_Status_t result;
    uint32_t product_id = 0;

    printf("[GT911] Initializing with default configuration...\n");

    // Step 1: Perform hardware reset
    printf("[GT911] Performing hardware reset...\n");
    GT911_Reset();

    // Step 2: Check device communication by reading Product ID
    printf("[GT911] Reading device ID...\n");
    result = GT911_GetProductID(&product_id);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Device not responding (I2C error)\n");
        return result;
    }

    if (product_id == 0) {
        printf("[GT911] ERROR: Invalid product ID (0x%08lX)\n", product_id);
        return GT911_NotResponse;
    }

    // Step 3: Display device information
    printf("[GT911] Device detected: Product ID = 0x%08lX\n", product_id);

    // Step 4: Final reset and set to working mode
    printf("[GT911] Finalizing initialization...\n");
    GT911_Reset();
    GT911_SetCommandRegister(GOODIX_CMD_READ);

    printf("[GT911] Initialization complete\n");
    return GT911_OK;
}

/**
 * @brief Initialize GT911 with custom configuration
 *
 * This function performs full initialization including sending
 * custom configuration to the device. Use this when you need
 * to change resolution, touch points, or other parameters.
 *
 * @param config Custom configuration structure
 * @return GT911_Status_t Status of initialization
 */
GT911_Status_t GT911_InitWithConfig(GT911_Config_t config)
{
    GT911_Status_t result;
    uint32_t product_id = 0;

    printf("[GT911] Initializing with custom configuration...\n");
    printf("[GT911] Config: X=%d, Y=%d, Touch points=%d\n",
           config.X_Resolution, config.Y_Resolution, config.Number_Of_Touch_Support);

    // Step 1: Update configuration array with new parameters
    printf("[GT911] Updating configuration parameters...\n");
    result = GT911_UpdateConfigResolution(config.X_Resolution, config.Y_Resolution);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Failed to update configuration\n");
        return result;
    }

    // Update touch points (byte 5)
    GT911_Config[4] = config.Number_Of_Touch_Support & 0x0F;

    // Update module switch 1 (byte 6)
    GT911_Config[5] = 0;
    GT911_Config[5] |= (config.ReverseY ? 1 : 0) << 7;  // Bit 7: Reverse Y
    GT911_Config[5] |= (config.ReverseX ? 1 : 0) << 6;  // Bit 6: Reverse X
    GT911_Config[5] |= (config.SwithX2Y ? 1 : 0) << 3;  // Bit 3: Switch X/Y
    GT911_Config[5] |= (config.SoftwareNoiseReduction ? 1 : 0) << 2;  // Bit 2: Noise reduction

    // Step 2: Perform hardware reset
    printf("[GT911] Performing hardware reset...\n");
    GT911_Reset();

    // Step 3: Check device communication
    printf("[GT911] Reading device ID...\n");
    result = GT911_GetProductID(&product_id);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Device not responding\n");
        return result;
    }

    if (product_id == 0) {
        printf("[GT911] ERROR: Invalid product ID\n");
        return GT911_NotResponse;
    }

    printf("[GT911] Device detected: Product ID = 0x%08lX\n", product_id);

    // Step 4: Send new configuration to device
    printf("[GT911] Sending configuration to device...\n");
    result = GT911_SendConfig();
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Failed to send configuration\n");
        return result;
    }

    // Step 5: Final reset and verification
    printf("[GT911] Verifying configuration...\n");
    GT911_Reset();

    // Set device to working mode
    GT911_SetCommandRegister(GOODIX_CMD_READ);

    printf("[GT911] Custom configuration initialization complete\n");
    return GT911_OK;
}

/**
 * @brief Read touch coordinates from GT911
 *
 * This function reads the current touch status and coordinates.
 * It supports multi-touch up to 5 points.
 *
 * @param cordinate Array to store touch coordinates
 * @param number_of_cordinate Pointer to store number of touches
 * @return GT911_Status_t Status of read operation
 */
GT911_Status_t GT911_ReadTouch(TouchCoordinate_t *cordinate, uint8_t *number_of_cordinate)
{
    uint8_t status_register;
    GT911_Status_t result;

    // Read status register to check if new data is available
    result = GT911_GetStatus(&status_register);
    if (result != GT911_OK) {
        return result;
    }

    // Check if buffer status bit is set (new data available)
    if ((status_register & 0x80) != 0) {
        // Extract number of touch points (lower 4 bits)
        *number_of_cordinate = status_register & 0x0F;

        if (*number_of_cordinate > 0 && *number_of_cordinate <= GT911_MAX_TOUCH_POINTS) {
            // Read coordinates for each touch point
            for (uint8_t i = 0; i < *number_of_cordinate; i++) {
                // Calculate address for each touch point (each point = 8 bytes)
                uint16_t point_addr = GOODIX_POINT1_X_ADDR + (i * GT911_TOUCH_POINT_SIZE);

                // Set read address
                TxBuffer[0] = (point_addr >> 8) & 0xFF;
                TxBuffer[1] = point_addr & 0xFF;

                // Write address and read 6 bytes (X_L, X_H, Y_L, Y_H, Size, Reserved)
                result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
                if (result != GT911_OK) {
                    return result;
                }

                result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 6);
                if (result != GT911_OK) {
                    return result;
                }

                // Combine low and high bytes to get coordinates
                cordinate[i].x = RxBuffer[0] | (RxBuffer[1] << 8);
                cordinate[i].y = RxBuffer[2] | (RxBuffer[3] << 8);

                // Debug output (can be disabled)
                #ifdef GT911_DEBUG
                printf("[GT911] Touch %d: X=%d, Y=%d\n", i, cordinate[i].x, cordinate[i].y);
                #endif
            }
        }

        // Clear buffer status by writing 0 to status register
        GT911_SetStatus(0x00);
    } else {
        // No new touch data
        *number_of_cordinate = 0;
    }

    return GT911_OK;
}

/**
 * @brief Read configuration from GT911 device
 *
 * Reads the complete configuration from GT911 device memory.
 * Useful for debugging or verifying current configuration.
 *
 * @param config_buffer Buffer to store configuration (min 190 bytes recommended)
 * @param max_length Maximum buffer length
 * @return GT911_Status_t Status of read operation
 */
GT911_Status_t GT911_ReadConfig(uint8_t *config_buffer, uint16_t max_length)
{
    GT911_Status_t result;
    uint16_t addr = GOODIX_REG_CONFIG_DATA;
    uint16_t total_read = 0;

    if (max_length < 190) {
        printf("[GT911] ERROR: Buffer too small for GT911 config (requires at least 190 bytes)\n");
        return GT911_Error;
    }

    printf("[GT911] Reading configuration from device...\n");

    // Read configuration data (184 bytes + checksum + fresh flag)
    while (total_read < 186 && addr <= 0x8100) {
        // Set read address
        TxBuffer[0] = (addr >> 8) & 0xFF;
        TxBuffer[1] = addr & 0xFF;

        // Write address
        result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
        if (result != GT911_OK) {
            printf("[GT911] ERROR: Address setting error at 0x%04X\n", addr);
            return result;
        }

        // Calculate how many bytes to read in this chunk
        uint8_t to_read = (total_read + 32 <= 186) ? 32 : (186 - total_read);

        // Read data chunk
        result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, to_read);
        if (result != GT911_OK) {
            printf("[GT911] ERROR: Reading error at address 0x%04X\n", addr);
            return result;
        }

        // Copy to output buffer
        memcpy(&config_buffer[total_read], RxBuffer, to_read);

        // Debug output (hex dump)
        #ifdef GT911_DEBUG
        printf("0x%04X: ", addr);
        for (uint8_t i = 0; i < to_read; i++) {
            printf("%02X ", RxBuffer[i]);
            if ((i + 1) % 16 == 0) printf("\n          ");
        }
        printf("\n");
        #endif

        total_read += to_read;
        addr += to_read;
    }

    // Read fresh flag (0x8100)
    TxBuffer[0] = 0x81;
    TxBuffer[1] = 0x00;
    result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
    if (result == GT911_OK) {
        uint8_t fresh_flag;
        result = GT911_I2C_Read(GOODIX_ADDRESS, &fresh_flag, 1);
        if (result == GT911_OK) {
            config_buffer[186] = fresh_flag;
            #ifdef GT911_DEBUG
            printf("0x8100 (Config Fresh Flag): 0x%02X\n", fresh_flag);
            #endif
        }
    }

    // Verify checksum (optional)
    uint8_t read_checksum = config_buffer[184];
    uint8_t calc_checksum = 0;

    for (uint16_t i = 0; i < 184; i++) {
        calc_checksum += config_buffer[i];
    }
    calc_checksum = (~calc_checksum) + 1;

    printf("[GT911] Configuration read: %d bytes\n", total_read);
    printf("[GT911] Checksum: read=0x%02X, calculated=0x%02X (%s)\n",
           read_checksum, calc_checksum,
           (read_checksum == calc_checksum) ? "OK" : "MISMATCH");

    return GT911_OK;
}

/**
 * @brief Perform hardware calibration of GT911
 *
 * Executes the benchmark calibration procedure.
 * Note: Screen should not be touched during calibration.
 *
 * @return GT911_Status_t Status of calibration
 */
GT911_Status_t GT911_Calibrate(void)
{
    uint8_t mode;
    uint32_t timeout = 0;

    printf("[GT911] Starting benchmark calibration...\n");

    // Step 1: Write command to command register
    TxBuffer[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_COMMAND & 0xFF;
    TxBuffer[2] = GOODIX_CMD_CALIBRATE;

    GT911_Status_t result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Failed to write calibration command\n");
        return result;
    }

    printf("[GT911] Calibration started, waiting for completion...\n");

    // Step 2: Wait for calibration to complete
    do {
        // Read current command register value
        TxBuffer[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
        TxBuffer[1] = GOODIX_REG_COMMAND & 0xFF;

        result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
        if (result != GT911_OK) {
            printf("[GT911] ERROR: Failed to read command register\n");
            return result;
        }

        result = GT911_I2C_Read(GOODIX_ADDRESS, &mode, 1);
        if (result != GT911_OK) {
            printf("[GT911] ERROR: Failed to read command register data\n");
            return result;
        }

        // Check if calibration is complete (command register returns to 0x00)
        if (mode == GOODIX_CMD_READ) {
            printf("[GT911] Calibration completed successfully!\n");
            return GT911_OK;
        }

        // Wait before checking again
        HAL_Delay(200);
        timeout += 200;

        // Progress indication
        if (timeout % 2000 == 0) {
            printf("[GT911] Calibrating... (%lu ms elapsed)\n", timeout);
        }

    } while (timeout < 20000);  // Timeout after 20 seconds

    printf("[GT911] ERROR: Calibration timeout! Last mode: 0x%02X\n", mode);
    return GT911_Error;
}

/**
 * @brief Set device mode
 *
 * @param mode Device mode (0x00 = working, 0x01 = sleep, 0x04 = factory)
 * @return GT911_Status_t Status of operation
 */
GT911_Status_t GT911_SetDeviceMode(uint8_t mode)
{
    uint8_t tx[3];
    tx[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
    tx[1] = GOODIX_REG_COMMAND & 0xFF;
    tx[2] = mode;

    return GT911_I2C_Write(GOODIX_ADDRESS, tx, 3);
}

/**
 * @brief Get current device mode
 *
 * @param mode Pointer to store device mode
 * @return GT911_Status_t Status of operation
 */
GT911_Status_t GT911_GetDeviceMode(uint8_t *mode)
{
    uint8_t tx[2];
    tx[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
    tx[1] = GOODIX_REG_COMMAND & 0xFF;

    GT911_Status_t res = GT911_I2C_Write(GOODIX_ADDRESS, tx, 2);
    if (res != GT911_OK) {
        return res;
    }

    return GT911_I2C_Read(GOODIX_ADDRESS, mode, 1);
}

/**
 * @brief Perform software reset of GT911
 *
 * @return GT911_Status_t Status of reset
 */
GT911_Status_t GT911_SoftReset(void)
{
    printf("[GT911] Performing software reset...\n");

    TxBuffer[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_COMMAND & 0xFF;
    TxBuffer[2] = GOODIX_CMD_SOFTRESET;

    GT911_Status_t result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Software reset failed\n");
        return result;
    }

    // Wait for reset to complete
    GT911_Delay(100);

    printf("[GT911] Software reset completed\n");
    return GT911_OK;
}

/* Private functions Implementation -----------------------------------------*/

/**
 * @brief Perform hardware reset of GT911
 *
 * Resets the device using RST and INT pins according to GT911 datasheet.
 * Timing is critical for proper initialization.
 */
static void GT911_Reset(void)
{
    // Configure INT pin as input to detect device ready
    GT911_INT_Input();

    // Pull RST low to start reset sequence
    GT911_RST_Control(0);
    GT911_Delay(20);        // Wait 20ms (datasheet: >5ms)

    // Pull INT low (configured as output)
    GT911_INT_Control(0);
    GT911_Delay(50);        // Wait 50ms

    // Release RST (pull high)
    GT911_RST_Control(1);
    GT911_Delay(100);       // Wait 100ms for device to initialize

    // Configure INT as output for normal operation
    GT911_INT_Output();
    GT911_Delay(100);       // Final stabilization delay
}

/**
 * @brief Calculate checksum for configuration data
 *
 * Calculates 8-bit checksum for first 184 bytes of configuration.
 * Checksum is stored at position 184.
 */
static void GT911_CalculateCheckSum(void)
{
    uint8_t checksum = 0;

    // Sum all configuration bytes (0-183)
    for (uint8_t i = 0; i < CONFIG_DATA_SIZE; i++) {
        checksum += GT911_Config[i];
    }

    // Two's complement of sum
    GT911_Config[CONFIG_CHECKSUM_POS] = (~checksum) + 1;

    #ifdef GT911_DEBUG
    printf("[GT911] Calculated checksum: 0x%02X\n", GT911_Config[CONFIG_CHECKSUM_POS]);
    #endif
}

/**
 * @brief Update resolution in configuration array
 *
 * @param x_res X resolution
 * @param y_res Y resolution
 * @return GT911_Status_t Status of operation
 */
static GT911_Status_t GT911_UpdateConfigResolution(uint16_t x_res, uint16_t y_res)
{
    if (x_res == 0 || y_res == 0) {
        return GT911_Error;
    }

    // Update X resolution (bytes 0-1, little-endian)
    GT911_Config[0] = x_res & 0xFF;
    GT911_Config[1] = (x_res >> 8) & 0xFF;

    // Update Y resolution (bytes 2-3, little-endian)
    GT911_Config[2] = y_res & 0xFF;
    GT911_Config[3] = (y_res >> 8) & 0xFF;

    // Recalculate checksum after modification
    GT911_CalculateCheckSum();

    return GT911_OK;
}

/**
 * @brief Send configuration to GT911 device
 *
 * @return GT911_Status_t Status of operation
 */
GT911_Status_t GT911_SendConfig(void)
{
    GT911_Status_t result;

    // Calculate checksum before sending
    GT911_CalculateCheckSum();

    // Set configuration data address
    TxBuffer[0] = (GOODIX_REG_CONFIG_DATA >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_CONFIG_DATA & 0xFF;

    // Copy configuration to transmit buffer (skip fresh flag at position 185)
    memcpy(&TxBuffer[2], GT911_Config, CONFIG_DATA_SIZE + 1); // +1 for checksum

    // Write configuration to device
    result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, CONFIG_DATA_SIZE + 1 + 2);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Failed to write configuration\n");
        return result;
    }

    // Set fresh flag to activate new configuration
    TxBuffer[0] = (GOODIX_REG_CONFIG_FRESH >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_CONFIG_FRESH & 0xFF;
    TxBuffer[2] = CONFIG_FRESH_FLAG;

    result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
    if (result != GT911_OK) {
        printf("[GT911] ERROR: Failed to set fresh flag\n");
        return result;
    }

    printf("[GT911] Configuration sent successfully\n");
    return GT911_OK;
}

/**
 * @brief Write command to command register
 *
 * @param command Command to write
 * @return GT911_Status_t Status of operation
 */
static GT911_Status_t GT911_SetCommandRegister(uint8_t command)
{
    TxBuffer[0] = (GOODIX_REG_COMMAND >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_COMMAND & 0xFF;
    TxBuffer[2] = command;

    return GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
}

/**
 * @brief Read Product ID from GT911
 *
 * @param id Pointer to store Product ID (4 bytes)
 * @return GT911_Status_t Status of operation
 */
static GT911_Status_t GT911_GetProductID(uint32_t *id)
{
    GT911_Status_t result;

    // Set address to Product ID register
    TxBuffer[0] = (GOODIX_REG_ID >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_REG_ID & 0xFF;

    // Write address and read 4 bytes
    result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
    if (result != GT911_OK) {
        return result;
    }

    result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 4);
    if (result == GT911_OK) {
        *id = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
    }

    return result;
}

/**
 * @brief Read status register from GT911
 *
 * @param status Pointer to store status byte
 * @return GT911_Status_t Status of operation
 */
static GT911_Status_t GT911_GetStatus(uint8_t *status)
{
    GT911_Status_t result;

    // Set address to status register
    TxBuffer[0] = (GOODIX_READ_COORD_ADDR >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_READ_COORD_ADDR & 0xFF;

    // Write address and read 1 byte
    result = GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 2);
    if (result != GT911_OK) {
        return result;
    }

    result = GT911_I2C_Read(GOODIX_ADDRESS, RxBuffer, 1);
    if (result == GT911_OK) {
        *status = RxBuffer[0];
    }

    return result;
}

/**
 * @brief Write to status register (clear buffer)
 *
 * @param status Value to write (usually 0x00 to clear buffer)
 * @return GT911_Status_t Status of operation
 */
static GT911_Status_t GT911_SetStatus(uint8_t status)
{
    TxBuffer[0] = (GOODIX_READ_COORD_ADDR >> 8) & 0xFF;
    TxBuffer[1] = GOODIX_READ_COORD_ADDR & 0xFF;
    TxBuffer[2] = status;

    return GT911_I2C_Write(GOODIX_ADDRESS, TxBuffer, 3);
}
