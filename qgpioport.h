#ifndef QGPIOPORT_H
#define QGPIOPORT_H

#include <QThread>
#include <QPointer>
#include "qgpio.h"

class QGpioPort : public QThread
{
public:

    // QThread interface
    friend class QGpio;
protected:
    QGpioPort(int port, QGpio::GpioDirection direction, QGpio::PullUpDown pud, QThread* parent = nullptr);
    void run() override;
    void setGpio(QGpio* gpio);

private:
    void clearEventDetect();
    int eventDetected();
    void setRisingEvent(bool enable);
    void setFallingEvent(bool enable);
    void setHighEvent(bool enable);
    void setLowEvent(bool enable);
    void setPullupdn(QGpio::PullUpDown pud);
    void setup();
    QGpio::GpioDirection getDirection();
    void setValue(QGpio::GpioValue value);
    QGpio::GpioValue value();

private:
    int m_port;
    QGpio::GpioDirection m_direction;
    QGpio::PullUpDown m_pud;
    QPointer<QGpio> m_gpio;

    // PWM data for the port
    float m_pwmFreq;
    float m_pwmDutyCycle;
    float m_pwmBaseTime;
    float m_pwmSliceTime;
    struct timespec m_pwmReqOn, m_pwmReqOff;
};

#endif // QGPIOPORT_H
