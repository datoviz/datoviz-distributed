/*************************************************************************************************/
/* Shape                                                                                         */
/*************************************************************************************************/

#ifndef DVZ_HEADER_SHAPE
#define DVZ_HEADER_SHAPE



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_log.h"
#include "_math.h"



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzShape DvzShape;



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/

typedef enum
{
    DVZ_SHAPE_NONE,
    DVZ_SHAPE_SQUARE,
    DVZ_SHAPE_DISC,
    DVZ_SHAPE_CUBE,
    DVZ_SHAPE_SPHERE,
    DVZ_SHAPE_CYLINDER,
    DVZ_SHAPE_CONE,
    DVZ_SHAPE_OBJ,
    DVZ_SHAPE_OTHER,
} DvzShapeType;



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzShape
{
    DvzShapeType type;
    uint32_t vertex_count;
    uint32_t index_count;
    vec3* pos;
    vec3* normal;
    cvec4* color;
    vec4* texcoords; // u, v, *, a
    DvzIndex* index;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Shape functions                                                                              */
/*************************************************************************************************/

DVZ_EXPORT void dvz_shape_print(DvzShape* shape);

DVZ_EXPORT void dvz_shape_destroy(DvzShape* shape);



/*************************************************************************************************/
/*  2D shapes                                                                                    */
/*************************************************************************************************/

DVZ_EXPORT DvzShape dvz_shape_square(cvec4 color);

DVZ_EXPORT DvzShape dvz_shape_disc(uint32_t count, cvec4 color);



/*************************************************************************************************/
/*  3D shapes                                                                                    */
/*************************************************************************************************/

DVZ_EXPORT DvzShape dvz_shape_cube(cvec4* colors);

DVZ_EXPORT DvzShape dvz_shape_sphere(uint32_t rows, uint32_t cols, cvec4 color);

DVZ_EXPORT DvzShape dvz_shape_cone(uint32_t count, cvec4 color);

DVZ_EXPORT DvzShape dvz_shape_cylinder(uint32_t count, cvec4 color);



EXTERN_C_OFF

#endif
