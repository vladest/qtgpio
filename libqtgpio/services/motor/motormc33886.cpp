#include "motormc33886.h"
#include "services/pwm/pwmsoftware.h"

/**
 * @brief MotorMC33886::MotorMC33886
 * @param parent
 * support for MC33886 Raspberry Pi Motor Driver Board
 *
 */
MotorMC33886::MotorMC33886(QObject *parent) : QObject(parent)
{
    QGpio* gpio = QGpio::getInstance();
    m_speedLeft = new PwmSoftware(12);
    m_speedRight = new PwmSoftware(26);
    m_portA1 = gpio->allocateGpioPort(6, QGpio::DIRECTION_OUTPUT);
    m_portA2 = gpio->allocateGpioPort(13, QGpio::DIRECTION_OUTPUT);
    m_portB1 = gpio->allocateGpioPort(20, QGpio::DIRECTION_OUTPUT);
    m_portB2 = gpio->allocateGpioPort(21, QGpio::DIRECTION_OUTPUT);
    m_speedLeft->pwmSetFrequency(500);
    m_speedLeft->startPwm(-1, m_speed);
    m_speedRight->pwmSetFrequency(500);
    m_speedRight->startPwm(-1, m_speed);
}

void MotorMC33886::setMotor(QGpio::GpioValue a1, QGpio::GpioValue a2, QGpio::GpioValue b1, QGpio::GpioValue b2)
{
    m_portA1->setValue(a1);
    m_portA2->setValue(a2);
    m_portB1->setValue(b1);
    m_portB2->setValue(b2);
}

float MotorMC33886::speed() const
{
    return m_speed;
}

void MotorMC33886::forward()
{
    setMotor(QGpio::VALUE_HIGH, QGpio::VALUE_LOW, QGpio::VALUE_HIGH, QGpio::VALUE_LOW);
}

void MotorMC33886::stop()
{
    setMotor(QGpio::VALUE_LOW, QGpio::VALUE_LOW, QGpio::VALUE_LOW, QGpio::VALUE_LOW);
}

void MotorMC33886::reverse()
{
    setMotor(QGpio::VALUE_LOW, QGpio::VALUE_HIGH, QGpio::VALUE_LOW, QGpio::VALUE_HIGH);
}

void MotorMC33886::left()
{
    setMotor(QGpio::VALUE_HIGH, QGpio::VALUE_LOW, QGpio::VALUE_LOW, QGpio::VALUE_LOW);
}

void MotorMC33886::right()
{
    setMotor(QGpio::VALUE_LOW, QGpio::VALUE_LOW, QGpio::VALUE_HIGH, QGpio::VALUE_LOW);
}

void MotorMC33886::setSpeed(float speed) {
    if (speed < 0.0f || speed > 100.0f)
        return;
    m_speed = speed;
    m_speedLeft->pwmSetDutyCycle(-1, speed);
    m_speedRight->pwmSetDutyCycle(-1, speed);
}
