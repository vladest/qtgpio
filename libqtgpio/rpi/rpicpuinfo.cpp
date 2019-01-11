#include "rpicpuinfo.h"

#include <QFile>
#include <QDebug>

RpiCpuInfo::RpiCpuInfo()
{
    parseCpuInfo();
}

QString RpiCpuInfo::boardString()
{
    QString s_type;
    QString s_manufacturer;
    QString s_soc;
    switch (soc) {
    case RpiCpuInfo::SOC_UNDEFINED: s_soc = QStringLiteral("Unknown"); break;
    case SOC_BCM2708: s_soc = QStringLiteral("BCM2708"); break;
    case SOC_BCM2709: s_soc = QStringLiteral("BCM2709"); break;
    case SOC_BCM2835: s_soc = QStringLiteral("BCM2835"); break;
    case SOC_BCM2836: s_soc = QStringLiteral("BCM2836"); break;
    case SOC_BCM2837: s_soc = QStringLiteral("BCM2837"); break;
    }
    switch (type) {
    case RpiCpuInfo::RPI_UNKNOWN: s_type = QStringLiteral("Unknown"); break;
    case RPI_CM: s_type = QStringLiteral("Computer Module"); break;
    case RPI_ZERO: s_type = QStringLiteral("Zero"); break;
    case RPI_ALPHA: s_type = QStringLiteral("Alpha"); break;
    case RPI_MODEL_A: s_type = QStringLiteral("Model A"); break;
    case RPI_MODEL_B: s_type = QStringLiteral("Model B"); break;
    case RPI_MODEL_A_PLUS: s_type = QStringLiteral("Model A+"); break;
    case RPI_MODEL_B_PLUS: s_type = QStringLiteral("Model B+"); break;
    case RPI_PI2_MODEL_B: s_type = QStringLiteral("Pi 2 Model B"); break;
    case RPI_PI3_MODEL_B: s_type = QStringLiteral("Pi 3 Model B"); break;
    }
    switch (manufacturer) {
    case RpiCpuInfo::MANUFACTURER_UNKNOWN: s_manufacturer = QStringLiteral("Unknown"); break;
    case MANUFACTURER_SONY: s_manufacturer = QStringLiteral("Sony"); break;
    case MANUFACTURER_QISDA: s_manufacturer = QStringLiteral("Qisda"); break;
    case MANUFACTURER_EGOMAN: s_manufacturer = QStringLiteral("Egoman"); break;
    case MANUFACTURER_EMBEST: s_manufacturer = QStringLiteral("Embest"); break;
    }
    return QStringLiteral("Raspberry Pi board. SoC: %1, Type: %2, Manufacturer: %3, RAM size: %4M, P1 rev. %5")
            .arg(s_soc).arg(s_type).arg(s_manufacturer).arg(ram).arg(p1Revision);
}


/*

32 bits
NEW                   23: will be 1 for the new scheme, 0 for the old scheme
MEMSIZE             20: 0=256M 1=512M 2=1G
MANUFACTURER  16: 0=SONY 1=EGOMAN
PROCESSOR         12: 0=2835 1=2836
TYPE                   04: 0=MODELA 1=MODELB 2=MODELA+ 3=MODELB+ 4=Pi2 MODEL B 5=ALPHA 6=CM
REV                     00: 0=REV0 1=REV1 2=REV2

pi2 = 1<<23 | 2<<20 | 1<<12 | 4<<4 = 0xa01040

--------------------

SRRR MMMM PPPP TTTT TTTT VVVV

S scheme (0=old, 1=new)
R RAM (0=256, 1=512, 2=1024)
M manufacturer (0='SONY',1='EGOMAN',2='EMBEST',3='UNKNOWN',4='EMBEST')
P processor (0=2835, 1=2836 2=2837)
T type (0='A', 1='B', 2='A+', 3='B+', 4='Pi 2 B', 5='Alpha', 6='Compute Module')
V revision (0-15)

*/

