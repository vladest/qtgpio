#ifndef PWMSOFTWARE_H
#define PWMSOFTWARE_H

#include <QtCore/QThread>
#include <QtCore/QPointer>
#include "qgpioport.h"
#include "pwmbase.h"

class PwmSoftware : public PwmBase
{
public:
    PwmSoftware(int port);
    virtual ~PwmSoftware();

    void pwmSetDutyCycle(int channel, const QVariant& dutycycle) override;
    QVariant pwmDutyCycle(int channel) override;

    void pwmSetFrequency(float freq) override;
    float pwmFrequency() override;

    void startPwm(int channel, const QVariant& dutyCycle = 0) override;
    void stopPwm(int channel) override;
    QPointer<QGpioPort> pwmPort() const;

private:
    /**
     * @brief pwmThreadRun: runner function for PWM thread
     */
    void pwmThreadRun();
    void pwmCalculateTimes();
private:
    QPointer<QGpioPort> m_pwmPort;
    // PWM data for the port
    qint64 m_pwmReqOn = 0;
    qint64 m_pwmReqOff = 0;
    float m_pwmBaseTime = 1.0f;
    float m_pwmSliceTime = 0.01f;

    QGpio* m_gpio;
    QThread* m_pwmRunner = nullptr;
};

#endif // PWMSOFTWARE_H
