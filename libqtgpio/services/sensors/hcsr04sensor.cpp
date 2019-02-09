#include "hcsr04sensor.h"
#include "qgpio.h"
#include <QDebug>

const static float MAX_DISTANCE = 400;
const static float DIST_SCALE = 58.0;
const static float TRAVEL_TIME_MAX = MAX_DISTANCE * DIST_SCALE;

HCSR04Sensor::HCSR04Sensor(int echoPort, int triggerPort, QObject *parent) : QThread(parent)
{
    if (QGpio::getInstance()->init() == QGpio::INIT_OK) {
        m_triggerPort = QGpio::getInstance()->allocateGpioPort(triggerPort, QGpio::DIRECTION_OUTPUT);
        m_echoPort = QGpio::getInstance()->allocateGpioPort(echoPort, QGpio::DIRECTION_INPUT);
        m_triggerPort->setValue(QGpio::VALUE_LOW);
        delay(500);
    }
}

HCSR04Sensor::~HCSR04Sensor()
{
    stop();
    QGpio::getInstance()->deallocateGpioPort(m_triggerPort);
    QGpio::getInstance()->deallocateGpioPort(m_echoPort);
}

float HCSR04Sensor::distance() const
{
    return m_distance;
}

void HCSR04Sensor::stop()
{
    requestInterruption();
    wait(1000);
}


void HCSR04Sensor::run()
{
    while (!isInterruptionRequested()) {

        // Send trig pulse
        m_triggerPort->setValue(QGpio::VALUE_HIGH);
        delayMicroseconds(20);
        m_triggerPort->setValue(QGpio::VALUE_LOW);

        // Wait for echo. Very rarely (2 of 12K at 20Hz)
        // see ECHO never go HIGH so we include a way to
        // bail.
        int bail = 1000;
        while(m_echoPort->value() == QGpio::VALUE_LOW) {
            if (--bail == 0) {
                break;
            }
        }

        if (bail == 0) {
            delayMicroseconds(m_delay);
            continue;
        }
        // Measure time for echo. Return early if the
        // pulse is appearing to take too long. Note:
        // error case of never going LOW results in
        // MAX reading :/
        uint64_t startTime = bcm2835_st_read();
        uint64_t travelTime = 0;
        while(m_echoPort->value() == QGpio::VALUE_HIGH) {
            travelTime = bcm2835_st_read() - startTime;
            if (travelTime > TRAVEL_TIME_MAX) {
                break;
            }
        }

        if (travelTime <= TRAVEL_TIME_MAX) {
            // Return distance in cm
            if (travelTime > 0) {
                float _dist = (double)travelTime / DIST_SCALE;
                if (_dist != m_distance) {
                    m_distance = _dist;
                    emit distanceChanged(m_distance);
                }
            }
        }

        delayMicroseconds(m_delay);
    }
}

int HCSR04Sensor::measureRate() const
{
    return m_measureRate;
}

void HCSR04Sensor::setMeasureRate(int measureRate)
{
    m_measureRate = measureRate;
    m_delay = 1000000ULL/m_measureRate;
}