void RpiCpuInfo::parseCpuInfo()
{
    QFile _f("/proc/cpuinfo");
    if (_f.open(QIODevice::ReadOnly)) {
        const QByteArray& ba = _f.readAll();
        const QList<QByteArray>& lines = ba.split('\n');
        for (auto line : lines) {
            const QList<QByteArray>& splittedLine = line.split(':');
            if (splittedLine.size() == 2) {
                if (splittedLine.at(0).contains("Revision")) {
                    bool ok = false;
                    quint32 rev = splittedLine.at(1).trimmed().toUInt(&ok, 16);
                    if (ok == true) {
                        bool new_scheme = rev & (1 << 23);
                        revision = rev & 0xf;
                        if (new_scheme) {
                            quint32 _type = (rev >> 4) & 0xff;
                            quint32 _soc = (rev >> 12) & 0xf;
                            quint32 _manuf = (rev >> 16) & 0xf;
                            quint32 _ram = (rev >> 20) & 0x7;
                            switch (_type) {
                            case 0: type = RPI_MODEL_A; p1Revision = 2; break;
                            case 1: type = RPI_MODEL_B; p1Revision = 2; break;
                            case 2: type = RPI_MODEL_A_PLUS; p1Revision = 3; break;
                            case 3: type = RPI_MODEL_B_PLUS; p1Revision = 3; break;
                            case 4: type = RPI_PI2_MODEL_B; p1Revision = 3; break;
                            case 5: type = RPI_ALPHA;p1Revision = 3; break;
                            case 6: type = RPI_CM; p1Revision = 0; break;
                            case 8: type = RPI_PI3_MODEL_B; p1Revision = 3; break;
                            case 9: type = RPI_ZERO; p1Revision = 3; break;
                            default : p1Revision = 3; break;
                            }
                            switch (_soc) {
                            case 0: soc = SOC_BCM2835; break;
                            case 1: soc = SOC_BCM2836; break;
                            case 2: soc = SOC_BCM2837; break;
                            }
                            switch (_manuf) {
                            case 0: manufacturer = MANUFACTURER_SONY; break;
                            case 1: manufacturer = MANUFACTURER_EGOMAN; break;
                            case 2: manufacturer = MANUFACTURER_EMBEST; break;
                            case 4: manufacturer = MANUFACTURER_EMBEST; break;
                            }
                            switch (_ram) {
                            case 0: ram = 256; break;
                            case 1: ram = 512; break;
                            case 2: ram = 1024; break;
                            }

                        } else {
                            if (rev == 0x2 || rev == 0x3) {
                                type = RPI_MODEL_B;
                                p1Revision = 1;
                                ram = 256;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x4) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_SONY;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x5) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_QISDA;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x6) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_EGOMAN;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x7) {
                                type = RPI_MODEL_A;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_EGOMAN;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x8) {
                                type = RPI_MODEL_A;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_SONY;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x9) {
                                type = RPI_MODEL_A;
                                p1Revision = 2;
                                ram = 256;
                                manufacturer = MANUFACTURER_QISDA;
                                soc = SOC_BCM2835;
                            } else if (rev == 0xd) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 512;
                                manufacturer = MANUFACTURER_EGOMAN;
                                soc = SOC_BCM2835;
                            } else if (rev == 0xe) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 512;
                                manufacturer = MANUFACTURER_SONY;
                                soc = SOC_BCM2835;
                            } else if (rev == 0xf) {
                                type = RPI_MODEL_B;
                                p1Revision = 2;
                                ram = 512;
                                manufacturer = MANUFACTURER_QISDA;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x11 || rev == 0x14) {
                                type = RPI_CM;
                                p1Revision = 0;
                                ram = 512;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x12) {
                                type = RPI_MODEL_A_PLUS;
                                p1Revision = 3;
                                ram = 256;
                                soc = SOC_BCM2835;
                            } else if (rev == 0x10 || rev == 0x13) {
                                type = RPI_MODEL_B_PLUS;
                                p1Revision = 3;
                                ram = 512;
                                soc = SOC_BCM2835;
                            } else {  // don't know - assume revision 3 p1 connector
                                p1Revision = 3;
                            }

                        }
                    }
                } else if (splittedLine.at(0).contains("Hardware")) {
                    const QByteArray& hardware = splittedLine.at(1).trimmed();
                    if (hardware.contains("BCM2708"))
                        soc = SOC_BCM2708;
                    if (hardware.contains("BCM2709"))
                        soc = SOC_BCM2709;
                    if (hardware.contains(SOC_BCM2835))
                        soc = SOC_BCM2835;
                    if (hardware.contains("BCM2836"))
                        soc = SOC_BCM2836;
                    if (hardware.contains("BCM2837"))
                        soc = SOC_BCM2837;
                }
            }
        }
        _f.close();
    } else {
        qWarning() << "Error opening /proc/cpuinfo";
    }
}

RpiCpuInfo::RaspberryPiType RpiCpuInfo::getType() const
{
    return type;
}

RpiCpuInfo::RaspberryPiSoCs RpiCpuInfo::getSoc() const
{
    return soc;
}

RpiCpuInfo::RaspberryPiManufacturer RpiCpuInfo::getManufacturer() const
{
    return manufacturer;
}

int RpiCpuInfo::getRam() const
{
    return ram;
}

int RpiCpuInfo::getP1Revision() const
{
    return p1Revision;
}
