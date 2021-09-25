#pragma once

#include <QObject>
#include "qgpio.h"
#include <stdint.h>
#include <linux/spi/spidev.h>

class QGpioSPI: public QObject //for QPointer
{
public:
    enum SPIMode {
        SPIMODE_0 = SPI_MODE_0,
        SPIMODE_1 = SPI_MODE_1,
        SPIMODE_2 = SPI_MODE_2,
        SPIMODE_3 = SPI_MODE_3,
    };

    enum SPIChipSelectMode {
        SPI_CS_Mode_LOW  = 0,     /*!< Chip Select 0 */
        SPI_CS_Mode_HIGH = 1,     /*!< Chip Select 1 */
        SPI_CS_Mode_NONE = 3      /*!< No CS, control it yourself */
    };

    enum SPIBitOrder {
        SPI_BIT_ORDER_LSBFIRST = 0,  /*!< LSB First */
        SPI_BIT_ORDER_MSBFIRST = 1   /*!< MSB First */
    };

    enum SPIBusMode {
        SPI_3WIRE_Mode = 0,
        SPI_4WIRE_Mode = 1
    };

    explicit QGpioSPI(uint8_t busNum = 0, SPIMode mode = SPIMODE_0, uint32_t speed = 2000000, uint16_t timeout = 40000);
    virtual ~QGpioSPI();
    void setGpioParent(QGpio *gpio);

    uint8_t busNum() const;

    uint8_t SPIWrite(uint8_t buf);
    bool SPIWrite(uint8_t *buf, uint32_t len);

private:
    bool setSpeed(uint32_t speed);
    bool setSPIMode(SPIMode mode);
    bool setChipSelectEnable(bool enable);
    bool setChipSelectMode(SPIChipSelectMode mode);
    bool setBitOrder(SPIBitOrder order);
    bool setBusMode(SPIBusMode mode);
    void setDataInterval(uint16_t us);

private:
    uint8_t m_busNum = 0;
    uint16_t m_delay = 0;           // i2c operation delay in ms
    uint16_t m_timeout = 0;
    uint16_t m_SCLKPin;
    uint16_t m_MOSIPin;
    uint16_t m_MISOPin;

    uint16_t m_CS0Pin;
    uint16_t m_CS1Pin;

    uint32_t m_speed;
    uint16_t m_mode;

    QPointer<QGpio> m_gpio;
    int m_fd = -1;
    struct spi_ioc_transfer m_tr;
};
