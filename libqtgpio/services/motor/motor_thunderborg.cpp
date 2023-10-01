#include "motor_thunderborg.h"
#include "services/pwm/pwmsoftware.h"
#include <QDebug>

// Constant values
constexpr quint32 I2C_SLAVE = 0x0703;
constexpr quint32 PWM_MAX = 255;
constexpr quint32 I2C_MAX_LEN = 6;
constexpr float VOLTAGE_PIN_MAX = 36.3; //Maximum voltage from the analog voltage monitoring pin
constexpr float VOLTAGE_PIN_CORRECTION = 0.0; // Correction value for the analog voltage monitoring pin
constexpr float BATTERY_MIN_DEFAULT = 7.0; // Default minimum battery monitoring voltage
constexpr float BATTERY_MAX_DEFAULT = 35.0; // Default maximum battery monitoring voltage

constexpr quint8 I2C_ID_THUNDERBORG = 0x15;

constexpr quint8 COMMAND_SET_LED1 = 1; // Set the colour of the ThunderBorg LED
constexpr quint8 COMMAND_GET_LED1 = 2; //  Get the colour of the ThunderBorg LED
constexpr quint8 COMMAND_SET_LED2 = 3; //  Set the colour of the ThunderBorg Lid LED
constexpr quint8 COMMAND_GET_LED2 = 4; //  Get the colour of the ThunderBorg Lid LED
constexpr quint8 COMMAND_SET_LEDS = 5; //  Set the colour of both the LEDs
constexpr quint8 COMMAND_SET_LED_BATT_MON
    = 6; //  Set the colour of both LEDs to show the current battery level
constexpr quint8 COMMAND_GET_LED_BATT_MON
    = 7; //  Get the state of showing the current battery level via the LEDs
constexpr quint8 COMMAND_SET_A_FWD = 8; //  Set motor A PWM rate in a forwards direction
constexpr quint8 COMMAND_SET_A_REV = 9; //  Set motor A PWM rate in a reverse direction
constexpr quint8 COMMAND_GET_A = 10; //  Get motor A direction and PWM rate
constexpr quint8 COMMAND_SET_B_FWD = 11; //  Set motor B PWM rate in a forwards direction
constexpr quint8 COMMAND_SET_B_REV = 12; //  Set motor B PWM rate in a reverse direction
constexpr quint8 COMMAND_GET_B = 13; //  Get motor B direction and PWM rate
constexpr quint8 COMMAND_ALL_OFF = 14; //  Switch everything off
constexpr quint8 COMMAND_GET_DRIVE_A_FAULT
    = 15; //  Get the drive fault flag for motor A, indicates faults such as short-circuits and under voltage
constexpr quint8 COMMAND_GET_DRIVE_B_FAULT
    = 16; //  Get the drive fault flag for motor B, indicates faults such as short-circuits and under voltage
constexpr quint8 COMMAND_SET_ALL_FWD = 17; //  Set all motors PWM rate in a forwards direction
constexpr quint8 COMMAND_SET_ALL_REV = 18; //  Set all motors PWM rate in a reverse direction
constexpr quint8 COMMAND_SET_FAILSAFE
    = 19; //  Set the failsafe flag, turns the motors off if communication is interrupted
constexpr quint8 COMMAND_GET_FAILSAFE = 20; //  Get the failsafe flag
constexpr quint8 COMMAND_GET_BATT_VOLT = 21; //  Get the battery voltage reading
constexpr quint8 COMMAND_SET_BATT_LIMITS = 22; //  Set the battery monitoring limits
constexpr quint8 COMMAND_GET_BATT_LIMITS = 23; //  Get the battery monitoring limits
constexpr quint8 COMMAND_WRITE_EXTERNAL_LED = 24; //  Write a 32bit pattern out to SK9822 / APA102C
constexpr quint8 COMMAND_GET_ID = 0x99; //  Get the board identifier
constexpr quint8 COMMAND_SET_I2C_ADD = 0xAA; //  Set a new I2C address

constexpr quint8 COMMAND_VALUE_FWD = 1; //  I2C value representing forward
constexpr quint8 COMMAND_VALUE_REV = 2; //  I2C value representing reverse

constexpr quint8 COMMAND_VALUE_ON = 1; //  I2C value representing on
constexpr quint8 COMMAND_VALUE_OFF = 0; //  I2C value representing off

