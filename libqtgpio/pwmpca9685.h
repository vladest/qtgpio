#ifndef PWMPCA9685_H
#define PWMPCA9685_H

#include <stdint.h>

#include "pwmabstract.h"
#include "pca9685/pca9685.h"

class PwmPCA9685 : public PwmAbstract, public PCA9685
{
public:
    PwmPCA9685(uint8_t port = PCA9685_I2C_ADDRESS_DEFAULT);

    // PwmAbstract interface
public:
    void pwmSetDutyCycle(int channel, uint16_t dutycycle) override;
    uint16_t pwmDutyCycle(int channel) override;
    void pwmSetFrequency(float freq) override;
    float pwmFrequency() override;
    void startPwm(int channel, uint16_t dutyCycle) override;
    void stopPwm(int channel) override;

};

#endif // PWMPCA9685_H
