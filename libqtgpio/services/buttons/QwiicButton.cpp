#include "QwiicButton.h"
#include <QDebug>

#include <unistd.h>

#define DEV_ID 0x5D

#pragma pack(1)
typedef union {
    struct
    {
        bool eventAvailable : 1; //This is bit 0. User mutable, gets set to 1 when a new event occurs. User is expected to write 0 to clear the flag.
        bool hasBeenClicked : 1; //Defaults to zero on POR. Gets set to one when the button gets clicked. Must be cleared by the user.
        bool isPressed : 1;      //Gets set to one if button is pushed.
        bool : 5;
    };
    uint8_t byteWrapped;
} StatusRegisterBitField;

typedef union {
    struct
    {
        bool clickedEnable : 1; //This is bit 0. user mutable, set to 1 to enable an interrupt when the button is clicked. Defaults to 0.
        bool pressedEnable : 1; //user mutable, set to 1 to enable an interrupt when the button is pressed. Defaults to 0.
        bool : 6;
    };
    uint8_t byteWrapped;
} InterruptConfigBitField;

typedef union {
    struct
    {
        bool popRequest : 1; //This is bit 0. User mutable, user sets to 1 to pop from queue, we pop from queue and set the bit back to zero.
        bool isEmpty : 1;    //user immutable, returns 1 or 0 depending on whether or not the queue is empty
        bool isFull : 1;     //user immutable, returns 1 or 0 depending on whether or not the queue is full
        bool : 5;
    };
    uint8_t byteWrapped;
} QueueStatusBitField;
#pragma pack()

QwiicButton::QwiicButton(QThread *parent): QThread(parent)
{
}

QwiicButton::~QwiicButton()
{
    requestInterruption();
    wait(1000);
}

void QwiicButton::init(uint8_t bus, uint8_t address, uint8_t delay)
{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        m_i2c = gpio->allocateI2CSlave(address, delay, bus, 40000);
        qWarning() << "BUTTON VID:" << Qt::hex << m_i2c->i2cRead(ID);
        start(NormalPriority);
    }
}

bool QwiicButton::isPressed(bool& isOk)
{
    StatusRegisterBitField statusRegister;
    statusRegister.byteWrapped = m_i2c->i2cRead(BUTTON_STATUS);
    if (statusRegister.byteWrapped == 0xff) {
        isOk = false;
        return false;
    }
    isOk = true;
    //qWarning() << "isPressed ret:" << Qt::hex << statusRegister.byteWrapped;
    return statusRegister.isPressed;
}

bool QwiicButton::hasBeenClicked()
{
    StatusRegisterBitField statusRegister;
    statusRegister.byteWrapped = m_i2c->i2cRead(BUTTON_STATUS);
    if (statusRegister.byteWrapped == 0xff) {
        return false;
    } else {
        qWarning() << "has been clicked" << Qt::hex << statusRegister.byteWrapped;
    }
    return statusRegister.hasBeenClicked;
}

uint16_t QwiicButton::getDebounceTime()
{
    return m_i2c->i2cRead16(BUTTON_DEBOUNCE_TIME);
}

uint8_t QwiicButton::setDebounceTime(uint16_t time)
{
    return m_i2c->i2cWrite(BUTTON_DEBOUNCE_TIME, time);
}

/*------------------- Interrupt Status/Configuration ---------------- */
uint8_t QwiicButton::enablePressedInterrupt()
{
    InterruptConfigBitField interruptConfigure;
    interruptConfigure.byteWrapped = m_i2c->i2cRead(INTERRUPT_CONFIG);
    interruptConfigure.pressedEnable = 1;
    return m_i2c->i2cWrite(INTERRUPT_CONFIG, interruptConfigure.byteWrapped);
}

uint8_t QwiicButton::disablePressedInterrupt()
{
    InterruptConfigBitField interruptConfigure;
    interruptConfigure.byteWrapped = m_i2c->i2cRead(INTERRUPT_CONFIG);
    interruptConfigure.pressedEnable = 0;
    return m_i2c->i2cWrite(INTERRUPT_CONFIG, interruptConfigure.byteWrapped);
}

uint8_t QwiicButton::enableClickedInterrupt()
{
    InterruptConfigBitField interruptConfigure;
    interruptConfigure.byteWrapped = m_i2c->i2cRead(INTERRUPT_CONFIG);
    interruptConfigure.clickedEnable = 1;
    return m_i2c->i2cWrite(INTERRUPT_CONFIG, interruptConfigure.byteWrapped);
}

uint8_t QwiicButton::disableClickedInterrupt()
{
    InterruptConfigBitField interruptConfigure;
    interruptConfigure.byteWrapped = m_i2c->i2cRead(INTERRUPT_CONFIG);
    interruptConfigure.clickedEnable = 0;
    return m_i2c->i2cWrite(INTERRUPT_CONFIG, interruptConfigure.byteWrapped);
}

bool QwiicButton::available()
{
    StatusRegisterBitField buttonStatus;
    buttonStatus.byteWrapped = m_i2c->i2cRead(BUTTON_STATUS);
    //qWarning() << "available" << Qt::hex << buttonStatus.byteWrapped;
    if (buttonStatus.byteWrapped == 0xff) {
        return false;
    }
    return buttonStatus.eventAvailable;
}

