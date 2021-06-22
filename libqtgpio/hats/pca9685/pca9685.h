#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "qgpioi2cslave.h"

#define PCA9685_I2C_ADDRESS_DEFAULT	0x40
#define PCA9685_I2C_ADDRESS_FIXED	0x70
#define PCA9685_I2C_ADDRESSES_MAX	62

#define CHANNEL(x)	((uint8_t)(x))
#define VALUE(x)	((uint16_t)(x))

#define PCA9685_VALUE_MIN	VALUE(0)
#define PCA9685_VALUE_MAX	VALUE(4096)

#define PCA9685_PWM_CHANNELS	16

enum TPCA9685FrequencyRange {
    PCA9685_FREQUENCY_MIN = 24,
    PCA9685_FREQUENCY_MAX = 1526
};

// OCH stands for Output Change
enum TPCA9685Och {
    PCA9685_OCH_STOP = 0,
    PCA9685_OCH_ACK = 1 << 3
};

class PCA9685 {
public:
    PCA9685(uint8_t i2cBus, uint8_t nAddress = PCA9685_I2C_ADDRESS_DEFAULT);
    ~PCA9685(void);

    void SetPreScaller(uint8_t);
    uint8_t GetPreScaller(void);

    void SetFrequency(uint16_t);
    uint16_t GetFrequency(void);

    void SetOCH(TPCA9685Och);
    TPCA9685Och GetOCH(void);

    void SetInvert(bool);
    bool GetInvert(void);

    void SetOutDriver(bool);
    bool GetOutDriver(void);

    void Write(uint8_t, uint16_t, uint16_t);
    void Read(uint8_t, uint16_t *, uint16_t *);

    void Write(uint16_t, uint16_t);
    void Read(uint16_t *, uint16_t *);

    void Write(uint8_t, uint16_t);
    void Write(uint16_t);

    void SetFullOn(uint8_t, bool);
    void SetFullOff(uint8_t, bool);

    void Dump(void);

private:
    uint8_t CalcPresScale(uint16_t);
    uint16_t CalcFrequency(uint8_t);

private:
    void Sleep(bool);
    void AutoIncrement(bool);

private:
    QPointer<QGpioI2CSlave> m_i2c;
};
