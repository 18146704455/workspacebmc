#include <stdint.h>
#include <stdlib.h>

extern uint32_t scu_read(uint16_t);
extern void scu_write(uint16_t, uint32_t);
extern uint32_t gpio_read(uint16_t);
extern void gpio_write(uint16_t, uint32_t);

static uint32_t reg_val;
#define CLEAR_SCU_REG_BIT(off, bit)  \
do{                                  \
    reg_val = scu_read(off);         \
    reg_val &= ~(1 << bit);          \
    scu_write(off, reg_val);         \
}while(0);

#define SET_SCU_REG_BIT(off, bit)    \
do{                                  \
    reg_val = scu_read(off);         \
    reg_val |= (1 << bit);           \
    scu_write(off, reg_val);         \
}while(0);

#define CLEAR_GPIO_REG_BIT(off, bit) \
do{                                  \
    reg_val = gpio_read(off);        \
    reg_val &= ~(1 << bit);          \
    gpio_write(off, reg_val);        \
}while(0);

#define SET_GPIO_REG_BIT(off, bit)   \
do{                                  \
    reg_val = gpio_read(off);        \
    reg_val |= (1 << bit);           \
    gpio_write(off, reg_val);        \
}while(0);

extern void scu_init(void)
{
    int i;

    for(i = 0; i < 8; i++) {
        CLEAR_SCU_REG_BIT(0x88, i);
    }

    for(i = 8; i <= 12; i++) {
        CLEAR_SCU_REG_BIT(0x7C, i);
    }

    CLEAR_SCU_REG_BIT(0x70, 19);
    CLEAR_SCU_REG_BIT(0xa4, 10);
    CLEAR_SCU_REG_BIT(0x94, 11);
    CLEAR_SCU_REG_BIT(0x90, 31);
    CLEAR_SCU_REG_BIT(0xa4, 24);
}

/*GPIOY2 output 0*/
extern void payload_reset(void)
{
    SET_GPIO_REG_BIT(0x1e4, 2);
    CLEAR_GPIO_REG_BIT(0x1e0, 2);
}

/*GPIOAA0, GPIOAA1 output 0*/
extern void pll_oe_enable(void)
{
    SET_GPIO_REG_BIT(0x1e4, 16);
    SET_GPIO_REG_BIT(0x1e4, 17);
    CLEAR_GPIO_REG_BIT(0x1e0, 16);
    CLEAR_GPIO_REG_BIT(0x1e0, 17);
}

/*GPIOAA0, GPIOAA1 output 1*/
extern void pll_oe_disable(void)
{
    SET_GPIO_REG_BIT(0x1e0, 16);
    SET_GPIO_REG_BIT(0x1e0, 17);
}

/*GPION5, GPION6 ouput 1*/
extern void payload_power_on(void)
{
    SET_GPIO_REG_BIT(0x7C, 13);
    SET_GPIO_REG_BIT(0x7C, 14);

    SET_GPIO_REG_BIT(0x78, 13);
    SET_GPIO_REG_BIT(0x78, 14);
}

extern void payload_power_off(void)
{
    CLEAR_GPIO_REG_BIT(0x78, 13);
    CLEAR_GPIO_REG_BIT(0x78, 14);
}

/*GPIOY2 output 1*/
extern void payload_unbind_reset(void)
{
    SET_GPIO_REG_BIT(0x1e0, 2);
}
