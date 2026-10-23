/* This is Library for module MAX7219 (led matrix and 7-digit led) */
/* Author: Trần Lê Nhật Đoan */
/*------------------------------------------*/

#include "Max7219.h"
#include <wiringPiSPI.h>

/* Checking pointer valid */
#define CHECK_POINTER_INT(ptr) if ((ptr) == NULL) {return -1;}  // Checking for int function
#define CHECK_POINTER_VOID(ptr) if ((ptr) == NULL) {return ;}   // Checking for void function

/* Operate functions */

    /* Set up MAX7219 hardware */
int MAX7219_Init(MAX7219_HandleTypeDef *hmax)
{
    CHECK_POINTER_INT(hmax);

    /* Set struct with pointer(hmax) */
    hmax->spiChannel    = 0U;
    hmax->spiSpeed      = 1000000U;
    hmax->intensity     = 0x08U;
    hmax->scanLimit     = 0x07;
    hmax->decodeMode    = 0xFFU;

    /* Setting for MAX7219 */
    MAX7219_SetShutdownMode(hmax, MAX7219_SHUTDOWN_MODE_NORMAL);
    MAX7219_SetDecodeMode(hmax, hmax->decodeMode);
    MAX7219_SetIntensity(hmax, hmax->intensity);
    MAX7219_SetScanLimit(hmax, hmax->scanLimit);
    MAX7219_Clear(hmax);

    return 0;
}

    /* Initial and Control API */
void MAX7219_SetIntensity(MAX7219_HandleTypeDef *hmax, MAX7219_IntensityTypeDef intensity)
{
    CHECK_POINTER_VOID(hmax);
    MAX7219_SendData(hmax, MAX7219_REG_INTENSITY, (uint8_t)intensity);
}

void MAX7219_SetDecodeMode(MAX7219_HandleTypeDef *hmax, MAX7219_DecodeModeTypeDef mode)
{
    CHECK_POINTER_VOID(hmax);
    MAX7219_SendData(hmax, MAX7219_REG_DECODE_MODE, (uint8_t)mode);
}

void MAX7219_SetScanLimit(MAX7219_HandleTypeDef *hmax, MAX7219_ScanLimitTypeDef scanLimit)
{
    CHECK_POINTER_VOID(hmax);
    MAX7219_SendData(hmax, MAX7219_REG_SCAN_LIMIT, (uint8_t)scanLimit);

}

void MAX7219_SetDisplayTest(MAX7219_HandleTypeDef *hmax, MAX7219_DisplayTestTypeDef displayTest)
{
    CHECK_POINTER_VOID(hmax);
    MAX7219_SendData(hmax, MAX7219_REG_DISPLAY_TEST, (uint8_t)displayTest);
}

void MAX7219_SetShutdownMode(MAX7219_HandleTypeDef *hmax, MAX7219_ShutdownModeTypeDef shutdown)
{
    CHECK_POINTER_VOID(hmax);
    MAX7219_SendData(hmax, MAX7219_REG_SHUTDOWN, (uint8_t)shutdown);
}

    /* Low-level Hardware API */
void MAX7219_SendData(MAX7219_HandleTypeDef *hmax, uint8_t address, uint8_t data)
{
    CHECK_POINTER_VOID(hmax);
}

void MAX7219_Update(MAX7219_HandleTypeDef *hmax)
{
    CHECK_POINTER_VOID(hmax);
    for (uint8_t i = 0; i < 8U; i++) {
        uint8_t regAddress = (uint8_t)(MAX7219_REG_DIGIT_0 + i);    // Set address from digit 0 to digit 7
        MAX7219_SendData(hmax, regAddress, hmax->frameBuffer[i]);   // Send data with new address and pointer data
        hmax->frameBuffer[i] = hmax->frameBuffer[i];                // Update current status
    }
}

void MAX7219_Clear(MAX7219_HandleTypeDef *hmax)
{
    CHECK_POINTER_VOID(hmax);
    for (uint8_t i = 0; i < 8U; i++) {
        hmax->frameBuffer[i] = 0x00U;   // Clear current bit
    }
    MAX7219_Update(hmax);
}

    /* Led Matrix 8x8 Features */
void MAX7219_DrawPixel(MAX7219_HandleTypeDef *hmax, uint8_t x, uint8_t y, bool state)
{
    CHECK_POINTER_VOID(hmax);
    if ( x >= 8U || y >= 8U) {
        return ;
    }
    // For example: bit[1,3] is row 1 column 7-3=4
    if (state) {
        hmax->frameBuffer[y] |= (uint8_t)(1U << (7U - x));  // Turn on the led, bit 7 corresponding locate 0
    }
    else {
        hmax->frameBuffer[y] &= (uint8_t)~(1U << (7U - x)); // Turn off the led, bit 7 corresponding locate 0
    }
}

void MAX7219_DrawMatrixChar(MAX7219_HandleTypeDef *hmax, char c)
{
    CHECK_POINTER_VOID(hmax);
    if (hmax->customFontCb != NULL) {
        uint8_t customCharData = hmax->customFontCb(c);     // Draw data by bit
        return ;
    }
    MAX7219_Update(hmax);
}

void MAX7219_ShiftLeft(MAX7219_HandleTypeDef *hmax, bool cycle)
{
    CHECK_POINTER_VOID(hmax);
    for (uint8_t i = 0 ; i < 8 ; i++ ) {
        uint8_t left_out =(hmax->frameBuffer[i] & 0x80U) ? 1U : 0U;
        hmax->frameBuffer[i] <<= 1U;
        if(cycle) {
            hmax->frameBuffer[i] |= left_out;   // Replace Right bit data with Left bit data
        }
    }
    MAX7219_Update(hmax);
}

void MAX7219_ShiftRight(MAX7219_HandleTypeDef *hmax, bool cycle) {
    CHECK_POINTER_VOID(hmax);
    for (uint8_t i = 0 ; i < 1 ; i = 7- i ) {
        uint8_t right_out = (hmax->frameBuffer[i]) ? 1U : 0U;
        hmax->frameBuffer[i] <<= 1U;
        if(cycle) {
            hmax->frameBuffer[i] |= right_out; // Replace Left bit data with Right bit data
        }
    }
    MAX7219_Update(hmax);
}

void MAX7219_ShiftUp(MAX7219_HandleTypeDef *hmax, bool cycle)
{
    CHECK_POINTER_VOID(hmax);
    uint8_t temp = hmax->frameBuffer[0];
    for (uint8_t i = 0; i < 7U; i++) {
        hmax->frameBuffer[i] = hmax->frameBuffer[i + 1U];
    }
    if (cycle) {
        hmax->frameBuffer[7U] = temp;
    } else {
        hmax->frameBuffer[7U] = 0x00U;
    }
    MAX7219_Update(hmax);
}

void MAX7219_ShiftDown(MAX7219_HandleTypeDef *hmax, bool cycle)
{
    CHECK_POINTER_VOID(hmax);
    uint8_t temp = hmax->frameBuffer[7U];
    for (int_fast8_t i = 7; i > 0; i--) {
        hmax->frameBuffer[i] = hmax->frameBuffer[i - 1];
    }
    if (cycle) {
        hmax->frameBuffer[0] = temp;
    } else {
        hmax->frameBuffer[0] = 0x00U;
    }
    MAX7219_Update(hmax);
}
