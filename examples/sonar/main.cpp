#include <QCoreApplication>
#include <QDebug>

#include "sonarexample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SonarExample servoExample;
    int ret = a.exec();
    return ret;
}
