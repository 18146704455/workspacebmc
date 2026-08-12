#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <syslog.h>
#define LPC_HOST_MAGIC  'L'
#define EPLD_FILE       "/dev/ast-lpc-host"
#define LPC_SET_EPLD    _IOW(LPC_HOST_MAGIC, 0, uint32_t)
#define LPC_RD_EPLD     _IOR(LPC_HOST_MAGIC, 1, uint32_t)
#define LPC_SET_CLK     _IOW(LPC_HOST_MAGIC, 2, uint32_t)

int epld_read(uint32_t offset, uint8_t* value)
{
    int fd = open(EPLD_FILE, O_RDWR);
    if(fd < 0)
    {
        syslog(LOG_ERR,"cpld read reg 0x%x err on open /dev/ast-lpc-host",offset);
        return -1;
    }
    offset |=0xfe<<8;
    if(ioctl(fd, LPC_RD_EPLD, &offset)) {
        syslog(LOG_ERR,"cpld read reg|0xfe00 0x%x err on ioctl",offset);
        close(fd);
        return -2;
    }

    *value = offset & 0xff;
    close(fd);
    return 0;
}

int epld_write(uint32_t offset, uint8_t value)
{
    uint32_t ioctl_arg[2];

    int fd = open(EPLD_FILE, O_RDWR);
    if(fd < 0){
        syslog(LOG_ERR,"cpld write reg 0x%x value 0x%x err on open /dev/ast-lpc-host",offset,value);
        return -1;
    }

    offset |=0xfe<<8;
    ioctl_arg[0] = offset;
    ioctl_arg[1] = value;
    if(ioctl(fd, LPC_SET_EPLD, ioctl_arg)) {
        syslog(LOG_ERR,"cpld write reg|0xfe00 0x%x value 0x%x err on ioctl",offset ,value);
        close(fd);
        return -2;
    }

    close(fd);
    return 0;
}
