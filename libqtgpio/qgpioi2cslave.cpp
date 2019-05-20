#include "qgpioi2cslave.h"
#include <QDebug>

extern "C" {
 extern void bcm2835_delayMicroseconds (uint64_t);
 #define udelay bcm2835_delayMicroseconds
}

// maximum counter for read repeats if NACK
static const int kMaxCount = 10;

QGpioI2CSlave::QGpioI2CSlave(uint8_t address, uint16_t clockDivider, uint16_t timeout, QObject *parent) :
    QObject(parent),
    m_address(address),
    m_clockDivider(clockDivider),
    m_timeout(timeout)
{
}

uint8_t QGpioI2CSlave::address() const
{
    return m_address;
}

void QGpioI2CSlave::setGpioParent(QGpio *gpio)
{
    m_gpio = gpio;
}

void QGpioI2CSlave::i2cSetup()
{
    bcm2835_i2c_setClockDivider(m_clockDivider);

    // sets read timeout
    volatile uint32_t* stimeout = bcm2835_bsc1 + BCM2835_BSC_CLKT / 4;
    bcm2835_peri_write(stimeout, m_timeout);

    bcm2835_i2c_setSlaveAddress(m_address);
}


uint8_t QGpioI2CSlave::read(uint8_t reg) {
    char data = reg;
    uint8_t buffer[2] = { 0, 0 };

    i2cSetup();

    (void) bcm2835_i2c_write((char *)&data, 1);
    int _counter = 0;
    udelay(500);
    while (bcm2835_i2c_read((char *)&buffer, 2) != BCM2835_I2C_REASON_OK && ++_counter < kMaxCount) {
        udelay(500);
    };

    if (reg != buffer[0]) {
        qWarning() << "8bit. reading error. wanted:" << reg << "got:" << buffer[0];
    }
    return buffer[1];
}

uint16_t QGpioI2CSlave::read16(uint8_t reg) {
    char data = reg;
    uint8_t buffer[3] = { 0, 0, 0 };

    i2cSetup();

    uint8_t rc = bcm2835_i2c_write((char *)&data, 1);
    if (rc != BCM2835_I2C_REASON_OK) {
        qWarning() << "Error writing reg" << hex << reg;
    }
    int _counter = 0;
    udelay(500);
    while (bcm2835_i2c_read((char *)&buffer, 3) != BCM2835_I2C_REASON_OK && ++_counter < kMaxCount) {
        udelay(500);
    };

    if (rc != BCM2835_I2C_REASON_OK || reg != buffer[0]) {
        qWarning() << "16bit. reading error. rc:" << rc << "wanted:"<< hex  << reg << "got:" << hex << buffer;
    }
    return (uint16_t) ((uint16_t) buffer[1] << 8 | (uint16_t) buffer[2]);
}

uint32_t QGpioI2CSlave::read32(uint8_t reg) {
    char data = reg;
    uint8_t buffer[5] = { 0, 0, 0, 0, 0 };

    i2cSetup();

    (void) bcm2835_i2c_write((char *)&data, 1);
    int _counter = 0;
    udelay(500);
    while (bcm2835_i2c_read((char *)&buffer, 5) != BCM2835_I2C_REASON_OK && ++_counter < kMaxCount) {
        udelay(500);
    };
    if (reg != buffer[0]) {
        qWarning() << "reading error. wanted:" << reg << "got:" << buffer[0];
    }
    return (uint32_t) ((uint32_t) buffer[1] << 24 | (uint32_t) buffer[2] << 16 | (uint32_t) buffer[3] << 8 | (uint32_t) buffer[4]);
}

uint8_t QGpioI2CSlave::write(uint8_t reg, uint8_t data) {
    uint8_t buffer[2];

    buffer[0] = reg;
    buffer[1] = data;

    i2cSetup();

    uint8_t rc = bcm2835_i2c_write((char *)buffer, 2);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << rc;
    }
    return rc;
}

uint8_t QGpioI2CSlave::write(uint8_t reg, uint16_t data) {
    uint8_t buffer[3];

    buffer[0] = reg;
    buffer[1] = (uint8_t) ((data >> 8) & 0xFF);
    buffer[2] = (uint8_t) (data & 0xFF);

    i2cSetup();

    uint8_t rc = bcm2835_i2c_write((char *) buffer, 3);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << reg  << data << rc;
    }
    return rc;
}

uint8_t QGpioI2CSlave::write(uint8_t reg, uint16_t data, uint16_t data1) {
    uint8_t buffer[5];

    buffer[0] = reg;
    buffer[1] = (uint8_t) (data & 0xFF);
    buffer[2] = (uint8_t) (data >> 8);
    buffer[3] = (uint8_t) (data1 & 0xFF);
    buffer[4] = (uint8_t) (data1 >> 8);

    i2cSetup();

    uint8_t rc = bcm2835_i2c_write((char *) buffer, 5);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << rc;
    }
    return rc;
}

uint8_t QGpioI2CSlave::write(uint8_t reg, uint32_t data) {
    uint8_t buffer[5];

    buffer[0] = reg;
    buffer[1] = (uint8_t) (data & 0xFF);
    buffer[2] = (uint8_t) (data >> 8) & 0xFF;
    buffer[3] = (uint8_t) (data >> 16) & 0xFF;
    buffer[4] = (uint8_t) (data >> 24) & 0xFF;

    i2cSetup();

    uint8_t rc = bcm2835_i2c_write((char *) buffer, 5);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << rc;
    }
    return rc;
}
