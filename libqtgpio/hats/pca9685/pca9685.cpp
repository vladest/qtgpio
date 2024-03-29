#include <stdint.h>
#if !defined(NDEBUG) || defined(__linux__)
 #include <stdio.h>
#endif
#include <assert.h>

#include "pca9685.h"
#include "qgpio.h"

#include <QDebug>

extern "C" {
 extern void bcm2835_delayMicroseconds (uint64_t);
 #define udelay bcm2835_delayMicroseconds
}

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))

#define PCA9685_OSC_FREQ 25000000L

enum TPCA9685Reg {
    PCA9685_REG_MODE1 = 0x00,
    PCA9685_REG_MODE2 = 0x01,
    PCA9685_REG_ALLCALLADR = 0x05,
    PCA9685_REG_LED0_ON_L = 0x06,
    PCA9685_REG_LED0_ON_H = 0x07,
    PCA9685_REG_LED0_OFF_L = 0x08,
    PCA9685_REG_LED0_OFF_H = 0x09,
    PCA9685_REG_ALL_LED_ON_L = 0xFA,
    PCA9685_REG_ALL_LED_ON_H = 0xFB,
    PCA9685_REG_ALL_LED_OFF_L = 0xFC,
    PCA9685_REG_ALL_LED_OFF_H = 0xFD,
    PCA9685_REG_PRE_SCALE = 0xFE
};

#define PCA9685_PRE_SCALE_MIN	0x03
#define PCA9685_PRE_SCALE_MAX	0xFF

/*
 * 7.3.1 Mode register 1, MODE1
 */
enum TPCA9685Mode1 {
    PCA9685_MODE1_ALLCALL = 1 << 0,
    PCA9685_MODE1_SUB3 = 1 << 1,
    PCA9685_MODE1_SUB2 = 1 << 2,
    PCA9685_MODE1_SUB1 = 1 << 3,
    PCA9685_MODE1_SLEEP = 1 << 4,
    PCA9685_MODE1_AI = 1 << 5,
    PCA9685_MODE1_EXTCLK = 1 << 6,
    PCA9685_MODE1_RESTART = 1 << 7
};

/*
 * 7.3.2 Mode register 2, MODE2
 */
enum TPCA9685Mode2 {
    PCA9685_MODE2_OUTDRV = 1 << 2,
    PCA9685_MODE2_OCH = 1 << 3,
    PCA9685_MODE2_INVRT = 1 << 4
};

PCA9685::PCA9685(uint8_t i2cBus, uint8_t nAddress) /*: m_nAddress(nAddress) */{
    QGpio* gpio = QGpio::getInstance();
    if (gpio->init() != QGpio::INIT_OK) {
        printf("Error init QGpio\n");
        return;
    }

    //TODO: setup i2c bus number
    m_i2c = gpio->allocateI2CSlave(nAddress, 1, i2cBus, 40000);

    AutoIncrement(true);

    for (uint8_t i = 0; i < 16; i ++) {
        Write(i, (uint16_t) 0, (uint16_t) 0x1000);
    }

    Sleep(false);
}

PCA9685::~PCA9685(void) {
}

void PCA9685::Sleep(bool bMode) {
    uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE1);

    Data &= ~PCA9685_MODE1_SLEEP;

    if (bMode) {
        Data |= PCA9685_MODE1_SLEEP;
    }

    m_i2c->i2cWrite(PCA9685_REG_MODE1, Data);

    if (Data & ~PCA9685_MODE1_RESTART) {
        udelay(500);
        Data |= PCA9685_MODE1_RESTART;
    }
}

void PCA9685::SetPreScaller(uint8_t nPrescale) {
    nPrescale = nPrescale < PCA9685_PRE_SCALE_MIN ? PCA9685_PRE_SCALE_MIN : nPrescale;

    Sleep(true);
    m_i2c->i2cWrite(PCA9685_REG_PRE_SCALE, nPrescale);
    Sleep(false);
}

uint8_t PCA9685::GetPreScaller(void) {
    return m_i2c->i2cRead(PCA9685_REG_PRE_SCALE);
}

void PCA9685::SetFrequency(uint16_t nFreq) {
    SetPreScaller(CalcPresScale(nFreq));
}

uint16_t PCA9685::GetFrequency(void) {
    return CalcFrequency(GetPreScaller());
}

