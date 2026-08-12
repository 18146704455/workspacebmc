#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAC0_BASE         0x1E660000
#define MAC1_BASE         0x1E680000
#define RETRY_TIMES       0xff

//TODO: Check MAC40: Feature Register (FEAR), BIT31 must be set
//otherwise use old MDIO interface

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
static int _phy_read_old(char *base, char phyaddr, char regaddr, uint16_t* val)
{
    volatile uint32_t value;
    value = *(volatile uint32_t *)(base+MDIO_CTL_OFF);
    value &= 0x3f;
    value |= (phyaddr & 0x1f) << 16;
    value |= (regaddr & 0x1f) << 21;
    value |= 1 << 26;
    *(volatile uint32_t *)(base+MDIO_CTL_OFF)=value;

    for(int i=0; i<10 ;i ++)
    {
        value = *(volatile uint32_t *)(base+MDIO_CTL_OFF);
        if((value & 1<<26) == 0)
        {
            value = *(volatile uint32_t *)(base+MDIO_DAT_OFF);
            *val = (value >>16) & 0xffff ;
            return 0;
        }
        usleep(1);

    }
    return -1;



}
static int _phy_write_old(char *base, char phyaddr, char regaddr, uint16_t val)
{

    volatile uint32_t value;
    value=val&0xffff;
    *(volatile uint32_t *)(base+MDIO_DAT_OFF)=value;

    value = *(volatile uint32_t *)(base+MDIO_CTL_OFF);
    value &= 0x3f;
    value |= (phyaddr & 0x1f) << 16;
    value |= (regaddr & 0x1f) << 21;
    value |= 1 << 27;
    *(volatile uint32_t *)(base+MDIO_CTL_OFF)=value;

    for(int i=0; i<10 ;i ++)
    {
        value = *(volatile uint32_t *)(base+MDIO_CTL_OFF);
        if((value & 1<<27) == 0)
        {
            return 0;
        }
        usleep(1);
    }
    return -1;
}
static int _phy_read_new(char *base, char phyaddr, char regaddr, uint16_t* val)
{
    int ret=0, i = 0;
    MDIO_CTL_T mdio_ctl;
    mdio_ctl.u32 = 0;

    mdio_ctl.b.regaddr = regaddr;
    mdio_ctl.b.phyaddr = phyaddr;
    mdio_ctl.b.st      = 1;  /*ST22*/
    mdio_ctl.b.opcode  = 2;  /*Read*/
    mdio_ctl.b.fire    = 1; /*fire mdio read*/

    *(volatile uint32_t *)((char*)base + MDIO_CTL_OFF) = mdio_ctl.u32;
    do{  /*wait for compl*/
        mdio_ctl.u32 = *(volatile uint32_t *)((char*)base + MDIO_CTL_OFF);
        if(0 == mdio_ctl.b.fire){
            break;
        }else{
            usleep(1);
        }
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        printf("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }

    *val = (*(volatile uint32_t *)((char*)base + MDIO_DAT_OFF)) & 0xffff;
out:
    return ret;

}
static int _phy_write_new(char *base, char phyaddr, char phyreg, uint16_t phydata)
{
#define MAC_PHYWR_NEW             0x00009400
#define MAC_PHYBUSY_NEW           0x00008000   

    int ret = 0, i = 0;
    uint32_t tmp = 0;
    volatile uint32_t *mdio_ctl;
    tmp |= ( phydata << 16 );
    tmp |= ( ( phyaddr & 0x1f ) << 5  );
    tmp |= ( phyreg & 0x1f);
    tmp  |= MAC_PHYWR_NEW;

    mdio_ctl = (uint32_t *)((char*)base + MDIO_CTL_OFF);
    *mdio_ctl = tmp;

    do{  //wait for compl
        if(!( *mdio_ctl & MAC_PHYBUSY_NEW )){
            break;
        }
        usleep(100);

    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        printf("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }


out:
    return ret;

}
extern int phy_read(char busno, char phyaddr, char regaddr, uint16_t* val)
{
    char *base;
    int ret = 0;
    volatile uint32_t *value;
    if(ast_mem_map()){
        printf("ast_mem_map failed\n");
        return -1;
    }

    if(0 == busno){
        base=MAC0_virt;
    }else if(1 == busno){   
        base=MAC1_virt;
    }else{
        printf("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }
    value = (uint32_t *)((char*)base + 0x40);
    if(((*value) >>31)&0x1 )
    {
        //printf("use new mdio interface\n");
        ret = _phy_read_new(base,phyaddr,regaddr,val);

    }else{
       // printf("use old mdio interface\n");
        ret = _phy_read_old(base,phyaddr,regaddr,val);
    }

out:
    ast_mem_unmap();
    return ret;


}
extern int phy_write(char busno, char phyaddr, char regaddr, uint16_t val)
{
    char *base=NULL;
    int ret = 0;
    volatile uint32_t *value;

    if(ast_mem_map()){
        printf("ast_mem_map failed\n");
        return -1;
    }

    if(0 == busno){
        base=MAC0_virt;
    }else if(1 == busno){   
        base=MAC1_virt;
    }else{
        printf("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }
    value = (uint32_t *)((char*)base + 0x40);
    if(((*value) >>31)&0x1 )
    {
        ret = _phy_write_new(base,phyaddr,regaddr,val);

    }else{
        ret = _phy_write_old(base,phyaddr,regaddr,val);
    }

out:
    ast_mem_unmap();
    return ret;

}
#if 0
extern int phy_read(char busno, char phyaddr, char regaddr, uint16_t* val)
{
    int ret = 0, i = 0;
    MDIO_CTL_T* mdio_ctl = NULL;
    MDIO_DAT_T* mdio_dat = NULL;

    if(ast_mem_map()){
        printf("ast_mem_map failed\n");
        return -1;
    }

    if(0 == busno){
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC0_virt + MDIO_CTL_OFF);
        mdio_dat = (MDIO_DAT_T *)((char*)MAC0_virt + MDIO_DAT_OFF);
    }else if(1 == busno){   
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC1_virt + MDIO_CTL_OFF);
        mdio_dat = (MDIO_DAT_T *)((char*)MAC1_virt + MDIO_DAT_OFF);
    }else{
        printf("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    mdio_ctl->b.regaddr = regaddr;
    mdio_ctl->b.phyaddr = phyaddr;
    mdio_ctl->b.st      = 1;  //ST22
    mdio_ctl->b.opcode  = 2;  //Read
    mdio_ctl->b.fire    = 1; //fire mdio read
    do{  //wait for compl
        if(0 == mdio_ctl->b.fire){
            break;
        }else{
            usleep(1);
        }
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        printf("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }

    *val = mdio_dat->b.miirdat;

out:
    ast_mem_unmap();
    return ret;
}
#if 0
extern int phy_write(char busno, char phyaddr, char regaddr, uint16_t val)
{
    int ret = 0, i = 0;
    MDIO_CTL_T* mdio_ctl = NULL;

    if(ast_mem_map()){
        printf("ast_mem_map failed\n");
        return -1;
    }

    if(0 == busno){
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC0_virt + MDIO_CTL_OFF);
    }else if(1 == busno){    
        mdio_ctl = (MDIO_CTL_T *)((char*)MAC1_virt + MDIO_CTL_OFF);
    }else{
        printf("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    mdio_ctl->b.miiwdata = val;
    mdio_ctl->b.regaddr  = regaddr;
    mdio_ctl->b.phyaddr  = phyaddr;
    mdio_ctl->b.st       = 1;  //ST22
    mdio_ctl->b.opcode   = 1;  //Write
    mdio_ctl->b.fire     = 1; //fire mdio write
    do{  //wait for compl
        if(0 == mdio_ctl->b.fire){
            break;
        }else{
            usleep(1);
        }
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        printf("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }

out:
    ast_mem_unmap();
    return ret;
}
#endif
#define MAC_PHYWR_NEW             0x00009400
#define MAC_PHYBUSY_NEW           0x00008000   
 int phy_write(char busno, char phyaddr, char phyreg, uint16_t phydata)
{
    int ret = 0, i = 0;
    uint32_t tmp = 0;
    volatile uint32_t *mdio_ctl;
    tmp |= ( phydata << 16 );
    tmp |= ( ( phyaddr & 0x1f ) << 5  );
    tmp |= ( phyreg & 0x1f);
    tmp  |= MAC_PHYWR_NEW;

    if(ast_mem_map()){
        printf("ast_mem_map failed\n");
        return -1;
    }

    if(0 == busno){
        mdio_ctl = (uint32_t *)((char*)MAC0_virt + MDIO_CTL_OFF);
    }else if(1 == busno){    
        mdio_ctl = (uint32_t *)((char*)MAC1_virt + MDIO_CTL_OFF);
    }else{
        printf("Incorrect busno %d\n", busno);
        ret = -1;
        goto out;
    }

    *mdio_ctl = tmp;
	
    do{  //wait for compl
	if(!( *mdio_ctl & MAC_PHYBUSY_NEW )){
		break;
	}
	usleep(100);
	
    }while(i++ < RETRY_TIMES);
    if(i >= RETRY_TIMES){
        printf("Wait for MDIO read complete timeout\n");
        ret = -2;
        goto out;
    }

out:
    ast_mem_unmap();
    return ret;
}

#endif
