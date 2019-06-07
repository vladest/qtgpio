#include "motor_hat_stepper.h"

#include "math.h"
#include <QDebug>


/**
  * PCA9685
*/
//GPIO config
static const uint8_t SUBADR1 = 0x02;
static const uint8_t SUBADR2 = 0x03;
static const uint8_t SUBADR3 = 0x04;
static const uint8_t MODE1 = 0x00;
static const uint8_t PRESCALE = 0xFE;
static const uint8_t LED0_ON_L = 0x06;
static const uint8_t LED0_ON_H = 0x07;
static const uint8_t LED0_OFF_L = 0x08;
static const uint8_t LED0_OFF_H = 0x09;
static const uint8_t ALLLED_ON_L = 0xFA;
static const uint8_t ALLLED_ON_H = 0xFB;
static const uint8_t ALLLED_OFF_L = 0xFC;
static const uint8_t ALLLED_OFF_H = 0xFD;

/**
 * Write bytes in PCA9685
 *
 * @param reg: register.
 * @param value: value.
 *
 * Example:
 * PCA9685_WriteByte(0x00, 0xff);
 */
void MotorHatStepperPCA9685::writeByte(uint8_t reg, uint8_t value)
{
    m_i2c->write(reg, value);
}

/**
 * read byte in PCA9685.
 *
 * @param reg: register.
 *
 * Example:
 * UBYTE buf = PCA9685_ReadByte(0x00);
 */
uint8_t MotorHatStepperPCA9685::readByte(uint8_t reg)
{
    return m_i2c->read(reg);
}

/**
 * Set the PWM output.
 *
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param on: 12-bit register will hold avalue for the ON time.  //(0 ~ 4095)
 * @param off: 12-bit register will hold the value for the OFF time.  //(0 ~ 4095)
 *
 * @For more information, please see page 15 - page 19 of the datasheet.
 * Example:
 * PCA9685_SetPWM(0, 0, 4095);
 */
void MotorHatStepperPCA9685::setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    writeByte(LED0_ON_L + 4 * channel, on & 0xFF);
    writeByte(LED0_ON_H + 4 * channel, on >> 8);
    writeByte(LED0_OFF_L + 4 * channel, off & 0xFF);
    writeByte(LED0_OFF_H + 4 * channel, off >> 8);
}

/**
 * PCA9685 Initialize.
 * For the PCA9685, the device address can be controlled by setting A0-A5.
 * On our driver board, control is set by setting A0-A4, and A5 is grounded.
 *
 * @param addr: PCA9685 address.  //0x40 ~ 0x5f
 *
 * Example:
 * PCA9685_Init(0x40);
 */
MotorHatStepperPCA9685::MotorHatStepperPCA9685()
{
}

void MotorHatStepperPCA9685::init(uint8_t address)
{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        m_i2c = gpio->allocateI2CSlave(address, BCM2835_I2C_CLOCK_DIVIDER_626, 40000);
        m_i2c->write(MODE1, (uint8_t)0x00);
    }
}

/**
 * Set the frequency (PWM_PRESCALE) and restart.
 *
 * For the PCA9685, Each channel output has its own 12-bit
 * resolution (4096 steps) fixed frequency individual PWM
 * controller that operates at a programmable frequency
 * from a typical of 40 Hz to 1000 Hz with a duty cycle
 * that is adjustable from 0 % to 100 %
 *
 * @param freq: Output frequency.  //40 ~ 1000
 *
 * Example:
 * PCA9685_SetPWMFreq(50);
 */
void MotorHatStepperPCA9685::setPWMFreq(uint16_t freq)
{
    freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
    double prescaleval = 25000000.0;
    prescaleval /= 4096.0;
    prescaleval /= freq;
    prescaleval -= 1;
    qDebug("prescaleval = %lf\r\n", prescaleval);

    uint8_t prescale = ::floor(prescaleval + 0.5);
    qDebug("prescaleval = %lf\r\n", prescaleval);

    uint8_t oldmode = readByte(MODE1);
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep

    writeByte(MODE1, newmode); // go to sleep
    writeByte(PRESCALE, prescale); // set the prescaler
    writeByte(MODE1, oldmode);
    bcm2835_delayMicroseconds(5000);
    writeByte(MODE1, oldmode | 0x80);  //  This sets the MODE1 register to turn on auto increment.
}

