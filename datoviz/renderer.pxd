# WARNING: parts of this file are auto-generated

from ._types cimport *


cdef class Renderer:
    cdef DvzRenderer * _c_rd
    cdef DvzGpu * _c_gpu


cdef extern from "<datoviz/renderer.h>":
    # Semi-opaque structs:

    ctypedef struct DvzRequest:
        pass

    ctypedef struct DvzRenderer:
        pass

    ctypedef struct VkPhysicalDeviceFeatures:
        pass

    ctypedef struct DvzHost:
        pass

    ctypedef struct DvzGpu:
        DvzHost * host

    ctypedef struct DvzBoard:
        uint32_t width
        uint32_t height

    ctypedef struct DvzCanvas:
        uint32_t width
        uint32_t height

    ctypedef struct DvzPipe:
        pass

    ctypedef struct DvzDat:
        pass

    ctypedef struct DvzTex:
        pass

    # ---------------------------------------------------------------------------------------------
    # ---------------------------------------------------------------------------------------------
    # ---------------------------------------------------------------------------------------------
    # !!!!!!!!!!!!!!!!!!!! AUTOMATICALLY-GENERATED PART: DO NOT EDIT MANUALLY !!!!!!!!!!!!!!!!!!!!
    # ---------------------------------------------------------------------------------------------
    # ---------------------------------------------------------------------------------------------
    # ---------------------------------------------------------------------------------------------

    # Structures
    # ---------------------------------------------------------------------------------------------

    # STRUCT START

    # STRUCT END

    # Functions
    # ---------------------------------------------------------------------------------------------

    # FUNCTION START
    DvzHost* dvz_host(DvzBackend backend)

    void dvz_host_wait(DvzHost* host)

    int dvz_host_destroy(DvzHost* host)

    DvzRenderer* dvz_renderer(DvzGpu* gpu, int flags)

    void dvz_renderer_request(DvzRenderer* rd, DvzRequest req)

    void dvz_renderer_requests(DvzRenderer* rd, uint32_t count, DvzRequest* reqs)

    DvzBoard* dvz_renderer_board(DvzRenderer* rd, DvzId id)

    DvzCanvas* dvz_renderer_canvas(DvzRenderer* rd, DvzId id)

    DvzDat* dvz_renderer_dat(DvzRenderer* rd, DvzId id)

    DvzTex* dvz_renderer_tex(DvzRenderer* rd, DvzId id)

    DvzPipe* dvz_renderer_pipe(DvzRenderer* rd, DvzId id)

    uint8_t* dvz_renderer_image(DvzRenderer* rd, DvzId board_id, DvzSize* size, uint8_t* rgb)

    void dvz_renderer_destroy(DvzRenderer* rd)


    # FUNCTION END
