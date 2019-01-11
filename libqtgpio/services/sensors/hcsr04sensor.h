#ifndef HCSR04SENSOR_H
#define HCSR04SENSOR_H

#include <QThread>
#include <QPointer>
#include <qgpioport.h>

class HCSR04Sensor : public QThread
{
    Q_OBJECT

    Q_PROPERTY(float distance READ distance NOTIFY distanceChanged)

public:
    /**
     * @brief HCSR04Sensor
     * @param echoPort
     * @param triggerPort
     * @param parent
     *
     * after creating the class instance, need to call start() method to start measuring
     */
    explicit HCSR04Sensor(int echoPort, int triggerPort, QObject *parent = nullptr);
    virtual ~HCSR04Sensor();

    /**
     * @brief distance
     * @return current distance in centimeters
     */
    float distance() const;
    /**
     * @brief stop: stops measuring
     */
    void stop();

    int measureRate() const;
    void setMeasureRate(int measureRate);

signals:
    // connect via queued connection
    void distanceChanged(float distance);
protected:
    void run() override;
private:
    QPointer<QGpioPort> m_echoPort;
    QPointer<QGpioPort> m_triggerPort;
    float m_distance = -1.0;
    int m_measureRate = 10; //times per second
};

#endif // HCSR04SENSOR_H
