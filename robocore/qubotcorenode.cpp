#include "qubotcorenode.h"
#include <QMqttClient>

QubotCoreNode::QubotCoreNode(QObject *parent) : QObject(parent)
{
}

QMqttSubscription *QubotCoreNode::mqttSubscription() const
{
    return m_mqttSubscription;
}

void QubotCoreNode::subscribe(QMqttClient *mqttClient)
{
    m_mqttSubscription = mqttClient->subscribe(QMqttTopicFilter(nodeTopicRoot() + QStringLiteral("/#")));
}
