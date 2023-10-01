#include "motor_driver_hat.h"
#include <unistd.h>
#include <QDebug>

MotorPCA9685::MotorPCA9685(uint8_t bus, uint8_t address, uint8_t delay,
                           uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2,
                           QObject *parent) :
    MotorBase(parent),
    m_speedChannel(speedChannel),
    m_inChannel1(inChannel1),
    m_inChannel2(inChannel2)
{
    m_pca9685.init(bus, address, delay);
    m_pca9685.setPWMFreq(100);
}

MotorPCA9685::~MotorPCA9685()
{
    stop();
}

void MotorPCA9685::forward(MotorsFlags motors)
{
    qDebug() << "motor forward at" << m_speedChannel << m_inChannel1 << m_inChannel2;
    m_motorActive = true;

    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel1, 0);
    m_pca9685.setLevel(m_inChannel2, 1);
}

void MotorPCA9685::stop(MotorsFlags motors)
{
    m_motorActive = false;
    m_pca9685.setPwmDutyCycle(m_speedChannel, 0);
}

void MotorPCA9685::reverse(MotorsFlags motors)
{
    m_motorActive = true;

    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel1, 1);
    m_pca9685.setLevel(m_inChannel2, 0);
}

void MotorPCA9685::setSpeed(float speed, MotorsFlags motors) {
    if (speed < 0.0f || speed > 100.0f)
        return;
    MotorBase::setSpeed(speed);
    if (m_motorActive)
        m_pca9685.setPwmDutyCycle(m_speedChannel, speed);
}


MotorDriver::MotorDriver(uint8_t bus, const MotorMapping &motorsMapping, uint8_t address, uint8_t delay, QObject *parent) :
      QObject(parent)
{
    for (const auto& motor : motorsMapping) {
        auto m = new MotorPCA9685(bus, address, delay, motor.second.ENABLE, motor.second.INPUT1, motor.second.INPUT2);
        m->setMotor(motor.first);
        m_motors.append(m);
    }
}

MotorDriver::~MotorDriver()
{
    qDeleteAll(m_motors);
}

void MotorDriver::forward(Motors::MotorsEnum motors) {
    qDebug() << __PRETTY_FUNCTION__ << motors;
    for (const auto& motor : m_motors) {
        if (motor->motor() & motors) {
            motor->forward();
        }
    }
}

void MotorDriver::stop(Motors::MotorsEnum motors)
{ 
    for (const auto& motor : m_motors) {
        if (motor->motor() & motors) {
            motor->stop();
        }
    }
}

void MotorDriver::reverse(Motors::MotorsEnum motors) {
    qDebug() << __PRETTY_FUNCTION__ << motors;
    for (const auto& motor : m_motors) {
        if (motor->motor() & motors) {
            motor->reverse();
        }
    }
}

void MotorDriver::setSpeed(Motors::MotorsEnum motors, float speed)
{
    for (const auto& motor : m_motors) {
        if (motor->motor() & motors) {
            motor->setSpeed(speed);
        }
    }
}

float MotorDriver::speed(Motors::MotorsEnum motor) const
{
    for (const auto& m : m_motors) {
        if (m->motor() == motor) {
            return m->speed();
        }
    }
    return 0.0f;
}

