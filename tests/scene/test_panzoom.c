/*************************************************************************************************/
/*  Testing panzoom                                                                              */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_panzoom.h"
#include "scene/panzoom.h"
#include "test.h"
#include "testing.h"
#include "testing_utils.h"



/*************************************************************************************************/
/*  Panzoom test utils                                                                           */
/*************************************************************************************************/

#define PAN(x, y)                                                                                 \
    dvz_panzoom_pan_shift(&pz, (vec2){WIDTH * x, HEIGHT * y}, (vec2){WIDTH / 2, HEIGHT / 2});

#define AP(x, y)                                                                                  \
    AC(pz.pan[0], x, EPS);                                                                        \
    AC(pz.pan[1], y, EPS);

#define SHOW                                                                                      \
    log_info(                                                                                     \
        "pan: (%.2f, %.2f)  zoom: (%.2f, %.2f)", pz.pan[0], pz.pan[1], pz.zoom[0], pz.zoom[1]);



/*************************************************************************************************/
/*  Panzoom tests                                                                                */
/*************************************************************************************************/

int test_panzoom_1(TstSuite* suite)
{
    ANN(suite);

    DvzPanzoom pz = dvz_panzoom(WIDTH, HEIGHT, 0);

    PAN(0, 0);
    AP(0, 0);
    SHOW;

    PAN(.5, 0);
    AP(1, 0);
    SHOW;

    dvz_panzoom_destroy(&pz);
    return 0;
}
