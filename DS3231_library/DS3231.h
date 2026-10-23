/* This is labrary Header for module DS3231 (real time module) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

#pragma once

/* Include libraries */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define constants */
#define DS3231_DEFAULT_ADDRESS      0x68U   // Default I2C address for DS3231
#define DS3231_12H_FLAG             0x40U   // Bit 6: 1 = 12H mode, 2 = 24H mode
#define DS3231_PM_FLAG              0x20U   // Bit 5: 1 = PM, 2 = AM (use in 12H mode)

#define DS3231_CTRL_EOSC            0x80U   // Enable Oscillator (0 = Ena, 1 = Disa)
#define DS3231_CTRL_INTCN           0x04U   // Interupt Control
#define DS3231_CTRL_A2IE            0x02U   // Alarm 2 Interupt enable
#define DS3231_CTRL_A1IE            0x01U   // Alarm 1 Interupt enable
#define DS3231_STAT_OSF             0x80U   // Oscillator Stop Flag

/** @brief (DS3231 Interal Register Map definition) */
typedef enum {
    DS3231_REG_SECONDS          = 0x00U,
    DS3231_REG_MINUTES          = 0x01U,
    DS3231_REG_HOURS            = 0x02U,
    DS3231_REG_DAY              = 0x03U,
    DS3231_REG_DATE             = 0x04U,
    DS3231_REG_MONTH            = 0x05U,
    DS3231_REG_YEAR             = 0x06U,
    DS3231_REG_A1_SEC           = 0x07U,
    DS3231_REG_A1_MIN           = 0x08U,
    DS3231_REG_A1_HOUR          = 0x09U,
    DS3231_REG_A1_DAY           = 0x0AU,
    DS3231_REG_A2_MIN           = 0x0BU,
    DS3231_REG_A2_HOUR          = 0x0CU,
    DS3231_REG_A2_DAY           = 0x0DU,
    DS3231_REG_CONTROL          = 0x0EU,
    DS3231_REG_STATUS           = 0x0FU,
    DS3231_REG_AGING_OFFSET     = 0x10U,
    DS3231_REG_TEMP_MSB         = 0x11U,
    DS3231_REG_TEMP_LSB         = 0x12U
} DS3231_RegisterTypeDef;

/** @brief Time format selection */
typedef enum {
    DS3231_FORMAT_24H = 0U, // Format 24H
    DS3231_FORMAT_12H = 1U, // Format 12H
} DS3231_TimeFormatTypeDef;

/** @brief AM/PM Format for 12H Format */
typedef enum {
    DS3231_AM = 0U, // Format AM
    DS3231_PM = 1U, // Format PM
} DS3231_AmPmTypeDef;

/** @brief Square Wave Output */
typedef enum {
    DS3231_SQW_1HZ      = 0x00U,    // 1 Hz Output
    DS3231_SQW_1024HZ   = 0x08U,    // 1024 Hz Output
    DS3231_SQW_4096HZ   = 0x10U,    // 4096 Hz Output
    DS3231_SQW_8192HZ   = 0x18U,    // 8192 Hz Output
} DS3231_SQWFreqTypeDef;

/** @brief Alarm 1 Trigger Modes */
typedef enum {
    DS3231_A1_EVERY_SECOND          = 0x0FU, // Alarm triggers every second
    DS3231_A1_MATCH_SEC             = 0x0EU, // Alarm triggers when Seconds matc
    DS3231_A1_MATCH_SEC_MIN         = 0x0CU, // Alarm triggers when Minutes and Seconds match 
    DS3231_A1_MATCH_SEC_MIN_HOUR    = 0x08U, // Alarm triggers when Hours, Minutes, and Seconds match
    DS3231_A1_MATCH_DATE_HR_MIN_SEC = 0x00U, // Alarm triggers when Date, Hours, Minutes, and Seconds match
    DS3231_A1_MATCH_DAY_HR_MIN_SEC  = 0x10U  // Alarm triggers when Day of Week, Hours, Minutes, and Seconds match
} DS3231_Alarm1ModeTypeDef;