uint8_t QwiicButton::clearEventBits()
{
    StatusRegisterBitField buttonStatus;
    buttonStatus.byteWrapped = m_i2c->i2cRead(BUTTON_STATUS);
    buttonStatus.isPressed = 0;
    buttonStatus.hasBeenClicked = 0;
    buttonStatus.eventAvailable = 0;
    return m_i2c->i2cWrite(BUTTON_STATUS, buttonStatus.byteWrapped);
}

uint8_t QwiicButton::resetInterruptConfig()
{
    InterruptConfigBitField interruptConfigure;
    interruptConfigure.pressedEnable = 1;
    interruptConfigure.clickedEnable = 1;
    return m_i2c->i2cWrite(INTERRUPT_CONFIG, interruptConfigure.byteWrapped);
//    StatusRegisterBitField buttonStatus;
//    buttonStatus.eventAvailable = 0;
//    return m_i2c->i2cWrite(BUTTON_STATUS, buttonStatus.byteWrapped);
}

/*------------------------- Queue Manipulation ---------------------- */
//pressed queue manipulation
bool QwiicButton::isPressedQueueFull()
{
    QueueStatusBitField pressedQueueStatus;
    pressedQueueStatus.byteWrapped = m_i2c->i2cRead(PRESSED_QUEUE_STATUS);
    return pressedQueueStatus.isFull;
}

bool QwiicButton::isPressedQueueEmpty()
{
    QueueStatusBitField pressedQueueStatus;
    pressedQueueStatus.byteWrapped = m_i2c->i2cRead(PRESSED_QUEUE_STATUS);
    return pressedQueueStatus.isEmpty;
}

unsigned long QwiicButton::timeSinceLastPress()
{
    return m_i2c->i2cRead32(PRESSED_QUEUE_FRONT);
}

unsigned long QwiicButton::timeSinceFirstPress()
{
    return m_i2c->i2cRead32(PRESSED_QUEUE_BACK);
}

unsigned long QwiicButton::popPressedQueue()
{
    unsigned long tempData = timeSinceFirstPress(); //grab the oldest value on the queue

    QueueStatusBitField pressedQueueStatus;
    pressedQueueStatus.byteWrapped = m_i2c->i2cRead(PRESSED_QUEUE_STATUS);
    pressedQueueStatus.popRequest = 1;
    m_i2c->i2cWrite(PRESSED_QUEUE_STATUS, pressedQueueStatus.byteWrapped); //remove the oldest value from the queue

    return tempData; //return the value we popped
}

//clicked queue manipulation
bool QwiicButton::isClickedQueueFull()
{
    QueueStatusBitField clickedQueueStatus;
    clickedQueueStatus.byteWrapped = m_i2c->i2cRead(CLICKED_QUEUE_STATUS);
    return clickedQueueStatus.isFull;
}

bool QwiicButton::isClickedQueueEmpty()
{
    QueueStatusBitField clickedQueueStatus;
    clickedQueueStatus.byteWrapped = m_i2c->i2cRead(CLICKED_QUEUE_STATUS);
    return clickedQueueStatus.isEmpty;
}

unsigned long QwiicButton::timeSinceLastClick()
{
    return m_i2c->i2cRead32(CLICKED_QUEUE_FRONT);
}

unsigned long QwiicButton::timeSinceFirstClick()
{
    return m_i2c->i2cRead32(CLICKED_QUEUE_BACK);
}

unsigned long QwiicButton::popClickedQueue()
{
    unsigned long tempData = timeSinceFirstClick();
    QueueStatusBitField clickedQueueStatus;
    clickedQueueStatus.byteWrapped = m_i2c->i2cRead(CLICKED_QUEUE_STATUS);
    clickedQueueStatus.popRequest = 1;
    m_i2c->i2cWrite(CLICKED_QUEUE_STATUS, clickedQueueStatus.byteWrapped);
    return tempData;
}

/*------------------------ LED Configuration ------------------------ */
bool QwiicButton::LEDconfig(uint8_t brightness, uint16_t cycleTime, uint16_t offTime, uint8_t granularity)
{
    bool success = m_i2c->i2cWrite(LED_BRIGHTNESS, brightness);
    success &= m_i2c->i2cWrite(LED_PULSE_GRANULARITY, granularity);
    success &= m_i2c->i2cWrite(LED_PULSE_CYCLE_TIME, cycleTime);
    success &= m_i2c->i2cWrite(LED_PULSE_OFF_TIME, offTime);
    return success;
}

bool QwiicButton::LEDoff()
{
    return LEDconfig(0, 0, 0);
}

bool QwiicButton::LEDon(uint8_t brightness)
{
    return LEDconfig(brightness, 0, 0);
}

void QwiicButton::run()
{
    bool isOk = false;
    bool _pressed = isPressed(isOk);
    if (_pressed && isOk) {
        emit pressed();
    }
    while (!isInterruptionRequested()) {
        if (available()) {

            bool _nowPresed = isPressed(isOk);
            if (isOk) {
                if (_pressed == false && _nowPresed) {
                    _pressed = true;
                    LEDon();
                    emit pressed();
                } else if (_pressed && _nowPresed == false) {
                    _pressed = false;
                    LEDoff();
                    emit released();
                }
            }
            // Seems click doesnt handled correctly yet
            // TODO: simulate through press/release events
//            if (hasBeenClicked()) {
//                emit clicked();
//            }
        }
        msleep(20);
    }
}
