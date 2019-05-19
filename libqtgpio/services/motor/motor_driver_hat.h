#pragma once

#include <QObject>
#include <QScopedPointer>
#include "qgpioport.h"
#include "qgpioi2cslave.h"
#include "motorbase.h"


/**
 * @brief MotorDriverHat
 * @param parent
 * support for I2C controlled Raspberry Pi Motor Driver Hat by Waveshare
 * https://www.waveshare.com/wiki/Motor_Driver_HAT
 *
 */

class MotorHatPCA9685
{
public:

    MotorHatPCA9685();
    void init(uint8_t address);

    void setPWM(uint8_t channel, uint16_t on, uint16_t off);
    void setPWMFreq(uint16_t freq);
    void setPwmDutyCycle(uint8_t channel, uint16_t pulse);
    void setLevel(uint8_t channel, uint16_t value);

private:
    void writeByte(uint8_t reg, uint8_t value);
    uint8_t readByte(uint8_t reg);
private:
    QPointer<QGpioI2CSlave> m_i2c;
};

class MotorHat : public MotorBase
{
    Q_OBJECT
public:
    explicit MotorHat(uint8_t address, uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2, QObject *parent = nullptr);
    virtual ~MotorHat();

    void forward() override;
    void stop() override;
    void reverse() override;
    void setSpeed(float speed) override;

private:
    uint8_t m_speedChannel;
    uint8_t m_inChannel1;
    uint8_t m_inChannel2;
    bool m_motorActive = false;
    MotorHatPCA9685 m_pca9685;
};

class MotorDriverHat: public QObject {

    Q_OBJECT
public:

    explicit MotorDriverHat(uint8_t address = 0x40, QObject *parent = nullptr);
    virtual ~MotorDriverHat();

    void forward(MotorBase::MotorsEnum motors);
    void stop(MotorBase::MotorsEnum motors);
    void reverse(MotorBase::MotorsEnum motors);
    void setSpeed(float speedLeft, float speedRight);
    float speedLeft() const;
    float speedRight() const;

private:
    QScopedPointer<MotorHat> m_motorLeft;
    QScopedPointer<MotorHat> m_motorRight;
};

