#include "imu.h"
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>

ImuExample::ImuExample(QObject *parent) : QObject(parent)
{
    m_imu = new IMU20948Sensor(this);
}


ImuExample::~ImuExample()
{
}
