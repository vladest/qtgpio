#include "videostreamcontroller.h"
#include <QtDebug>

VideoStreamController::VideoStreamController(QObject *parent) : QubotCoreNode(parent)
{

}
//ffmpeg -f video4linux2 -s 640x480 -r 24 -re -i /dev/video0 -vcodec libx264 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -threads 2 -f mpegts udp://192.168.1.206:35001?pkt_size=1316

void VideoStreamController::parseMessage(const QStringList &topicItems, const QByteArray &payload)
{
    if (topicItems.size() != 3) {
        qWarning() << "Invalid topic" << topicItems;
        return;
    }
    bool ok  = false;
    int video_source = topicItems.at(2).toInt(&ok);
    if (ok == false) {
        qWarning() << "Invalid video source number" << topicItems.at(2);
        return;
    }
    if (topicItems.at(1) == QStringLiteral("start")) {
        if (!m_processes.keys().contains(video_source)) {
            QProcess* process = new QProcess;

            connect(process, &QProcess::errorOccurred, this, [this](){
                QProcess* process = (QProcess*)sender();
                qWarning() << "Process starting error" << process->errorString();
            });
            connect(process, &QProcess::stateChanged, this, [this](){
                QProcess* process = (QProcess*)sender();
                int video_source = m_processes.key(process, -1);
                if (video_source == -1) {
                    return;
                }
                if (process->state() == QProcess::Running) {
                    m_processes[video_source] = process;
                } else if (process->state() == QProcess::NotRunning) {
                    m_processes.remove(video_source);
                }
            });
            QString command = QString("ffmpeg -f video4linux2 -s 640x480 -r 24 -re -i /dev/video%1 -vcodec libx264 -pix_fmt yuv420p -preset ultrafast -tune zerolatency -threads 2 -f mpegts udp://%2?pkt_size=1316").arg(video_source).arg(payload.data());
            process->start(command);
        } else {
            qWarning() << "stream" << video_source << "already started";
        }
    } else if (topicItems.at(1) == QStringLiteral("stop")) {
        QProcess *process = m_processes.value(video_source);
        if (process != nullptr) {
            process->kill();
            m_processes.remove(video_source);
            delete process;
        } else {
            qWarning() << "Process not found for" << video_source;
        }
    }
}


QString VideoStreamController::description()
{
    return tr("Node responsible for video translation from robot");
}

QString VideoStreamController::nodeTopicRoot()
{
    return QStringLiteral("videocontrols");
}

bool VideoStreamController::saveState()
{
    return true;
}

bool VideoStreamController::restoreState()
{
    return true;
}

QubotCoreNode::NodeDirection VideoStreamController::nodeDirection()
{
    return QubotCoreNode::NodeReceiving;
}
