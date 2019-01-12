#include "buzzer.h"
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>

BuzzerExample::BuzzerExample(QObject *parent) : QObject(parent)
{
    buzzer = new PwmSoftware(18);
    buzzer->pwmSetFrequency(450);
    buzzer->startPwm(-1, 1.0);
}

BuzzerExample::~BuzzerExample()
{
    delete buzzer;
}
