#pragma once

#include <QObject>
#include "pwm/pwmsoftware.h"
#include "consolereader.h"
#include "servo/servobase.h"

class ServoExample : public QObject
{
    Q_OBJECT
public:
    explicit ServoExample(QObject *parent = nullptr);
    virtual ~ServoExample();

public slots:
    void OnConsoleKeyPressed(char ch);
private:
    ConsoleReader *m_consoleReader = nullptr;
    ServoBase* servoPortLeftRight = nullptr;
    ServoBase* servoPortUpDown = nullptr;
    PwmSoftware* buzzer;
};

