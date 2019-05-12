#include "i2c.h"
// i2c
#include "rpi/bcm2835.h"
#include "qgpio.h"

#include <sys/mman.h>

#include <QDebug>

I2C::I2C()
{}

void I2C::i2cPresetup()
{
    bcm2835_i2c_setSlaveAddress(m_nAddress);
    bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
}

void I2C::init(uint8_t address)
{
    m_nAddress = address;
    if (QGpio::getInstance()->init() != QGpio::INIT_OK) {
        qDebug("Error init QGpio\n");
    }
    if (bcm2835_regbase(BCM2835_REGBASE_BSC0) == MAP_FAILED) {
        if (bcm2835_init() == 0) {
            qDebug("Not able to init the bmc2835 library\n");
        }
    }

    bcm2835_i2c_begin();
}

void I2C::deinit()
{
    bcm2835_i2c_end();
}


uint8_t I2C::I2CReadReg(uint8_t reg) {
    char data = reg;
    uint8_t buffer[2] = { 0, 0 };

    i2cPresetup();

    (void) bcm2835_i2c_write((char *)&data, 1);
    udelay(500);
    (void) bcm2835_i2c_read((char *)&buffer, 2);

    if (reg != buffer[0]) {
        qWarning() << "8bit. reading error. wanted:" << reg << "got:" << buffer[0];
    }
    return buffer[1];
}

uint16_t I2C::I2CReadReg16(uint8_t reg) {
    char data = reg;
    uint8_t buffer[3] = { 0, 0, 0 };

    i2cPresetup();

    uint8_t rc = bcm2835_i2c_write((char *)&data, 1);
    if (rc != BCM2835_I2C_REASON_OK) {
        qWarning() << "Error writing reg" << hex << reg;
    }
    udelay(500);
    rc = bcm2835_i2c_read((char *)&buffer, 3);
    if (rc != BCM2835_I2C_REASON_OK || reg != buffer[0]) {
        qWarning() << "16bit. reading error. rc:" << rc << "wanted:"<< hex  << reg << "got:" << hex << buffer;
    }
    return (uint16_t) ((uint16_t) buffer[1] << 8 | (uint16_t) buffer[2]);
}

uint32_t I2C::I2CReadReg32(uint8_t reg) {
    char data = reg;
    uint8_t buffer[5] = { 0, 0, 0, 0, 0 };

    i2cPresetup();

    (void) bcm2835_i2c_write((char *)&data, 1);
    udelay(500);
    (void) bcm2835_i2c_read((char *)&buffer, 5);
    if (reg != buffer[0]) {
        qWarning() << "reading error. wanted:" << reg << "got:" << buffer[0];
    }
    return (uint32_t) ((uint32_t) buffer[1] << 24 | (uint32_t) buffer[2] << 16 | (uint32_t) buffer[3] << 8 | (uint32_t) buffer[4]);
}

uint8_t I2C::I2CWriteReg(uint8_t reg, uint8_t data) {
    uint8_t buffer[2];

    buffer[0] = reg;
    buffer[1] = data;

    i2cPresetup();

    uint8_t rc = bcm2835_i2c_write((char *)buffer, 2);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << rc;
    }
    return rc;
}

uint8_t I2C::I2CWriteReg(uint8_t reg, uint16_t data) {
    uint8_t buffer[3];

    buffer[0] = reg;
    buffer[1] = (uint8_t) ((data >> 8) & 0xFF);
    buffer[2] = (uint8_t) (data & 0xFF);

    i2cPresetup();

    uint8_t rc = bcm2835_i2c_write((char *) buffer, 3);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << reg  << data << rc;
    }
    return rc;
}

uint8_t I2C::I2CWriteReg(uint8_t reg, uint32_t data) {
    uint8_t buffer[5];

    buffer[0] = reg;
    buffer[1] = (uint8_t) (data & 0xFF);
    buffer[2] = (uint8_t) (data >> 8) & 0xFF;
    buffer[3] = (uint8_t) (data >> 16) & 0xFF;
    buffer[4] = (uint8_t) (data >> 24) & 0xFF;

    i2cPresetup();

    uint8_t rc = bcm2835_i2c_write((char *) buffer, 5);
    if (rc != BCM2835_I2C_REASON_OK) {
        qDebug() << "Error writing i2c UltraBorg" << __PRETTY_FUNCTION__ << rc;
    }
    return rc;
}
