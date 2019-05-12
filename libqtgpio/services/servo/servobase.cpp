#include "servobase.h"
#include <QMetaEnum>
#include <QMap>
#include <QDebug>
const QMap<ServoBase::KnownServos, ServoBase::ServoParameters > knownServosPulses = {
    { ServoBase::MG90S, { ServoBase::MG90S, 550, 2250, 180.0 } },
    { ServoBase::MG995, { ServoBase::MG995, 500, 2300, 180.0 } },
    { ServoBase::SG90, { ServoBase::SG90, 500, 2400, 135.0 } },
};

ServoBase::ServoBase(QObject *parent) : QObject(parent)
{

}

float ServoBase::actuactionRange() const
{
    return m_actuactionRange;
}

void ServoBase::setActuactionRange(float actuactionRange)
{
    m_actuactionRange = actuactionRange;
}

uint16_t ServoBase::minDuty() const
{
    return m_minDuty;
}

void ServoBase::setDuties(uint16_t minDuty, uint16_t maxDuty)
{
    m_minDuty = minDuty;
    m_maxDuty = maxDuty;
    m_dutyRange = m_maxDuty - m_minDuty;
    qDebug() << "duties" << m_minDuty << m_maxDuty;
}

void ServoBase::setServoPulses(uint16_t minPulse, uint16_t maxPulse)
{
    m_servoMinPulse = minPulse;
    m_servoMaxPulse = maxPulse;
    recalcDuty();
}

void ServoBase::setServoPulses(ServoBase::KnownServos servo)
{
    ServoParameters _pulses = knownServosPulses.value(servo);
    setServoPulses(_pulses.minPulse, _pulses.maxPulse);
    setActuactionRange(_pulses.actuationRange);
}

ServoBase::ServoParameters ServoBase::servoPulses(ServoBase::KnownServos servo)
{
    return knownServosPulses.value(servo);
}

ServoBase::ServoParameters ServoBase::servoPulses(const char* name)
{
    const QMetaObject &mo = ServoBase::staticMetaObject;
    int index = mo.indexOfEnumerator("KnownServos");
    QMetaEnum metaEnum = mo.enumerator(index);
    // note the explicit cast:
    ServoBase::KnownServos enumVal = (ServoBase::KnownServos)metaEnum.keyToValue(name);
    return servoPulses(enumVal);
}

uint16_t ServoBase::dutyRange() const
{
    return m_dutyRange;
}

uint16_t ServoBase::servoMinPulse() const
{
    return m_servoMinPulse;
}

uint16_t ServoBase::servoMaxPulse() const
{
    return m_servoMaxPulse;
}

uint16_t ServoBase::maxDuty() const
{
    return m_maxDuty;
}

void ServoBase::recalcDuty()
{

}
