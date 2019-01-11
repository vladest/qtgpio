#pragma once

#include <QObject>
#include "pwm/pwmsoftware.h"
#include "consolereader.h"
#include "servo/servobase.h"
#include "sensors/hcsr04sensor.h"

class ServoExample : public QObject
{
    Q_OBJECT
public:
    explicit ServoExample(QObject *parent = nullptr);
    virtual ~ServoExample();

public slots:
    void OnConsoleKeyPressed(char ch);
    void onDistanceChanged(float distance);
private:
    ConsoleReader *m_consoleReader = nullptr;
    ServoBase* servoPortLeftRight = nullptr;
    ServoBase* servoPortUpDown = nullptr;
    PwmSoftware* buzzer;
    HCSR04Sensor* ultrasound = nullptr;
};

