#pragma once

#include <QThread>
#include <QPointer>
#include <qgpioport.h>
#include "ultraborg/ultraborg.h"

class UltraborgSensor : public QThread
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
    explicit UltraborgSensor(int sensorNum, uint8_t i2cPort = UB_DEFAULT_I2C_ADDRESS, QObject *parent = nullptr);
    virtual ~UltraborgSensor();

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
    UltraBorg* m_ultraborg = nullptr;
    float m_distance = -1.0;
    int m_measureRate = 10; //times per second
    uint64_t m_delay = 100000ULL;
    int m_sensor = 0;
};
