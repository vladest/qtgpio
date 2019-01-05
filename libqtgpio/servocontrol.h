#pragma once

#include <QObject>
#include "qgpio.h"
#include "qgpioport.h"

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
    QGpio* m_gpio = nullptr;
    QPointer<QGpioPort> servoPort;
};

