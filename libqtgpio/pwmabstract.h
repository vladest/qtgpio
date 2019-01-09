#ifndef PWMABSTRACT_H
#define PWMABSTRACT_H


class PwmAbstract
{
public:
    virtual void pwmSetDutyCycle(float dutycycle) = 0;
    virtual float pwmDutyCycle() const = 0;

    virtual void pwmSetFrequency(float freq) = 0;
    virtual float pwmFrequency() const = 0;

    virtual void startPwm(float dutyCycle = 0.0) = 0;
    virtual void stopPwm()  = 0;

protected:
    float m_pwmFreq = 1000.0f;
    float m_pwmDutyCycle = 0.0f;
    float m_pwmBaseTime = 1.0f;
    float m_pwmSliceTime = 0.01f;
};

#endif // PWMABSTRACT_H
