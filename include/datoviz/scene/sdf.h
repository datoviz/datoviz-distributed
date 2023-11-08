/*************************************************************************************************/
/* Sdf                                                                                           */
/*************************************************************************************************/

#ifndef DVZ_HEADER_SDF
#define DVZ_HEADER_SDF



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_log.h"
#include "_macros.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzSdf DvzSdf;

// Forward declarations.
typedef struct DvzBatch DvzBatch;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

/**
 */
DVZ_EXPORT DvzSdf* dvz_sdf(void);



/**
 */
DVZ_EXPORT void dvz_sdf_destroy(DvzSdf* sdf);



EXTERN_C_OFF

#endif
