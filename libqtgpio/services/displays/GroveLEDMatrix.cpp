#include "GroveLEDMatrix.h"
#include <QDebug>

#include <unistd.h>
#include "math.h"
#define I2C_CMD_CONTINUE_DATA	0x81

#define GROVE_TWO_RGB_LED_MATRIX_VID 			0x2886 // Vender ID of the device
#define GROVE_TWO_RGB_LED_MATRIX_PID 			0x8005 // Product ID of the device

#define I2C_CMD_GET_DEV_ID		    			0x00 // This command gets device ID information
#define I2C_CMD_DISP_BAR		    			0x01 // This command displays LED bar
#define I2C_CMD_DISP_EMOJI		    			0x02 // This command displays emoji
#define I2C_CMD_DISP_NUM          				0x03 // This command displays number
#define I2C_CMD_DISP_STR		    			0x04 // This command displays string
#define I2C_CMD_DISP_CUSTOM		    			0x05 // This command displays user-defined pictures
#define I2C_CMD_DISP_OFF		    			0x06 // This command cleans the display
#define I2C_CMD_DISP_ASCII		    			0x07 // not use
#define I2C_CMD_DISP_FLASH						0x08 // This command displays pictures which are stored in flash
#define I2C_CMD_DISP_COLOR_BAR          		0x09 // This command displays colorful led bar
#define I2C_CMD_DISP_COLOR_WAVE         		0x0a // This command displays built-in wave animation
#define I2C_CMD_DISP_COLOR_CLOCKWISE    		0x0b // This command displays built-in clockwise animation
#define I2C_CMD_DISP_COLOR_ANIMATION       		0x0c // This command displays other built-in animation
#define I2C_CMD_DISP_COLOR_BLOCK                0x0d // This command displays an user-defined color
#define I2C_CMD_STORE_FLASH						0xa0 // This command stores frames in flash
#define I2C_CMD_DELETE_FLASH        			0xa1 // This command deletes all the frames in flash

#define I2C_CMD_LED_ON			    			0xb0 // This command turns on the indicator LED flash mode
#define I2C_CMD_LED_OFF			    			0xb1 // This command turns off the indicator LED flash mode
#define I2C_CMD_AUTO_SLEEP_ON	    			0xb2 // This command enable device auto sleep mode
#define I2C_CMD_AUTO_SLEEP_OFF	    			0xb3 // This command disable device auto sleep mode (default mode)

#define I2C_CMD_DISP_ROTATE         			0xb4 // This command setting the display orientation
#define I2C_CMD_DISP_OFFSET         			0xb5 // This command setting the display offset

#define I2C_CMD_SET_ADDR		    			0xc0 // This command sets device i2c address
#define I2C_CMD_RST_ADDR		    			0xc1 // This command resets device i2c address
#define I2C_CMD_TEST_TX_RX_ON       			0xe0 // This command enable TX RX pin test mode
#define I2C_CMD_TEST_TX_RX_OFF      			0xe1 // This command disable TX RX pin test mode
#define I2C_CMD_TEST_GET_VER        			0xe2 // This command use to get software version
#define I2C_CMD_GET_DEVICE_UID      			0xf1 // This command use to get chip id

GroveLEDMatrix::GroveLEDMatrix()
{
}

void GroveLEDMatrix::init(uint8_t bus, uint8_t address, uint8_t delay)
{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() == QGpio::INIT_OK) {
        m_i2c = gpio->allocateI2CSlave(address, delay, bus, 40000);
        m_i2c->i2cWrite(I2C_CMD_LED_ON, nullptr, 0);
        m_i2c->i2cWrite(I2C_CMD_AUTO_SLEEP_OFF, nullptr, 0);
        m_i2c->i2cWrite(I2C_CMD_GET_DEV_ID, nullptr, 0);
        qWarning() << "VID:" << Qt::hex << m_i2c->i2cRead32(I2C_CMD_GET_DEV_ID);
    }
}

void GroveLEDMatrix::writeContinue(uint8_t reg, const void *buf, size_t len) {
    if (len == 0)
        return;
    m_i2c->i2cWrite(I2C_CMD_CONTINUE_DATA, nullptr, 0);
    m_i2c->i2cWrite(reg,  buf, len);
}

