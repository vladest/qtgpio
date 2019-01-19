#ifndef MOTORMC33886_H
#define MOTORMC33886_H

#include <QObject>
#include "qgpioport.h"

class PwmSoftware;

class MotorMC33886 : public QObject
{
    Q_OBJECT
public:
    explicit MotorMC33886(QObject *parent = nullptr);

    void forward();
    void stop();
    void reverse();
    void left();
    void right();
    void setSpeed(float speed);
    float speed() const;

signals:

public slots:
private:
    void setMotor(QGpio::GpioValue a1, QGpio::GpioValue a2, QGpio::GpioValue b1, QGpio::GpioValue b2);

private:
    PwmSoftware* m_speedLeft = nullptr;
    PwmSoftware* m_speedRight = nullptr;
    QGpioPort* m_portA1 = nullptr;
    QGpioPort* m_portA2 = nullptr;
    QGpioPort* m_portB1 = nullptr;
    QGpioPort* m_portB2 = nullptr;
    float m_speed = 50.0f;
};

#endif // MOTORMC33886_H
