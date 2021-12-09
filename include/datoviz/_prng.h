/*************************************************************************************************/
/*  Random 64-bit integer                                                                        */
/*************************************************************************************************/

#ifndef DVZ_HEADER_PRNG
#define DVZ_HEADER_PRNG



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_macros.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzPrng DvzPrng;



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

// TODO/ docstrings

DVZ_EXPORT DvzPrng* dvz_prng(void);



DVZ_EXPORT uint64_t dvz_prng_uuid(DvzPrng* prng);



DVZ_EXPORT void dvz_prng_destroy(DvzPrng* prng);



EXTERN_C_OFF

#endif
