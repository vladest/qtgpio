#pragma once

#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttSubscription>
#include "qubotnodesmanager.h"

class Mqtt : public QObject
{
    Q_OBJECT

public:
    Mqtt(QubotNodesManager* nodesManager, QObject* parent = nullptr);
    virtual ~Mqtt();

    QMqttClient *client() const;

private:
    Q_DISABLE_COPY(Mqtt)

    QMqttClient *m_client = nullptr;
    QubotNodesManager* m_nodesManager = nullptr;
};

