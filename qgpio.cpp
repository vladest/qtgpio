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

#define BCM2708_PERI_BASE_DEFAULT   0x20000000
#define BCM2709_PERI_BASE_DEFAULT   0x3f000000
#define GPIO_BASE_OFFSET            0x200000

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

volatile uint32_t* QGpio::m_gpioMap = nullptr;
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
    int mem_fd;
    uint64_t *gpio_mem;
    uint32_t peri_base = 0;
    uint32_t gpio_base;
    unsigned char buf[4];
    FILE *fp;
    int found = 0;

    // try /dev/gpiomem first - this does not require root privs
    if ((mem_fd = open("/dev/gpiomem", O_RDWR|O_SYNC)) > 0) {
        m_gpioMap = (uint32_t *)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0);
        if ((void *)m_gpioMap == MAP_FAILED) {
            return INIT_MMAP_FAIL;
        } else {
            return INIT_OK;
        }
    }

    // revert to /dev/mem method - requires root

    // determine peri_base
    if ((fp = fopen("/proc/device-tree/soc/ranges", "rb")) != NULL) {
        // get peri base from device tree
        fseek(fp, 4, SEEK_SET);
        if (fread(buf, 1, sizeof buf, fp) == sizeof buf) {
            peri_base = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] << 0;
        }
        fclose(fp);
    } else {
        // guess peri base based on /proc/cpuinfo hardware field
        RpiCpuInfo::RaspberryPiSoCs soc = m_rpiCpuInfo.getSoc();
        if (soc == RpiCpuInfo::SOC_UNDEFINED)
            return INIT_CPUINFO_FAIL;
        if (soc == RpiCpuInfo::SOC_BCM2708 || soc == RpiCpuInfo::SOC_BCM2835) {
            peri_base = BCM2708_PERI_BASE_DEFAULT;
            found = 1;
        } else if (soc == RpiCpuInfo::SOC_BCM2709 || soc == RpiCpuInfo::SOC_BCM2836) {
            peri_base = BCM2709_PERI_BASE_DEFAULT;
            found = 1;
        }
        if (!found)
            return INIT_NOT_RPI_FAIL;
    }

    if (!peri_base)
        return INIT_NOT_RPI_FAIL;
    gpio_base = peri_base + GPIO_BASE_OFFSET;

    // mmap the GPIO memory registers
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
        return INIT_DEVMEM_FAIL;

    if ((gpio_mem = (uint64_t*)malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == nullptr)
        return INIT_MALLOC_FAIL;

    if ((uint64_t)gpio_mem % PAGE_SIZE)
        gpio_mem += PAGE_SIZE - ((uint64_t)gpio_mem % PAGE_SIZE);

    m_gpioMap = (uint32_t *)mmap( (void *)gpio_mem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, gpio_base);

    if ((void *)m_gpioMap == MAP_FAILED) {
        return INIT_MMAP_FAIL;
    }
    return INIT_OK;
}

void QGpio::deinit()
{
    if (m_gpioMap != nullptr) {
        munmap((void *)m_gpioMap, BLOCK_SIZE);
    }
}

QPointer<QGpioPort> QGpio::allocateGpioPort(int port, GpioDirection direction, GpioPullUpDown pud)
{
    QPointer<QGpioPort> _port = m_PortsAllocated.value(port, nullptr);
    if (_port == nullptr) {
        _port = new QGpioPort(port, direction, pud);
        _port->setGpioParent(this);
        m_PortsAllocated[port] = _port;
    }
    return _port;
}

QGpio::QGpio() : QObject(nullptr)
{
    qDebug() << m_rpiCpuInfo.boardString();
}

QGpio::~QGpio()
{
    deinit();
}

const QGpio &QGpio::operator=(const QGpio &)
{
    return *this;
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
