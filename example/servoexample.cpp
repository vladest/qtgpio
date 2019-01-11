#include "servoexample.h"
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>

#include "servo/servosoftware.h"
#include "servo/servocontrol9685.h"

inline unsigned long delayus(float angle) {
    //SG90 servo speed 60 grad by 0.1sec
    return (unsigned long)((100000.0/60.0)*angle);
}

ServoExample::ServoExample(QObject *parent) : QObject(parent)
{
    m_consoleReader = new ConsoleReader();
    connect (m_consoleReader, SIGNAL (KeyPressed(char)), this, SLOT(OnConsoleKeyPressed(char)));
    m_consoleReader->start();

    buzzer = new PwmSoftware(18);
    buzzer->pwmSetFrequency(450);
    buzzer->startPwm(0);
    servoPortLeftRight = new ServoControl9685(10);
    //servoPortUpDown = new ServoControl9685(9);
    servoPortUpDown = new ServoSoftware(17);

    //MG995
    servoPortLeftRight->setServoPulses(500, 2300);
    //MG90S
    servoPortUpDown->setServoPulses(550, 2250);
    ultrasound = new HCSR04Sensor(12, 16);
    connect(ultrasound, &HCSR04Sensor::distanceChanged, this, &ServoExample::onDistanceChanged/*, Qt::QueuedConnection*/);
    ultrasound->start(QThread::NormalPriority);
}

ServoExample::~ServoExample()
{
    qDebug() << "Shutdown servo";
    m_consoleReader->disconnect();
    m_consoleReader->wait(1000);
    delete m_consoleReader;
    delete servoPortUpDown;
    delete servoPortLeftRight;
    delete buzzer;
    //delete ultrasound;
    qDebug() << "Shutdown servo finished";
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
            if (ch == 0xA || ch == 'q') {
                m_consoleReader->requestInterruption();
                qApp->exit();
            } else if (ch == 'b') { //UP
                if (buzzer->pwmDutyCycle(-1) < 1.0)
                    buzzer->pwmSetDutyCycle(-1, 1.0);
                else
                    buzzer->pwmSetDutyCycle(-1, 0.0);
            }  else if (ch == 0x41) { //UP
                qDebug() << "UP" << esc_seq;
                if (esc_seq)
                    servoPortUpDown->startRotating(-1.0);
            } else if (ch == 0x42) { //DOWN
                qDebug() << "DOWN" << esc_seq;
                if (esc_seq)
                    servoPortUpDown->startRotating(1.0);
            } else if (ch == 0x44) { //LEFT
                qDebug() << "LEFT" << esc_seq;
                if (esc_seq)
                    servoPortLeftRight->startRotating(-1.0);
            } else if (ch == 0x43) { //RIGHT
                qDebug() << "RIGHT" << esc_seq;
                if (esc_seq)
                    servoPortLeftRight->startRotating(1.0);
            } else if (ch == 'a') { //LEFT
                servoPortLeftRight->setAngle(0);
            } else if (ch == 'd') { //RIGHT
                servoPortLeftRight->setAngle(180);
            } else if (ch == 'w') { //LEFT
                servoPortUpDown->setAngle(0);
            } else if (ch == 's') { //RIGHT
                servoPortUpDown->setAngle(180);
            }else if (ch == 0x20) { //SPACE
                qDebug() << "SPACE" << esc_seq;
                servoPortLeftRight->stopRotating();
                servoPortUpDown->stopRotating();
            }

            esc = esc_seq = false;
        }
    }
}

void ServoExample::onDistanceChanged(float distance)
{
    qDebug() << "distance" << distance;
}
