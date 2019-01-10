#ifndef QGPIO_H
#define QGPIO_H
/*
 * Based on Rpi.GPIO
*/

#include <QObject>
#include <QPointer>
#include <QMap>
#include "rpicpuinfo.h"

#include "rpi/bcm2835.h"

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
        DIRECTION_INPUT = BCM2835_GPIO_FSEL_INPT, // is really 0 for control register!
        DIRECTION_OUTPUT = BCM2835_GPIO_FSEL_OUTP, // is really 1 for control register!
        DIRECTION_ALT0 = BCM2835_GPIO_FSEL_ALT0,   /*!< Alternate function 0 0b100 */
        DIRECTION_ALT1 = BCM2835_GPIO_FSEL_ALT1,   /*!< Alternate function 1 0b101 */
        DIRECTION_ALT2 = BCM2835_GPIO_FSEL_ALT2,   /*!< Alternate function 2 0b110, */
        DIRECTION_ALT3 = BCM2835_GPIO_FSEL_ALT3,   /*!< Alternate function 3 0b111 */
        DIRECTION_ALT4 = BCM2835_GPIO_FSEL_ALT4,   /*!< Alternate function 4 0b011 */
        DIRECTION_ALT5 = BCM2835_GPIO_FSEL_ALT5
    };

    enum GpioValue {
        VALUE_HIGH = 1,
        VALUE_LOW = 0
    };

    enum GpioPullUpDown {
        PUD_OFF  = BCM2835_GPIO_PUD_OFF,
        PUD_DOWN = BCM2835_GPIO_PUD_DOWN,
        PUD_UP = BCM2835_GPIO_PUD_UP
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
    QPointer<QGpioPort> allocateGpioPort(int port, GpioDirection direction, GpioPullUpDown pud = PUD_OFF);
    void deallocateGpioPort(int port);
    void deallocateGpioPort(QPointer<QGpioPort> port);

    /**
     * @brief getGpioMap
     * @return memory mapped address for GPIOs
     */
    uint32_t *getGpioMap();
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
    static QMap<int, QPointer<QGpioPort> > m_EventFDsAllocated;
    RpiCpuInfo m_rpiCpuInfo;
    QThread* m_eventsRunner = nullptr;
    int m_epollFd = -1;
};

#endif // QGPIO_H
