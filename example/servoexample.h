#pragma once

#include <QObject>
#include "pwmsoftware.h"
#include "consolereader.h"
#include "servocontrol.h"
#include "servocontrol9685.h"

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
    ServoControl9685* servoPortLeftRight = nullptr;
    ServoControl9685* servoPortUpDown = nullptr;
    PwmSoftware* buzzer;
};

