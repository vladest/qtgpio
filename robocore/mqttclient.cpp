#include "mqttclient.h"
#include <QDebug>

Mqtt::Mqtt(QubotNodesManager *nodesManager, QObject* parent) : QObject(parent)
  , m_nodesManager(nodesManager)
  , m_client(new QMqttClient)
{
    m_client->setHostname("127.0.0.1");
    m_client->setPort(1883);
    m_client->setKeepAlive(1000);

    QObject::connect(m_client, &QMqttClient::connected, [this]() {
        m_nodesManager->subscribeNodes(m_client);
    });
    connect(m_client, &QMqttClient::disconnected, [this]() {
        qDebug() << "Client disconnected";
    });
    connect(m_client, &QMqttClient::errorChanged, [this]() {
        qDebug() << "error" << m_client->error();
    });
    connect(m_client, &QMqttClient::stateChanged, [this]() {
        qDebug() << "state" << m_client->state();
    });
}

Mqtt::~Mqtt()
{
}

QMqttClient *Mqtt::client() const
{
    return m_client;
}



