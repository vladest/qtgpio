#pragma once

#include <QObject>
#include "pwm/pwmsoftware.h"

class BuzzerExample : public QObject
{
    Q_OBJECT
public:
    explicit BuzzerExample(QObject *parent = nullptr);
    virtual ~BuzzerExample();

private:
    PwmSoftware* buzzer;
};

