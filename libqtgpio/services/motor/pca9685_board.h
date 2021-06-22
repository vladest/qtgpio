#pragma once

#include <stdint.h>
#include <QPointer>
#include "qgpioport.h"
#include "qgpioi2cslave.h"

class PCA9685Bboard
{
public:
    PCA9685Bboard();

    void init(uint8_t bus, uint8_t address, uint8_t delay = 1);

    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
    void setPWM(uint8_t channel, uint16_t value);
    void setPWMFreq(uint16_t freq);
    void setPwmDutyCycle(uint8_t channel, uint16_t pulse);
    void setLevel(uint8_t channel, uint16_t value);

private:
    void writeByte(uint8_t reg, uint8_t value);
    uint8_t readByte(uint8_t reg);
private:
    QPointer<QGpioI2CSlave> m_i2c;
};

