#pragma once

#include <QObject>
#include "servobase.h"
#include "pwm/pwmpca9685.h"

class ServoControl9685 : public ServoBase
{
    Q_OBJECT
public:
    explicit ServoControl9685(int channel, QObject *parent = nullptr);
    virtual ~ServoControl9685();

    void stopRotating() override;
    void startRotating(float torque) override;
    void rotateAngle(float angle) override;
    void setAngle(float angle) override;
    void recalcDuty() override;

private:
    PwmPCA9685* m_pwm = nullptr;
    uint8_t m_channel = 0xff;
};

