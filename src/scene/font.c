/*************************************************************************************************/
/*  Font                                                                                         */
/*************************************************************************************************/


/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/font.h"
#include "../_pointer.h"
#include "_macros.h"
#include "fileio.h"
#include "request.h"

#include <ft2build.h>
#include FT_FREETYPE_H



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Utility functions                                                                            */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

extern struct DvzFont
{
    FT_Library library;
    FT_Face face;
    double size;
};



/*************************************************************************************************/
/*  Font functions                                                                               */
/*************************************************************************************************/

DvzFont* dvz_font(unsigned long ttf_size, unsigned char* ttf_bytes)
{
    DvzFont* font = (DvzFont*)calloc(1, sizeof(DvzFont));
    ANN(font);

    return font;
}


void dvz_font_destroy(DvzFont* font)
{
    ANN(font);
    FREE(font);
}
