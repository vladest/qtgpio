#pragma once

#include <stdint.h>
#include <QPointer>
#include <QThread>
#include "qgpioport.h"
#include "qgpioi2cslave.h"

#define QWIIC_BUTTON_I2C_ADDR	0x6F // The device i2c address in default

enum Qwiic_Button_Register : uint8_t
{
    ID = 0x00,
    FIRMWARE_MINOR = 0x01,
    FIRMWARE_MAJOR = 0x02,
    BUTTON_STATUS = 0x03,
    INTERRUPT_CONFIG = 0x04,
    BUTTON_DEBOUNCE_TIME = 0x05,
    PRESSED_QUEUE_STATUS = 0x07,
    PRESSED_QUEUE_FRONT = 0x08,
    PRESSED_QUEUE_BACK = 0x0C,
    CLICKED_QUEUE_STATUS = 0x10,
    CLICKED_QUEUE_FRONT = 0x11,
    CLICKED_QUEUE_BACK = 0x15,
    LED_BRIGHTNESS = 0x19,
    LED_PULSE_GRANULARITY = 0x1A,
    LED_PULSE_CYCLE_TIME = 0x1B,
    LED_PULSE_OFF_TIME = 0x1D,
    I2C_ADDRESS = 0x1F,
};

class QwiicButton: public QThread
{
    Q_OBJECT
public:
    QwiicButton(QThread* parent = nullptr);
    virtual ~QwiicButton();

    void init(uint8_t bus, uint8_t address = QWIIC_BUTTON_I2C_ADDR, uint8_t delay = 1);

    //Button status/config
    bool isPressed(bool &isOk);                       //Returns 1 if the button/switch is pressed, and 0 otherwise
    bool hasBeenClicked();                  //Returns 1 if the button/switch is clicked, and 0 otherwise
    uint16_t getDebounceTime();             //Returns the time that the button waits for the mechanical contacts to settle, (in milliseconds).
    uint8_t setDebounceTime(uint16_t time); //Sets the time that the button waits for the mechanical contacts to settle (in milliseconds) and checks if the register was set properly. Returns 0 on success, 1 on register I2C write fail, and 2 if the value didn't get written into the register properly.

    //Interrupt status/config
    uint8_t enablePressedInterrupt();  //When called, the interrupt will be configured to trigger when the button is pressed. If enableClickedInterrupt() has also been called, then the interrupt will trigger on either a push or a click.
    uint8_t disablePressedInterrupt(); //When called, the interrupt will no longer be configured to trigger when the button is pressed. If enableClickedInterrupt() has also been called, then the interrupt will still trigger on the button click.
    uint8_t enableClickedInterrupt();  //When called, the interrupt will be configured to trigger when the button is clicked. If enablePressedInterrupt() has also been called, then the interrupt will trigger on either a push or a click.
    uint8_t disableClickedInterrupt(); //When called, the interrupt will no longer be configured to trigger when the button is clicked. If enablePressedInterrupt() has also been called, then the interrupt will still trigger on the button press.
    bool available();                  //Returns the eventAvailable bit
    uint8_t clearEventBits();          //Sets isPressed, hasBeenClicked, and eventAvailable to zero
    uint8_t resetInterruptConfig();    //Resets the interrupt configuration back to defaults.

    //Queue manipulation
    bool isPressedQueueFull();           //Returns true if the queue of button press timestamps is full, and false otherwise.
    bool isPressedQueueEmpty();          //Returns true if the queue of button press timestamps is empty, and false otherwise.
    unsigned long timeSinceLastPress();  //Returns how many milliseconds it has been since the last button press. Since this returns a 32-bit unsigned int, it will roll over about every 50 days.
    unsigned long timeSinceFirstPress(); //Returns how many milliseconds it has been since the first button press. Since this returns a 32-bit unsigned int, it will roll over about every 50 days.
    unsigned long popPressedQueue();     //Returns the oldest value in the queue (milliseconds since first button press), and then removes it.

    bool isClickedQueueFull();           //Returns true if the queue of button click timestamps is full, and false otherwise.
    bool isClickedQueueEmpty();          //Returns true if the queue of button press timestamps is empty, and false otherwise.
    unsigned long timeSinceLastClick();  //Returns how many milliseconds it has been since the last button click. Since this returns a 32-bit unsigned int, it will roll over about every 50 days.
    unsigned long timeSinceFirstClick(); //Returns how many milliseconds it has been since the first button click. Since this returns a 32-bit unsigned int, it will roll over about every 50 days.
    unsigned long popClickedQueue();     //Returns the oldest value in the queue (milliseconds since first button click), and then removes it.

    //LED configuration
    bool LEDconfig(uint8_t brightness, uint16_t cycleTime,
                   uint16_t offTime, uint8_t granularity = 1); //Configures the LED with the given max brightness, granularity (1 is fine for most applications), cycle time, and off time.
    bool LEDoff();                                             //Turns the onboard LED off
    bool LEDon(uint8_t brightness = 255);                      //Turns the onboard LED on with specified brightness. Set brightness to an integer between 0 and 255, where 0 is off and 255 is max brightness.

signals:
    void pressed();
    void released();
    void clicked();

    // QThread interface
protected:
    void run() override;

private:
    QPointer<QGpioI2CSlave> m_i2c;
};

