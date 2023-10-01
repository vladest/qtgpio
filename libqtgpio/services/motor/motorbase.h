#ifndef MOTORBASE_H
#define MOTORBASE_H

#include <QObject>

namespace Motors {
    using MotorsEnum = uint8_t;
    constexpr MotorsEnum MOTOR_NONE = 0;
    constexpr MotorsEnum MOTOR_1    = 1 << 0;
    constexpr MotorsEnum MOTOR_2    = 1 << 1;
    constexpr MotorsEnum MOTOR_3    = 1 << 2;
    constexpr MotorsEnum MOTOR_4    = 1 << 3;
    constexpr MotorsEnum MOTOR_5    = 1 << 4;
    constexpr MotorsEnum MOTOR_6    = 1 << 5;
    constexpr MotorsEnum MOTOR_7    = 1 << 6;
    constexpr MotorsEnum MOTOR_8    = 1 << 7;
    constexpr MotorsEnum ALL_MOTORS = 0xFF;
}

class MotorBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
public:

    enum Motor {
        MOTOR_NONE = 0,
        MOTOR_1    = 1 << 0,
        MOTOR_2    = 1 << 1,
        MOTOR_3    = 1 << 2,
        MOTOR_4    = 1 << 3,
        MOTOR_5    = 1 << 4,
        MOTOR_6    = 1 << 5,
        MOTOR_7    = 1 << 6,
        MOTOR_8    = 1 << 7,
        ALL_MOTORS = 0xFF
    };

    Q_DECLARE_FLAGS(MotorsFlags, Motor)

    explicit MotorBase(QObject *parent = nullptr);

    virtual void forward(MotorsFlags motors = ALL_MOTORS) = 0;
    virtual void stop(MotorsFlags motors = ALL_MOTORS) = 0;
    virtual void reverse(MotorsFlags motors = ALL_MOTORS) = 0;
    virtual void setSpeed(float speed, MotorsFlags motors = ALL_MOTORS);
    virtual float speed(MotorsFlags motors = ALL_MOTORS) const;

    Motors::MotorsEnum motor() const;
    void setMotor(Motors::MotorsEnum newMotor);

signals:
    void speedChanged(float speed);

protected:
    float m_speed = 50.0f;
    Motors::MotorsEnum m_motor =  Motors::MOTOR_NONE;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MotorBase::MotorsFlags)
#endif // MOTORBASE_H
