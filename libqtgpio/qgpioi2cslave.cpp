#include "qgpioi2cslave.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <QDebug>
#include <QThread>

/* I2C default delay */
#define I2C_DEFAULT_DELAY 1

/* I2C internal address max length */
#define INT_ADDR_MAX_BYTES 4

/* I2C page max bytes */
#define PAGE_MAX_BYTES 4096

#define GET_I2C_DELAY(delay) ((delay) == 0 ? I2C_DEFAULT_DELAY : (delay))
#define GET_I2C_FLAGS(tenbit, flags) ((tenbit) ? ((flags) | I2C_M_TEN) : (flags))
#define GET_WRITE_SIZE(addr, remain, page_bytes) ((addr) + (remain) > (page_bytes) ? (page_bytes) - (addr) : remain)
//extern "C" {
// extern void bcm2835_delayMicroseconds (uint64_t);
// #define udelay bcm2835_delayMicroseconds
//}

/*
**	@brief	:	i2c delay
**	#msec	:	milliscond to be delay
*/
static void i2c_delay(unsigned char msec)
{
    usleep(msec * 1e3);
}

// maximum counter for read repeats if NACK
static const int kMaxCount = 10;

QGpioI2CSlave::QGpioI2CSlave(uint8_t address, uint8_t delay, uint8_t busNum, uint16_t timeout) :
      m_address(address),
      m_busNum(busNum),
      m_delay(delay),
      m_timeout(timeout)
{
    /* Open i2c-bus devcice */
    m_fd = open(QString("/dev/i2c-%1").arg(m_busNum).toLatin1().data(), O_RDWR);
    if (m_fd == -1) {
        qWarning() << "Error opening i2C file" << QString("/dev/i2c-%1").arg(m_busNum) << "error:" << errno;
    }
}

QGpioI2CSlave::~QGpioI2CSlave()
{
    if (m_fd != -1) {
        close(m_fd);
        m_fd = -1;
    }
}

uint8_t QGpioI2CSlave::address() const
{
    return m_address;
}

void QGpioI2CSlave::setGpioParent(QGpio *gpio)
{
    m_gpio = gpio;
}

void QGpioI2CSlave::i2cSetup()
{
    int ret = ioctl(m_fd, I2C_SLAVE_FORCE, m_address);
    if (ret < 0) {
        fprintf(stderr, "I2C ioctl(I2C_SLAVE_FORCE) error %d\n", errno);
    }
    QThread::usleep(500);

//    bcm2835_i2c_setClockDivider(m_clockDivider);

//    // sets read timeout
//    volatile uint32_t* stimeout = bcm2835_bsc1 + BCM2835_BSC_CLKT / 4;
//    bcm2835_peri_write(stimeout, m_timeout);
//    bcm2835_i2c_setSlaveAddress(m_address);
    //udelay(500);
}

    /**
     *
**	i2c_ioctl_read/write
**	I2C bus top layer interface to operation different i2c devide
**	This function will call XXX:ioctl system call and will be related
**	i2c-dev.c i2cdev_ioctl to operation i2c device.
**	1. it can choice ignore or not ignore i2c bus ack signal (flags set I2C_M_IGNORE_NAK)
**	2. it can choice ignore or not ignore i2c internal address
**
    */
