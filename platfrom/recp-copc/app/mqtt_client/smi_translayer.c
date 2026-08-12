/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-10-21 17:41:22
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-11-06 14:06:41
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common_log.h"
#include <stdbool.h>


#define MAC0_BASE         0x1E660000
#define MAC1_BASE         0x1E680000
#define MAC_PHYWR_NEW             0x00009400
#define MAC_PHYBUSY_NEW           0x00008000   
#define RETRY_TIMES 0xff

typedef union MDIO_CTL_U{
    volatile uint32_t u32;
    struct MDIO_CTL_B{
        volatile uint32_t regaddr:5; //PHY register address(clause 22) or Device address(clause 45)
        volatile uint32_t phyaddr:5; //PHY address(clause 22) or Port address(clause 45)
        volatile uint32_t opcode:2;  //clause22: 01->W,   10->R
                                     //clause45: 00->Addr 01->W, 10->R addrInc,  11->R
        volatile uint32_t st:1;      //0->clause45, 1->clause22
        volatile uint32_t rev:2;
        volatile uint32_t fire:1;
        volatile uint32_t miiwdata:16; //data to phy
    }b;
}MDIO_CTL_T;

typedef union MDIO_DATA_U{
    volatile uint32_t u32;
    struct MDIO_DATA_B{
        volatile uint32_t miirdat:16;    /*Read data from phy*/
        volatile uint32_t rev:8;
        volatile uint32_t threshold:8;   /*MDC cycle threshold*/
    }b;
}MDIO_DAT_T;

enum MAC_MDIO_REG{
    MDIO_CTL_OFF = 0x60,
    MDIO_DAT_OFF = 0x64,
};

typedef enum SMI_OPCODE_E {
    SMI_OPCODE_ADDR = 0,
    SMI_OPCODE_WRITE,
    SMI_OPCODE_READ,
    SMI_OPCODE_R,
}SMI_OPCODE_E;

typedef enum SMI_STCODE_E {
    SMI_STCODE_C45,
    SMI_STCODE_C22,
}SMI_STCODE_E;


static void* MAC0_virt, *MAC1_virt;
static int ast_mem_map(void)
{
    int fd;

    if((fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0){
        return (-1);
    }

    MAC0_virt = mmap(NULL,
                    getpagesize(),
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    MAC0_BASE);

    MAC1_virt = mmap(NULL,
                    getpagesize(),
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    fd,
                    MAC1_BASE);

    if (MAP_FAILED == MAC0_virt || \
        MAP_FAILED == MAC1_virt){
        close(fd);
        return (-1);
    }

    close(fd);
    return 0;
}

static void ast_mem_unmap(void)
{
    munmap(MAC0_virt, getpagesize());
    munmap(MAC1_virt, getpagesize());
}


int initSmiMem()
{
    if(ast_mem_map()){
        pr_err("ast_mem_map failed\n");
        return -1;
    }
    return 0;
}

void unmapSmiMem()
{
    return  ast_mem_unmap();
}

static int smi_c45_addr_cycle(uint8_t busno,uint16_t smiadd,uint16_t devport,uint16_t reg)
{
    MDIO_CTL_T* mdio_ctl = NULL;
    int ret = 0;
    int i = 0;

    if(0 == busno){
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC0_virt + MDIO_CTL_OFF);
    }else if(1 == busno){   
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC1_virt + MDIO_CTL_OFF);
    }else{
        pr_err("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    mdio_ctl->b.miiwdata = reg;
    mdio_ctl->b.regaddr = devport & 0x1f;
    mdio_ctl->b.phyaddr = smiadd & 0x1f;
    mdio_ctl->b.st      = SMI_STCODE_C45;  //C45
    mdio_ctl->b.opcode  = SMI_OPCODE_ADDR;  //ADDR cycle
    mdio_ctl->b.fire    = 1; //fire
    do{  //wait for compl
        if(0 == mdio_ctl->b.fire){
            break;
        }
        usleep(1);
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        pr_err("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }
out:
    return ret;
}


extern int smi_read(uint8_t busno, uint16_t smiaddr, uint16_t port,uint16_t regaddr, uint16_t* val,bool C45)
{
    int ret = 0, i = 0;
    MDIO_CTL_T* mdio_ctl = NULL;
    MDIO_DAT_T* mdio_dat = NULL;
    uint8_t st;


    if(0 == busno){
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC0_virt + MDIO_CTL_OFF);
        mdio_dat = (MDIO_DAT_T *)((char*)MAC0_virt + MDIO_DAT_OFF);
    }else if(1 == busno){   
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC1_virt + MDIO_CTL_OFF);
        mdio_dat = (MDIO_DAT_T *)((char*)MAC1_virt + MDIO_DAT_OFF);
    }else{
        pr_err("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    if(C45) {
        if(smi_c45_addr_cycle(busno,smiaddr,port,regaddr) != 0) {
            DB_PRINT(DB_ERR,"C45 ADDR cycle failed(%d 0x%x 0x%x 0x%x)\n", busno,smiaddr,port,regaddr);
            return 1;
        }
        st = SMI_STCODE_C45;
    } else {
        st = SMI_STCODE_C22;     
    }

    mdio_ctl->b.regaddr = port;
    mdio_ctl->b.phyaddr = smiaddr;
    mdio_ctl->b.st      = st;  //ST22
    mdio_ctl->b.opcode  = SMI_OPCODE_READ;  //Read
    mdio_ctl->b.fire    = 1; //fire mdio read
    do{  //wait for compl
        if(0 == mdio_ctl->b.fire){
            break;
        }else{
            usleep(1);
        }
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        DB_PRINT(DB_ERR,"Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }
    *val = mdio_dat->b.miirdat;
out:
    return ret;
}

extern int smi_write(uint8_t busno, uint16_t smiaddr, uint16_t port,uint16_t regaddr, uint16_t val,bool C45)
{
    int ret = 0, i = 0;
    MDIO_CTL_T* mdio_ctl = NULL;
    uint8_t st;

    if(0 == busno){
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC0_virt + MDIO_CTL_OFF);
    }else if(1 == busno){   
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC1_virt + MDIO_CTL_OFF);
    }else{
        pr_err("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    if(C45) {
        if(smi_c45_addr_cycle(busno,smiaddr,port,regaddr) != 0) {
            DB_PRINT(DB_ERR,"C45 ADDR cycle failed(%d 0x%x 0x%x 0x%x)\n", busno,smiaddr,port,regaddr);
            return 1;
        }
        st = SMI_STCODE_C45;
    } else {
        st = SMI_STCODE_C22;     
    }

    mdio_ctl->b.miiwdata = val;
    mdio_ctl->b.regaddr = port;
    mdio_ctl->b.phyaddr = smiaddr;
    mdio_ctl->b.st      = st;
    mdio_ctl->b.opcode  = SMI_OPCODE_WRITE; 
    mdio_ctl->b.fire    = 1;
    do{  //wait for compl
        if(0 == mdio_ctl->b.fire){
            break;
        }else{
            usleep(1);
        }
    } while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        DB_PRINT(DB_ERR,"Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }

out:
    return ret;
}