void PCA9685::SetOCH(TPCA9685Och enumTPCA9685Och) {
    uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2);

    Data &= ~PCA9685_MODE2_OCH;

    if (enumTPCA9685Och == PCA9685_OCH_ACK) {
        Data |= PCA9685_OCH_ACK;
    } // else, default Outputs change on STOP command

    m_i2c->i2cWrite(PCA9685_REG_MODE2, Data);
}

TPCA9685Och PCA9685::GetOCH(void) {
    const uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2) & PCA9685_MODE2_OCH;

    return (TPCA9685Och) Data;
}

void PCA9685::SetInvert(bool bInvert) {
    uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2);

    Data &= ~PCA9685_MODE2_INVRT;

    if (bInvert) {
        Data |= PCA9685_MODE2_INVRT;
    }

    m_i2c->i2cWrite(PCA9685_REG_MODE2, Data);
}

bool PCA9685::GetInvert(void) {
    const uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2) & PCA9685_MODE2_INVRT;

    return (Data == PCA9685_MODE2_INVRT);
}

void PCA9685::SetOutDriver(bool bOutDriver) {
    uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2);

    Data &= ~PCA9685_MODE2_OUTDRV;

    if (bOutDriver) {
        Data |= PCA9685_MODE2_OUTDRV;
    }

    m_i2c->i2cWrite(PCA9685_REG_MODE2, Data);
}

bool PCA9685::GetOutDriver(void) {
    const uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE2) & PCA9685_MODE2_OUTDRV;

    return (Data == PCA9685_MODE2_OUTDRV);
}

void PCA9685::Write(uint8_t nChannel, uint16_t nOn, uint16_t nOff) {
    uint8_t reg;

    if (nChannel <= 15) {
        reg = PCA9685_REG_LED0_ON_L + (nChannel << 2);
    } else {
        reg = PCA9685_REG_ALL_LED_ON_L;
    }

    m_i2c->i2cWrite(reg, nOn, nOff);
}

void PCA9685::Write(uint8_t nChannel, uint16_t nValue) {
    Write (nChannel, (uint16_t) 0, nValue);
}

void PCA9685::Write(uint16_t nOn, uint16_t nOff) {
    Write((uint8_t) 16, nOn, nOff);
}

void PCA9685::Write(uint16_t nValue) {
    Write((uint8_t) 16, nValue);
}

void PCA9685::Read(uint8_t nChannel, uint16_t *pOn, uint16_t *pOff) {
    assert(pOn != 0);
    assert(pOff != 0);

    uint8_t reg;

    if (nChannel <= 15) {
        reg = PCA9685_REG_LED0_ON_L + (nChannel << 2);
    } else {
        reg = PCA9685_REG_ALL_LED_ON_L;
    }

    if (pOn != 0) {
        *pOn = m_i2c->i2cRead16(reg);
    }

    if (pOff) {
        *pOff = m_i2c->i2cRead16(reg + 2);
    }
}

void PCA9685::Read(uint16_t *pOn, uint16_t *pOff) {
    Read((uint8_t) 16, pOn, pOff);
}

void PCA9685::SetFullOn(uint8_t nChannel, bool bMode) {
    uint8_t reg;

    if (nChannel <= 15) {
        reg = PCA9685_REG_LED0_ON_H + (nChannel << 2);
    } else {
        reg = PCA9685_REG_ALL_LED_ON_H;
    }

    uint8_t Data = m_i2c->i2cRead(reg);

    Data = bMode ? (Data | 0x10) : (Data & 0xEF);

    m_i2c->i2cWrite(reg, Data);

    if (bMode) {
        SetFullOff(nChannel, false);
    }

}

void PCA9685::SetFullOff(uint8_t nChannel, bool bMode) {
    uint8_t reg;

    if (nChannel <= 15) {
        reg = PCA9685_REG_LED0_OFF_H + (nChannel << 2);
    } else {
        reg = PCA9685_REG_ALL_LED_OFF_H;
    }

    uint8_t Data = m_i2c->i2cRead(reg);

    Data = bMode ? (Data | 0x10) : (Data & 0xEF);

    m_i2c->i2cWrite(reg, Data);
}

uint8_t PCA9685::CalcPresScale(uint16_t nFreq) {
    nFreq = (nFreq > PCA9685_FREQUENCY_MAX ? PCA9685_FREQUENCY_MAX : (nFreq < PCA9685_FREQUENCY_MIN ? PCA9685_FREQUENCY_MIN : nFreq));

    const float f = (float) PCA9685_OSC_FREQ / 4096;

    const uint8_t Data = (uint8_t) DIV_ROUND_UP(f, nFreq) - 1;

    return Data;
}

