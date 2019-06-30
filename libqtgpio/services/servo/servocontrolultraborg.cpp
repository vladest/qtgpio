#include "servocontrolultraborg.h"
#include <QDebug>

ServoControlUltraborg::ServoControlUltraborg(int channel, uint8_t port, QObject *parent):
    m_channel(channel), ServoBase(parent)
{
    m_pwmUltraborg.reset(new PwmUltraborg(port));
}

ServoControlUltraborg::~ServoControlUltraborg() {
    saveServoPulses();
}

void ServoControlUltraborg::stopRotating()
{
    m_pwmUltraborg->stopPwm(m_channel);
}

void ServoControlUltraborg::startRotating(float torque)
{
    uint16_t duty_cycle = 0;
    if (torque < 0)
        duty_cycle = minDuty();
    else if (torque > 0)
        duty_cycle = minDuty() + dutyRange();

    //duty_cycle = (duty_cycle + 1) >> 4;
    //qDebug() << "duty cycle" << duty_cycle;
    m_pwmUltraborg->pwmSetDutyCycle(m_channel, duty_cycle);
}

void ServoControlUltraborg::rotateAngle(float angle)
{
    Q_UNUSED(angle)
}

void ServoControlUltraborg::setAngle(float angle)
{
    float fraction = angle / actuactionRange();
    float powerOut = (fraction + 1.0) / 2.0;
    uint16_t pulsesMin =  servoMinPulse() * 2; //Ultraborg pulses: 2000 for 1000 mks
    uint16_t pulsesMax = servoMaxPulse() * 2;
    uint16_t pwmDuty = (uint16_t)((fraction * (pulsesMax - pulsesMin)) + pulsesMin);
    //duty_cycle = (duty_cycle + 1) >> 4;
    //qDebug() << __PRETTY_FUNCTION__ << pulsesMin << pulsesMax << actuactionRange() << pwmDuty << angle << fraction << powerOut;
    m_pwmUltraborg->pwmSetDutyCycle(m_channel, pwmDuty);
}

void ServoControlUltraborg::recalcDuty()
{

}

void ServoControlUltraborg::setServoPulses(uint16_t minPulse, uint16_t maxPulse)
{
    ServoBase::setServoPulses(minPulse, maxPulse);
    //qDebug() << __PRETTY_FUNCTION__ << minDuty() << minPulse << maxPulse;
}

void ServoControlUltraborg::saveServoPulses()
{
    // save last value
    m_pwmUltraborg->ultraborg()->setPWMLimits(m_channel, servoMinPulse()*2, servoMaxPulse()*2,
                                              m_pwmUltraborg->ultraborg()->getPWMValue(m_channel));
}
