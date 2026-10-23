/* This is Library for module DS3231 (real time module) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

/* Include Libraries */
#include "DS3231.h"
#include <stddef.h>
#include <wiringPiI2C.h>

/* Private Macros */
/** @brief Convert from Decimal to BCD */
#define DEC2BCD(val)    ((uint8_t)((((val) / 10U) << 4U) | ((val) % 10U)))

/** @brief Convert from BCD to Decimal */
#define BCD2DEC(val)    ((uint8_t)((((val) >> 4U) * 10U) + ((val) & 0x0FU)))

/* API functions */
/**
* @brief Initializes DS3231 module via I2C
* @param hds: Pointer for DS3231 module struct
* @retval 0 if successful, -1 if I2C setup fails.
*/
int DS3231_Init(DS3231_HandleTypeDef *hds) {
    hds->i2cFd = wiringPiI2CSetup((int)hds->deviceAddress);
    if (hds->i2cFd < 0) {
        return -1;
    }

    /* Ensure the oscillator always running */
    uint8_t ctrl = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_CONTROL);
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_CONTROL, (int)(ctrl & (uint8_t)(~DS3231_CTRL_EOSC)));

    return 0;
}

/**
 * @brief  Checks if the RTC has lost power (battery removed/dead).
 *         Automatically clears the error flag if detected.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @retval true if power was lost (time is inaccurate), false otherwise.
 */
bool DS3231_CheckLostPower(DS3231_HandleTypeDef *hds) {
    uint8_t status = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_STATUS);
    if ((status & DS3231_STAT_OSF) != 0U) {
        /* Clear the error flag after detection to prepare for the next power loss */
        wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_STATUS, (int)(status & (uint8_t)(~DS3231_STAT_OSF)));
        return true;
    }
    return false;
}

/* Time & Date API */
/**
 * @brief  Sets the current time and date to the DS3231 hardware.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  sTime: Pointer to the time structure containing target time.
 */
void DS3231_SetTime(DS3231_HandleTypeDef *hds, const DS3231_TimeTypeDef *sTime) {
    if (sTime == NULL) { return; }

    uint8_t hourReg = DEC2BCD(sTime->hours);

    /* Apply 12H/24H and AM/PM configurations */
    if (sTime->format == DS3231_FORMAT_12H) {
        hourReg |= DS3231_12H_FLAG;
        if (sTime->amPm == DS3231_PM) {
            hourReg |= DS3231_PM_FLAG;
        }
    } else {
        hourReg &= (uint8_t)(~DS3231_12H_FLAG); /* Ensure bit 6 is cleared for 24H mode */
    }

    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_SECONDS, (int)DEC2BCD(sTime->seconds));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_MINUTES, (int)DEC2BCD(sTime->minutes));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_HOURS,   (int)hourReg);
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_DAY,     (int)DEC2BCD(sTime->day));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_DATE,    (int)DEC2BCD(sTime->date));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_MONTH,   (int)DEC2BCD(sTime->month)); 
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_YEAR,    (int)DEC2BCD(sTime->year));
}

/**
 * @brief  Reads the current time and date from the DS3231 hardware.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  sTime: Pointer to the time structure to store read values.
 */
void DS3231_GetTime(DS3231_HandleTypeDef *hds, DS3231_TimeTypeDef *sTime) {
    if (sTime == NULL) { return; }

    uint8_t rawSec  = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_SECONDS);
    uint8_t rawMin  = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_MINUTES);
    uint8_t rawHour = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_HOURS);
    uint8_t rawDay  = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_DAY);
    uint8_t rawDate = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_DATE);
    uint8_t rawMon  = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_MONTH);
    uint8_t rawYear = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_YEAR);

    sTime->seconds = BCD2DEC(rawSec & 0x7FU);
    sTime->minutes = BCD2DEC(rawMin & 0x7FU);

    /* Decode Hours based on 12H_FLAG */
    if ((rawHour & DS3231_12H_FLAG) != 0U) {
        sTime->format = DS3231_FORMAT_12H;
        sTime->amPm   = ((rawHour & DS3231_PM_FLAG) != 0U) ? DS3231_PM : DS3231_AM;
        sTime->hours  = BCD2DEC(rawHour & 0x1FU); /* Extract lower 5 bits */
    } else {
        sTime->format = DS3231_FORMAT_24H;
        sTime->amPm   = DS3231_AM; 
        sTime->hours  = BCD2DEC(rawHour & 0x3FU); /* Extract lower 6 bits */
    }

    sTime->day   = BCD2DEC(rawDay  & 0x07U);
    sTime->date  = BCD2DEC(rawDate & 0x3FU);
    sTime->month = BCD2DEC(rawMon  & 0x1FU);      /* Ignore bit 7 (Century flag) */
    sTime->year  = BCD2DEC(rawYear);
}

