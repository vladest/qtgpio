#include <QCoreApplication>
#include <QDebug>
#include "qgpio.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QGpio::InitResult res = QGpio::getInstance().init();
    qDebug() << "Result" << (int)res << QGpio::getInstance().getGpioMap();
    //return a.exec();
}