uint16_t PCA9685::CalcFrequency(uint8_t nPreScale) {
    uint16_t f_min;
    uint16_t f_max;
    const float f = (float) PCA9685_OSC_FREQ / 4096;
    const uint16_t Data = (uint16_t) DIV_ROUND_UP(f, ((uint16_t) nPreScale + 1));

    for (f_min = Data; f_min > PCA9685_FREQUENCY_MIN; f_min--) {
        if (CalcPresScale(f_min) != nPreScale) {
            break;
        }
    }

    for (f_max = Data; f_max < PCA9685_FREQUENCY_MAX; f_max++) {
        if (CalcPresScale(f_max) != nPreScale) {
            break;
        }
    }

    return (f_max + f_min) / 2;
}

void PCA9685::Dump(void) {
    uint8_t reg = m_i2c->i2cRead(PCA9685_REG_MODE1);

    printf("MODE1 - Mode register 1 (address 00h) : %02Xh\n", reg);
    printf("\tbit 7 - RESTART : Restart %s\n", reg & PCA9685_MODE1_RESTART ? "enabled" : "disabled");
    printf("\tbit 6 - EXTCLK  : %s\n", reg & PCA9685_MODE1_EXTCLK ? "Use EXTCLK pin clock" : "Use internal clock");
    printf("\tbit 5 - AI      : Register Auto-Increment %s\n", reg & PCA9685_MODE1_AI ? "enabled" : "disabled");
    printf("\tbit 4 - SLEEP   : %s\n", reg & PCA9685_MODE1_SLEEP ? "Low power mode. Oscillator off" : "Normal mode");
    printf("\tbit 3 - SUB1    : PCA9685 %s to I2C-bus subaddress 1\n", reg & PCA9685_MODE1_SUB1 ? "responds" : "does not respond");
    printf("\tbit 2 - SUB1    : PCA9685 %s to I2C-bus subaddress 2\n", reg & PCA9685_MODE1_SUB2 ? "responds" : "does not respond");
    printf("\tbit 1 - SUB1    : PCA9685 %s to I2C-bus subaddress 3\n", reg & PCA9685_MODE1_SUB3 ? "responds" : "does not respond");
    printf("\tbit 0 - ALLCALL : PCA9685 %s to LED All Call I2C-bus address\n", reg & PCA9685_MODE1_ALLCALL ? "responds" : "does not respond");

    reg = m_i2c->i2cRead(PCA9685_REG_MODE2);

    printf("\nMODE2 - Mode register 2 (address 01h) : %02Xh\n", reg);
    printf("\tbit 7 to 5      : Reserved\n");
    printf("\tbit 4 - INVRT   : Output logic state %sinverted\n", reg & PCA9685_MODE2_INVRT ? "" : "not ");
    printf("\tbit 3 - OCH     : Outputs change on %s\n", reg & PCA9685_MODE2_OCH ? "ACK" : "STOP command");
    printf("\tbit 2 - OUTDRV  : The 16 LEDn outputs are configured with %s structure\n", reg & PCA9685_MODE2_OUTDRV ? "a totem pole" : "an open-drain");
    printf("\tbit 10- OUTNE   : %01x\n", reg & 0x3);

    reg = m_i2c->i2cRead(PCA9685_REG_PRE_SCALE);

    printf("\nPRE_SCALE register (address FEh) : %02Xh\n", reg);
    printf("\t Frequency : %d Hz\n", CalcFrequency(reg));

    printf("\n");

    uint16_t on, off;

    for (uint8_t nLed = 0; nLed <= 15; nLed ++) {
        Read(nLed, &on, &off);
        printf("LED%d_ON  : %04x\n", nLed, on);
        printf("LED%d_OFF : %04x\n", nLed, off);
    }

    printf("\n");

    Read(16, &on, &off);
    printf("ALL_LED_ON  : %04x\n", on);
    printf("ALL_LED_OFF : %04x\n", off);

}

void PCA9685::AutoIncrement(bool bMode) {
    uint8_t Data = m_i2c->i2cRead(PCA9685_REG_MODE1);

    Data &= ~PCA9685_MODE1_AI;	// 0 Register Auto-Increment disabled. {default}

    if (bMode) {
        Data |= PCA9685_MODE1_AI;	// 1 Register Auto-Increment enabled.
    }

    m_i2c->i2cWrite(PCA9685_REG_MODE1, Data);
}

