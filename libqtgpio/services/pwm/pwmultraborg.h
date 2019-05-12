#pragma once

#include <stdint.h>

#include "pwmbase.h"
#include "ultraborg/ultraborg.h"

class PwmUltraborg : public PwmBase
{
public:
    PwmUltraborg(uint8_t port = UB_DEFAULT_I2C_ADDRESS);
    virtual ~PwmUltraborg();

    // PwmAbstract interface
public:
    void pwmSetDutyCycle(int channel, const QVariant& dutycycle) override;
    QVariant pwmDutyCycle(int channel) override;
    void pwmSetFrequency(float freq) override;
    float pwmFrequency() override;
    void startPwm(int channel, const QVariant& dutyCycle) override;
    void stopPwm(int channel) override;
    UltraBorg* ultraborg() const;

private:
    UltraBorg* m_ultraborg = nullptr;
};
