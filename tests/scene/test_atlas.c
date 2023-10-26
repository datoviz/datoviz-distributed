/*************************************************************************************************/
/*  Testing atlas                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_atlas.h"
#include "scene/atlas.h"
#include "test.h"
#include "testing.h"
#include "testing_utils.h"



/*************************************************************************************************/
/*  Atlas test utils                                                                             */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Atlas tests                                                                                  */
/*************************************************************************************************/

int test_atlas_1(TstSuite* suite)
{
    ANN(suite);
    unsigned long ttf_size = 0;
    unsigned char* ttf_bytes = dvz_resource_font("Roboto_Medium", &ttf_size);
    ASSERT(ttf_size > 0);
    ANN(ttf_bytes);

    DvzAtlas* atlas = dvz_atlas(ttf_size, ttf_bytes);

    // dvz_atlas_string(atlas, "abc");

    dvz_atlas_generate(atlas);

    vec4 coords = {0};
    dvz_atlas_glyph(atlas, 97, coords);

    glm_vec4_print(coords, stdout);

    dvz_atlas_destroy(atlas);
    return 0;
}
