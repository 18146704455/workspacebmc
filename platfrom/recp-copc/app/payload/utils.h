#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <linux/types.h>

enum erase_type{
    CHIP_ERASE,
    SECTOR_ERASE,
    BLOCK_ERASE_32K,
    BLOCK_ERASE_64K,
};

typedef struct w25q128_ops {
    int  (*open)(void);
    void (*close)(void);
    int  (*detect)(uint16_t* id);
    int  (*wel)(void);
    int  (*wdl)(void);
    int  (*erase)(int type, uint32_t addr, uint32_t size);
    int  (*read)(uint32_t addr, uint8_t* data, uint32_t size);
    int  (*program)(uint32_t addr, uint8_t* data, uint32_t size);
    int  (*protect_off_all)(void);
}W25Q128_OPS;

extern W25Q128_OPS w25q128_intf;

#ifdef DEBUG
static inline void SPI_DEBUG(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}
#else
#define SPI_DEBUG(...)
#endif

static inline void SPI_ERR(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static inline void SPI_PRINT(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}
