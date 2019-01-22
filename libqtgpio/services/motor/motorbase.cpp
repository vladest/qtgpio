#include "motorbase.h"

MotorBase::MotorBase(QObject *parent) : QObject(parent)
{

}

void MotorBase::setSpeed(float speed)
{
    if (speed != m_speed) {
        m_speed = speed;
        emit speedChanged(speed);
    }
}

float MotorBase::speed() const
{
    return m_speed;
}
