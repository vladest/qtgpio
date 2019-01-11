#pragma once

#include <QObject>
#include "servobase.h"
#include "pwmsoftware.h"

class ServoSoftware : public ServoBase
{
    Q_OBJECT
public:
    explicit ServoSoftware(int port, QObject *parent = nullptr);
    virtual ~ServoSoftware();

    void stopRotating() override;
    void startRotating(float torque) override;
    void rotateAngle(float angle) override;
    void setAngle(float angle) override;
    void recalcDuty() override;

private:
    PwmSoftware* m_pwm = nullptr;
};

