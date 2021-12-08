/*************************************************************************************************/
/*  Map                                                                                          */
/*************************************************************************************************/

#ifndef DVZ_HEADER_MAP
#define DVZ_HEADER_MAP



/*************************************************************************************************/
/*  Includes                                                                                    */
/*************************************************************************************************/

#include <stdint.h>

#include "_macros.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define DVZ_ID_NONE 0



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzMap DvzMap;
typedef uint64_t DvzId;

// Forward declarations.



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

// TODO: docstrings

DVZ_EXPORT DvzMap* dvz_map(void);



DVZ_EXPORT DvzId dvz_map_id(DvzMap* map);



DVZ_EXPORT void dvz_map_add(DvzMap* map, DvzId key, int type, void* value);



DVZ_EXPORT void dvz_map_remove(DvzMap* map, DvzId key);



DVZ_EXPORT void* dvz_map_get(DvzMap* map, DvzId key);



DVZ_EXPORT uint64_t dvz_map_count(DvzMap* map, int type);



DVZ_EXPORT void* dvz_map_first(DvzMap* map, int type);



DVZ_EXPORT void* dvz_map_last(DvzMap* map, int type);



DVZ_EXPORT void dvz_map_destroy(DvzMap* map);



EXTERN_C_OFF

#endif
