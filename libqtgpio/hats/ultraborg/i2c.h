#ifndef I2C_H
#define I2C_H

#include <stdint.h>

extern "C" {
 extern void bcm2835_delayMicroseconds (uint64_t);
 #define udelay bcm2835_delayMicroseconds
}

class I2C
{
public:
    I2C();
    void init(uint8_t address);
    void deinit();
    uint8_t I2CWriteReg(uint8_t reg, uint8_t data);
    uint8_t I2CWriteReg(uint8_t reg, uint16_t data);
    uint8_t I2CWriteReg(uint8_t reg, uint32_t data);

    uint8_t I2CReadReg(uint8_t reg);
    uint16_t I2CReadReg16(uint8_t reg);
    uint32_t I2CReadReg32(uint8_t reg);
private:
    void i2cPresetup();

private:
    uint8_t m_nAddress = 0;
};

#endif // I2C_H
