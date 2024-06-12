/*************************************************************************************************/
/*  Widgets                                                                                      */
/*************************************************************************************************/

#ifndef DVZ_HEADER_WIDGETS
#define DVZ_HEADER_WIDGETS



/*************************************************************************************************/
/*  FPS widget                                                                                   */
/*************************************************************************************************/

static inline void _gui_callback_fps(DvzGuiWindow* gui_window, void* user_data)
{
    ANN(gui_window);
    DvzFps* fps = (DvzFps*)user_data;
    ANN(fps);

    dvz_gui_pos((vec2){100, 100}, DVZ_DIALOG_DEFAULT_PIVOT);
    dvz_gui_corner(DVZ_DIALOG_CORNER_UPPER_RIGHT, (vec2){0, 0});
    dvz_gui_size((vec2){140, 70});
    dvz_gui_alpha(.5f);
    dvz_gui_begin("FPS", dvz_gui_flags(DVZ_DIALOG_FLAGS_OVERLAY));

    dvz_fps_tick(fps);
    dvz_fps_histogram(fps);

    dvz_gui_end();
}



/*************************************************************************************************/
/*  Monitoring widget                                                                            */
/*************************************************************************************************/

static inline void _show_alloc(const char* name, DvzSizePair sizes)
{
    if (sizes[0] > 0)
        dvz_gui_progress(sizes[0] * 1.0 / sizes[1], -1, 0, "%s (%s)", name, pretty_size(sizes[0]));
}

static inline void _gui_callback_monitoring(DvzGuiWindow* gui_window, void* user_data)
{
    ANN(gui_window);
    DvzDatAlloc* datalloc = (DvzDatAlloc*)user_data;
    ANN(datalloc);

    dvz_gui_corner(DVZ_DIALOG_CORNER_UPPER_RIGHT, (vec2){0, 70});
    dvz_gui_size((vec2){230, 80});
    dvz_gui_alpha(.5f);
    dvz_gui_begin("Monitoring", dvz_gui_flags(DVZ_DIALOG_FLAGS_OVERLAY));

    // Retrieve the allocation information.
    DvzAllocMonitor monitor = {0};
    dvz_datalloc_monitoring(datalloc, &monitor);

    // Show the progress bars.
    _show_alloc("Staging", monitor.staging);
    _show_alloc("Vertex", monitor.vertex);
    _show_alloc("Vertex mapped", monitor.vertex_map);
    _show_alloc("Index", monitor.index);
    _show_alloc("Index mapped", monitor.index_map);
    _show_alloc("Storage", monitor.storage);
    _show_alloc("Storage mapped", monitor.storage_map);

    dvz_gui_end();
}



#endif
