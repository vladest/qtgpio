#pragma once

#include <QObject>
#include <QScopedPointer>
#include "qgpioport.h"
#include "qgpioi2cslave.h"
#include "motorbase.h"
#include "pca9685_board.h"
/**
 * @brief MotorDriver
 * @param parent
 * support for I2C controlled Motor Drivers usin PCA9685 chip
 * for example: Waveshare https://www.waveshare.com/wiki/Motor_Driver_HAT
 *
 */

class MotorPCA9685;

// describes structure for L298-like motor controllers
// https://www.electronicshub.org/raspberry-pi-l298n-interface-tutorial-control-dc-motor-l298n-raspberry-pi/
struct MotorTripple {
    uint8_t ENABLE;
    uint8_t INPUT1;
    uint8_t INPUT2;
};

using MotorMapping = std::vector<std::pair<Motors::MotorsEnum, MotorTripple>>;
using ScopedMotor = QScopedPointer<MotorPCA9685>;

class MotorPCA9685 : public MotorBase
{
    Q_OBJECT
public:
    explicit MotorPCA9685(uint8_t bus, uint8_t address, uint8_t delay,
                          uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2,
                          QObject *parent = nullptr);
    virtual ~MotorPCA9685();

    void forward(MotorsFlags motors = ALL_MOTORS) override;
    void stop(MotorsFlags motors = ALL_MOTORS) override;
    void reverse(MotorsFlags motors = ALL_MOTORS) override;
    void setSpeed(float speed, MotorsFlags motors = ALL_MOTORS) override;

private:
    uint8_t m_speedChannel;
    uint8_t m_inChannel1;
    uint8_t m_inChannel2;
    bool m_motorActive = false;
    PCA9685Bboard m_pca9685;
};

/**
  * @brief The MotorDriverHat class
  * @details Controlling motors drivers, connected to PCA9685, f.ex L298
  * requires to set mapping between motor and PCA channel
  */
class MotorDriver: public QObject {

    Q_OBJECT
public:

    explicit MotorDriver(uint8_t bus, const MotorMapping& motorsMapping, uint8_t address = 0x40, uint8_t delay = 1,
                         QObject *parent = nullptr);
    virtual ~MotorDriver();

    void forward(Motors::MotorsEnum motors);
    void stop(Motors::MotorsEnum motors);
    void reverse(Motors::MotorsEnum motors);
    void setSpeed(Motors::MotorsEnum motors, float speed);
    float speed(Motors::MotorsEnum motor) const;

private:
    QVector<MotorPCA9685*> m_motors;
};


