#include "ultraborgsensor.h"
#include "qgpio.h"
#include <QDebug>

const static float MAX_DISTANCE = 400;
const static float DIST_SCALE = 58.0;
const static float TRAVEL_TIME_MAX = MAX_DISTANCE * DIST_SCALE;

UltraborgSensor::UltraborgSensor(int sensorNum, uint8_t i2cPort, QObject *parent) : m_sensor(sensorNum), QThread(parent)
{
    m_ultraborg = UltraBorg::getInstance();
    m_ultraborg->init(i2cPort);
}

UltraborgSensor::~UltraborgSensor()
{
    stop();
    m_ultraborg->deinit();
}

float UltraborgSensor::distance() const
{
    return m_distance;
}

void UltraborgSensor::stop()
{
    requestInterruption();
    wait(1000);
}


void UltraborgSensor::run()
{
    while (!isInterruptionRequested()) {
        m_distance = m_ultraborg->getRawDistance(m_sensor);
        emit distanceChanged(m_distance);
        delayMicroseconds(m_delay);
    }

    delayMicroseconds(m_delay);
}


int UltraborgSensor::measureRate() const
{
    return m_measureRate;
}

void UltraborgSensor::setMeasureRate(int measureRate)
{
    m_measureRate = measureRate;
    m_delay = 1000000ULL/m_measureRate;
}


