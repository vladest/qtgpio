#include "pwmpca9685.h"
#include <QDebug>

PwmPCA9685::PwmPCA9685(uint8_t port) :
    PCA9685(port)
{
    SetInvert(false);
    SetOutDriver(true);
    SetFrequency(50);
}

void PwmPCA9685::pwmSetDutyCycle(int channel, uint16_t dutycycle)
{
    Write((uint8_t)channel, dutycycle);
}

// unused
uint16_t PwmPCA9685::pwmDutyCycle(int channel)
{
    uint16_t nOn, nOff;
    Read((uint8_t)channel, &nOn, &nOff);
    return nOff;
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

void PwmPCA9685::startPwm(int channel, uint16_t dutyCycle)
{
    pwmSetDutyCycle(channel, dutyCycle);
}

void PwmPCA9685::stopPwm(int channel)
{
    Write((uint8_t)channel, (uint16_t) 0, (uint16_t) 0x1000);
}
