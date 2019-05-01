#pragma once

#include <QObject>
#include "sensors/hcsr04sensor.h"
#include "sensors/octosonarsensor.h"

class SonarExample : public QObject
{
    Q_OBJECT
public:
    explicit SonarExample(QObject *parent = nullptr);
    virtual ~SonarExample();

public slots:
    void onDistanceChanged(float distance);
private:
    OctoSonarSensor* ultrasound = nullptr;
};

