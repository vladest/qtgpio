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

    explicit MotorBase(QObject *parent = nullptr);

    virtual void forward() = 0;
    virtual void stop() = 0;
    virtual void reverse() = 0;
    virtual void setSpeed(float speed);
    virtual float speed() const;

    Motors::MotorsEnum motor() const;
    void setMotor(Motors::MotorsEnum newMotor);

signals:
    void speedChanged(float speed);

protected:
    float m_speed = 50.0f;
    Motors::MotorsEnum m_motor =  Motors::MOTOR_NONE;
};

#endif // MOTORBASE_H
