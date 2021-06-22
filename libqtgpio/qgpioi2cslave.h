#ifndef I2CDEVICE_H
#define I2CDEVICE_H

#include <QObject>
#include "qgpio.h"
#include <stdint.h>

class QGpioI2CSlave: public QObject //for QPointer
{
public:
    explicit QGpioI2CSlave(uint8_t address, uint8_t delay = 0, uint8_t busNum = 0, uint16_t timeout = 40000);
    virtual ~QGpioI2CSlave();
    uint8_t address() const;
    void setGpioParent(QGpio *gpio);

    uint8_t i2cRead(uint8_t reg);
    uint16_t i2cRead16(uint8_t reg);
    uint32_t i2cRead32(uint8_t reg);

    uint8_t i2cWrite(uint8_t reg, uint8_t data);
    uint8_t i2cWrite(uint8_t reg, uint16_t data);
    uint8_t i2cWrite(uint8_t reg, uint32_t data);
    uint8_t i2cWrite(uint8_t reg, uint16_t data, uint16_t data1);
private:
    void i2cSetup();
    ssize_t ioctlReadHelper(unsigned int iaddr, void *buf, size_t len);
    void internalAddressConvert(unsigned int iaddr, unsigned int len, unsigned char *addr);
    ssize_t ioctlWriteHelper(unsigned int iaddr, const void *buf, size_t len);
    ssize_t readHelper(unsigned int iaddr, void *buf, size_t len);
    ssize_t writeHelper(unsigned int iaddr, const void *buf, size_t len);
    bool addressSelect(unsigned long dev_addr, unsigned long tenbit);

private:
    uint8_t m_address = 0;
    uint8_t m_busNum = 0;

    uint32_t m_internalAddrBytes = 1;    // i2c device internal(word) address bytes, such as: 24C04 1 byte, 24C64 2 bytes
    uint16_t m_delay = 0;           // i2c operation delay in ms
    uint16_t m_timeout = 0;
    uint16_t m_ioctlFlags = 0;      // i2c ioctl operations flags
    uint8_t m_tenBitsAddress = 0;      // i2c is 10 bit device address
    uint32_t m_bytesPerPage = 8;    // i2c max number of bytes per page, 1K/2K 8, 4K/8K/16K 16, 32K/64K 32 etc

    QPointer<QGpio> m_gpio;
    int m_fd = -1;
};

#endif // I2CDEVICE_H
