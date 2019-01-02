#include "qgpioport.h"

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

QGpioPort::QGpioPort(int port, QGpio::GpioDirection direction, QGpio::PullUpDown pud, QThread *parent) :
    m_port(port), m_direction(direction), m_pud(pud),
    QThread (parent)
{
    setup();
}

void QGpioPort::run()
{
}

void QGpioPort::setGpio(QGpio *gpio)
{
    m_gpio = gpio;
}

void QGpioPort::clearEventDetect()
{
    int offset = EVENT_DETECT_OFFSET + (m_port/32);
    int shift = (m_port%32);

    *(m_gpio->getGpioMap()+offset) |= (1 << shift);
    short_wait();
    *(m_gpio->getGpioMap()+offset) = 0;
}

int QGpioPort::eventDetected()
{
    int offset, value, bit;

    offset = EVENT_DETECT_OFFSET + (m_port / 32);
    bit = (1 << (m_port % 32));
    value = *(m_gpio->getGpioMap() + offset) & bit;
    if (value)
        clearEventDetect();
    return value;
}

void QGpioPort::setRisingEvent(bool enable)
{
    int offset = RISING_ED_OFFSET + (m_port / 32);
    int shift = (m_port % 32);

    if (enable)
        *(m_gpio->getGpioMap()+offset) |= 1 << shift;
    else
        *(m_gpio->getGpioMap()+offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setFallingEvent(bool enable)
{
    int offset = FALLING_ED_OFFSET + (m_port / 32);
    int shift = (m_port % 32);

    if (enable) {
        *(m_gpio->getGpioMap()+offset) |= (1 << shift);
        *(m_gpio->getGpioMap()+offset) = (1 << shift);
    } else {
        *(m_gpio->getGpioMap()+offset) &= ~(1 << shift);
    }
    clearEventDetect();
}

void QGpioPort::setHighEvent(bool enable)
{
    int offset = HIGH_DETECT_OFFSET+ (m_port / 32);
    int shift = (m_port % 32);

    if (enable)
        *(m_gpio->getGpioMap()+offset) |= (1 << shift);
    else
        *(m_gpio->getGpioMap()+offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setLowEvent(bool enable)
{
    int offset = LOW_DETECT_OFFSET+ (m_port / 32);
    int shift = (m_port % 32);

    if (enable)
        *(m_gpio->getGpioMap()+offset) |= 1 << shift;
    else
        *(m_gpio->getGpioMap()+offset) &= ~(1 << shift);
    clearEventDetect();
}

void QGpioPort::setPullupdn(QGpio::PullUpDown pud)
{
    int clk_offset = PULLUPDNCLK_OFFSET + (m_port / 32);
    int shift = (m_port % 32);

    if (pud == QGpio::PUD_DOWN)
        *(m_gpio->getGpioMap()+PULLUPDN_OFFSET) = (*(m_gpio->getGpioMap()+PULLUPDN_OFFSET) & ~3) | QGpio::PUD_DOWN;
    else if (pud == QGpio::PUD_UP)
        *(m_gpio->getGpioMap()+PULLUPDN_OFFSET) = (*(m_gpio->getGpioMap()+PULLUPDN_OFFSET) & ~3) | QGpio::PUD_UP;
    else  // pud == PUD_OFF
        *(m_gpio->getGpioMap()+PULLUPDN_OFFSET) &= ~3;

    short_wait();
    *(m_gpio->getGpioMap() + clk_offset) = 1 << shift;
    short_wait();
    *(m_gpio->getGpioMap() + PULLUPDN_OFFSET) &= ~3;
    *(m_gpio->getGpioMap() + clk_offset) = 0;
    m_pud = pud;
}

void QGpioPort::setup()
{
    int offset = FSEL_OFFSET + (m_port / 10);
    int shift = (m_port % 10) * 3;

    setPullupdn(m_pud);
    if (m_direction == QGpio::DIRECTION_OUTPUT)
        *(m_gpio->getGpioMap()+offset) = (*(m_gpio->getGpioMap()+offset) & ~(7<<shift)) | (1<<shift);
    else  // direction == INPUT
        *(m_gpio->getGpioMap()+offset) = (*(m_gpio->getGpioMap()+offset) & ~(7<<shift));
}

// Contribution by Eric Ptak <trouch@trouch.com>
QGpio::GpioDirection QGpioPort::getDirection()
{
    int offset = FSEL_OFFSET + (m_port / 10);
    int shift = (m_port % 10) * 3;
    int value = *(m_gpio->getGpioMap()+offset);
    value >>= shift;
    value &= 7;
    return (QGpio::GpioDirection)value; // 0=input, 1=output, 4=alt0
}

void QGpioPort::setValue(QGpio::GpioValue value)
{
    int offset, shift;

    if (value == QGpio::VALUE_HIGH) // value == HIGH
        offset = SET_OFFSET + (m_port / 32);
    else       // value == LOW
       offset = CLR_OFFSET + (m_port / 32);

    shift = (m_port % 32);

    *(m_gpio->getGpioMap()+offset) = 1 << shift;
}

QGpio::GpioValue QGpioPort::value()
{
   int offset, value, mask;

   offset = PINLEVEL_OFFSET + (m_port / 32);
   mask = (1 << m_port % 32);
   value = *(m_gpio->getGpioMap() + offset) & mask;
   return (QGpio::GpioValue)value;
}
