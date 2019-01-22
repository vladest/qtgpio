#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/time.h>

#include "qgpio.h"
#include "qgpioport.h"
#include <QMutexLocker>
#include <QFile>
#include <QDateTime>
#include <QDebug>

volatile uint32_t* QGpio::m_gpioMap = bcm2835_gpio;
QMap<int, QPointer<QGpioPort> > QGpio::m_PortsAllocated;
QMap<int, QPointer<QGpioPort> > QGpio::m_EventFDsAllocated;

QGpio* QGpio::getInstance()
{
    QMutex mutex;
    QMutexLocker lock(&mutex);  // to unlock mutex on exit
    static QGpio instance;
    return &instance;
}

QGpio::InitResult QGpio::init() const
{
    if (bcm2835_peripherals != MAP_FAILED || bcm2835_init() == 1) {
        m_gpioMap = bcm2835_gpio;
        return INIT_OK;
    }
    return INIT_MMAP_FAIL;
}

void QGpio::deinit()
{
    bcm2835_close();
}

QPointer<QGpioPort> QGpio::allocateGpioPort(int port, GpioDirection direction, GpioPullUpDown pud)
{
    QPointer<QGpioPort> _port = m_PortsAllocated.value(port, nullptr);
    if (_port == nullptr) {
        _port = new QGpioPort(port, direction, pud);
        _port->setGpioParent(this);
        m_PortsAllocated[port] = _port;
    } else {
        qWarning() << "port" << port << "already allocated";
    }
    return _port;
}

void QGpio::deallocateGpioPort(int port)
{
    QPointer<QGpioPort> _port = m_PortsAllocated.value(port, nullptr);
    if (_port != nullptr) {
        delete _port;
        m_PortsAllocated.remove(port);
    }
}

void QGpio::deallocateGpioPort(QPointer<QGpioPort> port)
{
    if (!port.isNull()) {
        m_PortsAllocated.remove(port->getPort());
        delete port;
    }
}

QGpio::QGpio() : QObject(nullptr)
{
    qDebug() << m_rpiCpuInfo.boardString();
}

QGpio::~QGpio()
{
    while(!m_PortsAllocated.values().isEmpty()) {
        QPointer<QGpioPort> port = m_PortsAllocated.values().takeFirst();
        deallocateGpioPort(port);
    }
    deinit();
}

void QGpio::removeFromInputEventsThread(QGpioPort *gpioPort)
{
    struct epoll_event ev;
    // add to epoll fd
    ev.events = EPOLLIN | EPOLLET | EPOLLPRI;
    ev.data.fd = gpioPort->getValueFd();
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
    m_EventFDsAllocated.remove(ev.data.fd);
    if (m_EventFDsAllocated.isEmpty() && m_eventsRunner != nullptr) {
        m_eventsRunner->requestInterruption();
        m_eventsRunner->deleteLater();
        m_eventsRunner = nullptr;
    }
}

void QGpio::addToInputEventsThread(QGpioPort *gpioPort)
{
    // create epfd_thread if not already open
    if (m_epollFd == -1) {
        m_epollFd = epoll_create(1);
        if (m_epollFd == -1) {
            qWarning() << "Error creating epoll" << errno;
            return;
        }
    }

    if (gpioPort->addEdgeDetect(QGpio::RISING_EDGE, 666) == false) {
        qWarning() << "Error adding edge detecion";
        return;
    }
    struct epoll_event ev;
    // add to epoll fd
    ev.events = EPOLLIN | EPOLLET | EPOLLPRI;
    ev.data.fd = gpioPort->getValueFd();
    m_EventFDsAllocated[ev.data.fd] = gpioPort;
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
        gpioPort->removeEdgeDetect();
        return;
    }
    if (m_eventsRunner == nullptr) {
        m_eventsRunner = QThread::create([&]{
            inputEventThreadFunc();
        });
        m_eventsRunner->start(QThread::TimeCriticalPriority);
    }
}

void QGpio::inputEventThreadFunc() {
    struct epoll_event events;
    char buf;
    struct timeval tv_timenow;
    quint64 timenow = 0;

    while (!m_eventsRunner->isInterruptionRequested()) {
        int epoll_ret = epoll_wait(m_epollFd, &events, 1, -1);
        if (epoll_ret > 0) {
            lseek(events.data.fd, 0, SEEK_SET);
            if (read(events.data.fd, &buf, 1) != 1) {
                m_eventsRunner->requestInterruption();
            }
            QGpioPort* g = m_EventFDsAllocated.value(events.data.fd);
            if (g == nullptr) {
                qWarning() << "No GPIO port associated with FD" << events.data.fd;
            }
            if (g->getInitialTrigger()) {     // ignore first epoll trigger
                g->setInitialTrigger(true);
            } else {
                gettimeofday(&tv_timenow, NULL);
                timenow = tv_timenow.tv_sec*1E6 + tv_timenow.tv_usec;
                const quint64 ts = g->getLastCallTimestamp();
                if (g->getBouncetime() == -666 || timenow - ts >
                        (unsigned int)g->getBouncetime()*1000 || ts == 0 ||
                        ts > timenow) {
                    g->setLastCallTimestamp(timenow);
                    emit inputEvent(g);
//                    event_occurred[g->gpio] = 1;
//                    run_callbacks(g->gpio);
                }
            }
        } else if (epoll_ret == -1) {
            /*  If a signal is received while we are waiting,
                epoll_wait will return with an EINTR error.
                Just try again in that case.  */
            if (errno == EINTR) {
                continue;
            }
            m_eventsRunner->requestInterruption();
        }
    }
    qDebug() << Q_FUNC_INFO << " thread stopped";
}

uint32_t *QGpio::getGpioMap()
{
    return (uint32_t *)m_gpioMap;
}
