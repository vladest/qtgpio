#include "pwmpca9685.h"

PwmPCA9685::PwmPCA9685(uint8_t port) :
    PCA9685(port)
{
    SetInvert(false);
    SetOutDriver(true);
    SetFrequency(50);
}

// unused
void PwmPCA9685::pwmSetDutyCycle(int channel, float dutycycle)
{
    //value = (value + 1) >> 4
}

// unused
float PwmPCA9685::pwmDutyCycle(int channel)
{
    return 0.0f;
}

// PCA9685_OSC_FREQ / 4096 / prescale_reg
void PwmPCA9685::pwmSetFrequency(float freq)
{
    SetFrequency((uint16_t)freq);
}

float PwmPCA9685::pwmFrequency()
{
    return (uint16_t)GetFrequency();
}

void PwmPCA9685::startPwm(int channel, float dutyCycle)
{
}

void PwmPCA9685::stopPwm(int channel)
{
    for (uint8_t i = 0; i < 16; i ++) {
        Write(i, (uint16_t) 0, (uint16_t) 0x1000);
    }
}
