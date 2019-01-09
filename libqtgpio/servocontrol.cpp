#include "servocontrol.h"
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>

inline unsigned long delayus(float angle) {
    //SG90 servo speed 60 grad by 0.1sec
    return (unsigned long)((100000.0/60.0)*angle);
}

ServoControl::ServoControl(int port, QObject *parent) : QObject(parent)
{

    m_pwm = new PwmSoftware(port);
    m_pwm->pwmSetFrequency(50.0);
    m_pwm->startPwm(0);
}

ServoControl::~ServoControl()
{
    m_pwm->stopPwm();
    delete m_pwm;
}

void ServoControl::startCounterClockWise() {
    m_pwm->pwmSetDutyCycle(12);
}

void ServoControl::startClockWise() {
    m_pwm->pwmSetDutyCycle(2);
}

void ServoControl::counterClockWiseAngle(float angle) {
    startCounterClockWise();
    QThread::usleep(delayus(angle));
    m_pwm->pwmSetDutyCycle(0);
}

void ServoControl::clockWiseAngle(float angle) {
    startClockWise();
    QThread::usleep(delayus(angle));
    m_pwm->pwmSetDutyCycle(0);
}

void ServoControl::stop() {
    m_pwm->pwmPort()->setValue(QGpio::VALUE_LOW);
    m_pwm->pwmSetDutyCycle(0.0);
}

