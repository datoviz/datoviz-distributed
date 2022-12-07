/*************************************************************************************************/
/*  App                                                                                          */
/*************************************************************************************************/


/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "scene/app.h"
#include "../render_utils.h"
#include "client.h"
#include "host.h"
#include "presenter.h"
#include "renderer.h"
#include "scene/scene.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  App functions                                                                                */
/*************************************************************************************************/

DvzApp* dvz_app(DvzBackend backend)
{
    DvzApp* app = (DvzApp*)calloc(1, sizeof(DvzApp));
    app->host = dvz_host(backend);
    app->client = dvz_client(backend);
    return app;
}



DvzDevice* dvz_device(DvzApp* app)
{
    ANN(app);
    ANN(app->host);
    ANN(app->client);

    DvzDevice* device = (DvzDevice*)calloc(1, sizeof(DvzDevice));
    device->app = app;
    device->gpu = make_gpu(app->host);
    device->rd = dvz_renderer(device->gpu, 0);
    device->prt = dvz_presenter(device->rd, app->client, 0);

    return device;
}



void dvz_device_run(DvzDevice* device, DvzScene* scene, uint64_t n_frames)
{
    ANN(device);
    ANN(device->prt);

    ANN(scene);
    ANN(scene->rqr);

    DvzApp* app = device->app;
    ANN(app);
    ANN(app->client);

    // Submit a client event with type REQUESTS and with a pointer to the requester.
    // The Presenter will register a REQUESTS callback sending the requests to the underlying
    // renderer.
    dvz_presenter_submit(device->prt, scene->rqr);

    // Dequeue and process all pending events.
    dvz_client_run(app->client, n_frames);
}



void dvz_device_destroy(DvzDevice* device)
{
    ANN(device);

    dvz_presenter_destroy(device->prt);
    dvz_renderer_destroy(device->rd);
    dvz_gpu_destroy(device->gpu);

    FREE(device);
}



void dvz_app_destroy(DvzApp* app)
{
    ANN(app);
    dvz_client_destroy(app->client);
    dvz_host_destroy(app->host);
    FREE(app);
}