ssize_t QGpioI2CSlave::ioctlReadHelper(unsigned int iaddr, void *buf, size_t len)
{
    struct i2c_msg ioctl_msg[2];
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    unsigned short flags = GET_I2C_FLAGS(m_tenBitsAddress, m_ioctlFlags);

    memset(addr, 0, sizeof(addr));
    memset(ioctl_msg, 0, sizeof(ioctl_msg));
    memset(&ioctl_data, 0, sizeof(ioctl_data));

    /* Target have internal address */
    if (m_internalAddrBytes != 0) {

        internalAddressConvert(iaddr, m_internalAddrBytes, addr);

        /* First message is write internal address */
        ioctl_msg[0].len	=	m_internalAddrBytes;
        ioctl_msg[0].addr	= 	m_address;
        ioctl_msg[0].buf	= 	addr;
        ioctl_msg[0].flags	= 	flags;

        /* Second message is read data */
        ioctl_msg[1].len	= 	len;
        ioctl_msg[1].addr	= 	m_address;
        ioctl_msg[1].buf	=	static_cast<uint8_t*>(buf);
        ioctl_msg[1].flags	=	m_ioctlFlags | I2C_M_RD;

        /* Package to i2c message to operation i2c device */
        ioctl_data.nmsgs	=	2;
        ioctl_data.msgs		=	ioctl_msg;
    }
    /* Target did not have internal address */
    else {

        /* Direct send read data message */
        ioctl_msg[0].len	= 	len;
        ioctl_msg[0].addr	= 	m_address;
        ioctl_msg[0].buf	=	static_cast<uint8_t*>(buf);
        ioctl_msg[0].flags	=	m_ioctlFlags | I2C_M_RD;

        /* Package to i2c message to operation i2c device */
        ioctl_data.nmsgs	=	1;
        ioctl_data.msgs		=	ioctl_msg;
    }

    /* Using ioctl interface operation i2c device */
    if (ioctl(m_fd, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {

        qWarning("Ioctl read i2c error:");
        return -1;
    }

    return len;
}


ssize_t QGpioI2CSlave::ioctlWriteHelper(unsigned int iaddr, const void *buf, size_t len)
{
    // take in account commands w/o data
    ssize_t remain = len == 0 ? 1 : len;
    size_t size = 0, cnt = 0;
    const unsigned char *buffer = static_cast<const unsigned char *>(buf);
    unsigned char delay = GET_I2C_DELAY(m_delay);
    unsigned short flags = GET_I2C_FLAGS(m_tenBitsAddress, m_ioctlFlags);

    struct i2c_msg ioctl_msg;
    struct i2c_rdwr_ioctl_data ioctl_data;
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    while (remain > 0) {

        size = GET_WRITE_SIZE(iaddr % m_bytesPerPage, remain, m_bytesPerPage);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        internalAddressConvert(iaddr, m_internalAddrBytes, tmp_buf);

        /* Connect write data after device internal address */
        if (buffer != nullptr) { // check if just a command should be sent
            memcpy(tmp_buf + m_internalAddrBytes, buffer, size);
        }

        /* Fill kernel ioctl i2c_msg */
        memset(&ioctl_msg, 0, sizeof(ioctl_msg));
        memset(&ioctl_data, 0, sizeof(ioctl_data));

        ioctl_msg.len	=	m_internalAddrBytes + size;
        ioctl_msg.addr	=	m_address;
        ioctl_msg.buf	=	tmp_buf;
        ioctl_msg.flags	=	flags;

        ioctl_data.nmsgs =	1;
        ioctl_data.msgs	=	&ioctl_msg;

        if (ioctl(m_fd, I2C_RDWR, (unsigned long)&ioctl_data) == -1) {
            perror("Ioctl write i2c error");
            return -1;
        }

        /* XXX: Must have a little time delay */
        i2c_delay(delay);

        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


/*
**	@brief	:	read #len bytes data from #device #iaddr to #buf
**	#iaddr	:	i2c_device internal address will read data from this address, no address set zero
**	#buf	:	i2c data will read to here
**	#len	:	how many data to read, lenght must less than or equal to buf size
**	@return : 	success return read data length, failed -1
*/
ssize_t QGpioI2CSlave::readHelper(unsigned int iaddr, void *buf, size_t len)
{
    ssize_t cnt;
    unsigned char addr[INT_ADDR_MAX_BYTES];
    unsigned char delay = GET_I2C_DELAY(m_delay);

    /* Set i2c slave address */
    if (addressSelect(m_address, m_tenBitsAddress) == false) {
        qWarning("readHelper: cant select address: %x %x", m_address, m_tenBitsAddress);
        return -1;
    }

    /* Convert i2c internal address */
    memset(addr, 0, sizeof(addr));
    internalAddressConvert(iaddr, m_internalAddrBytes, addr);

    /* Write internal address to devide  */
    if (write(m_fd, addr, m_internalAddrBytes) != m_internalAddrBytes) {
        qWarning("Write i2c internal address error");
        return -1;
    }

    /* Wait a while */
    i2c_delay(delay);

    /* Read count bytes data from int_addr specify address */
    if ((cnt = read(m_fd, buf, len)) == -1) {

        qWarning() << "Read i2c data error" << errno << "from file id" << m_fd;
        return -1;
    }

    return cnt;
}


/*
**	@brief	:	write #buf data to i2c #device #iaddr address
**	#device	:	I2CDevice struct, must call i2c_device_init first
**	#iaddr	: 	i2c_device internal address, no address set zero
**	#buf	:	data will write to i2c device
**	#len	:	buf data length without '/0'
**	@return	: 	success return write data length, failed -1
*/
ssize_t   __attribute__ ((optimize(0))) QGpioI2CSlave::writeHelper(unsigned int iaddr, const void *buf, size_t len)
{
    // take in account commands w/o data
    ssize_t remain = len == 0 ? 1 : len;
    ssize_t ret;
    size_t cnt = 0, size = 0;
    const unsigned char *buffer = static_cast<const unsigned char *>(buf);
    unsigned char delay = GET_I2C_DELAY(m_delay);
    unsigned char tmp_buf[PAGE_MAX_BYTES + INT_ADDR_MAX_BYTES];

    /* Set i2c slave address */
    if (addressSelect(m_address, m_tenBitsAddress) == false) {
        return -1;
    }

    /* Once only can write less than 4 byte */
    while (remain > 0) {
        size = GET_WRITE_SIZE(iaddr % m_bytesPerPage, remain, m_bytesPerPage);

        /* Convert i2c internal address */
        memset(tmp_buf, 0, sizeof(tmp_buf));
        internalAddressConvert(iaddr, m_internalAddrBytes, tmp_buf);

        /* Copy data to tmp_buf */
        if (buffer != nullptr) { // check if just a command should be sent
            memcpy(tmp_buf + m_internalAddrBytes, buffer, size);
        }

        /* Write to buf content to i2c device length  is address length and
                write buffer length */
        ret = write(m_fd, tmp_buf, m_internalAddrBytes + size);
        if (ret == -1 || (size_t)ret != m_internalAddrBytes + size) {
            if (ret == -1) {
                perror("write i2c error");
            }
            qWarning() << "I2C write error to iaddress" << iaddr << ret;
            return -1;
        }

        /* XXX: Must have a little time delay */
        i2c_delay(delay);

        /* Move to next #size bytes */
        cnt += size;
        iaddr += size;
        buffer += size;
        remain -= size;
    }

    return cnt;
}


/*
**	@brief	:	i2c internal address convert
**	#iaddr	:	i2c device internal address
**	#len	:	i2c device internal address length
**	#addr	:	save convert address
*/
void QGpioI2CSlave::internalAddressConvert(unsigned int iaddr, unsigned int len, unsigned char *addr)
{
    union {
        unsigned int iaddr;
        unsigned char caddr[INT_ADDR_MAX_BYTES];
    } convert;

    /* I2C internal address order is big-endian, same with network order */
    convert.iaddr = htonl(iaddr);

    /* Copy address to addr buffer */
    int i = len - 1;
    int j = INT_ADDR_MAX_BYTES - 1;

    while (i >= 0 && j >= 0) {

        addr[i--] = convert.caddr[j--];
    }
}


/*
**	@brief		:	Select i2c address @i2c bus
**	#bus		:	i2c bus fd
**	#dev_addr	:	i2c device address
**	#tenbit		:	i2c device address is tenbit
**	#return		:	success return 0, failed return -1
*/
bool QGpioI2CSlave::addressSelect(unsigned long dev_addr, unsigned long tenbit)
{
    if (m_fd == -1) {
        qWarning("QGpioI2CSlave::addressSelect: i2c file not opened");
        return false;
    }
    /* Set i2c device address bit */
    if (ioctl(m_fd, I2C_TENBIT, tenbit)) {
        qWarning("Set I2C_TENBIT failed");
        return false;
    }

    /* Set i2c device as slave ans set it address */
    if (ioctl(m_fd, I2C_SLAVE, dev_addr)) {

        qWarning("Set i2c device address failed");
        return false;
    }

    return true;
}


uint8_t QGpioI2CSlave::i2cRead(uint8_t reg) {
    uint8_t buffer[2] = { 0, 0 };

    i2cSetup();

    auto rc = readHelper(reg, buffer, 2);

    if (rc == -1 || 0xff == buffer[1]) {
//        qWarning() << "8 bit. reading error. rc:" << rc << "wanted:" << Qt::hex  << reg
//                   << "got:" << buffer[0] << buffer[1]
//                   << "i2c address" << m_address;
        return 0xff;
    } else {
//        qWarning() << "8 bit. reading ok. rc:" << rc << "wanted:" << Qt::hex  << reg
//                   << "got:" << buffer[0] << buffer[1]
//                   << "i2c address" << m_address;
    }
    return buffer[0];
}

uint16_t QGpioI2CSlave::i2cRead16(uint8_t reg) {
    uint8_t buffer[3] = { 0, 0, 0 };

    i2cSetup();

    auto rc = readHelper(reg, buffer, 3);

    if (rc == -1 || reg != buffer[0]) {
        qWarning() << "16 bit. reading error. rc:" << rc << "wanted:" << Qt::hex  << reg
                   << "got:" << buffer[0] << buffer[1] << buffer[2]
                   << "i2c address" << m_address;
    }
    return (uint16_t) ((uint16_t) buffer[1] << 8 | (uint16_t) buffer[2]);
}

uint32_t QGpioI2CSlave::i2cRead32(uint8_t reg) {
    uint8_t buffer[5] = { 0, 0, 0, 0, 0 };

    i2cSetup();

    auto rc = readHelper(reg, buffer, 5);

    if (rc == -1 || reg != buffer[0]) {
        qWarning() << "32 bit. reading error. rc:" << rc << "wanted:" << Qt::hex  << reg
                   << "got:" << buffer[0] << buffer[1] << buffer[2] << buffer[3] << buffer[4]
                   << "i2c address" << m_address;
    }
    return (uint32_t) ((uint32_t) buffer[1] << 24 | (uint32_t) buffer[2] << 16 | (uint32_t) buffer[3] << 8 | (uint32_t) buffer[4]);
}

uint8_t QGpioI2CSlave::i2cWrite(uint8_t reg, uint8_t data) {
    i2cSetup();

    auto rc = writeHelper(reg, &data, 1);

    return rc;
}

uint8_t QGpioI2CSlave::i2cWrite(uint8_t reg, uint16_t data) {
    uint8_t buffer[2];

    buffer[0] = (uint8_t) ((data >> 8) & 0xFF);
    buffer[1] = (uint8_t) (data & 0xFF);

    i2cSetup();
    auto rc = writeHelper(reg, &buffer, 2);
    return rc;
}

uint8_t QGpioI2CSlave::i2cWrite(uint8_t reg, uint16_t data, uint16_t data1) {
    uint8_t buffer[4];

    buffer[0] = (uint8_t) (data & 0xFF);
    buffer[1] = (uint8_t) (data >> 8);
    buffer[2] = (uint8_t) (data1 & 0xFF);
    buffer[3] = (uint8_t) (data1 >> 8);

    i2cSetup();

    auto rc = writeHelper(reg, &buffer, 4);
    return rc;
}

ssize_t QGpioI2CSlave::i2cWrite(uint8_t reg, const void *buf, size_t len)
{
    i2cSetup();
    return writeHelper(reg, buf, len);
}

uint8_t QGpioI2CSlave::i2cWrite(uint8_t reg, uint32_t data) {
    uint8_t buffer[4];

    buffer[0] = (uint8_t) (data & 0xFF);
    buffer[1] = (uint8_t) (data >> 8) & 0xFF;
    buffer[2] = (uint8_t) (data >> 16) & 0xFF;
    buffer[3] = (uint8_t) (data >> 24) & 0xFF;

    i2cSetup();

    auto rc = writeHelper(reg, &buffer, 4);
    return rc;
}
