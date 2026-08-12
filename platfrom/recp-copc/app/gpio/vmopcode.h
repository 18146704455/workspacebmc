
#define GPIO_IN	1
#define GPIO_OUT 0

/*for GPIO JTAG*/
#define GPIO_BASE_ADDR                      0x1e780000
#define SYSTEM_SCU_BASE_ADDR                0x1e6e2000
#define SYSTEM_LPC_BASE_ADDR                0x1E789000
#define SYSTEM_SCU80                       0x80
#define SYSTEM_SCU84                       0x84
#define SYSTEM_SCU8C                       0x8C
#define SYSTEM_SCU90                       0x90
#define LHCR0                                      0xa0

#define GPIOL_DIR_OFFSET                   0x74
#define GPIOL_DAT_OFFSET                    0x70
#define GPIOL_DATA_ADDR                     (GPIO_BASE_ADDR + GPIOL_DAT_OFFSET)
#define GPIOL_DIR_ADDR                       (GPIO_BASE_ADDR + GPIOL_DIR_OFFSET)
#define CPLD_UPDATE_GPIOL_ENABLE   	(0x1 << 5)


#define GPIOM_DIR_OFFSET                   0x7c
#define GPIOM_DAT_OFFSET                  0x78
#define GPIOM_DATA_ADDR                   (GPIO_BASE_ADDR + GPIOM_DAT_OFFSET)
#define GPIOM_DIR_ADDR                       (GPIO_BASE_ADDR + GPIOM_DIR_OFFSET)

#define GPIOF_DIR_OFFSET                    0x24
#define GPIOF_DAT_OFFSET                    0x20
#define GPIOF_DATA_ADDR                     (GPIO_BASE_ADDR + GPIOF_DAT_OFFSET)
#define GPIOF_DIR_ADDR                        (GPIO_BASE_ADDR + GPIOF_DIR_OFFSET)

#define GPIOA_DIR_OFFSET                    0x04
#define GPIOA_DAT_OFFSET                    0x00
#define GPIOA_DATA_ADDR                     (GPIO_BASE_ADDR + GPIOD_DAT_OFFSET)
#define GPIOA_DIR_ADDR                        (GPIO_BASE_ADDR + GPIOD_DIR_OFFSET)


#define GPIOAA_DIR_OFFSET                    0x1e0
#define GPIOAA_DAT_OFFSET                    0x1e4
#define GPIOAA_DATA_ADDR                     (GPIO_BASE_ADDR + GPIOAA_DAT_OFFSET)
#define GPIOAA_DIR_ADDR                      (GPIO_BASE_ADDR + GPIOAA_DIR_OFFSET)

#define GPIOD_DIR_OFFSET                    0x04
#define GPIOD_DAT_OFFSET                    0x00
#define GPIOD_DATA_ADDR                     (GPIO_BASE_ADDR + GPIOD_DAT_OFFSET)
#define GPIOD_DIR_ADDR                      (GPIO_BASE_ADDR + GPIOD_DIR_OFFSET)

#define GPION_DIR_OFFSET                    0x7c
#define GPION_DAT_OFFSET                    0x78
#define GPION_DATA_ADDR                     (GPIO_BASE_ADDR + GPION_DAT_OFFSET)
#define GPION_DIR_ADDR                        (GPIO_BASE_ADDR + GPION_DIR_OFFSET)



#define BMC_GPIOL0 (24)
#define BMC_GPIOL1 (25)
#define BMC_GPIOL2 (26)
#define BMC_GPIOL3 (27)
#define CPLD_TMS_CONFIG 	BMC_GPIOL0
#define CPLD_TDI_CONFIG 	BMC_GPIOL1
#define CPLD_TCK_CONFIG 	BMC_GPIOL2
#define CPLD_TDO_CONFIG 	BMC_GPIOL3

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)


/***************************************************************
*
* ispVM Embedded Return Codes.
*
***************************************************************/

#define VME_VERIFICATION_FAILURE		-1
#define VME_FILE_READ_FAILURE			  -2
#define VME_VERSION_FAILURE				  -3
#define VME_INVALID_FILE				    -4
#define VME_ARGUMENT_FAILURE			  -5
#define VME_CRC_FAILURE					    -6

/***************************************************************
*
* Type definitions.
*
***************************************************************/

