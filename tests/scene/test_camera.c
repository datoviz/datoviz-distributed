/*************************************************************************************************/
/*  Testing camera                                                                               */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_camera.h"
#include "scene/camera.h"
#include "test.h"
#include "testing.h"
#include "testing_utils.h"



/*************************************************************************************************/
/*  Camera test utils                                                                            */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Camera tests                                                                                 */
/*************************************************************************************************/

int test_camera_1(TstSuite* suite)
{
    ANN(suite);
    DvzCamera* camera = dvz_camera(WIDTH, HEIGHT, 0);

    DvzMVP mvp = dvz_mvp_default();
    dvz_camera_mvp(camera, &mvp);

    dvz_camera_print(camera);

    vec3 pos = {1, 2, 3};
    vec3 lookat = {4, 5, 6};
    vec3 up = {0, 1, 0};
    dvz_camera_initial(camera, pos, lookat, up);
    AT(glm_vec3_eqv(camera->pos, pos));
    AT(glm_vec3_eqv(camera->lookat, lookat));
    AT(glm_vec3_eqv(camera->up, up));

    dvz_camera_destroy(camera);
    return 0;
}
