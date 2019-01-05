#pragma once

#include <QObject>
#include "qgpio.h"
#include "qgpioport.h"
#include "consolereader.h"
#include "servocontrol.h"

class ServoExample : public QObject
{
    Q_OBJECT
public:
    explicit ServoExample(QObject *parent = nullptr);
    virtual ~ServoExample();

    void stop();

    void startCounterClockWise();
    void startClockWise();
    void counterClockWiseAngle(float angle);
    void clockWiseAngle(float angle);

signals:

public slots:
    void OnConsoleKeyPressed(char ch);
private:
    ConsoleReader *m_consoleReader = nullptr;
    QGpio* m_gpio = nullptr;
    ServoControl* servoPortLeftRight = nullptr;
    ServoControl* servoPortUpDown = nullptr;
    QPointer<QGpioPort> buzzer;
};

