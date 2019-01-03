#ifndef QGPIO_H
#define QGPIO_H
/*
 * Based on Rpi.GPIO
*/

#include <QObject>
#include <QPointer>
#include <QMap>

#include "rpicpuinfo.h"

class QGpioPort;

class QGpio: public QObject
{
    Q_OBJECT
public:

    enum InitResult {
        INIT_OK = 0,
        INIT_DEVMEM_FAIL,
        INIT_MALLOC_FAIL,
        INIT_MMAP_FAIL,
        INIT_CPUINFO_FAIL,
        INIT_NOT_RPI_FAIL
    };

    enum GpioDirection {
        DIRECTION_INPUT = 1, // is really 0 for control register!
        DIRECTION_OUTPUT = 0, // is really 1 for control register!
        DIRECTION_ALT0 = 4
    };

    enum GpioValue {
        VALUE_HIGH = 1,
        VALUE_LOW = 0
    };

    enum GpioPullUpDown {
        PUD_OFF  = 0,
        PUD_DOWN = 1,
        PUD_UP = 2
    };

    enum GpioEdge {
        NO_EDGE     = 0,
        RISING_EDGE = 1,
        FALLING_EDGE = 2,
        BOTH_EDGE    = 3
    };

    friend class QGpioPort;
    /**
     * @brief getInstance
     * @return singleton instance of QGpio class
     */
    static QGpio *getInstance();

    /**
     * @brief init. Map GPIO ports to an address
     * @return result of init
     */
    InitResult init() const;

    /**
     * @brief deinit. Unmaps memory mapped GPIOs
     */
    void deinit();

    /**
     * @brief allocateGpioPort. Allocates QGpioPort class for specific port.
     * If port already allocated, just returns already allocated address
     * @param port
     * @param direction
     * @param pud
     * @return pointer to QGpioPort
     */
    QPointer<QGpioPort> allocateGpioPort(int port, GpioDirection direction, GpioPullUpDown pud = PUD_OFF);


    /**
     * @brief getGpioMap
     * @return memory mapped address for GPIOs
     */
    uint32_t *getGpioMap();

private:
    QGpio();
    virtual ~QGpio();
    QGpio(const QGpio&) = default;
    const QGpio& operator=(const QGpio&);

    /**
     * @brief eventThreadRun: runner function for events thread
     */
    void eventThreadRun();

    /**
     * @brief createInputEventsThread: function to be called in case of INPUT event needed
     * @param gpioPort
     */
    void addToInputEventsThread(QGpioPort* gpioPort);
    void removeFromInputEventsThread(QGpioPort *gpioPort);

    void inputEventThreadFunc();

signals:
    void inputEvent(QGpioPort* gpio);

private:
    static volatile uint32_t* m_gpioMap;
    static QMap<int, QPointer<QGpioPort> > m_PortsAllocated;
    static QMap<int, QPointer<QGpioPort> > m_EventFDsAllocated;
    RpiCpuInfo m_rpiCpuInfo;
    QThread* m_eventsRunner = nullptr;
    int m_epollFd = -1;
};

#endif // QGPIO_H
