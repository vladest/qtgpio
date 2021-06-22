#include "motor_hat_stepper.h"
#include <unistd.h>
#include "math.h"
#include <QDebug>


Motor::Motor(uint8_t address, uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2, QObject *parent) :
    MotorBase(parent),
    m_speedChannel(speedChannel),
    m_inChannel1(inChannel1),
    m_inChannel2(inChannel2)
{
    m_pca9685.init(address);
    m_pca9685.setPWMFreq(1600);

    for (uint8_t i=0; i < 16; i++)
        m_pca9685.setPWM(i, 0, 0);
}

Motor::~Motor()
{
    stop();
}

void Motor::forward()
{
    m_motorActive = true;

    qDebug() << "fwd" << m_inChannel1 << m_inChannel2;
    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel2, 0);
    m_pca9685.setLevel(m_inChannel1, 1);
}

void Motor::stop()
{
    m_motorActive = false;
    //m_pca9685.setPwmDutyCycle(m_speedChannel, 0);
    m_pca9685.setLevel(m_inChannel2, 0);
    m_pca9685.setLevel(m_inChannel1, 0);
}

void Motor::reverse()
{
    m_motorActive = true;

    qDebug() << "backw" << m_inChannel1 << m_inChannel2;
    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel1, 0);
    m_pca9685.setLevel(m_inChannel2, 1);
}

void Motor::setSpeed(float speed) {
    if (speed < 0.0f || speed > 100.0f)
        return;
    MotorBase::setSpeed(speed);
    if (m_motorActive)
        m_pca9685.setPwmDutyCycle(m_speedChannel, speed);
}

MotorStepperHat::MotorStepperHat(uint8_t address, QObject *parent) : QObject(parent)
{
    m_motors.append(new Motor(address, 8, 10, 9, this));
    m_motors.append(new Motor(address, 13, 11, 12, this));
    m_motors.append(new Motor(address, 2, 4, 3, this));
    m_motors.append(new Motor(address, 7, 5, 6, this));
}

MotorStepperHat::~MotorStepperHat()
{
}

void MotorStepperHat::forward(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag) {
            m_motors.at(var)->forward();
        }
        motor_flag = motor_flag << 1;
    }
}

void MotorStepperHat::stop(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag)
            m_motors.at(var)->stop();
        motor_flag = motor_flag << 1;
    }
}

void MotorStepperHat::reverse(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag)
            m_motors.at(var)->reverse();
        motor_flag = motor_flag << 1;
    }
}

void MotorStepperHat::setSpeed(QList<QPair<MotorBase::MotorsEnum, float>> motors)
{
    int motor_flag = 0x01;
    for (const QPair<MotorBase::MotorsEnum, float>& _motSpeed : motors) {
        motor_flag = 0x01;
        for (int var = 0; var < m_motors.size(); ++var) {
            if (_motSpeed.first == motor_flag)
                m_motors.at(var)->setSpeed(_motSpeed.second);
            motor_flag = motor_flag << 1;
        }
    }
}

float MotorStepperHat::speed(MotorBase::MotorsEnum motor) const
{
    return m_motors.at(0)->speed();
}

