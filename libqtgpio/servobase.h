#ifndef SERVOBASE_H
#define SERVOBASE_H

#include <QObject>

class ServoBase : public QObject
{
    Q_OBJECT
public:
    explicit ServoBase(QObject *parent = nullptr);

    float actuactionRange() const;
    void setActuactionRange(float actuactionRange);

    void setDuties(uint16_t minDuty, uint16_t maxDuty);
    uint16_t minDuty() const;
    uint16_t maxDuty() const;

    /**
     * @brief setServoPulses: set pulses range for specific servo in us
     * @param minPulse
     * @param maxPulse
     * Each servo has its own pulses range
     * For example
     * TowerPro MG995. min: 500, max: 2300
     * TowerPro MG90S. min: 550, max: 2250
     * need to experiment with specific servo to make sure it will not stuck in an adge position
     * trying to reach out behind its threshold. In this case it will consume power and overheat
     */
    void setServoPulses(uint16_t minPulse, uint16_t maxPulse);
    uint16_t servoMinPulse() const;
    uint16_t servoMaxPulse() const;

    uint16_t dutyRange() const;

    virtual void recalcDuty() = 0;
    /**
     * @brief stop: stops all rotations
     */
    virtual void stopRotating() = 0;

    /**
     * @brief startRotating
     * @param torque: -1.0 -  start rotaticn couter clockwise, 1.0 - star rotating clockwise, 0 - stop rotating
     */

    virtual void startRotating(float torque) = 0;
    /**
     * @brief rotateAngle: add angle to current angle
     * @param angle: negative values rotates counter clock wise, positives - clockwise
     */
    virtual void rotateAngle(float angle) = 0;
    /**
     * @brief setAngle : sets specific angle
     * @param angle
     */
    virtual void setAngle(float angle) = 0;

signals:

public slots:
private:
    uint16_t m_minDuty = 0;
    uint16_t m_maxDuty = 0;
    uint16_t m_dutyRange = 0;

    // set min and max PWM pulse to get minimum and maximum angle
    // set according to servo
    uint16_t m_servoMinPulse = 1000;
    uint16_t m_servoMaxPulse = 2000;

    float m_actuactionRange = 180.0f;
};

#endif // SERVOBASE_H
