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

    void pwmSetDutyCycle(float dutycycle);
    float pwmDutyCycle() const;

    void pwmSetFrequency(float freq);
    float pwmFrequency() const;

    void startPwm(float dutyCycle = 0.0);
    void stopPwm();
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

    QGpio* m_gpio;
    QThread* m_pwmRunner = nullptr;
};

#endif // PWMSOFTWARE_H