/** @brief Alarm 2 Trigger Modes */
typedef enum {
    DS3231_A2_EVERY_MINUTE          = 0x07U, // Alarm triggers every minute (at 00 seconds)
    DS3231_A2_MATCH_MIN             = 0x06U, // Alarm triggers when Minutes match
    DS3231_A2_MATCH_MIN_HOUR        = 0x04U, // Alarm triggers when Hours and Minutes match
    DS3231_A2_MATCH_DATE_HR_MIN     = 0x00U, // Alarm triggers when Date, Hours, and Minutes match
    DS3231_A2_MATCH_DAY_HR_MIN      = 0x08U  // Alarm triggers when Day of Week, Hours, and Minutes match
} DS3231_Alarm2ModeTypeDef;

/** @brief Time structure supporting both 12H and 24H */
typedef struct {
    uint8_t seconds;                    // Seconds: 0-59
    uint8_t minutes;                    // Minutes: 0-59
    uint8_t hours;                      // Hours: 1-12 (12H mode) or 0-23 (24H mode)
    uint8_t day;                        // Day of Week: 1-7 (User defined, e.g., 1=Mon)
    uint8_t date;                       // Date of Month: 1-31
    uint8_t month;                      // Month: 1-12
    uint8_t year;                       // Year: 0-99 (represents 2000-2099)
    DS3231_TimeFormatTypeDef format;    // Time format: 24H or 12H
    DS3231_AmPmTypeDef amPm;            // AM/PM status: Ignored if format is 24H
} DS3231_TimeTypeDef;

/** @brief Configuration structure for alarm 1 */
typedef struct {
    uint8_t seconds;                    // Trigger Seconds
    uint8_t minutes;                    // Trigger Minutes
    uint8_t hours;                      // Trigger Hours
    uint8_t dayOrDate;                  // Trigger Day or Date (depends on mode)
    DS3231_TimeFormatTypeDef format;    // Alarm time format
    DS3231_AmPmTypeDef amPm;            // Alarm AM/PM status
    DS3231_Alarm1ModeTypeDef mode;      // Alarm 1 matching condition
} DS3231_Alarm1TypeDef;

/** @brief Configuration structure for alarm 2 */
typedef struct {
    uint8_t minutes;                    // Trigger Minutes
    uint8_t hours;                      // Trigger Hours
    uint8_t dayOrDate;                  // Trigger Day or Date (depends on mode)
    DS3231_TimeFormatTypeDef format;    // Alarm time format
    DS3231_AmPmTypeDef amPm;            // Alarm AM/PM status
    DS3231_Alarm2ModeTypeDef mode;      // Alarm 2 matching condition
} DS3231_Alarm2TypeDef;

/** @brief DS3231 Main Object Handle */
typedef struct {
    uint8_t deviceAddress;              // I2C address of the DS3231
    int     i2cFd;                      // OS-assigned I2C file descriptor
} DS3231_HandleTypeDef;

/* Function Prototypes */
    /* Initialization & Status API */
int  DS3231_Init(DS3231_HandleTypeDef *hds);
bool DS3231_CheckLostPower(DS3231_HandleTypeDef *hds);

    /* RTC Core API */
bool   DS3231_SetTime(DS3231_HandleTypeDef *hds, const DS3231_TimeTypeDef *sTime);
bool   DS3231_GetTime(DS3231_HandleTypeDef *hds, DS3231_TimeTypeDef *sTime);
float  DS3231_GetTemperature(DS3231_HandleTypeDef *hds);
void   DS3231_SetAgingOffset(DS3231_HandleTypeDef *hds, int8_t offset);
int8_t DS3231_GetAgingOffset(DS3231_HandleTypeDef *hds);

    /* Alarm & Interrupt API */
void DS3231_SetAlarm1(DS3231_HandleTypeDef *hds, const DS3231_Alarm1TypeDef *sAlarm);
void DS3231_SetAlarm2(DS3231_HandleTypeDef *hds, const DS3231_Alarm2TypeDef *sAlarm);
void DS3231_EnableAlarmInterrupts(DS3231_HandleTypeDef *hds, bool enableA1, bool enableA2);
void DS3231_EnableSquareWave(DS3231_HandleTypeDef *hds, DS3231_SQWFreqTypeDef freq);

bool DS3231_CheckAlarm1Flag(DS3231_HandleTypeDef *hds);
bool DS3231_CheckAlarm2Flag(DS3231_HandleTypeDef *hds);
void DS3231_ClearAlarm1Flag(DS3231_HandleTypeDef *hds);
void DS3231_ClearAlarm2Flag(DS3231_HandleTypeDef *hds);

#ifdef __cplusplus
}
#endif
