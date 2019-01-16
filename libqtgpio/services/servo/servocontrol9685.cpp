#include "servocontrol9685.h"
#include <QDebug>
#include <QCoreApplication>


ServoControl9685::ServoControl9685(int channel, QObject *parent) : ServoBase(parent), m_channel(channel)
{
    m_pwm = new PwmPCA9685();
    m_pwm->pwmSetFrequency(50.0);
    m_pwm->startPwm(channel, 0);
}

ServoControl9685::~ServoControl9685()
{
    m_pwm->stopPwm(m_channel);
    delete m_pwm;
}

void ServoControl9685::stopRotating()
{
    m_pwm->pwmSetDutyCycle(m_channel, 0.0);
}

void ServoControl9685::startRotating(float torque)
{
    //TODO: implement torque speed
    uint16_t duty_cycle = 0;
    if (torque < 0)
        duty_cycle = minDuty();
    else if (torque > 0)
        duty_cycle = minDuty() + dutyRange();

    duty_cycle = (duty_cycle + 1) >> 4;
    //qDebug() << "duty cycle" << duty_cycle;
    m_pwm->pwmSetDutyCycle(m_channel, duty_cycle);
}

void ServoControl9685::rotateAngle(float angle)
{
    Q_UNUSED(angle)
    //TODO: implement
}

void ServoControl9685::setAngle(float angle)
{
    float fraction = angle / actuactionRange();
    uint16_t  duty_cycle = minDuty() + (int)(fraction * dutyRange());
    duty_cycle = (duty_cycle + 1) >> 4;
    m_pwm->pwmSetDutyCycle(m_channel, duty_cycle);
}

void ServoControl9685::recalcDuty()
{
    setDuties(((float)servoMinPulse() * m_pwm->pwmFrequency()) / 1000000.0 * 0xffff,
              ((float)servoMaxPulse() * m_pwm->pwmFrequency()) / 1000000.0 * 0xffff);
}
