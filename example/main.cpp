#include <QCoreApplication>
#include <QDebug>

#include "servoexample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ServoExample servoExample;
    int ret = a.exec();
    qDebug() << "exiting";
    return ret;
}
