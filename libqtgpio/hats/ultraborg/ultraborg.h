#ifndef ULTRABORG_H
#define ULTRABORG_H

#include <QObject>
#include <QThread>
#include <QAtomicInt>
#include "i2c.h"

static const uint8_t UB_DEFAULT_I2C_ADDRESS = 0x36;  // I2C address set by default (before using SET_I2C_ADD)

class UltraBorg
{

public:
    static UltraBorg* getInstance();

    ~UltraBorg();

    void init(uint8_t address = UB_DEFAULT_I2C_ADDRESS);
    void deinit();

    uint16_t getPWMMin(int servo);
    uint16_t getPWMMax(int servo);

    uint16_t getPWMValue(int servo);
    uint16_t getPWMBootValue(int servo);

    void setPWMValue(int servo, uint16_t value);
    void setPWMLimits(int servo, uint16_t min, uint16_t max, uint16_t bootvalue);

    float getDistance(int usm);
    float getRawDistance(int usm);

private:
    explicit UltraBorg();
    uint8_t repeatedWrite(uint8_t reg, uint8_t checkReg, uint16_t value, int numRepeats = 5, int delayAfterWrite = 10000);

private:
    Q_DISABLE_COPY(UltraBorg);
    uint8_t m_nAddress = UB_DEFAULT_I2C_ADDRESS;
    I2C i2c;
    volatile bool m_inited = false;
    static QAtomicInt m_references;
};

#endif // ULTRABORG_H
