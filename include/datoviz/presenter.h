/*************************************************************************************************/
/*  Presenter                                                                                    */
/*************************************************************************************************/

#ifndef DVZ_HEADER_PRESENTER
#define DVZ_HEADER_PRESENTER



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "canvas.h"
#include "client.h"
#include "fps.h"
#include "gui.h"
#include "renderer.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzPresenter DvzPresenter;
typedef struct DvzGuiCallbackPayload DvzGuiCallbackPayload;

// Forward declarations.
typedef struct DvzList DvzList;

// Callback types.
typedef void (*DvzGuiCallback)(DvzGuiWindow* gui_window, void* user_data);



/*************************************************************************************************/
/*  Event structs                                                                                */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzGuiCallbackPayload
{
    DvzId window_id;
    DvzGuiCallback callback;
    void* user_data;
};



struct DvzPresenter
{
    DvzRenderer* rd;
    DvzClient* client;
    int flags;

    // HACK: need to keep a list of surfaces so that we can delete them when the presenter is
    // destroyed.
    DvzList* surfaces;

    // GUI callbacks.
    DvzGui* gui;
    DvzList* callbacks;

    DvzFps fps;

    // Mappings.
    struct
    {
        DvzMap* guis;
    } maps;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Functions                                                                                    */
/*************************************************************************************************/

DVZ_EXPORT DvzPresenter* dvz_presenter(DvzRenderer* rd, DvzClient* client, int flags);



DVZ_EXPORT void dvz_presenter_frame(DvzPresenter* prt, DvzId window_id);



DVZ_EXPORT void
dvz_presenter_gui(DvzPresenter* prt, DvzId window_id, DvzGuiCallback callback, void* user_data);



DVZ_EXPORT void dvz_presenter_submit(DvzPresenter* prt, DvzRequester* rqr);



DVZ_EXPORT void dvz_presenter_destroy(DvzPresenter* prt);



EXTERN_C_OFF

#endif
