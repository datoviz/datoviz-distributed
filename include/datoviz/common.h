/*************************************************************************************************/
/*  Common symbols, macros, and includes                                                         */
/*************************************************************************************************/

#ifndef DVZ_HEADER_COMMON
#define DVZ_HEADER_COMMON



/*************************************************************************************************/
/*  Standard includes                                                                            */
/*************************************************************************************************/

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>



/*************************************************************************************************/
/*  External includes                                                                            */
/*************************************************************************************************/

#include "_macros.h"



/*************************************************************************************************/
/*  Internal includes                                                                            */
/*************************************************************************************************/

#include "_log.h"
#include "_math.h"
#include "_obj.h"
#include "_thread.h"
#include "_time.h"

#include "_debug.h"



/*************************************************************************************************/
/*  Built-in fixed constants                                                                     */
/*************************************************************************************************/

#define ENGINE_NAME         "Datoviz"
#define APPLICATION_NAME    "Datoviz"
#define APPLICATION_VERSION VK_MAKE_VERSION(0, 2, 0)

#define DVZ_NEVER -1000000



/*************************************************************************************************/
/*  Defaults                                                                                     */
/*************************************************************************************************/

#define DVZ_DEFAULT_VIEWPORT                                                                      \
    (vec2) { 0, 0 }
#define DVZ_DEFAULT_FORMAT DVZ_FORMAT_B8G8R8A8_UNORM
#define DVZ_DEFAULT_CLEAR_COLOR                                                                   \
    (cvec4) { 0, 8, 18, 255 }



#endif
