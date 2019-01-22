#ifndef MOTORBASE_H
#define MOTORBASE_H

#include <QObject>

class MotorBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float speed READ speed WRITE setSpeed NOTIFY speedChanged)
public:
    explicit MotorBase(QObject *parent = nullptr);

    virtual void forward() = 0;
    virtual void stop() = 0;
    virtual void reverse() = 0;
    virtual void setSpeed(float speed);
    virtual float speed() const;

signals:
    void speedChanged(float speed);

protected:
    float m_speed = 50.0f;
};

#endif // MOTORBASE_H
