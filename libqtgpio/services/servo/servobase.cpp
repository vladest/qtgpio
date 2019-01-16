#include "servobase.h"
#include <QMetaEnum>
#include <QMap>

const QMap<ServoBase::KnownServos, QPair<uint16_t, uint16_t> > knownServosPulses = {
    { ServoBase::TowerPro_MG90S, QPair<uint16_t, uint16_t>(550, 2250) },
    { ServoBase::TowerPro_MG995, QPair<uint16_t, uint16_t>(500, 2300) }
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
}

void ServoBase::setServoPulses(uint16_t minPulse, uint16_t maxPulse)
{
    m_servoMinPulse = minPulse;
    m_servoMaxPulse = maxPulse;
    recalcDuty();
}

void ServoBase::setServoPulses(ServoBase::KnownServos servo)
{
    QPair<uint16_t, uint16_t> _pulses = knownServosPulses.value(servo);
    setServoPulses(_pulses.first, _pulses.second);
}

QPair<uint16_t, uint16_t> ServoBase::servoPulses(ServoBase::KnownServos servo)
{
    return knownServosPulses.value(servo);
}

QPair<uint16_t, uint16_t> ServoBase::servoPulses(const char* name)
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
