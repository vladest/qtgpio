#include <QCoreApplication>
#include <QDebug>
#include "qgpio.h"
#include "qgpioport.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QGpio* gpio = QGpio::getInstance();
    QGpio::InitResult res = gpio->init();
    qDebug() << "Result" << (int)res << gpio->getGpioMap();
    QGpioPort* servoPort = gpio->allocateGpioPort(17, QGpio::DIRECTION_OUTPUT);
    if (servoPort == nullptr)
        return 2;
    servoPort->pwmSetFrequency(50.0);
    servoPort->startPwm(2.5);
    servoPort->pwmSetDutyCycle(5);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(7.5);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(10);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(12.5);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(10);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(7.5);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(5);
    QThread::msleep(500);
    servoPort->pwmSetDutyCycle(2.5);
    QThread::msleep(500);
    servoPort->stopPwm();
    //return a.exec();
}
