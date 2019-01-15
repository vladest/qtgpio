#include "qubotnodesmanager.h"
#include <QMqttClient>

QubotNodesManager::QubotNodesManager(QObject *parent) : QObject(parent)
{
}

void QubotNodesManager::initNodes()
{
    VideoStreamController* videoNode = new VideoStreamController;
    m_registeredNodes.append(videoNode);
}

void QubotNodesManager::handleMessage(const QMqttMessage &qmsg)
{
    qDebug() << qmsg.topic().name() << qmsg.payload();
    const QStringList& topicItems = qmsg.topic().name().split("/");
    if (!topicItems.isEmpty()) {
        for (auto node : m_registeredNodes) {
            if (topicItems.at(0) == node->nodeTopicRoot()) {
                node->parseMessage(topicItems, qmsg.payload());
            }
        }
    }
}

void QubotNodesManager::subscribeNodes(QMqttClient *mqttClient)
{
    for (auto node : m_registeredNodes) {
        node->subscribe(mqttClient);
        connect(node->mqttSubscription(), &QMqttSubscription::messageReceived, this, &QubotNodesManager::handleMessage);
    }
}

