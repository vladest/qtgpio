#include "pca9685_board.h"
#include <unistd.h>
#include "math.h"
/**
        * PCA9685
            */
    //GPIO config
constexpr uint8_t SUBADR1 = 0x02;
constexpr uint8_t SUBADR2 = 0x03;
constexpr uint8_t SUBADR3 = 0x04;
constexpr uint8_t MODE1 = 0x00;
constexpr uint8_t PRESCALE = 0xFE;
constexpr uint8_t LED0_ON_L = 0x06;
constexpr uint8_t LED0_ON_H = 0x07;
constexpr uint8_t LED0_OFF_L = 0x08;
constexpr uint8_t LED0_OFF_H = 0x09;
constexpr uint8_t ALLLED_ON_L = 0xFA;
constexpr uint8_t ALLLED_ON_H = 0xFB;
constexpr uint8_t ALLLED_OFF_L = 0xFC;
constexpr uint8_t ALLLED_OFF_H = 0xFD;

constexpr uint8_t  PCA_CHANNEL_0  = 0;
constexpr uint8_t  PCA_CHANNEL_1  = 1;
constexpr uint8_t  PCA_CHANNEL_2  = 2;
constexpr uint8_t  PCA_CHANNEL_3  = 3;
constexpr uint8_t  PCA_CHANNEL_4  = 4;
constexpr uint8_t  PCA_CHANNEL_5  = 5;
constexpr uint8_t  PCA_CHANNEL_6  = 6;
constexpr uint8_t  PCA_CHANNEL_7  = 7;
constexpr uint8_t  PCA_CHANNEL_8  = 8;
constexpr uint8_t  PCA_CHANNEL_9  = 9;
constexpr uint8_t  PCA_CHANNEL_10 = 10;
constexpr uint8_t  PCA_CHANNEL_11 = 11;
constexpr uint8_t  PCA_CHANNEL_12 = 12;
constexpr uint8_t  PCA_CHANNEL_13 = 13;
constexpr uint8_t  PCA_CHANNEL_14 = 14;
constexpr uint8_t  PCA_CHANNEL_15 = 15;

PCA9685Bboard::PCA9685Bboard()
{
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

void PCA9685Bboard::init(uint8_t bus, uint8_t address, uint8_t delay)
{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        m_i2c = gpio->allocateI2CSlave(address, delay, bus, 40000);
        m_i2c->i2cWrite(MODE1, (uint8_t)0x00);
    }
}
/**
 * Write bytes in PCA9685
 *
 * @param reg: register.
 * @param value: value.
 *
 * Example:
 * PCA9685_WriteByte(0x00, 0xff);
 */
void PCA9685Bboard::writeByte(uint8_t reg, uint8_t value)
{
    m_i2c->i2cWrite(reg, value);
}

/**
 * read byte in PCA9685.
 *
 * @param reg: register.
 *
 * Example:
 * UBYTE buf = PCA9685_ReadByte(0x00);
 */
uint8_t PCA9685Bboard::readByte(uint8_t reg)
{
    return m_i2c->i2cRead(reg);
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
void PCA9685Bboard::setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    writeByte(LED0_ON_L + 4 * channel, on & 0xFF);
    writeByte(LED0_ON_H + 4 * channel, on >> 8);
    writeByte(LED0_OFF_L + 4 * channel, off & 0xFF);
    writeByte(LED0_OFF_H + 4 * channel, off >> 8);
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
void PCA9685Bboard::setPWMFreq(uint16_t freq)
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
    usleep(5000);
    // ?? writeByte(MODE1, oldmode | 0xa1)
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
void PCA9685Bboard::setPwmDutyCycle(uint8_t channel, uint16_t pulse)
{
    setPWM(channel, 0, pulse * (4096.0 / 100.0) - 1);
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
void PCA9685Bboard::setLevel(uint8_t channel, uint16_t value)
{
    if (value == 1) {
        //setPWM(channel, 4096, 0);
        setPWM(channel, 0, 4095);
    } else {
        setPWM(channel, 0, 0);
    }
}


