#pragma once

#include <QThread>
#include <QPointer>
#include <qgpioport.h>
#include "ICM_20948.h"

class IMU20948Sensor : public QThread
{
    Q_OBJECT

public:
    explicit IMU20948Sensor(QObject *parent = nullptr);
    virtual ~IMU20948Sensor();

    /**
     * @brief stop: stops measuring
     */
    void stop();

signals:
protected:
    void run() override;
private:
    ICM_20948_I2C m_icm;
};
