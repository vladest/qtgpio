#pragma once

#include <QObject>
#include "pwmsoftware.h"

class ServoControl : public QObject
{
    Q_OBJECT
public:
    explicit ServoControl(int port, QObject *parent = nullptr);
    virtual ~ServoControl();

    void stop();

    void startCounterClockWise();
    void startClockWise();
    void counterClockWiseAngle(float angle);
    void clockWiseAngle(float angle);

signals:

private:
    PwmSoftware* m_pwm = nullptr;
};

