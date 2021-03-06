/*************************************************************************************************/
/*  Mutex                                                                                        */
/*************************************************************************************************/

#ifndef DVZ_HEADER_MUTEX
#define DVZ_HEADER_MUTEX



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_macros.h"
#include "_time.h"

#include "tinycthread.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef tct_cnd_t DvzCond;



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

typedef tct_mtx_t DvzMutex;



EXTERN_C_ON

/*************************************************************************************************/
/*  Mutex functions                                                                              */
/*************************************************************************************************/

/**
 * Initialize an mutex.
 *
 * @param mutex the mutex to initialize
 */
DVZ_EXPORT int dvz_mutex_init(DvzMutex* mutex);



/**
 * Create a mutex.
 *
 * @returns mutex
 */
DVZ_EXPORT DvzMutex dvz_mutex(void);



/**
 * Lock a mutex.
 *
 * @param mutex the mutex
 */
DVZ_EXPORT int dvz_mutex_lock(DvzMutex* mutex);



/**
 * Unlock a mutex.
 *
 * @param mutex the mutex
 */
DVZ_EXPORT int dvz_mutex_unlock(DvzMutex* mutex);



/**
 * Destroy an mutex.
 *
 * @param mutex the mutex to destroy
 */
DVZ_EXPORT void dvz_mutex_destroy(DvzMutex* mutex);



/*************************************************************************************************/
/*  Cond functions                                                                               */
/*************************************************************************************************/

/**
 * Initialize a cond.
 *
 * @param cond the cond to initialize
 */
DVZ_EXPORT int dvz_cond_init(DvzCond* cond);



/**
 * Create a cond.
 *
 * @returns cond
 */
DVZ_EXPORT DvzCond dvz_cond(void);


/**
 * Signal a cond.
 *
 * @param cond the cond
 */
DVZ_EXPORT int dvz_cond_signal(DvzCond* cond);



/**
 * Wait until a cond is signaled.
 *
 * @param cond the cond
 */
DVZ_EXPORT int dvz_cond_wait(DvzCond* cond, DvzMutex* mutex);



/**
 * Wait until the cond is signaled, or until wait.
 *
 * @param cond the cond
 * @param wait waiting limit
 */
DVZ_EXPORT int dvz_cond_timedwait(DvzCond* cond, DvzMutex* mutex, struct timespec* wait);



/**
 * Destroy a cond.
 *
 * @param cond the cond
 */
DVZ_EXPORT void dvz_cond_destroy(DvzCond* cond);



EXTERN_C_OFF

#endif
