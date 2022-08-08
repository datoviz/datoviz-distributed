/*************************************************************************************************/
/*  Client input                                                                                 */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "client_input.h"
#include "client.h"
#include "client_utils.h"
#include "common.h"
#include "fifo.h"
#include "glfw_utils.h"
#include "input.h"
#include "keyboard.h"
#include "mouse.h"
#include "window.h"



/*************************************************************************************************/
/*  Input callbacks                                                                              */
/*************************************************************************************************/

static void _on_mouse(DvzMouse* mouse, DvzMouseEvent ev)
{
    ANN(mouse);

    DvzWindow* window = (DvzWindow*)ev.user_data;
    ANN(window);

    DvzClient* client = window->client;
    ANN(client);

    DvzClientEvent cev = {0};
    cev.type = DVZ_CLIENT_EVENT_MOUSE;
    cev.window_id = window->obj.id;
    cev.content.m = ev;
    dvz_client_event(client, cev);
}

static void _on_keyboard(DvzKeyboard* keyboard, DvzKeyboardEvent ev)
{
    ANN(keyboard);

    DvzWindow* window = (DvzWindow*)ev.user_data;
    ANN(window);

    DvzClient* client = window->client;
    ANN(client);

    DvzClientEvent cev = {0};
    cev.type = DVZ_CLIENT_EVENT_KEYBOARD;
    cev.window_id = window->obj.id;
    cev.content.k = ev;
    dvz_client_event(client, cev);
}



/*************************************************************************************************/
/*  Window creation client callback                                                              */
/*************************************************************************************************/

static void _create_window_input(DvzDeq* deq, void* item, void* user_data)
{
    ANN(deq);

    DvzClient* client = (DvzClient*)user_data;
    ANN(client);

    DvzClientEvent* ev = (DvzClientEvent*)item;
    ANN(ev);
    ASSERT(ev->type == DVZ_CLIENT_EVENT_WINDOW_CREATE);

    DvzId id = ev->window_id;

    // Retrieve the created window with id2window().
    DvzWindow* window = id2window(client, id);
    ANN(window);

    // Create the input and associate it to the window.
    // This call uses the GLFW callback functions to update the Mouse and Keyboard state machines.
    window->input = dvz_input(window);

    // Register mouse callbacks that enqueue events to the client queue.
    DvzMouse* mouse = dvz_input_mouse(window->input);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_ALL, _on_mouse, window);

    // Register keyboard callbacks that enqueue events to the client queue.
    DvzKeyboard* keyboard = dvz_input_keyboard(window->input);
    dvz_keyboard_callback(keyboard, DVZ_KEYBOARD_EVENT_PRESS, _on_keyboard, window);
    dvz_keyboard_callback(keyboard, DVZ_KEYBOARD_EVENT_RELEASE, _on_keyboard, window);
}



/*************************************************************************************************/
/*  Client input functions                                                                       */
/*************************************************************************************************/

void dvz_client_input(DvzClient* client)
{
    ANN(client);

    // Register a window_create callback that is called after the default one.
    dvz_deq_callback(
        client->deq, 0, (int)DVZ_CLIENT_EVENT_WINDOW_CREATE, _create_window_input, client);
}
