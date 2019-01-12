#include <QCoreApplication>
#include <QDebug>

#include "buzzer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BuzzerExample servoExample;
    int ret = a.exec();
    return ret;
}
