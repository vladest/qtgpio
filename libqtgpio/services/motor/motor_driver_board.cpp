#include "motor_driver_board.h"
#include "services/pwm/pwmsoftware.h"
#include <QDebug>

MotorMC33886::MotorMC33886(int speedPort, int p1, int p2, QObject *parent) : MotorBase(parent)
{
    QGpio* gpio = QGpio::getInstance();
    m_speedPwm = new PwmSoftware(speedPort);
    m_port1 = gpio->allocateGpioPort(p1, QGpio::DIRECTION_OUTPUT);
    m_port2 = gpio->allocateGpioPort(p2, QGpio::DIRECTION_OUTPUT);
    m_speedPwm->pwmSetFrequency(500);
    m_speedPwm->startPwm(-1, m_speed);
}

MotorMC33886::~MotorMC33886()
{
    stop();
    QGpio* gpio = QGpio::getInstance();
    m_speedPwm->stopPwm(-1);
    gpio->deallocateGpioPort(m_port1);
    gpio->deallocateGpioPort(m_port2);
    delete m_speedPwm;
}

void MotorMC33886::setMotor(QGpio::GpioValue a1, QGpio::GpioValue a2)
{
    m_port1->setValue(a1);
    m_port2->setValue(a2);
}

void MotorMC33886::forward()
{
    setMotor(QGpio::VALUE_HIGH, QGpio::VALUE_LOW);
}

void MotorMC33886::stop()
{
    setMotor(QGpio::VALUE_LOW, QGpio::VALUE_LOW);
}

void MotorMC33886::reverse()
{
    setMotor(QGpio::VALUE_LOW, QGpio::VALUE_HIGH);
}

void MotorMC33886::setSpeed(float speed) {
    if (speed < 0.0f || speed > 100.0f)
        return;
    MotorBase::setSpeed(speed);
    m_speedPwm->pwmSetDutyCycle(-1, speed);
}

MotorDriverBoard::MotorDriverBoard(QObject *parent) : QThread(parent)
{
    m_motorLeft.reset(new MotorMC33886(12, 6, 13));
    m_motorRight.reset(new MotorMC33886(26, 20, 21));
    if (QGpio::getInstance()->init() == QGpio::INIT_OK) {
        m_irPort = QGpio::getInstance()->allocateGpioPort(18, QGpio::DIRECTION_OUTPUT);
    }
}

MotorDriverBoard::~MotorDriverBoard()
{
    stopIR();
}

void MotorDriverBoard::forward(MotorsEnum motors)
{
    if (motors == LEFT_MOTOR || motors == BOTH_MOTORS)
        m_motorLeft->forward();
    if (motors == RIGHT_MOTOR || motors == BOTH_MOTORS)
        m_motorRight->forward();
}

void MotorDriverBoard::stop(MotorsEnum motors)
{
    if (motors == LEFT_MOTOR || motors == BOTH_MOTORS)
        m_motorLeft->stop();
    if (motors == RIGHT_MOTOR || motors == BOTH_MOTORS)
        m_motorRight->stop();
}

void MotorDriverBoard::reverse(MotorsEnum motors)
{
    if (motors == LEFT_MOTOR || motors == BOTH_MOTORS)
        m_motorLeft->reverse();
    if (motors == RIGHT_MOTOR || motors == BOTH_MOTORS)
        m_motorRight->reverse();
}

void MotorDriverBoard::setSpeed(float speedLeft, float speedRight)
{
    m_motorLeft->setSpeed(speedLeft);
    m_motorRight->setSpeed(speedRight);
}

float MotorDriverBoard::speedLeft() const
{
    return m_motorLeft->speed();
}

float MotorDriverBoard::speedRight() const
{
    return m_motorRight->speed();
}

void MotorDriverBoard::startIR()
{
    start(NormalPriority);
}

void MotorDriverBoard::stopIR()
{
    requestInterruption();
    wait(1000);
}

void MotorDriverBoard::run()
{
    while (!isInterruptionRequested()) {
        if (m_irPort->value() == QGpio::VALUE_LOW) {
            int count = 0;

            while (m_irPort->value() == QGpio::VALUE_LOW && count < 10) { //  #9ms
                count += 1;
                usleep(900);
            }

            count = 0;
            while (m_irPort->value() == QGpio::VALUE_HIGH && count < 10) { //  #4.5ms
                count += 1;
                usleep(450);
            }

            int idx = 0;
            int cnt = 0;
            int data[4] = { 0,0,0,0 };
            for (int i = 0; i < 32; i++) {
                count = 0;
                while (m_irPort->value() == QGpio::VALUE_LOW && count < 15) { //  #0.56ms
                    count += 1;
                    usleep(60);
                }
                count = 0;
                while (m_irPort->value() == QGpio::VALUE_HIGH && count < 20) { //  #0: 0.56ms #1: 1.69ms
                    count += 1;
                    usleep(120);
                }

                if (count > 8)
                    data[idx] |= 1 << cnt;
                if (cnt == 7) {
                    cnt = 0;
                    idx += 1;
                } else {
                    cnt += 1;
                }
            }
            if ((data[0] + data[1] == 0xFF) && (data[2] + data[3] == 0xFF)) { //  #check
                qDebug() << "send ir command" << data[2];
                emit irCommandChanged(data[2]);
            } else {
                //qDebug() << "ir data" << data[0]<< data[1]<< data[2]<< data[3];
                msleep(10);
            }
        } else {
            msleep(10);
        }
    }
}
