/*************************************************************************************************/
/*  Testing input                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "test_input.h"
#include "glfw_utils.h"
#include "input.h"
#include "keyboard.h"
#include "mouse.h"
#include "test.h"
#include "testing.h"
#include "time.h"



/*************************************************************************************************/
/*  Macros                                                                                       */
/*************************************************************************************************/

#define DEBUG_TEST (getenv("DVZ_DEBUG") != NULL)
#define N_FRAMES   (DEBUG_TEST ? 0 : 5)



/*************************************************************************************************/
/*  Input tests                                                                                  */
/*************************************************************************************************/

static void _on_mouse(DvzMouse* mouse, DvzMouseEvent ev)
{
    ASSERT(mouse != NULL);

    switch (ev.type)
    {

    case DVZ_MOUSE_EVENT_CLICK:
        log_debug(
            "mouse click button #%d at (%.1f, %.1f), mods %d", //
            ev.content.c.button, ev.content.c.pos[0], ev.content.c.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_DOUBLE_CLICK:
        log_debug(
            "mouse double click button #%d at (%.1f, %.1f), mods %d", //
            ev.content.c.button, ev.content.c.pos[0], ev.content.c.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_DRAG:
        log_debug(
            "mouse drag button #%d at (%.1f, %.1f), mods %d", //
            ev.content.d.button, ev.content.d.pos[0], ev.content.d.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_DRAG_START:
        log_debug(
            "mouse drag start button #%d at (%.1f, %.1f), mods %d", //
            ev.content.d.button, ev.content.d.pos[0], ev.content.d.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_DRAG_STOP:
        log_debug(
            "mouse drag stop button #%d at (%.1f, %.1f), mods %d", //
            ev.content.d.button, ev.content.d.pos[0], ev.content.d.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_MOVE:
        log_debug(
            "mouse move to (%.1f, %.1f), mods %d", //
            ev.content.m.pos[0], ev.content.m.pos[1], ev.mods);
        break;

    case DVZ_MOUSE_EVENT_PRESS:
        log_debug("mouse press button #%d, mods %d", ev.content.b.button, ev.mods);
        break;

    case DVZ_MOUSE_EVENT_RELEASE:
        log_debug("mouse release button #%d, mods %d", ev.content.b.button, ev.mods);
        break;

    case DVZ_MOUSE_EVENT_WHEEL:
        log_debug(
            "mouse wheel at (%.1f, %.1f), dir (%.1f, %.1f), mods %d", //
            ev.content.w.pos[0], ev.content.w.pos[1], ev.content.w.dir[0], ev.content.w.dir[1],
            ev.mods);
        break;

    default:
        break;
    }
}

int test_input_mouse(TstSuite* suite)
{
    // Create window and input.
    DvzWindow window = dvz_window(DVZ_BACKEND_GLFW, 800, 600, 0);
    DvzInput* input = dvz_input(&window);

    // Retrieve the mouse and keyboard.
    DvzMouse* mouse = dvz_input_mouse(input);

    // Callbacks.
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_CLICK, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_DOUBLE_CLICK, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_DRAG, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_DRAG_START, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_DRAG_STOP, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_MOVE, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_PRESS, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_RELEASE, _on_mouse, NULL);
    dvz_mouse_callback(mouse, DVZ_MOUSE_EVENT_WHEEL, _on_mouse, NULL);

    // Main loop.
    backend_loop(&window, N_FRAMES);

    // Cleanup.
    dvz_input_destroy(input);
    dvz_window_destroy(&window);
    return 0;
}



static void _on_keyboard(DvzKeyboard* keyboard, DvzKeyboardEvent ev)
{
    ASSERT(keyboard != NULL);

    switch (ev.type)
    {

    case DVZ_KEYBOARD_EVENT_PRESS:
        log_debug("keyboard press key %d, mods %d", ev.key, ev.mods);
        break;

    case DVZ_KEYBOARD_EVENT_RELEASE:
        log_debug("keyboard release key %d, mods %d", ev.key, ev.mods);
        break;

    default:
        break;
    }

    log_debug(
        "keys pressed: %d %d %d %d", //
        dvz_keyboard_get(keyboard, 0), dvz_keyboard_get(keyboard, 1),
        dvz_keyboard_get(keyboard, 2), dvz_keyboard_get(keyboard, 3));
}

int test_input_keyboard(TstSuite* suite)
{
    // Create window and input.
    DvzWindow window = dvz_window(DVZ_BACKEND_GLFW, 800, 600, 0);
    DvzInput* input = dvz_input(&window);

    // Retrieve the keyboard and keyboard.
    DvzKeyboard* keyboard = dvz_input_keyboard(input);

    // Callbacks.
    dvz_keyboard_callback(keyboard, DVZ_KEYBOARD_EVENT_PRESS, _on_keyboard, NULL);
    dvz_keyboard_callback(keyboard, DVZ_KEYBOARD_EVENT_RELEASE, _on_keyboard, NULL);

    // Main loop.
    backend_loop(&window, N_FRAMES);

    // Cleanup.
    dvz_input_destroy(input);
    dvz_window_destroy(&window);
    return 0;
}
