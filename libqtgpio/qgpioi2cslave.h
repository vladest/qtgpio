#ifndef I2CDEVICE_H
#define I2CDEVICE_H

#include <QObject>
#include "qgpio.h"
#include <stdint.h>

class QGpioI2CSlave : public QObject
{
    Q_OBJECT
public:
    explicit QGpioI2CSlave(uint8_t address, uint16_t clockDivider, uint16_t timeout = 40000, QObject *parent = nullptr);
    uint8_t address() const;
    void setGpioParent(QGpio *gpio);

    uint8_t read(uint8_t reg);
    uint16_t read16(uint8_t reg);
    uint32_t read32(uint8_t reg);

    uint8_t write(uint8_t reg, uint8_t data);
    uint8_t write(uint8_t reg, uint16_t data);
    uint8_t write(uint8_t reg, uint32_t data);

private:
    void i2cSetup();

private:
    uint8_t m_address = 0;
    uint16_t m_clockDivider = BCM2835_I2C_CLOCK_DIVIDER_626;
    uint16_t m_timeout = 0;

    QPointer<QGpio> m_gpio;
};

#endif // I2CDEVICE_H
