#ifndef QGPIOPORT_H
#define QGPIOPORT_H

#include <QThread>
#include <QPointer>
#include "qgpio.h"

class QGpioPort : public QObject
{
public:

    // QThread interface
    friend class QGpio;

    void setPullupdn(QGpio::GpioPullUpDown pud);
    QGpio::GpioDirection getDirection();
    void setValue(QGpio::GpioValue value);
    QGpio::GpioValue value();

    void pwmSetDutyCycle(float dutycycle);
    void pwmSetFrequency(float freq);

    void startPwm(float dutyCycle = 0.0);
    void stopPwm();

    /**
     * @brief addEdgeDetect: setup GPIO filesystem for epoll and add GPIO port to epoll thread
     * @return file descriptor for value
     */
    bool addEdgeDetect(QGpio::GpioEdge edge, int bouncetime);
    void removeEdgeDetect();
    int getValueFd() const;
    int getPort() const;

    int getBouncetime() const;
    void setBouncetime(int bouncetime);

    bool getInitialTrigger() const;
    void setInitialTrigger(bool initialTrigger);

    quint64 getLastCallTimestamp() const;
    void setLastCallTimestamp(const quint64 &lastCallTimestamp);

protected:

    /**
     * @brief pwmThreadRun: runner function for PWM thread
     */
    void pwmThreadRun();

    QGpioPort(int port, QGpio::GpioDirection direction, QGpio::GpioPullUpDown pud, QObject* parent = nullptr);
    virtual ~QGpioPort();
    void setGpioParent(QGpio* gpio);

private:
    void clearEventDetect();
    int eventDetected();
    void setRisingEvent(bool enable);
    void setFallingEvent(bool enable);
    void setHighEvent(bool enable);
    void setLowEvent(bool enable);
    void setup();
    void pwmCalculateTimes();

    // events
    bool gpioExport();
    int gpioUnexport();
    bool gpioSetDirection(QGpio::GpioDirection direction);
    int openValueFile();
    bool gpioSetEdge(QGpio::GpioEdge edge);
private:
    int m_port = 0;
    QGpio::GpioDirection m_direction = QGpio::DIRECTION_INPUT;
    QGpio::GpioPullUpDown m_pud = QGpio::PUD_OFF;
    QPointer<QGpio> m_gpio;

    // PWM data for the port
    float m_pwmFreq = 1000.0f;
    float m_pwmDutyCycle = 0.0f;
    float m_pwmBaseTime = 1.0f;
    float m_pwmSliceTime = 0.01f;
    qint64 m_pwmReqOn = 0;
    qint64 m_pwmReqOff = 0;

    int m_valueFd = -1;

    QGpio::GpioEdge m_edge = QGpio::NO_EDGE;
    int m_bouncetime = -666;
    bool m_initialTrigger = true;
    quint64 m_lastCallTimestamp = 0;
    QThread* m_pwmRunner = nullptr;
};

#endif // QGPIOPORT_H
