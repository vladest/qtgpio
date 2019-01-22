#pragma once

#include <QObject>
#include <QScopedPointer>
#include "qgpioport.h"
#include "motorbase.h"

class PwmSoftware;

/**
 * @brief MotorDriverBoard
 * @param parent
 * support for MC33886 Raspberry Pi Motor Driver Board by Waveshare
 * https://www.waveshare.com/wiki/RPi_Motor_Driver_Board
 *
 */

class MotorMC33886 : public MotorBase
{
    Q_OBJECT
public:
    explicit MotorMC33886(int speedPort, int p1, int p2, QObject *parent = nullptr);
    virtual ~MotorMC33886();

    void forward() override;
    void stop() override;
    void reverse() override;
    void setSpeed(float speed) override;

private:
    void setMotor(QGpio::GpioValue a1, QGpio::GpioValue a2);

private:
    PwmSoftware* m_speedPwm = nullptr;
    PwmSoftware* m_speedRight = nullptr;
    QGpioPort* m_port1 = nullptr;
    QGpioPort* m_port2 = nullptr;
    QGpioPort* m_portB1 = nullptr;
    QGpioPort* m_portB2 = nullptr;
};

class MotorDriverBoard: public QThread {

    Q_OBJECT
public:
    enum MotorsEnum {
        LEFT_MOTOR = 1,
        RIGHT_MOTOR = 2,
        BOTH_MOTORS = 3
    };
    Q_ENUM(MotorsEnum);

    explicit MotorDriverBoard(QObject *parent = nullptr);
    virtual ~MotorDriverBoard();

    void forward(MotorsEnum motors);
    void stop(MotorsEnum motors);
    void reverse(MotorsEnum motors);
    void setSpeed(float speedLeft, float speedRight);
    float speedLeft() const;
    float speedRight() const;

    void startIR();
    void stopIR();
signals:
    // connect via queued connection
    void irCommandChanged(int command);

protected:
    void run() override;

private:
    QScopedPointer<MotorMC33886> m_motorLeft;
    QScopedPointer<MotorMC33886> m_motorRight;
    QPointer<QGpioPort> m_irPort;
};

