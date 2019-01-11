#ifndef PWMABSTRACT_H
#define PWMABSTRACT_H

#include <stdint.h>

class PwmBase
{
public:
    virtual void pwmSetDutyCycle(int channel, uint16_t dutycycle) = 0;
    virtual uint16_t pwmDutyCycle(int channel) = 0;

    virtual void pwmSetFrequency(float freq) = 0;
    virtual float pwmFrequency() = 0;

    virtual void startPwm(int channel, uint16_t dutyCycle = 0) = 0;
    virtual void stopPwm(int channel)  = 0;

protected:
    float m_pwmFreq = 1000.0f;
    uint16_t m_pwmDutyCycle = 0;
};

#endif // PWMABSTRACT_H
