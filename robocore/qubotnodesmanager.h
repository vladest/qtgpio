#ifndef QUBOTNODESMANAGER_H
#define QUBOTNODESMANAGER_H

#include <QObject>
#include "qubotcorenode.h"
#include "videostreamcontroller.h"

class QMqttClient;

class QubotNodesManager : public QObject
{

    Q_OBJECT
public:

    explicit QubotNodesManager(QObject *parent = nullptr);

    void initNodes();
    void subscribeNodes(QMqttClient* mqttClient);

signals:

public slots:
    void handleMessage(const QMqttMessage &qmsg);

private:
    QList<QubotCoreNode*> m_registeredNodes;
};

#endif // QUBOTNODESMANAGER_H