/**
 * @brief  Reads the current internal ambient temperature from the DS3231.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @retval Temperature in Celsius.
 */
float DS3231_GetTemperature(DS3231_HandleTypeDef *hds) {
    int8_t  msb = (int8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_TEMP_MSB);
    uint8_t lsb = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_TEMP_LSB);
    return (float)msb + ((float)(lsb >> 6U) * 0.25f);
}

/* --- Alarms API --- */
/**
 * @brief  Sets Alarm 1 time and matching conditions.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  sAlarm: Pointer to Alarm 1 configuration structure.
 */
void DS3231_SetAlarm1(DS3231_HandleTypeDef *hds, const DS3231_Alarm1TypeDef *sAlarm) {
    if (sAlarm == NULL) { return; }

    /* Calculate Masks (A1M1 -> A1M4) from Mode configuration */
    uint8_t mSec = (sAlarm->mode & 0x01U) ? 0x80U : 0x00U;
    uint8_t mMin = (sAlarm->mode & 0x02U) ? 0x80U : 0x00U;
    uint8_t mHr  = (sAlarm->mode & 0x04U) ? 0x80U : 0x00U;
    uint8_t mDay = (sAlarm->mode & 0x08U) ? 0x80U : 0x00U;
    uint8_t dydt = (sAlarm->mode & 0x10U) ? 0x40U : 0x00U; /* Flag to choose Day or Date */

    uint8_t hourReg = DEC2BCD(sAlarm->hours);
    if (sAlarm->format == DS3231_FORMAT_12H) {
        hourReg |= DS3231_12H_FLAG;
        if (sAlarm->amPm == DS3231_PM) { hourReg |= DS3231_PM_FLAG; }
    } else {
        hourReg &= (uint8_t)(~DS3231_12H_FLAG);
    }

    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A1_SEC,  (int)(mSec | DEC2BCD(sAlarm->seconds)));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A1_MIN,  (int)(mMin | DEC2BCD(sAlarm->minutes)));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A1_HOUR, (int)(mHr  | hourReg));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A1_DAY,  (int)(mDay | dydt | DEC2BCD(sAlarm->dayOrDate)));
}

/**
 * @brief  Sets Alarm 2 time and matching conditions.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  sAlarm: Pointer to Alarm 2 configuration structure.
 */
void DS3231_SetAlarm2(DS3231_HandleTypeDef *hds, const DS3231_Alarm2TypeDef *sAlarm) {
    if (sAlarm == NULL) { return; }

    uint8_t mMin = (sAlarm->mode & 0x01U) ? 0x80U : 0x00U;
    uint8_t mHr  = (sAlarm->mode & 0x02U) ? 0x80U : 0x00U;
    uint8_t mDay = (sAlarm->mode & 0x04U) ? 0x80U : 0x00U;
    uint8_t dydt = (sAlarm->mode & 0x08U) ? 0x40U : 0x00U; 

    uint8_t hourReg = DEC2BCD(sAlarm->hours);
    if (sAlarm->format == DS3231_FORMAT_12H) {
        hourReg |= DS3231_12H_FLAG;
        if (sAlarm->amPm == DS3231_PM) { hourReg |= DS3231_PM_FLAG; }
    } else {
        hourReg &= (uint8_t)(~DS3231_12H_FLAG);
    }

    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A2_MIN,  (int)(mMin | DEC2BCD(sAlarm->minutes)));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A2_HOUR, (int)(mHr  | hourReg));
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_A2_DAY,  (int)(mDay | dydt | DEC2BCD(sAlarm->dayOrDate)));
}

