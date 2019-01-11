#include "servosoftware.h"
#include <QDebug>
#include <QCoreApplication>

inline unsigned long delayus(float angle) {
    //SG90 servo speed 60 grad by 0.1sec
    return (unsigned long)((100000.0/60.0)*angle);
}

ServoSoftware::ServoSoftware(int port, QObject *parent) : ServoBase (parent)
{
    m_pwm = new PwmSoftware(port);
    m_pwm->pwmSetFrequency(50.0);
    m_pwm->startPwm(0);
}

ServoSoftware::~ServoSoftware()
{
    m_pwm->stopPwm(-1);
    delete m_pwm;
}

void ServoSoftware::stopRotating()
{
     m_pwm->pwmSetDutyCycle(-1, 0.0);
}

void ServoSoftware::startRotating(float torque)
{

    if (torque < -1.0f || torque > 1.0f) {
        qWarning() << "Invalid torque value" << torque << "Must be betwee -1.0 and 1.0";
        return;
    }
    float duty_cycle = torque;
    if (torque < 0)
        duty_cycle = (float)servoMinPulse()/200.0 * qAbs(torque);
    else if (torque > 0.0)
        duty_cycle = (float)servoMaxPulse()/200.0 * torque;
    //qDebug() << servoMinPulse() << servoMaxPulse() << torque << duty_cycle;
    m_pwm->pwmSetDutyCycle(-1, duty_cycle);
}

void ServoSoftware::rotateAngle(float angle)
{
    startRotating(angle);
    QThread::usleep(delayus(qAbs(angle)));
    stopRotating();
}

void ServoSoftware::setAngle(float angle)
{
    Q_UNUSED(angle)
    //not yet implemented
}

void ServoSoftware::recalcDuty()
{
    //empty for now
}
