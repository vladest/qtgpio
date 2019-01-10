#pragma once

#include <QObject>
#include "pwmpca9685.h"

class ServoControl9685 : public QObject
{
    Q_OBJECT
public:
    explicit ServoControl9685(int channel, QObject *parent = nullptr);
    virtual ~ServoControl9685();

    void stop();

    void startCounterClockWise();
    void startClockWise();
    void counterClockWiseAngle(float angle);
    void clockWiseAngle(float angle);

signals:

private:
    PwmPCA9685* m_pwm = nullptr;
    uint8_t m_channel = 0xff;
    uint16_t m_minDuty = 0;
    uint16_t m_maxDuty = 0;
    uint16_t m_dutyRange = 0;
};

