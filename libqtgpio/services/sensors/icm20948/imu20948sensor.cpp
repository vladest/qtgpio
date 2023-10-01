#include "imu20948sensor.h"
#include <unistd.h>
#include <QDateTime>
#include "qgpio.h"
#include <QDebug>


IMU20948Sensor::IMU20948Sensor(QObject *parent)
    : QThread(parent)
{
    start();
}

IMU20948Sensor::~IMU20948Sensor()
{
    stop();
}

void IMU20948Sensor::stop()
{
    requestInterruption();
    wait(1000);
}


void IMU20948Sensor::run()
{
    QElapsedTimer timer;

    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        if (m_icm.begin(gpio, false) != ICM_20948_Stat_Ok) {
            qWarning() << "IMU not found";
            return;
        }
    }
    while (!isInterruptionRequested()) {
        if (m_icm.dataReady()) {
            m_icm.getAGMT();
            qDebug() << "temp = " << m_icm.temp();
            qDebug() << "acc = " << m_icm.accX() << " " << m_icm.accY() << " " << m_icm.accZ();
            qDebug() << "mag = " << m_icm.magX() << " " << m_icm.magY() << " " << m_icm.magZ();
            qDebug() << "gyr = " << m_icm.gyrX() << " " << m_icm.gyrY() << " " << m_icm.gyrZ();
        }

        usleep(100);
    }
}


