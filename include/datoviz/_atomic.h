/*************************************************************************************************/
/*  Atomic operations                                                                            */
/*************************************************************************************************/

#ifndef DVZ_HEADER_ATOMIC
#define DVZ_HEADER_ATOMIC



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_macros.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

// Atomic macro, for both C++ and C
#if defined(LANG_C) && !OS_WIN32
/*  C11 atomic is only used for C (not C++) and not on Windows                                   */
/*************************************************************************************************/
#define ATOMIC_C
#include <stdatomic.h>
// #define atomic(t, x) _Atomic t x
#define DvzAtomic   _Atomic int32_t
#define ATOMIC_DECL static inline
#else
/*  C++ atomic                                                                                   */
/*************************************************************************************************/
#define ATOMIC_CPP
#include <atomic>
// #define atomic(t, x) std::atomic<t> x
#define DvzAtomic   std::atomic<int32_t>
#define ATOMIC_DECL DVZ_EXPORT
#endif



/*************************************************************************************************/
/*  Atomic functions                                                                             */
/*************************************************************************************************/

/**
 * Initialize an atomic.
 *
 * @param atomic the atomic variable to initialize
 */
ATOMIC_DECL void dvz_atomic_init(DvzAtomic* atomic)
#ifdef ATOMIC_C
{
    ASSERT(atomic != NULL);
    atomic_init(atomic, 0);
}
#else
    ;
#endif


/**
 * Set an atomic variable to a given value.
 *
 * @param atomic the atomic variable
 * @param value the value
 */
ATOMIC_DECL void dvz_atomic_set(DvzAtomic* atomic, int32_t value)
#ifdef ATOMIC_C
{
    ASSERT(atomic != NULL);
    atomic_store(atomic, value);
}
#else
    ;
#endif



/**
 * Get the value of an atomic variable.
 *
 * @param atomic the atomic variable
 * @returns the value
 */
ATOMIC_DECL int32_t dvz_atomic_get(DvzAtomic* atomic)
#ifdef ATOMIC_C
{
    ASSERT(atomic != NULL);
    return atomic_load(atomic);
}
#else
    ;
#endif



#endif
