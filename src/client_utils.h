/*************************************************************************************************/
/*  Utils                                                                                        */
/*************************************************************************************************/

#ifndef DVZ_HEADER_CLIENT_UTILS
#define DVZ_HEADER_CLIENT_UTILS



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "client.h"
#include "common.h"
#include "glfw_utils.h"
#include "window.h"



/*************************************************************************************************/
/*  Client utils                                                                                 */
/*************************************************************************************************/

static DvzWindow* id2window(DvzClient* client, DvzId id)
{
    ANN(client);
    DvzWindow* window = dvz_map_get(client->map, id);
    return window;
}



static DvzId window2id(DvzWindow* window)
{
    ANN(window);
    return (DvzId)window->obj.id;
}



static DvzWindow*
create_client_window(DvzClient* client, DvzId id, uint32_t width, uint32_t height, int flags)
{
    ANN(client);
    ASSERT(width > 0);
    ASSERT(height > 0);
    ASSERT(id != DVZ_ID_NONE);

    DvzWindow* window = (DvzWindow*)dvz_container_alloc(&client->windows);
    *window = dvz_window(client->backend, width, height, flags);

    // Register the window id.
    window->obj.id = (uint64_t)id;
    window->client = client;
    dvz_map_add(client->map, id, DVZ_OBJECT_TYPE_WINDOW, window);

    return window;
}



static void delete_client_window(DvzClient* client, DvzId id)
{
    ANN(client);
    ASSERT(id != DVZ_ID_NONE);

    DvzWindow* window = id2window(client, id);
    if (window == NULL)
    {
        log_warn("window #%d not found", id);
        return;
    }
    ANN(window);

    dvz_map_remove(client->map, id);
    dvz_window_destroy(window);
}



#endif
