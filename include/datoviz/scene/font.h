/*************************************************************************************************/
/* Font                                                                                          */
/*************************************************************************************************/

#ifndef DVZ_HEADER_FONT
#define DVZ_HEADER_FONT



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_log.h"
#include "_map.h"
#include "_math.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define DVZ_DEFAULT_FONT_SIZE 24



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzFont DvzFont;



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
DVZ_EXPORT DvzFont* dvz_font(unsigned long ttf_size, unsigned char* ttf_bytes);



/**
 */
DVZ_EXPORT void dvz_font_size(DvzFont* font, double size);



/**
 */
DVZ_EXPORT vec4* dvz_font_layout(
    DvzFont* font, uint32_t length, const uint32_t* codepoints); // return an array of (x,y,w,h)



/**
 */
DVZ_EXPORT uint8_t* dvz_font_draw(
    DvzFont* font, uint32_t length, const uint32_t* codepoints, vec4* xywh,
    uvec2 out_size); // return a RGBA array



/**
 */
DVZ_EXPORT void dvz_font_destroy(DvzFont* font);



EXTERN_C_OFF

#endif
