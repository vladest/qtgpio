#pragma once

#include <QObject>
#include <QScopedPointer>
#include <cstdint>
#include <qgpioi2cslave.h>
#include "qgpioport.h"
#include "motorbase.h"

class PwmSoftware;

/**
 * @brief MotorDriverBoard
 * @param parent
 * support for ThunderBorg board by PiBorg.org
 * https://www.piborg.org/blog/thunderborg-getting-started
 *
 */

class MotorThunderborg : public MotorBase
{
    Q_OBJECT
public:
    explicit MotorThunderborg(uint8_t bus, uint8_t address, uint16_t delay = 0, QObject *parent = nullptr);
    virtual ~MotorThunderborg();

    void forward(MotorsFlags motors = ALL_MOTORS) override;
    void stop(MotorsFlags motors = ALL_MOTORS) override;
    void reverse(MotorsFlags motors = ALL_MOTORS) override;
    void setSpeed(float speed, MotorsFlags motors = ALL_MOTORS) override;

    // Motors fault
    bool getMotorFault(MotorsFlags motor);
    // Battery info
    float getBatteryLevel() const;

    // LEDs control
    void setLEDShowBattery(bool battery);
    void setLED1Color(uint8_t ledColor[3]);
    void setLED2Color(uint8_t ledColor[3]);

private:

private:
    QPointer<QGpioI2CSlave> m_i2c;
    MotorsFlags m_motors;
};


