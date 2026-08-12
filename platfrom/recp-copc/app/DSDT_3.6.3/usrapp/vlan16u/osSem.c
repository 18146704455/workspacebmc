/*
 * @Descripttion: 
 * @version: 
 * @Author: smith.zj
 * @Date: 2019-08-16 13:52:59
 * @LastEditors: smith.zj
 * @LastEditTime: 2019-10-23 19:53:18
 */
#include "Copyright.h"
#include <msApi.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include "portable.h"
#include "common_log.h"


/*******************************************************************************
 * * osSemCreate
 * *
 * * DESCRIPTION:
 * *       Create semaphore.
 * *
 * * INPUTS:
 * *       name   - semaphore Name
 * *       init   - init value of semaphore counter
 * *       count  - max counter (must be >= 1)
 * *
 * * OUTPUTS:
 * *       smid - semaphore Id
 * *
 * * RETURNS:
 * *       GT_OK   - on success
 * *       GT_FAIL - on error
 * *
 * * COMMENTS:
 * *       None
 * *
 * *******************************************************************************/
GT_STATUS osSemCreate(GT_SEM_BEGIN_STATE state,GT_SEM *lxSem)
{
    int iret = 1;
    iret = sem_init(lxSem, state, 1);
    if(iret != 0){
        DB_PRINT(DB_ERR,"sem init failed:%s",strerror(errno));
        iret = 0;
    } else {
        iret = 1;
    }

    return iret;
}

/*******************************************************************************
 * * osSemDelete
 * *
 * * DESCRIPTION:
 * *       Delete semaphore.
 * *
 * * INPUTS:
 * *       smid - semaphore Id
 * *
 * * OUTPUTS:
 * *       None
 * *
 * * RETURNS:
 * *       GT_OK   - on success
 * *       GT_FAIL - on error
 * *
 * * COMMENTS:
 * *       None
 * *
 * *******************************************************************************/
GT_STATUS osSemDelete(GT_SEM smid)
{
    int iret;
    iret = sem_destroy((semaphore*) &smid);
    if(iret != 0){
        DB_PRINT(DB_ERR,"sem init failed:%s",strerror(errno));
        return GT_FAIL;
    }
    return GT_OK;
}



/*******************************************************************************
 * * osSemWait
 * *
 * * DESCRIPTION:
 * *       Wait on semaphore.
 * *
 * * INPUTS:
 * *       smid    - semaphore Id
 * *       timeOut - time out in miliseconds or 0 to wait forever
 * *
 * * OUTPUTS:
 * *       None
 * *
 * * RETURNS:
 * *       GT_OK   - on success
 * *       GT_FAIL - on error
 * *       OS_TIMEOUT - on time out
 * *
 * * COMMENTS:
 * *       None
 * *
 * *******************************************************************************/
GT_STATUS osSemWait(GT_SEM smid, GT_U32 timeOut)
{
    int iret;
    iret = sem_wait((semaphore*) &smid) ; 
    if(iret != 0){
        DB_PRINT(DB_ERR,"sem init failed:%s",strerror(errno));
        return GT_FAIL;
    }
    return GT_OK;
}


/*******************************************************************************
 * * osSemSignal
 * *
 * * DESCRIPTION:
 * *       Signal a semaphore.
 * *
 * * INPUTS:
 * *       smid    - semaphore Id
 * *
 * * OUTPUTS:
 * *       None
 * *
 * * RETURNS:
 * *       GT_OK   - on success
 * *       GT_FAIL - on error
 * *
 * * COMMENTS:
 * *       None
 * *
 * *******************************************************************************/
GT_STATUS osSemSignal(GT_SEM smid)
{
    int iret = 0;
    sem_post((semaphore*)& smid) ;
    if(iret != 0){
        DB_PRINT(DB_ERR,"sem init failed:%s",strerror(errno));
        return GT_FAIL;
    }
    return GT_OK;
}






