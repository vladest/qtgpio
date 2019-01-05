#include "servoexample.h"
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>

inline unsigned long delayus(float angle) {
    //SG90 servo speed 60 grad by 0.1sec
    return (unsigned long)((100000.0/60.0)*angle);
}

ServoExample::ServoExample(QObject *parent) : QObject(parent)
{
    m_consoleReader = new ConsoleReader();
    connect (m_consoleReader, SIGNAL (KeyPressed(char)), this, SLOT(OnConsoleKeyPressed(char)));
    m_consoleReader->start();

    m_gpio = QGpio::getInstance();
    QGpio::InitResult res = m_gpio->init();
    qDebug() << "Result" << (int)res << m_gpio->getGpioMap();
    buzzer = m_gpio->allocateGpioPort(18, QGpio::DIRECTION_OUTPUT);
    buzzer->pwmSetFrequency(450);
    buzzer->startPwm(0);
    servoPortLeftRight = new ServoControl(17);
    servoPortUpDown = new ServoControl(27);
}

ServoExample::~ServoExample()
{
    qDebug() << "Shutdown servo";
    m_consoleReader->terminate();
    m_consoleReader->wait(500);
    delete m_consoleReader;
    delete servoPortUpDown;
    delete servoPortLeftRight;
}

void ServoExample::OnConsoleKeyPressed(char ch)
{
    static bool esc = false;
    static bool esc_seq = false;
    //QElapsedTimer escpressTimer;
    //qDebug() << "char pressed" << QString::number(ch, 16).toUpper();
    if (ch == 0x1B) {
        esc = true;
        //escpressTimer.start();
    } else {
        if (ch == 0x5B) {
            if (esc)
                esc_seq = true;
        } else {
            if (ch == 0xA) {
                m_consoleReader->requestInterruption();
                m_consoleReader->terminate();
                delete m_consoleReader;
                qApp->exit();
            } else if (ch == 'b') { //UP
                if (buzzer->pwmDutyCycle() < 1.0)
                    buzzer->pwmSetDutyCycle(1.0);
                else
                    buzzer->pwmSetDutyCycle(0.0);
            }  else if (ch == 0x41) { //UP
                qDebug() << "UP" << esc_seq;
//                if (esc_seq)
//                    upDown(true, 90);
            } else if (ch == 0x42) { //DOWN
                qDebug() << "DOWN" << esc_seq;
//                if (esc_seq)
//                    upDown(false, 90);
            } else if (ch == 0x44) { //LEFT
                qDebug() << "LEFT" << esc_seq;
                if (esc_seq)
                    servoPortLeftRight->startCounterClockWise();
            } else if (ch == 0x43) { //RIGHT
                qDebug() << "RIGHT" << esc_seq;
                if (esc_seq)
                    servoPortLeftRight->startClockWise();
            } else if (ch == 'a') { //LEFT
                servoPortLeftRight->counterClockWiseAngle(12);
            } else if (ch == 'd') { //RIGHT
                servoPortLeftRight->clockWiseAngle(12);
            } else if (ch == 0x20) { //SPACE
                qDebug() << "SPACE" << esc_seq;
                servoPortLeftRight->stop();
            }

            esc = esc_seq = false;
        }
    }
}
