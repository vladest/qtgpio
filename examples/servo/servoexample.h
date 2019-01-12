#pragma once

#include <QObject>
#include "consolereader.h"
#include "servo/servobase.h"

class ServoExample : public QObject
{
    Q_OBJECT
public:
    explicit ServoExample(QObject *parent = nullptr);
    virtual ~ServoExample();

public slots:
    void onConsoleKeyPressed(int ch);
private:
    ConsoleReader *m_consoleReader = nullptr;
    ServoBase* servoPortLeftRight = nullptr;
    ServoBase* servoPortUpDown = nullptr;
};

