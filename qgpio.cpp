#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include "qgpio.h"
#include "qgpioport.h"

#include <QMutexLocker>
#include <QFile>
#include <QDebug>

#define BCM2708_PERI_BASE_DEFAULT   0x20000000
#define BCM2709_PERI_BASE_DEFAULT   0x3f000000
#define GPIO_BASE_OFFSET            0x200000

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

volatile uint32_t* QGpio::m_gpioMap = nullptr;
QMap<int, QPointer<QGpioPort> > QGpio::m_PortsAllocated;

QGpio &QGpio::getInstance()
{
    QMutex mutex;
    QMutexLocker lock(&mutex);  // to unlock mutex on exit
    static QGpio instance;
    return instance;
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

QPointer<QGpioPort> QGpio::allocateGpioPort(int port, GpioDirection direction, PullUpDown pud)
{
    QPointer<QGpioPort> _port = m_PortsAllocated.value(port, nullptr);
    if (_port == nullptr) {
        _port = new QGpioPort(port, direction, pud);
        _port->setGpio(this);
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

uint32_t *QGpio::getGpioMap()
{
    return (uint32_t *)m_gpioMap;
}
