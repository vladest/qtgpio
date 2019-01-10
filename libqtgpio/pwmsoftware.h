#ifndef PWMSOFTWARE_H
#define PWMSOFTWARE_H

#include <QtCore/QThread>
#include <QtCore/QPointer>
#include "qgpioport.h"
#include "pwmabstract.h"

class PwmSoftware : public PwmAbstract
{
public:
    PwmSoftware(int port);
    virtual ~PwmSoftware();

    void pwmSetDutyCycle(int channel, uint16_t dutycycle) override;
    uint16_t pwmDutyCycle(int channel) override;

    void pwmSetFrequency(float freq) override;
    float pwmFrequency() override;

    void startPwm(int channel, uint16_t dutyCycle = 0) override;
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