/**
 * Set channel output the PWM duty cycle.
 *
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param pulse: duty cycle.  //(0 ~ 100  == 0% ~ 100%)
 *
 * Example:
 * PCA9685_SetPwmDutyCycle(1, 100);
 */
void MotorHatStepperPCA9685::setPwmDutyCycle(uint8_t channel, uint16_t pulse)
{
    setPWM(channel, 0, pulse * (4096 / 100) - 1);
}


/*!
    @brief  Helper that sets the PWM output on a pin and manages 'all on or off'
    @param  pin The PWM output on the driver that we want to control (0-15)
    @param  value The 12-bit PWM value we want to set (0-4095) - 4096 is a special 'all on' value
*/
void MotorHatStepperPCA9685::setPWM(uint8_t channel, uint16_t value) {
  if (value > 4095) {
    setPWM(channel, 4096, 0);
  } else
    setPWM(channel, 0, value);
}

/**
 * Set channel output level.
 *
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param value: output level, 0 low level, 1 high level.  //0 or 1
 *
 * Example:
 * PCA9685_SetLevel(3, 1);
 */
void MotorHatStepperPCA9685::setLevel(uint8_t channel, uint16_t value)
{
    if (value == 1)
        setPWM(channel, 4096, 0);
    else
        setPWM(channel, 0, 0);
}

Motor::Motor(uint8_t address, uint8_t speedChannel, uint8_t inChannel1, uint8_t inChannel2, QObject *parent) :
    MotorBase(parent),
    m_speedChannel(speedChannel),
    m_inChannel1(inChannel1),
    m_inChannel2(inChannel2)
{
    m_pca9685.init(address);
    m_pca9685.setPWMFreq(100);

    for (uint8_t i=0; i < 16; i++)
        m_pca9685.setPWM(i, 0, 0);
}

Motor::~Motor()
{
    stop();
}

void Motor::forward()
{
    m_motorActive = true;

    qDebug() << "fwd" << m_inChannel1 << m_inChannel2;
    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel1, 0);
    m_pca9685.setLevel(m_inChannel2, 1);
}

void Motor::stop()
{
    m_motorActive = false;
    //m_pca9685.setPwmDutyCycle(m_speedChannel, 0);
    m_pca9685.setLevel(m_inChannel2, 0);
    m_pca9685.setLevel(m_inChannel1, 0);
}

void Motor::reverse()
{
    m_motorActive = true;

    m_pca9685.setPwmDutyCycle(m_speedChannel, speed());
    m_pca9685.setLevel(m_inChannel2, 0);
    m_pca9685.setLevel(m_inChannel1, 1);
}

void Motor::setSpeed(float speed) {
    if (speed < 0.0f || speed > 100.0f)
        return;
    MotorBase::setSpeed(speed);
    if (m_motorActive)
        m_pca9685.setPwmDutyCycle(m_speedChannel, speed);
}

MotorStepperHat::MotorStepperHat(uint8_t address, QObject *parent) : QObject(parent)
{
    m_motors.append(new Motor(address, 8, 10, 9, this));
    m_motors.append(new Motor(address, 13, 11, 12, this));
    m_motors.append(new Motor(address, 2, 4, 3, this));
    m_motors.append(new Motor(address, 7, 5, 6, this));
}

MotorStepperHat::~MotorStepperHat()
{
}

void MotorStepperHat::forward(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag) {
            m_motors.at(var)->forward();
        }
        motor_flag = motor_flag << 1;
    }
}

void MotorStepperHat::stop(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag)
            m_motors.at(var)->stop();
        motor_flag = motor_flag << 1;
    }
}

void MotorStepperHat::reverse(MotorBase::MotorsEnum motors)
{
    int motor_flag = 0x01;
    for (int var = 0; var < m_motors.size(); ++var) {
        if (motors & motor_flag)
            m_motors.at(var)->reverse();
        motor_flag = motor_flag << 1;
    }

}

void MotorStepperHat::setSpeed(QList<QPair<MotorBase::MotorsEnum, float>> motors)
{
    int motor_flag = 0x01;
    for (const QPair<MotorBase::MotorsEnum, float>& _motSpeed : motors) {
        for (int var = 0; var < m_motors.size(); ++var) {
            if (_motSpeed.first == motor_flag)
                m_motors.at(var)->setSpeed(_motSpeed.second);
            motor_flag = motor_flag << 1;
        }
    }
}

float MotorStepperHat::speed(MotorBase::MotorsEnum motor) const
{
    return m_motors.at(0)->speed();
}

