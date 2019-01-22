#include "pwmsoftware.h"
#include "qgpio.h"

#include <QDebug>

PwmSoftware::PwmSoftware(int port)
{
    m_pwmDutyCycle = 0.0f;
    m_gpio = QGpio::getInstance();
    if (m_gpio->init() == QGpio::INIT_OK) {
        m_pwmPort = m_gpio->allocateGpioPort(port, QGpio::DIRECTION_OUTPUT);
    } else {
        qWarning() << "Error initing GPIO";
    }
}

PwmSoftware::~PwmSoftware()
{
    stopPwm(-1);
    m_gpio->deallocateGpioPort(m_pwmPort);
}

void PwmSoftware::pwmCalculateTimes()
{
    m_pwmReqOn = (long long)(m_pwmDutyCycle.toFloat() * m_pwmSliceTime * 1000.0);
    m_pwmReqOff = (long long)((100.0 - m_pwmDutyCycle.toFloat()) * m_pwmSliceTime * 1000.0);
    //qDebug() << "PWM pulse time: on" << m_pwmReqOn << "off" << m_pwmReqOff << m_pwmDutyCycle;
}

QPointer<QGpioPort> PwmSoftware::pwmPort() const
{
    return m_pwmPort;
}

void PwmSoftware::pwmSetDutyCycle(int channel, const QVariant &dutycycle)
{
    Q_UNUSED(channel)
    if (dutycycle.toFloat() < 0.0f || dutycycle.toFloat() > 100.0f) {
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

float PwmSoftware::pwmFrequency()
{
    return m_pwmFreq;
}

QVariant PwmSoftware::pwmDutyCycle(int channel)
{
    Q_UNUSED(channel)
    return m_pwmDutyCycle;
}

void PwmSoftware::startPwm(int channel, const QVariant &dutyCycle)
{
    pwmSetDutyCycle(channel, dutyCycle);
    if (m_pwmRunner == nullptr && m_pwmPort.isNull() == false) {
        pwmCalculateTimes();
        m_pwmRunner = QThread::create([&]{
            pwmThreadRun();
        });
        m_pwmRunner->start(QThread::NormalPriority);
    }
}

void PwmSoftware::stopPwm(int channel)
{
    Q_UNUSED(channel)
    if (m_pwmRunner != nullptr) {
        m_pwmRunner->requestInterruption();
        m_pwmRunner->wait(m_pwmReqOn + m_pwmReqOff + 1000);
        delete m_pwmRunner;
        m_pwmRunner = nullptr;
    }
}

void PwmSoftware::pwmThreadRun()
{
    qDebug() << Q_FUNC_INFO << "thread started for port" << m_pwmPort->getPort();
    while (!m_pwmRunner->isInterruptionRequested()) {
        if (m_pwmDutyCycle.toFloat() > 0.0) {
            m_pwmPort->setValue(QGpio::VALUE_HIGH);
            bcm2835_delayMicroseconds(m_pwmReqOn);
        }

        if (m_pwmDutyCycle.toFloat() < 100.0) {
            m_pwmPort->setValue(QGpio::VALUE_LOW);
            bcm2835_delayMicroseconds(m_pwmReqOff);
        }
    }

    // clean up
    m_pwmPort->setValue(QGpio::VALUE_LOW);
    qDebug() << Q_FUNC_INFO << "thread stopped for port" << m_pwmPort->getPort();
}
