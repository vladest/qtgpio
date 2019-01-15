#include "mqttclient.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include "qubotnodesmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QubotNodesManager qbNodeManager;
    Mqtt mqtt(&qbNodeManager);
    qbNodeManager.initNodes();
    mqtt.client()->connectToHost();

    return app.exec();
}
