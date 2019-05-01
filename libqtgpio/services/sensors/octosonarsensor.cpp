#include "octosonarsensor.h"
#include "qgpio.h"
#include <QDebug>

const static float MAX_DISTANCE = 400;
const static float DIST_SCALE = 58.0;
const static float TRAVEL_TIME_MAX = MAX_DISTANCE * DIST_SCALE;

OctoSonarSensor::OctoSonarSensor(int interruptPort, uint8_t i2cPort, QObject *parent) : QThread(parent)
{
    if (QGpio::getInstance()->init() == QGpio::INIT_OK) {
        m_interruptPort = QGpio::getInstance()->allocateGpioPort(interruptPort, QGpio::DIRECTION_INPUT);
    } else {
        qWarning() << "Error initing QGpio";
    }
    m_nAddress = i2cPort;
    bcm2835_i2c_begin();     //Start I2C operations.
    i2csetup();

}

void OctoSonarSensor::i2csetup(void) {
    bcm2835_i2c_setSlaveAddress(m_nAddress);
    bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626);
    //bcm2835_i2c_set_baudrate(10000); //1M baudrate
}

void OctoSonarSensor::i2cwrite(uint16_t data) {
    uint8_t buffer[2];

    buffer[0] = (uint8_t) (data & 0xFF);
    buffer[1] = (uint8_t) (data >> 8);

    i2csetup();

    if (bcm2835_i2c_write((char *) buffer, 2) != BCM2835_I2C_REASON_OK)
        qDebug() << "error writing to i2c";
}

OctoSonarSensor::~OctoSonarSensor()
{
    stop();
    bcm2835_i2c_end();
    bcm2835_close();
    QGpio::getInstance()->deallocateGpioPort(m_interruptPort);
}

float OctoSonarSensor::distance() const
{
    return m_distance;
}

void OctoSonarSensor::stop()
{
    requestInterruption();
    wait(1000);
}


void OctoSonarSensor::run()
{
    uint16_t data = 0xffff;
    i2cwrite(data);

    while (!isInterruptionRequested()) {

        data = ~((uint16_t)1 << /*_currentSonar*/14);
        i2cwrite(data);

        //            bcm2835_delay(500);
        //            buf[0] = 0xFF;     //LED OFF
        //            bcm2835_i2c_write(buf,1);
        //            bcm2835_delay(500);
        /*
        // Send trig pulse
        m_triggerPort->setValue(QGpio::VALUE_HIGH);
        delayMicroseconds(20);
        m_triggerPort->setValue(QGpio::VALUE_LOW);
*/
        // Wait for echo. Very rarely (2 of 12K at 20Hz)
        // see ECHO never go HIGH so we include a way to
        // bail.
        //qDebug() << "int val" << m_interruptPort->value();
        int bail = 10000;
        while(m_interruptPort->value() == QGpio::VALUE_LOW) {
            if (--bail == 0) {
                break;
            }
        }

        if (bail == 0) {
            qDebug() << "bail out" << m_interruptPort->value();
            delayMicroseconds(m_delay);
            continue;
        } else {
            qDebug() << "HIGH";
        }
        // Measure time for echo. Return early if the
        // pulse is appearing to take too long. Note:
        // error case of never going LOW results in
        // MAX reading :/
        uint64_t startTime = bcm2835_st_read();
        uint64_t travelTime = 0;
        while(m_interruptPort->value() == QGpio::VALUE_HIGH) {
            travelTime = bcm2835_st_read() - startTime;
            if (travelTime > TRAVEL_TIME_MAX) {
                break;
            }
        }

        if (travelTime > 0 && travelTime <= TRAVEL_TIME_MAX) {
            // Return distance in cm
            float _dist = (double)travelTime / DIST_SCALE;
            if (_dist != m_distance) {
                m_distance = _dist;
                qDebug() << "dist" << m_distance;
                emit distanceChanged(m_distance);
            }
        } else {
            qDebug() << "travel time" << travelTime;
        }
    }

    delayMicroseconds(m_delay);
}


int OctoSonarSensor::measureRate() const
{
    return m_measureRate;
}

void OctoSonarSensor::setMeasureRate(int measureRate)
{
    m_measureRate = measureRate;
    m_delay = 1000000ULL/m_measureRate;
}


