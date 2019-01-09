#include "pwmsoftware.h"
#include "qgpio.h"

#include <QDebug>

PwmSoftware::PwmSoftware(int port)
{
    m_gpio = QGpio::getInstance();
    if (m_gpio->init() == QGpio::INIT_OK) {
        m_pwmPort = m_gpio->allocateGpioPort(port, QGpio::DIRECTION_OUTPUT);
    }
}

PwmSoftware::~PwmSoftware()
{
    m_gpio->deallocateGpioPort(m_pwmPort);
}

void PwmSoftware::pwmCalculateTimes()
{
    m_pwmReqOn = (long long)(m_pwmDutyCycle * m_pwmSliceTime * 1000.0);
    m_pwmReqOff = (long long)((100.0 - m_pwmDutyCycle) * m_pwmSliceTime * 1000.0);
    //qDebug() << "PWM pulse time: on" << m_pwmReqOn << "off" << m_pwmReqOff;
}

QPointer<QGpioPort> PwmSoftware::pwmPort() const
{
    return m_pwmPort;
}

void PwmSoftware::pwmSetDutyCycle(float dutycycle)
{
    if (dutycycle < 0.0 || dutycycle > 100.0) {
        qWarning() << "Invalid duty cycle provided:" << dutycycle << "Valid values are from 0.0 to 100.0";
        return;
    }
    m_pwmDutyCycle = dutycycle;
    pwmCalculateTimes();
}

void PwmSoftware::pwmSetFrequency(float freq)
{
    if (freq <= 0.0) {// to avoid divide by zero
        qWarning() << "Invalid frequency provided:" << freq << "Valid value is greater than 0.0";
        return;
    }

    m_pwmFreq = freq;
    m_pwmBaseTime = 1000.0 / freq;    // calculated in ms
    m_pwmSliceTime = m_pwmBaseTime / 100.0;
    pwmCalculateTimes();
}

float PwmSoftware::pwmFrequency() const
{
    return m_pwmFreq;
}

float PwmSoftware::pwmDutyCycle() const
{
    return m_pwmDutyCycle;
}

void PwmSoftware::startPwm(float dutyCycle)
{
    pwmSetDutyCycle(dutyCycle);
    if (m_pwmRunner == nullptr && m_pwmPort.isNull() == false) {
        pwmCalculateTimes();
        m_pwmRunner = QThread::create([&]{
            pwmThreadRun();
        });
        m_pwmRunner->start(QThread::TimeCriticalPriority);
    }
}

void PwmSoftware::stopPwm()
{
    if (m_pwmRunner != nullptr) {
        m_pwmRunner->requestInterruption();
        m_pwmRunner->wait(m_pwmReqOn + m_pwmReqOff + 100);
        delete m_pwmRunner;
        m_pwmRunner = nullptr;
    }
}

void PwmSoftware::pwmThreadRun()
{
    qDebug() << Q_FUNC_INFO << "thread started for port" << m_pwmPort->getPort();
    while (!m_pwmRunner->isInterruptionRequested()) {
        if (m_pwmDutyCycle > 0.0) {
            m_pwmPort->setValue(QGpio::VALUE_HIGH);
            QThread::usleep(m_pwmReqOn);
        }

        if (m_pwmDutyCycle < 100.0) {
            m_pwmPort->setValue(QGpio::VALUE_LOW);
            QThread::usleep(m_pwmReqOff);
        }
    }

    // clean up
    m_pwmPort->setValue(QGpio::VALUE_LOW);
    qDebug() << Q_FUNC_INFO << "thread stopped for port" << m_pwmPort->getPort();
}
