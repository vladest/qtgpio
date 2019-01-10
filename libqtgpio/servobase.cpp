#include "servobase.h"

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
