#ifndef RPICPUINFO_H
#define RPICPUINFO_H

#include <QString>

class RpiCpuInfo
{
public:

    enum RaspberryPiSoCs {
        SOC_UNDEFINED,
        SOC_BCM2708,
        SOC_BCM2709,
        SOC_BCM2835,
        SOC_BCM2836,
        SOC_BCM2837
    };

    enum RaspberryPiType {
        RPI_UNKNOWN = -1,
        RPI_MODEL_A = 0,
        RPI_MODEL_B,
        RPI_MODEL_A_PLUS,
        RPI_MODEL_B_PLUS,
        RPI_PI2_MODEL_B,
        RPI_ALPHA,
        RPI_CM,
        RPI_PI3_MODEL_B,
        RPI_ZERO
    };

    enum RaspberryPiManufacturer {
        MANUFACTURER_UNKNOWN,
        MANUFACTURER_SONY,
        MANUFACTURER_QISDA,
        MANUFACTURER_EGOMAN,
        MANUFACTURER_EMBEST
    };

    RpiCpuInfo();
    QString boardString();
    int getP1Revision() const;
    int getRam() const;
    RaspberryPiManufacturer getManufacturer() const;
    RaspberryPiSoCs getSoc() const;
    RaspberryPiType getType() const;

private:
    void parseCpuInfo();

private:
    int revision = 0;
    int p1Revision = 0;
    int ram = 0;
    RaspberryPiManufacturer manufacturer = MANUFACTURER_UNKNOWN;
    RaspberryPiSoCs soc = SOC_UNDEFINED;
    RaspberryPiType type = RPI_UNKNOWN;
};

#endif // RPICPUINFO_H
