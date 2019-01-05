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
    m_gpio = QGpio::getInstance();
    QGpio::InitResult res = m_gpio->init();

    servoPort = m_gpio->allocateGpioPort(port, QGpio::DIRECTION_OUTPUT);
    servoPort->pwmSetFrequency(50.0);
    servoPort->startPwm(0);
}

ServoControl::~ServoControl()
{
    servoPort->stopPwm();
    m_gpio->deallocateGpioPort(servoPort);
}

void ServoControl::startCounterClockWise() {
    servoPort->pwmSetDutyCycle(12);
}

void ServoControl::startClockWise() {
    servoPort->pwmSetDutyCycle(2);
}

void ServoControl::counterClockWiseAngle(float angle) {
    startCounterClockWise();
    QThread::usleep(delayus(angle));
    servoPort->pwmSetDutyCycle(0);
}

void ServoControl::clockWiseAngle(float angle) {
    startClockWise();
    QThread::usleep(delayus(angle));
    servoPort->pwmSetDutyCycle(0);
}

void ServoControl::stop() {
    servoPort->setValue(QGpio::VALUE_LOW);
    servoPort->pwmSetDutyCycle(0.0);
}

