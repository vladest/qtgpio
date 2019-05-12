#include "pwmultraborg.h"
#include <QDebug>

PwmUltraborg::PwmUltraborg(uint8_t port)
{
    m_ultraborg = UltraBorg::getInstance();
    m_ultraborg->init(port);
}

PwmUltraborg::~PwmUltraborg()
{
    m_ultraborg->deinit();
}

// PWM value can be set anywhere from 0 for a 0% duty cycle to 65535 for a 100% duty cycle
void PwmUltraborg::pwmSetDutyCycle(int channel, const QVariant &dutycycle)
{
    uint16_t duty = 0;
    uint16_t _pwmMin = m_ultraborg->getPWMMin(channel);
    uint16_t _pwmMax= m_ultraborg->getPWMMax(channel);
    if (dutycycle.type() == QVariant::Double) {
        float dutyFloat = dutycycle.toFloat();
        if (dutyFloat >= 0.0 && dutyFloat <= 100.0) {
            duty = (uint16_t)((float)(_pwmMax - _pwmMin)/100.0*dutyFloat);
        }
    } else {
        duty = (uint16_t)dutycycle.toInt();
    }
    //qDebug() << "duty" << duty;
    m_ultraborg->setPWMValue(channel, duty);
}

QVariant PwmUltraborg::pwmDutyCycle(int channel)
{
    uint16_t duty = m_ultraborg->getPWMValue(channel);
    return duty;
}

void PwmUltraborg::pwmSetFrequency(float freq)
{
    Q_UNUSED(freq)
}

float PwmUltraborg::pwmFrequency()
{
    return 50.0;
}

void PwmUltraborg::startPwm(int channel, const QVariant &dutyCycle)
{
    pwmSetDutyCycle(channel, dutyCycle);
}

void PwmUltraborg::stopPwm(int channel)
{
    pwmSetDutyCycle(channel, 0);
}

UltraBorg *PwmUltraborg::ultraborg() const
{
    return m_ultraborg;
}