void GroveLEDMatrix::displayBar(uint8_t bar, uint16_t duration_time, bool forever_flag, uint8_t color) {
    uint8_t data[6] = {0, };


    if (bar > 32) {
        bar = 32;
    }
    data[0] = bar;
    data[1] = (uint8_t)(duration_time & 0xff);
    data[2] = (uint8_t)((duration_time >> 8) & 0xff);
    data[3] = (uint8_t)forever_flag;
    data[4] = color;
    m_i2c->i2cWrite(I2C_CMD_DISP_BAR, data, 5);
}

void GroveLEDMatrix::displayEmoji(uint8_t emoji, uint16_t duration_time, bool forever_flag) {
    uint8_t data[4] = {0, };

    data[0] = emoji;
    data[1] = (uint8_t)(duration_time & 0xff);
    data[2] = (uint8_t)((duration_time >> 8) & 0xff);
    data[3] = (uint8_t)forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_EMOJI, data, 4);
}

void GroveLEDMatrix::displayNumber(int16_t number, uint16_t duration_time, bool forever_flag,
                                              uint8_t color) {
    uint8_t data[6] = {0, };

    data[0] = (uint8_t)((uint16_t)number & 0xff);
    data[1] = (uint8_t)(((uint16_t)number >> 8) & 0xff);
    data[2] = (uint8_t)(duration_time & 0xff);
    data[3] = (uint8_t)((duration_time >> 8) & 0xff);
    data[4] = forever_flag;
    data[5] = color;

    m_i2c->i2cWrite(I2C_CMD_DISP_NUM, data, 6);
}

void GroveLEDMatrix::displayString(char* str, uint16_t duration_time, bool forever_flag, uint8_t color) {
    uint8_t data[36] = {0, }, len = strlen(str);

    if (len >= 28) {
        len = 28;
    }

    for (uint8_t i = 0; i < len; i++) {
        data[i + 5] = str[i];
    }

    data[0] = (uint8_t)forever_flag;
    data[1] = (uint8_t)(duration_time & 0xff);
    data[2] = (uint8_t)((duration_time >> 8) & 0xff);
    data[3] = len;
    data[4] = color;

    if (len > 25) {
        m_i2c->i2cWrite(I2C_CMD_DISP_STR, data, 31);
        writeContinue(I2C_CMD_DISP_STR,  data + 31, (len - 25));
    } else {
        m_i2c->i2cWrite(I2C_CMD_DISP_STR, data, (len + 5));
    }

}

void GroveLEDMatrix::displayFrames(uint8_t* buffer, uint16_t duration_time, bool forever_flag,
                                              uint8_t frames_number) {
    uint8_t data[72] = {0, };
    // max 5 frames in storage
    if (frames_number > 5) {
        frames_number = 5;
    } else if (frames_number == 0) {
        return;
    }

    data[0] = 0x0;
    data[1] = 0x0;
    data[2] = 0x0;
    data[3] = frames_number;

    for (int i = frames_number - 1; i >= 0; i--) {
        data[4] = i;
        for (int j = 0; j < 64; j++) {
            data[7 + j] = buffer[j + i * 64];
        }
        if (i == 0) {
            // display when everything is finished.
            data[0] = (uint8_t)(duration_time & 0xff);
            data[1] = (uint8_t)((duration_time >> 8) & 0xff);
            data[2] = (uint8_t)forever_flag;
        }
        m_i2c->i2cWrite(I2C_CMD_DISP_CUSTOM, data, 71);
    }
}

void GroveLEDMatrix::displayFrames(uint64_t* buffer, uint16_t duration_time, bool forever_flag,
                                              uint8_t frames_number) {
    uint8_t data[72] = {0, };
    // max 5 frames in storage
    if (frames_number > 5) {
        frames_number = 5;
    } else if (frames_number == 0) {
        return;
    }

    data[0] = 0x0;
    data[1] = 0x0;
    data[2] = 0x0;
    data[3] = frames_number;

    for (int i = frames_number - 1; i >= 0; i--) {
        data[4] = i;
        // different from uint8_t buffer
        for (int j = 0; j < 8; j++) {
            for (int k = 7; k >= 0; k--) {
                data[7 + j * 8 + (7 - k)] = ((uint8_t*)buffer)[j * 8 + k + i * 64];
            }
        }

        if (i == 0) {
            // display when everything is finished.
            data[0] = (uint8_t)(duration_time & 0xff);
            data[1] = (uint8_t)((duration_time >> 8) & 0xff);
            data[2] = (uint8_t)forever_flag;
        }
        m_i2c->i2cWrite(I2C_CMD_DISP_CUSTOM, data, frames_number*64+5);
    }
}