/**
 * @brief  Configures the SQW/INT pin to act as an Interrupt for Alarms.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  enableA1: Set true to enable INT pin output for Alarm 1.
 * @param  enableA2: Set true to enable INT pin output for Alarm 2.
 */
void DS3231_EnableAlarmInterrupts(DS3231_HandleTypeDef *hds, bool enableA1, bool enableA2) {
    uint8_t ctrl = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_CONTROL);
    
    ctrl |= DS3231_CTRL_INTCN; /* Convert SQW pin to INT (Interrupt) pin */

    if (enableA1) { 
        ctrl |= DS3231_CTRL_A1IE; 
    } else { 
        ctrl &= (uint8_t)(~DS3231_CTRL_A1IE); 
    }

    if (enableA2) { 
        ctrl |= DS3231_CTRL_A2IE; 
    } else { 
        ctrl &= (uint8_t)(~DS3231_CTRL_A2IE); 
    }

    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_CONTROL, (int)ctrl);
}

/**
 * @brief  Configures the SQW/INT pin to output a Square Wave.
 *         (Disables Alarm interrupts on that pin).
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  freq: Desired Square Wave frequency.
 */
void DS3231_EnableSquareWave(DS3231_HandleTypeDef *hds, DS3231_SQWFreqTypeDef freq) {
    uint8_t ctrl = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_CONTROL);
    
    ctrl &= (uint8_t)(~DS3231_CTRL_INTCN);  /* Disable Alarm interrupt to return pin to SQW output */
    ctrl &= 0xE7U;                          /* Clear RS1 and RS2 bits (Bits 3 and 4) */
    ctrl |= (uint8_t)freq;                  /* Apply new frequency */

    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_CONTROL, (int)ctrl);
}

/* --- Check and Clear Flags --- */
/**
 * @brief  Checks if Alarm 1 has triggered.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @retval true if Alarm 1 triggered, false otherwise.
 */
bool DS3231_CheckAlarm1Flag(DS3231_HandleTypeDef *hds) {
    uint8_t status = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_STATUS);
    return (status & 0x01U) != 0U;
}

/**
 * @brief  Checks if Alarm 2 has triggered.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @retval true if Alarm 2 triggered, false otherwise.
 */
bool DS3231_CheckAlarm2Flag(DS3231_HandleTypeDef *hds) {
    uint8_t status = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_STATUS);
    return (status & 0x02U) != 0U;
}

/**
 * @brief  Clears the Alarm 1 flag. Must be called after processing an Alarm 1 event.
 * @param  hds: Pointer to the DS3231 handle structure.
 */
void DS3231_ClearAlarm1Flag(DS3231_HandleTypeDef *hds) {
    uint8_t status = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_STATUS);
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_STATUS, (int)(status & 0xFEU)); /* Clear bit 0 */
}

/**
 * @brief  Clears the Alarm 2 flag. Must be called after processing an Alarm 2 event.
 * @param  hds: Pointer to the DS3231 handle structure.
 */
void DS3231_ClearAlarm2Flag(DS3231_HandleTypeDef *hds) {
    uint8_t status = (uint8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_STATUS);
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_STATUS, (int)(status & 0xFDU)); /* Clear bit 1 */
}
/* --- Aging Offset API --- */
/**
 * @brief  Sets the aging offset register to calibrate RTC frequency.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @param  offset: Signed 8-bit offset value.
 */
void DS3231_SetAgingOffset(DS3231_HandleTypeDef *hds, int8_t offset) {
    if (hds == NULL) { return; }
    wiringPiI2CWriteReg8(hds->i2cFd, (int)DS3231_REG_AGING_OFFSET, (int)((uint8_t)offset));
}

/**
 * @brief  Reads the current aging offset value from the DS3231.
 * @param  hds: Pointer to the DS3231 handle structure.
 * @retval Signed 8-bit aging offset value.
 */
int8_t DS3231_GetAgingOffset(DS3231_HandleTypeDef *hds) {
    if (hds == NULL) { return 0; }
    return (int8_t)wiringPiI2CReadReg8(hds->i2cFd, (int)DS3231_REG_AGING_OFFSET);
}
