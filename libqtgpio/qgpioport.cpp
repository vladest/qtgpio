#include "qgpioport.h"
#include <QDebug>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define FSEL_OFFSET                 0   // 0x0000
#define SET_OFFSET                  7   // 0x001c / 4
#define CLR_OFFSET                  10  // 0x0028 / 4
#define PINLEVEL_OFFSET             13  // 0x0034 / 4
#define EVENT_DETECT_OFFSET         16  // 0x0040 / 4
#define RISING_ED_OFFSET            19  // 0x004c / 4
#define FALLING_ED_OFFSET           22  // 0x0058 / 4
#define HIGH_DETECT_OFFSET          25  // 0x0064 / 4
#define LOW_DETECT_OFFSET           28  // 0x0070 / 4
#define PULLUPDN_OFFSET             37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET          38  // 0x0098 / 4

void short_wait(void)
{
    int i;

    for (i=0; i<150; i++) {    // wait 150 cycles
        asm volatile("nop");
    }
}

QGpioPort::QGpioPort(int port, QGpio::GpioDirection direction, QGpio::GpioPullUpDown pud, QObject *parent) :
    QObject(parent), m_port(port), m_direction(direction), m_pud(pud)
{
    setup();
}

QGpioPort::~QGpioPort()
{
    //set port to input/pud off
    m_pud = QGpio::PULL_OFF;
    m_direction = QGpio::DIRECTION_INPUT;
    if (m_valueFd != -1) {
        close(m_valueFd);
        m_valueFd = -1;
    }
    gpioUnexport();
}

void QGpioPort::setGpioParent(QGpio *gpio)
{
    m_gpio = gpio;
}

void QGpioPort::clearEventDetect()
{
//    int offset = EVENT_DETECT_OFFSET + (m_port / 32);
//    int shift = (m_port % 32);

//    *(m_gpio->getGpioMap() + offset) |= (1 << shift);
//    short_wait();
//    *(m_gpio->getGpioMap() + offset) = 0;
}

int QGpioPort::eventDetected()
{
    int offset, value, bit;

//    offset = EVENT_DETECT_OFFSET + (m_port / 32);
//    bit = (1 << (m_port % 32));
//    value = *(m_gpio->getGpioMap() + offset) & bit;
//    if (value)
//        clearEventDetect();
    return value;
}

