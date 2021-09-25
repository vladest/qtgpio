#ifndef QGPIO_H
#define QGPIO_H

#include <QObject>
#include <QPointer>
#include <QMap>

class QGpioPort;
class QGpioI2CSlave;
class QGpioSPI;

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
        DIRECTION_INPUT = 0,
        DIRECTION_OUTPUT = 1,
    };

    enum GpioValue {
        VALUE_HIGH = 1,
        VALUE_LOW = 0
    };

    enum GpioPullUpDown {
        PULL_OFF = -1,
        PULL_UP = 0,
        PULL_DOWN = 1,

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
     * @brief allocateGpioPort. Allocates QGpioPort class for specific port.
     * If port already allocated, just returns already allocated address
     * @param port
     * @param direction
     * @param pud
     * @return pointer to QGpioPort
     */
    QPointer<QGpioPort> allocateGpioPort(int port, GpioDirection direction, GpioPullUpDown pud = PULL_OFF);
    void deallocateGpioPort(int port);
    void deallocateGpioPort(QPointer<QGpioPort> port);

    QPointer<QGpioI2CSlave> allocateI2CSlave(uint8_t address, uint8_t delay, uint8_t busNum, uint16_t timeout);
    void deallocateI2CSlave(uint8_t address);
    void deallocateI2CSlave(QPointer<QGpioI2CSlave> i2cSlave);

    QPointer<QGpioSPI> allocateSPI(uint8_t busNum, uint16_t timeout);

    /**
     * @brief getGpioMap
     * @return memory mapped address for GPIOs
     */
    //uint32_t *getGpioMap();
private:
    QGpio();
    virtual ~QGpio();
    Q_DISABLE_COPY(QGpio);

    /**
     * @brief deinit. Unmaps memory mapped GPIOs
     */
    void deinit();

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
    static QMap<uint8_t, QPointer<QGpioI2CSlave> > m_i2cSlavesAllocated;
    static QMap<int, QPointer<QGpioPort> > m_EventFDsAllocated;
    //RpiCpuInfo m_rpiCpuInfo;
    QThread* m_eventsRunner = nullptr;
    int m_epollFd = -1;
};

#endif // QGPIO_H
