#include "qgpiospi.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/types.h>

#include <QDebug>
#include <QThread>

#define SPI_CS_HIGH     0x04                //Chip select high
#define SPI_LSB_FIRST   0x08                //LSB
#define SPI_3WIRE       0x10                //3-wire mode SI and SO same line
#define SPI_LOOP        0x20                //Loopback mode
#define SPI_NO_CS       0x40                //A single device occupies one SPI bus, so there is no chip select
#define SPI_READY       0x80                //Slave pull low to stop data transmission

constexpr uint8_t bits = 8;

QGpioSPI::QGpioSPI(uint8_t busNum, SPIMode mode, uint32_t speed, uint16_t timeout):
      m_busNum(busNum),
      m_timeout(timeout)
{
    /* Open SPI device */
    // TODO: check correct addressing on other SOM's
    m_fd = open(QString("/dev/spidev0.%1").arg(m_busNum).toLatin1().data(), O_RDWR);
    if (m_fd == -1) {
        qWarning() << "Error opening SPI file" << QString("/dev/spidev0.%1").arg(m_busNum) << "error:" << errno;
    }
    m_mode = 0;

    if (ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        qWarning() << "SPI: Cant set 8 bits per word for writing";
    }

    if (ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
        qWarning() << "SPI: Cant set 8 bits per word for reading";
    }

    m_tr.bits_per_word = bits;

    setSPIMode(mode);
    setChipSelectMode(SPI_CS_Mode_LOW);
    setBitOrder(SPI_BIT_ORDER_LSBFIRST);
    setSpeed(speed);
    setDataInterval(0);
}

QGpioSPI::~QGpioSPI()
{
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}


void QGpioSPI::setGpioParent(QGpio *gpio)
{
    m_gpio = gpio;
}

uint8_t QGpioSPI::busNum() const
{
    return m_busNum;
}

bool QGpioSPI::setSpeed(uint32_t speed)
{
    //Write speed
    if (ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        qWarning() << "can't set max writing speed hz" << speed;
        return false;
    }

    //Read the speed of just writing
    if (ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
        qWarning() << "can't set max reading speed hz" << speed;
        return false;
    }
    m_speed = speed;
    m_tr.speed_hz = speed;
    return true;
}

bool QGpioSPI::setSPIMode(SPIMode mode)
{
    m_mode &= 0xfC; //Clear low 2 digits
    m_mode |= mode; //Setting mode

    //Write device
    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1) {
        qWarning() << "Cant set SPI mode" << mode;
        return false;
    }
    return true;
}

bool QGpioSPI::setChipSelectEnable(bool enable)
{
    if (enable) {
        m_mode |= SPI_NO_CS;
    } else {
        m_mode &= ~SPI_NO_CS;
    }
    //Write device
    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1) {
        qWarning() << "Cant set SPI chip select enable mode" << enable;
        return false;
    }
    return true;
}

bool QGpioSPI::setChipSelectMode(SPIChipSelectMode mode)
{
    if (mode == SPI_CS_Mode_HIGH) {
        m_mode |= SPI_CS_HIGH;
        m_mode &= ~SPI_NO_CS;
    } else if (mode == SPI_CS_Mode_LOW) {
        m_mode &= ~SPI_CS_HIGH;
        m_mode &= ~SPI_NO_CS;
    } else if (m_mode == SPI_CS_Mode_NONE) {
        m_mode |= SPI_NO_CS;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1) {
        qWarning() << "Cant set SPI chip select mode" << mode;
        return false;
    }
    return true;
}

bool QGpioSPI::setBitOrder(SPIBitOrder order)
{
    if (order == SPI_BIT_ORDER_LSBFIRST) {
        m_mode |= SPI_LSB_FIRST;
    } else if (order == SPI_BIT_ORDER_MSBFIRST) {
        m_mode &= ~SPI_LSB_FIRST;
    }

    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1) {
        qWarning() << "Cant set SPI bit order" << order;
        return false;
    }
    return true;
}

bool QGpioSPI::setBusMode(SPIBusMode mode)
{
    if (mode == SPI_3WIRE_Mode) {
        m_mode |= SPI_3WIRE;
    } else if (mode == SPI_4WIRE_Mode) {
        m_mode &= ~SPI_3WIRE;
    }
    if (ioctl(m_fd, SPI_IOC_WR_MODE, &m_mode) == -1) {
        qWarning() << "Cant set SPI bus mode" << mode;
        return false;
    }
    return true;
}

void QGpioSPI::setDataInterval(uint16_t us)
{
    m_delay = us;
    m_tr.delay_usecs = m_delay;
}

uint8_t QGpioSPI::SPIWrite(uint8_t buf)
{
    uint8_t rbuf[1];
    m_tr.len = 1;
    m_tr.tx_buf = (unsigned long) &buf;
    m_tr.rx_buf = (unsigned long) rbuf;

    //ioctl Operation, transmission of data
    if (ioctl(m_fd, SPI_IOC_MESSAGE(1), &m_tr) < 1) {
        qWarning() << "SPI: error writing 1 byte";
    }
    return rbuf[0];
}

bool QGpioSPI::SPIWrite(uint8_t *buf, uint32_t len)
{
    m_tr.len = len;
    m_tr.tx_buf =  (unsigned long)buf;
    m_tr.rx_buf =  (unsigned long)buf;

    //ioctl Operation, transmission of data
    if (ioctl(m_fd, SPI_IOC_MESSAGE(1), &m_tr)  < 1 ) {
        qWarning() << "SPI: error writing" << len << "byte";
        return false;
    }

    return true;
}
