#include <QCoreApplication>
#include <QDebug>

#include "imu.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ImuExample imuExample;
    int ret = a.exec();
    return ret;
}
