#include "servocontrol9685.h"
#include <QDebug>
#include <QCoreApplication>

#define ANGLE(x)	((uint8_t)(x))

#define SERVO_MIN_PULSE	750
#define SERVO_MAX_PULSE	2250



//def set_pulse_width_range(self, min_pulse=750, max_pulse=2250):
//        self._min_duty = int((min_pulse * self._pwm_out.frequency) / 1000000 * 0xffff)
//        max_duty = (max_pulse * self._pwm_out.frequency) / 1000000 * 0xffff
//self._duty_range = int(max_duty - self._min_duty)


//if not 0.0 <= value <= 1.0:
//    raise ValueError("Must be 0.0 to 1.0")
//duty_cycle = self._min_duty + int(value * self._duty_range)

ServoControl9685::ServoControl9685(int channel, QObject *parent) : QObject(parent), m_channel(channel)
{
    m_pwm = new PwmPCA9685();
    m_pwm->pwmSetFrequency(50.0);
    m_pwm->startPwm(channel, 0);
    m_minDuty = ((float)SERVO_MIN_PULSE * 50.0) / 1000000.0 * 0xffff;
    m_maxDuty = ((float)SERVO_MAX_PULSE * 50.0) / 1000000.0 * 0xffff;
    m_dutyRange = m_maxDuty - m_minDuty;
    qDebug() << m_minDuty << m_maxDuty << m_dutyRange;
}

ServoControl9685::~ServoControl9685()
{
    m_pwm->stopPwm(m_channel);
    delete m_pwm;
}

void ServoControl9685::startCounterClockWise() {
    uint16_t duty_cycle = m_minDuty;
    duty_cycle = (duty_cycle + 1) >> 4;
    qDebug() << "duty cycle ccw" << duty_cycle;
    m_pwm->pwmSetDutyCycle(m_channel, duty_cycle);
}

void ServoControl9685::startClockWise() {
    uint16_t duty_cycle = m_minDuty + m_dutyRange;
    duty_cycle = (duty_cycle + 1) >> 4;
    qDebug() << "duty cycle cw" << duty_cycle;
    m_pwm->pwmSetDutyCycle(m_channel, duty_cycle);
}

void ServoControl9685::counterClockWiseAngle(float angle) {
//    duty_cycle = self._min_duty + int(value * self._duty_range)
//self._pwm_out.duty_cycle = duty_cycle
}

void ServoControl9685::clockWiseAngle(float angle) {

}

void ServoControl9685::stop() {
    m_pwm->pwmSetDutyCycle(m_channel, 0.0);
}

