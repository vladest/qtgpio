#ifndef PWMABSTRACT_H
#define PWMABSTRACT_H

#include <QtCore/QVariant>

class PwmBase
{
public:
    virtual void pwmSetDutyCycle(int channel, const QVariant& dutycycle) = 0;
    virtual QVariant pwmDutyCycle(int channel) = 0;

    virtual void pwmSetFrequency(float freq) = 0;
    virtual float pwmFrequency() = 0;

    virtual void startPwm(int channel, const QVariant& dutyCycle = 0) = 0;
    virtual void stopPwm(int channel)  = 0;

protected:
    float m_pwmFreq = 1000.0f;
    QVariant m_pwmDutyCycle;
};

#endif // PWMABSTRACT_H