void QGpioPort::setRisingEvent(bool enable)
{
    int offset = RISING_ED_OFFSET + (m_port / 32);
    int shift = (m_port % 32);

//    if (enable)
//        *(m_gpio->getGpioMap() + offset) |= 1 << shift;
//    else
//        *(m_gpio->getGpioMap() + offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setFallingEvent(bool enable)
{
    int offset = FALLING_ED_OFFSET + (m_port / 32);
    int shift = (m_port % 32);

//    if (enable) {
//        *(m_gpio->getGpioMap() + offset) |= (1 << shift);
//        *(m_gpio->getGpioMap() + offset) = (1 << shift);
//    } else {
//        *(m_gpio->getGpioMap() + offset) &= ~(1 << shift);
//    }
    clearEventDetect();
}

void QGpioPort::setHighEvent(bool enable)
{
    int offset = HIGH_DETECT_OFFSET+ (m_port / 32);
    int shift = (m_port % 32);

//    if (enable)
//        *(m_gpio->getGpioMap()+offset) |= (1 << shift);
//    else
//        *(m_gpio->getGpioMap()+offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setLowEvent(bool enable)
{
    int offset = LOW_DETECT_OFFSET+ (m_port / 32);
    int shift = (m_port % 32);

//    if (enable)
//        *(m_gpio->getGpioMap() + offset) |= 1 << shift;
//    else
//        *(m_gpio->getGpioMap() + offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setPullupdn(QGpio::GpioPullUpDown pud)
{
    //bcm2835_gpio_set_pud(m_port, pud);
    m_pud = pud;
}

void QGpioPort::setup()
{
    if (!gpioExport()) {
        return;
    }
    if (!gpioSetDirection(QGpio::DIRECTION_INPUT)) {
        gpioUnexport();
        return;
    }
    m_valueFd = openValueFile();
    if (m_valueFd < 0) {
        gpioUnexport();
        qWarning() << "Error opening value file";
        return;
    }
}

void QGpioPort::setDirection(QGpio::GpioDirection direction)
{
    m_direction = direction;
    if (gpioSetDirection(m_direction) == false) {
        qWarning() << "Error set direction" << m_direction << "for GPIO" << m_port;
    }
}

QGpio::GpioDirection QGpioPort::getDirection()
{
    return m_direction;
}

#define x_write(fd, buf, len) do {                                  \
size_t x_write_len = (len);                                     \
    \
    if ((size_t)write((fd), (buf), x_write_len) != x_write_len) {   \
        close(fd);                                                  \
        return (-1);                                                \
}                                                               \
} while (/* CONSTCOND */ 0)

void QGpioPort::setValue(QGpio::GpioValue value)
{
    char str_value[2];
    int len = snprintf(str_value, sizeof(str_value), "%s", value == QGpio::VALUE_HIGH ? "1" : "0");
    if (write(m_valueFd, str_value, len) != len) {
        qWarning() << "Error writing value" << value << "to port" << m_port;
    }
}

QGpio::GpioValue QGpioPort::value()
{
    QGpio::GpioValue value = QGpio::VALUE_LOW;
    char str_value[2];
    if (read(m_valueFd, str_value, 1) != 1) {
        qWarning() << "Error reading value from port" << m_port;
    } else {
        if (strncmp(str_value, "1", 1) == 0) {
            value = QGpio::VALUE_HIGH;
        }
    }
    return value;
}



void QGpioPort::removeEdgeDetect()
{
    gpioSetEdge(QGpio::NO_EDGE);
    if (m_valueFd > 0) {
        close(m_valueFd);
    }
    gpioUnexport();
    if (!m_gpio.isNull()) {
        m_gpio->removeFromInputEventsThread(this);
    }
}

bool QGpioPort::addEdgeDetect(QGpio::GpioEdge edge, int bouncetime)
{
    gpioSetEdge(edge);
    m_edge = edge;
    m_bouncetime = bouncetime;

    return true;
}

bool QGpioPort::gpioExport()
{
    int fd, len;
    char str_gpio[3];
    char filename[33];

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d", m_port);

    /* return if gpio already exported */
    if (access(filename, F_OK) != -1) {
        return true;
    }

    if ((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0) {
        return false;
    }

    len = snprintf(str_gpio, sizeof(str_gpio), "%d", m_port);
    x_write(fd, str_gpio, len);
    close(fd);

    return true;
}

int QGpioPort::gpioUnexport()
{
    int fd, len;
    char str_gpio[3];

    if ((fd = open("/sys/class/gpio/unexport", O_WRONLY)) < 0)
        return -1;

    len = snprintf(str_gpio, sizeof(str_gpio), "%d", m_port);
    x_write(fd, str_gpio, len);
    close(fd);

    return 0;
}

bool QGpioPort::gpioSetDirection(QGpio::GpioDirection direction)
{
    int retry;
    struct timespec delay;
    int fd;
    char filename[33];

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/direction", m_port);

    // retry waiting for udev to set correct file permissions
    delay.tv_sec = 0;
    delay.tv_nsec = 10000000L; // 10ms
    for (retry=0; retry<100; retry++) {
        if ((fd = open(filename, O_WRONLY)) >= 0)
            break;
        nanosleep(&delay, NULL);
    }
    if (retry >= 100)
        return false;

    if (direction == QGpio::DIRECTION_INPUT)
        x_write(fd, "in", 3);
    else
        x_write(fd, "out", 4);

    close(fd);
    m_direction = direction;
    return true;
}

const char *stredge[4] = {"none", "rising", "falling", "both"};

bool QGpioPort::gpioSetEdge(QGpio::GpioEdge edge)
{
    int fd;
    char filename[28];

    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/edge", m_port);

    if ((fd = open(filename, O_WRONLY)) < 0)
        return false;

    x_write(fd, stredge[edge], strlen(stredge[edge]) + 1);
    close(fd);
    m_edge = edge;
    return true;
}

quint64 QGpioPort::getLastCallTimestamp() const
{
    return m_lastCallTimestamp;
}

void QGpioPort::setLastCallTimestamp(const quint64 &lastCallTimestamp)
{
    m_lastCallTimestamp = lastCallTimestamp;
}

bool QGpioPort::getInitialTrigger() const
{
    return m_initialTrigger;
}

void QGpioPort::setInitialTrigger(bool initialTrigger)
{
    m_initialTrigger = initialTrigger;
}

int QGpioPort::getBouncetime() const
{
    return m_bouncetime;
}

void QGpioPort::setBouncetime(int bouncetime)
{
    m_bouncetime = bouncetime;
}

int QGpioPort::getPort() const
{
    return m_port;
}

int QGpioPort::getValueFd() const
{
    return m_valueFd;
}

int QGpioPort::openValueFile()
{
    int fd;
    char filename[29];

    // create file descriptor of value file
    snprintf(filename, sizeof(filename), "/sys/class/gpio/gpio%d/value", m_port);
    if ((fd = open(filename, O_RDONLY | O_NONBLOCK)) < 0)
        return -1;
    return fd;
}
