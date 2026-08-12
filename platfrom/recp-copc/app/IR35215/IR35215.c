#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include "IR35215.h"
#include "smbus.h" 
#include "i2cctrl.h"

#define CHANGE_16_ENDIAN(x)	(((x) & (uint32_t)0x0000ff00UL)>>8 | ((x) & (uint32_t)0x000000ffUL)<<8)

#ifdef DB_CURR_LEVEL 
#undef DB_CURR_LEVEL
#define DB_CURR_LEVEL DB_ERR
#endif

#define	IR_PMUBUS_ADDR  0x47

#define IR_MFR_WRITE_CMD 0xD1
#define IR_MFR_READ_CMD 0xD0

#define PEX_BLOC_WRITE_CMD 0xbe
#define PEX_BLOC_READ_CMD 0xba

#define REG_LANE_STATUS     0x1F4

static unsigned char ir_pmbus_addr = (unsigned char)0x3f;
unsigned int reversebytes_uint32t(unsigned int value)
{    return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |         (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24; }



int ir35215_MFR_read(uint16_t reg, uint16_t *data)
{
	int result;
	unsigned char command_val[4];
	command_val[0] = IR_MFR_READ_CMD;
	command_val[1] = reg & 0xff;
	command_val[2] = (reg & 0xff00) >> 8;

  struct i2c_msg rdwr_msgs[2] = {
    {  // Start address
      .addr = IR_PMUBUS_ADDR,
      .flags = 0, // write
      .len = 3,
      .buf = command_val
    },
    { // Read buffer
      .addr = IR_PMUBUS_ADDR,
      .flags = I2C_M_RD, // read
      .len = 2,
      .buf = (unsigned char *)data
    }
  };

  struct i2c_rdwr_ioctl_data rdwr_data = {
    .msgs = rdwr_msgs,
    .nmsgs = 2
  };

  result = ioctl(g_i2c_fd, I2C_RDWR, &rdwr_data );
  if ( result < 0 ) {
    printf( "rdwr ioctl error: %d\n", errno );
    perror( "reason" );
  } else {
    printf( "rdwr ioctl OK\n" );
  }

	return 0;
}


int ir35215_MFR_write(uint16_t reg, uint16_t data)
{
	int result;
	unsigned char command_val[6];
	command_val[0] = IR_MFR_READ_CMD;
	command_val[1] = 0x04;
	command_val[2] = reg & 0xff;
	command_val[3] = (reg & 0xff00) >> 8;
	command_val[4] = data & 0xff;
	command_val[5] = (data & 0xff00) >> 8;

  struct i2c_msg rdwr_msgs[1] = {
    {  // Start address
      .addr = IR_PMUBUS_ADDR,
      .flags = 0, // write
      .len = 6,
      .buf = command_val
    },
  };

  struct i2c_rdwr_ioctl_data rdwr_data = {
    .msgs = rdwr_msgs,
    .nmsgs = 1
  };

  result = ioctl(g_i2c_fd, I2C_RDWR, &rdwr_data );
  if ( result < 0 ) {
    printf( "rdwr ioctl error: %d\n", errno );
    perror( "reason" );
  } else {
    printf( "rdwr ioctl OK\n" );
  }
	return 0;
}


int ir_set_pmbus_addr(unsigned char addr)
{
    ir_pmbus_addr = addr;
    return 0;
}



