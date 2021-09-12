#pragma once

#include <stdint.h>
#include <QPointer>
#include "qgpioport.h"
#include "qgpioi2cslave.h"

#define GROVE_TWO_RGB_LED_MATRIX_DEF_I2C_ADDR	0x65 // The device i2c address in default

class GroveLEDMatrix
{
public:
#ifndef _GROVE_TWO_LED_MATRIX_H_
    enum orientation_type_t {
        DISPLAY_ROTATE_0 = 0,
        DISPLAY_ROTATE_90 = 1,
        DISPLAY_ROTATE_180 = 2,
        DISPLAY_ROTATE_270 = 3,
        };
#endif

        enum COLORS {
            red = 0x00,
            orange = 0x12,
            yellow = 0x18,
            green = 0x52,
            cyan = 0x7f,
            blue = 0xaa,
            purple = 0xc3,
            pink = 0xdc,
            white = 0xfe,
            black = 0xff,
            };

    GroveLEDMatrix();

    void init(uint8_t bus, uint8_t address = GROVE_TWO_RGB_LED_MATRIX_DEF_I2C_ADDR, uint8_t delay = 1);

    void displayBar(uint8_t bar, uint16_t duration_time, bool forever_flag, uint8_t color);
    void displayEmoji(uint8_t emoji, uint16_t duration_time, bool forever_flag);
    void displayNumber(int16_t number, uint16_t duration_time, bool forever_flag, uint8_t color);
    void displayString(char *str, uint16_t duration_time, bool forever_flag, uint8_t color);
    void displayFrames(uint8_t *buffer, uint16_t duration_time, bool forever_flag, uint8_t frames_number);
    void displayFrames(uint64_t *buffer, uint16_t duration_time, bool forever_flag, uint8_t frames_number);
    void stopDisplay();
    void storeFrames();
    void deleteFrames();
    void displayFramesFromFlash(uint16_t duration_time, bool forever_flag, uint8_t from, uint8_t to);
    void displayColorBlock(uint32_t rgb, uint16_t duration_time, bool forever_flag);
    void displayColorBar(uint8_t bar, uint16_t duration_time, bool forever_flag);
    void displayColorWave(uint8_t color, uint16_t duration_time, bool forever_flag);
    void displayClockwise(bool is_cw, bool is_big, uint16_t duration_time, bool forever_flag);
    void displayColorAnimation(uint8_t index, uint16_t duration_time, bool forever_flag);

private:
    void writeContinue(uint8_t reg, const void *buf, size_t len);
private:
    QPointer<QGpioI2CSlave> m_i2c;

};

