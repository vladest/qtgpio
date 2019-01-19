#include "servoexample.h"
#include <QDebug>
#include <QCoreApplication>

#include "servo/servosoftware.h"
#include "servo/servocontrol9685.h"

ServoExample::ServoExample(QObject *parent) : QObject(parent)
{
    m_consoleReader = new ConsoleReader();
    connect (m_consoleReader, &ConsoleReader::keyPressed, this, &ServoExample::onConsoleKeyPressed,
             Qt::QueuedConnection);
    m_consoleReader->start();

    servoPortLeftRight = new ServoControl9685(11);
    servoPortUpDown = new ServoControl9685(9);
    //servoPortUpDown = new ServoSoftware(17);

    //MG995
    //servoPortLeftRight->setServoPulses(500, 2300);

    //MG90S
    servoPortLeftRight->setServoPulses(550, 2250);
    servoPortUpDown->setServoPulses(550, 2250);

    m_motor = new MotorMC33886;
}

ServoExample::~ServoExample()
{
    m_consoleReader->disconnect();
    m_consoleReader->wait(1000);
    delete m_consoleReader;
    delete servoPortUpDown;
    delete servoPortLeftRight;
}

void ServoExample::onConsoleKeyPressed(int ch)
{
    static bool esc = false;
    static bool esc_seq = false;
    //qDebug() << "char pressed" << QString::number(ch, 16).toUpper();
    if (ch == 0x1B) {
        esc = true;
        esc_seq = false;
    } else {
        if (ch == 0x5B) {
            esc_seq = esc;
        } else {
            if (ch == 0xA || ch == 'q') {
                m_consoleReader->requestInterruption();
                qApp->exit();
            } else if (ch == 0x41) { //UP
                if (esc_seq)
                    m_motor->forward();
                    //servoPortUpDown->startRotating(-1.0);
            } else if (ch == 0x42) { //DOWN
                if (esc_seq)
                    m_motor->reverse();
                    //servoPortUpDown->startRotating(1.0);
            } else if (ch == 0x44) { //LEFT
                if (esc_seq)
                    m_motor->left();
                    //servoPortLeftRight->startRotating(-1.0);
            } else if (ch == 0x43) { //RIGHT
                if (esc_seq)
                    m_motor->right();
                    //servoPortLeftRight->startRotating(1.0);
            } else if (ch == 'a') { //LEFT
                servoPortLeftRight->setAngle(0);
            } else if (ch == 'd') { //RIGHT
                servoPortLeftRight->setAngle(180);
            } else if (ch == 'w') { //LEFT
                servoPortUpDown->setAngle(0);
            } else if (ch == 's') { //RIGHT
                servoPortUpDown->setAngle(180);
            }  else if (ch == '+') { //LEFT
                m_motor->setSpeed(m_motor->speed() + 5.0);
            } else if (ch == '-') { //RIGHT
                m_motor->setSpeed(m_motor->speed() - 5.0);
            } else if (ch == 0x20) { //SPACE
                servoPortLeftRight->stopRotating();
                servoPortUpDown->stopRotating();
                m_motor->stop();
            }

            esc = esc_seq = false;
        }
    }
}
