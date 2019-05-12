#ifndef SERVOCONTROLULTREBORG_H
#define SERVOCONTROLULTREBORG_H

#include <QObject>
#include <QScopedPointer>
#include "servobase.h"
#include "pwm/pwmultraborg.h"

class ServoControlUltraborg : public ServoBase
{
public:
    ServoControlUltraborg(int channel, uint8_t port = UB_DEFAULT_I2C_ADDRESS, QObject *parent = nullptr);
    virtual ~ServoControlUltraborg();
    void stopRotating() override;
    void startRotating(float torque) override;
    void rotateAngle(float angle) override;
    void setAngle(float angle) override;
    void recalcDuty() override;
    // reimplement
    void setServoPulses(uint16_t minPulse, uint16_t maxPulse) override;
private:
    QScopedPointer<PwmUltraborg> m_pwmUltraborg;
    int m_channel = -1;
};

#endif // SERVOCONTROLULTREBORG_H
