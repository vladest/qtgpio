#pragma once

#include <QObject>
#include "sensors/icm20948/imu20948sensor.h"

class ImuExample : public QObject
{
    Q_OBJECT
public:
    explicit ImuExample(QObject *parent = nullptr);
    virtual ~ImuExample();

private:
    IMU20948Sensor *m_imu = nullptr;
};

