#pragma once

#include <stdint.h>

#include "pwmbase.h"
#include "pca9685/pca9685.h"

class PwmPCA9685 : public PwmBase, public PCA9685
{
public:
    PwmPCA9685(uint8_t port = PCA9685_I2C_ADDRESS_DEFAULT);
    virtual ~PwmPCA9685() = default;

    // PwmAbstract interface
public:
    void pwmSetDutyCycle(int channel, const QVariant& dutycycle) override;
    QVariant pwmDutyCycle(int channel) override;
    void pwmSetFrequency(float freq) override;
    float pwmFrequency() override;
    void startPwm(int channel, const QVariant& dutyCycle) override;
    void stopPwm(int channel) override;

};

