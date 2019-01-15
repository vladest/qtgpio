#ifndef VIDEOSTREAMCONTROLLER_H
#define VIDEOSTREAMCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QProcess>
#include "qubotcorenode.h"

class VideoStreamController : public QubotCoreNode
{
    Q_OBJECT
public:
    explicit VideoStreamController(QObject *parent = nullptr);
    QString description() override;
    QString nodeTopicRoot() override;
    bool saveState() override;
    bool restoreState() override;
    NodeDirection nodeDirection() override;

signals:

public slots:
    void parseMessage(const QStringList& topicItems, const QByteArray& payload) override;

private:
    QMap<int, QProcess*> m_processes;
};

#endif // VIDEOSTREAMCONTROLLER_H
