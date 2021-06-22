#pragma once

#include <QObject>
#include <QScopedPointer>
#include <QVector>
#include "qgpioport.h"
#include "qgpioi2cslave.h"
#include "motorbase.h"


/**
 * @brief MotorDriverHat
 * @param parent
 * support for I2C controlled Raspberry Pi DC Motor Hat & Stepper by Adafruit
 * https://www.adafruit.com/product/2348
 */

class MotorHatStepperPCA9685
{
public:

    MotorHatStepperPCA9685();
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

class Motor : public MotorBase
{
    Q_OBJECT
public:
    explicit Motor(uint8_t address, uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2, QObject *parent = nullptr);
    virtual ~Motor();

    void forward() override;
    void stop() override;
    void reverse() override;
    void setSpeed(float speed) override;

private:
    uint8_t m_speedChannel;
    uint8_t m_inChannel1;
    uint8_t m_inChannel2;
    bool m_motorActive = false;
    MotorHatStepperPCA9685 m_pca9685;
};

class MotorStepperHat: public QObject {

    Q_OBJECT
public:

    explicit MotorStepperHat(uint8_t address = 0x60, QObject *parent = nullptr);
    virtual ~MotorStepperHat();

    void forward(MotorBase::MotorsEnum motors);
    void stop(MotorBase::MotorsEnum motors);
    void reverse(MotorBase::MotorsEnum motors);
    void setSpeed(QList<QPair<MotorBase::MotorsEnum, float> > motors);
    float speed(MotorBase::MotorsEnum motor) const;

private:
    QVector<Motor*> m_motors;
};

