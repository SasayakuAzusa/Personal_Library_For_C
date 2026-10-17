/* This is library Header for module MAX7219 (led matrix) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

#pragma once

/* Include available libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define constants */
/** * @brief  (Max7219 Regigter Map definition) */
typedef enum {
    MAX7219_REG_NO_OP            = 0x00U,
    MAX7219_REG_DIGIT_0          = 0x01U,
    MAX7219_REG_DIGIT_1          = 0x02U,
    MAX7219_REG_DIGIT_2          = 0x03U,
    MAX7219_REG_DIGIT_3          = 0x04U,
    MAX7219_REG_DIGIT_4          = 0x05U,
    MAX7219_REG_DIGIT_5          = 0x06U,
    MAX7219_REG_DIGIT_6          = 0x07U,
    MAX7219_REG_DIGIT_7          = 0x08U,
    MAX7219_REG_DECODE_MODE      = 0x09U,
    MAX7219_REG_INTENSITY        = 0x0AU,
    MAX7219_REG_SCAN_LIMIT       = 0x0BU,
    MAX7219_REG_SHUTDOWN         = 0x0CU,
    MAX7219_REG_DISPLAY_TEST     = 0x0FU
} MAX7219_RegisterTypeDef;

/** * @brief  (Max7219 display-test definition) */
typedef enum {
    MAX7219_DISPLAY_TEST_NORMAL     = 0x00U,    // Display normal operation
    MAX7219_TEST_DSP_TEST           = 0x01U,    // Display test mode
} MAX7219_DisplayTestTypeDef;

/** * @brief  (Max7219 shutdown mode definition) */
typedef enum {
    MAX7219_SHUTDOWN_MODE_SHUTDOWN  = 0x00U,    // Shutdown mode
    MAX7219_SHUTDOWN_MODE_NORMAL    = 0x01U,    // Normal operation
} MAX7219_ShutdownModeTypeDef;

/** * @brief  (Max7219 decode mode definition) */
typedef enum {
    MAX7219_DECODE_NONE             = 0x00U,    // No decode dor all digits
    MAX7219_DECODE_0                = 0x01U,    // Code BCD decode for digit 0
    MAX7219_DECODE_ALL              = 0xFFU,    // Code BCD decode all digits
} MAX7219_DecodeModeTypeDef;

/** * @brief  (Max7219 intensity definition) */
typedef enum {
    MAX7219_INTENSITY_0             = 0x00U,    // Turn off brightness
    MAX7219_INTENSITY_MAX           = 0x0FU,    // Show highest brightness
} MAX7219_IntensityTypeDef;
/** * @brief  (Max7219 scan limit definition) */
typedef enum {
    MAX7219_SCAN_LIMIT_0    = 0x00U,    // Display digit 0 only
    MAX7219_SCAN_LIMIT_0_1  = 0x01U,    // Display digits 0 and 1
    MAX7219_SCAN_LIMIT_0_2  = 0x02U,    // Display digits 0 to 2
    MAX7219_SCAN_LIMIT_0_3  = 0x03U,    // Display digits 0 to 3
    MAX7219_SCAN_LIMIT_0_4  = 0x04U,    // Display digits 0 to 4
    MAX7219_SCAN_LIMIT_0_5  = 0x05U,    // Display digits 0 to 5
    MAX7219_SCAN_LIMIT_0_6  = 0x06U,    // Display digits 0 to 6
    MAX7219_SCAN_LIMIT_0_7  = 0x07U,    // Display digits 0 to 7
} MAX7219_ScanLimitTypeDef;

/** 
 * @brief  Callback function type for custom decoding.
 * @param  c The character to decode.
 * @return 8-bit Hex code for the Display.
 */
typedef uint8_t (*MAX7219_CustomCharCb)(char c);

typedef struct {
    /* Hardware Config */
    uint8_t                         spiChannel;     // SPI channel (0 or 1)
    uint32_t                        spiSpeed;       // SPI speed in Hz (Max 10MHz)
    MAX7219_IntensityTypeDef        intensity;      // Current Brightness (0x00 -> 0x0F)
    MAX7219_ScanLimitTypeDef        scanLimit;      // Default 0x07 for 8 digits
    MAX7219_DecodeModeTypeDef       decodeMode;     // Decode Mode

    /* RAM Buffer */
    uint8_t     frameBuffer[8U];    // RAM buffer for 8 LED digits or 8 Matrix rows
    uint8_t     shadowBuffer[8U];   // Shadow buffer to save current hardware status
    
    /* Callback*/
    MAX7219_CustomCharCb            customFontCb;   // Optional Char Callback
} MAX7219_HandleTypeDef;

/* Exported Macros */
#define MAX7219_BLANK       0x00U // All Leds turn off in Decode Mode

/* Function Prototypes */
    /* Initial and Control API */
int MAX7219_Init(MAX7219_HandleTypeDef *hmax);
void MAX7219_SetDecodeMode(MAX7219_HandleTypeDef *hmax, MAX7219_DecodeModeTypeDef mode);
void MAX7219_SetIntensity(MAX7219_HandleTypeDef *hmax, MAX7219_IntensityTypeDef intensity);
void MAX7219_SetScanLimit(MAX7219_HandleTypeDef *hmax, MAX7219_ScanLimitTypeDef scanLimit);
void MAX7219_SetDisplayTest(MAX7219_HandleTypeDef *hmax, MAX7219_DisplayTestTypeDef displayTest);
void MAX7219_SetShutdownMode(MAX7219_HandleTypeDef *hmax, MAX7219_ShutdownModeTypeDef shutdown);

    /* Low-level Hardware API */
void MAX7219_SendData(MAX7219_HandleTypeDef *hmax, uint8_t address, uint8_t data);
void MAX7219_Update(MAX7219_HandleTypeDef *hmax);
void MAX7219_Clear(MAX7219_HandleTypeDef *hmax);

/* Led Matrix 8x8 Features */
void MAX7219_DrawPixel(MAX7219_HandleTypeDef *hmax, uint8_t x, uint8_t y, bool state);
void MAX7219_DrawMatrixChar(MAX7219_HandleTypeDef *hmax, char c);
void MAX7219_ShiftLeft(MAX7219_HandleTypeDef *hmax, bool cycle);
void MAX7219_ShiftRight(MAX7219_HandleTypeDef *hmax, bool cycle);
void MAX7219_ShiftUp(MAX7219_HandleTypeDef *hmax, bool cycle);
void MAX7219_ShiftDown(MAX7219_HandleTypeDef *hmax, bool cycle);

#ifdef __cplusplus
}
#endif