void GroveLEDMatrix::stopDisplay(void) {
    m_i2c->i2cWrite(0, (u_int8_t)I2C_CMD_DISP_OFF);
}

void GroveLEDMatrix::storeFrames(void) {
    m_i2c->i2cWrite(0, (u_int8_t)I2C_CMD_STORE_FLASH);
}

void GroveLEDMatrix::deleteFrames(void) {
    m_i2c->i2cWrite(0, (u_int8_t)I2C_CMD_DELETE_FLASH);
}


void GroveLEDMatrix::displayFramesFromFlash(uint16_t duration_time, bool forever_flag, uint8_t from,
                                                       uint8_t to) {
    uint8_t data[5] = {0, };

    uint8_t temp = 0;
    // 1 <= from <= to <= 5
    if (from < 1) {
        from = 1;
    } else if (from > 5) {
        from = 5;
    }

    if (to < 1) {
        to = 1;
    } else if (to > 5) {
        to = 5;
    }

    if (from > to) {
        temp = from;
        from = to;
        to = temp;
    }

    data[0] = (uint8_t)(duration_time & 0xff);
    data[1] = (uint8_t)((duration_time >> 8) & 0xff);
    data[2] = (uint8_t)forever_flag;
    data[3] = from - 1;
    data[4] = to - 1;

    m_i2c->i2cWrite(I2C_CMD_DISP_FLASH, data, 5);
}

void GroveLEDMatrix::displayColorBlock(uint32_t rgb, uint16_t duration_time, bool forever_flag) {
    uint8_t data[6] = {0, };
    // red green blue
    data[0] = (uint8_t)((rgb >> 16) & 0xff);
    data[1] = (uint8_t)((rgb >> 8) & 0xff);
    data[2] = (uint8_t)(rgb & 0xff);

    data[3] = (uint8_t)(duration_time & 0xff);
    data[4] = (uint8_t)((duration_time >> 8) & 0xff);
    data[5] = (uint8_t)forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_COLOR_BLOCK, data, 6);
}


void GroveLEDMatrix::displayColorBar(uint8_t bar, uint16_t duration_time, bool forever_flag) {
    uint8_t data[4] = {0, };

    if (bar > 32) {
        bar = 32;
    }
    data[0] = bar;
    data[1] = (uint8_t)(duration_time & 0xff);
    data[2] = (uint8_t)((duration_time >> 8) & 0xff);
    data[3] = (uint8_t)forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_COLOR_BAR, data, 4);
}


void GroveLEDMatrix::displayColorWave(uint8_t color, uint16_t duration_time, bool forever_flag) {
    uint8_t data[4] = {0, };

    data[0] = color;
    data[1] = (uint8_t)(duration_time & 0xff);
    data[2] = (uint8_t)((duration_time >> 8) & 0xff);
    data[3] = (uint8_t)forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_COLOR_WAVE, data, 4);
}


void GroveLEDMatrix::displayClockwise(bool is_cw, bool is_big, uint16_t duration_time, bool forever_flag) {
    uint8_t data[5] = {0, };
    data[0] = (uint8_t)is_cw;
    data[1] = (uint8_t)is_big;
    data[2] = (uint8_t)(duration_time & 0xff);
    data[3] = (uint8_t)((duration_time >> 8) & 0xff);
    data[4] = (uint8_t)forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_COLOR_CLOCKWISE, data, 5);
}

void GroveLEDMatrix::displayColorAnimation(uint8_t index, uint16_t duration_time, bool forever_flag) {
    uint8_t data[5] = {0, };
    uint8_t from = 0, to = 0;

    switch (index) {
    case 0:
        from = 0;
        to = 28;
        break;

    case 1:
        from = 29;
        to = 41;
        break;

    case 2:				// rainbow cycle
        from = 255;
        to = 255;
        break;

    case 3: 			// fire
        from = 254;
        to = 254;
        break;

    case 4: 			// walking
        from = 42;
        to = 43;
        break;

    case 5:				// broken heart
        from = 44;
        to = 52;
        break;

    default:
        break;
    }

    data[0] = from;
    data[1] = to;
    data[2] = (uint8_t)(duration_time & 0xff);
    data[3] = (uint8_t)((duration_time >> 8) & 0xff);
    data[4] = forever_flag;

    m_i2c->i2cWrite(I2C_CMD_DISP_COLOR_ANIMATION, data, 5);
}