constexpr quint32 COMMAND_ANALOG_MAX = 0x3FF; //  Maximum value for analog readings


MotorThunderborg::MotorThunderborg(uint8_t bus, uint8_t address, uint16_t delay, QObject *parent) : MotorBase(parent)
{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        m_i2c = gpio->allocateI2CSlave(address, delay, bus, 40000);
        bool found = false;
        for (int i = 0; i < 3; i++) {
            auto ret = m_i2c->i2cRead(COMMAND_GET_ID);
            if (ret == 0x15) {
                found = true;
                break;
            }
        }
        if (!found) {
            qWarning() << "Cannot find ThunderBorg on address" << Qt::hex << address;
        } else {
            qDebug() << "Found ThunderBorg on address" << Qt::hex << address;
        }
    }
    setLEDShowBattery(true);
//    setLED1Color(Qt::red);
//    setLED2Color(Qt::green);
    m_motors = MOTOR_1 | MOTOR_2;
    qWarning() << "battery level" << getBatteryLevel();
}

MotorThunderborg::~MotorThunderborg()
{
    setLEDShowBattery(true);
    stop();
    QGpio* gpio = QGpio::getInstance();
}


void MotorThunderborg::forward(MotorsFlags motors)
{
    quint8 pwm = quint8((float) PWM_MAX * speed()/100.0);
    if (motors & MOTOR_1 && motors & MOTOR_2) {
        m_i2c->i2cWrite(COMMAND_SET_ALL_FWD, pwm);
    } else {
        if (motors & MOTOR_1) {
            m_i2c->i2cWrite(COMMAND_SET_A_FWD, pwm);
        }
        if (motors & MOTOR_2) {
            m_i2c->i2cWrite(COMMAND_SET_B_FWD, pwm);
        }
    }
}

void MotorThunderborg::stop(MotorsFlags motors)
{
    quint8 pwm = 0;
    if (motors & MOTOR_1 && motors & MOTOR_2) {
        m_i2c->i2cWrite(COMMAND_ALL_OFF, pwm);
    } else {
        if (motors & MOTOR_1) {
            m_i2c->i2cWrite(COMMAND_SET_A_FWD, pwm);
        }
        if (motors & MOTOR_2) {
            m_i2c->i2cWrite(COMMAND_SET_B_FWD, pwm);
        }
    }
}

void MotorThunderborg::reverse(MotorsFlags motors)
{
    quint8 pwm = quint8((float) PWM_MAX * speed()/100.0);
    if (motors & MOTOR_1 && motors & MOTOR_2) {
        m_i2c->i2cWrite(COMMAND_SET_ALL_REV, pwm);
    } else {
        if (motors & MOTOR_1) {
            m_i2c->i2cWrite(COMMAND_SET_A_REV, pwm);
        }
        if (motors & MOTOR_2) {
            m_i2c->i2cWrite(COMMAND_SET_B_REV, pwm);
        }
    }
}

void MotorThunderborg::setSpeed(float speed, MotorsFlags motors) {
    Q_UNUSED(motors)
    if (speed < 0.0f || speed > 100.0f)
        return;
    MotorBase::setSpeed(speed);
}

bool MotorThunderborg::getMotorFault(MotorsFlags motor)
{
    return (m_i2c->i2cRead(motor & MOTOR_1 ? COMMAND_GET_DRIVE_A_FAULT : COMMAND_GET_DRIVE_B_FAULT)
            != COMMAND_VALUE_OFF);
}

float MotorThunderborg::getBatteryLevel() const
{
    auto batteryRaw = m_i2c->i2cRead16(COMMAND_GET_BATT_VOLT);
    float level = float(batteryRaw) / float(COMMAND_ANALOG_MAX);
    level *= VOLTAGE_PIN_MAX;
    return level + VOLTAGE_PIN_CORRECTION;
}


void MotorThunderborg::setLEDShowBattery(bool battery)
{
    m_i2c->i2cWrite(COMMAND_SET_LED_BATT_MON, quint8(battery));
}

void MotorThunderborg::setLED1Color(uint8_t ledColor[3])
{
    m_i2c->i2cWrite(COMMAND_SET_LED1, &ledColor, 3);
}

void MotorThunderborg::setLED2Color(uint8_t ledColor[3])
{
    m_i2c->i2cWrite(COMMAND_SET_LED2, &ledColor, 3);
}

