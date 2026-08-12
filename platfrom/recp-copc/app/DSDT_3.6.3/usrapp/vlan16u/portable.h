#include <Copyright.h>
#include <semaphore.h>

/*smi access*/
#include <msApi.h>
GT_BOOL gtBspReadMii (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,unsigned int* value);
GT_BOOL gtBspWriteMii (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg,unsigned int value);


/*semaphore*/
typedef    sem_t          semaphore ;
GT_STATUS osSemCreate( GT_SEM_BEGIN_STATE state,GT_SEM *sem);
GT_STATUS osSemDelete(GT_SEM smid);
GT_STATUS osSemWait(  GT_SEM smid, GT_U32 timeOut);
GT_STATUS osSemSignal(GT_SEM smid);
