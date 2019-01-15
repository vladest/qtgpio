#ifndef ROBOCORENODE_H
#define ROBOCORENODE_H

#include <QObject>
#include <qmqttsubscription.h>

class QubotCoreNode : public QObject
{
    Q_OBJECT
public:

    enum NodeDirection {
        NodeReceiving = 0x1, //receive messages from the host
        NodeSending = 0x2 //send messages to the host
    };

    Q_ENUMS(QubotCoreNode)

    explicit QubotCoreNode(QObject *parent = nullptr);

    virtual QString description()  = 0;
    /**
     * @brief nodeTopicRoot: returns root of the topic
     * @return
     */
    virtual QString nodeTopicRoot()  = 0;

    /**
     * @brief saveState: saves node state, so the node can return to previous state after restart
     * @return
     */
    virtual bool saveState() = 0;

    /**
     * @brief restoreState: restore state
     * @return
     */
    virtual bool restoreState() = 0;

    virtual NodeDirection nodeDirection() = 0;
    QMqttSubscription *mqttSubscription() const;
    void subscribe(QMqttClient *mqttClient);

signals:

public slots:
    virtual void parseMessage(const QStringList& topicItems, const QByteArray& payload) = 0;
private:
    QMqttSubscription *m_mqttSubscription;
};

#endif // ROBOCORENODE_H
