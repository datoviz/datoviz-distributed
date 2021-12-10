# WARNING: parts of this file are auto-generated

from ._types cimport *

cdef extern from "<datoviz/request.h>":
    # Semi-opaque structs:

    ctypedef struct DvzRequester:
        pass

    ctypedef struct DvzRequest:
        DvzId id

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
    DvzRequester dvz_requester()

    void dvz_requester_destroy(DvzRequester* rqr)

    void dvz_requester_begin(DvzRequester* rqr)

    void dvz_requester_add(DvzRequester* rqr, DvzRequest req)

    DvzRequest* dvz_requester_end(DvzRequester* rqr, uint32_t* count)

    void dvz_request_print(DvzRequest* req)

    DvzRequest dvz_create_board(DvzRequester* rqr, uint32_t width, uint32_t height, int flags)

    DvzRequest dvz_update_board(DvzRequester* rqr, DvzId id)

    DvzRequest dvz_delete_board(DvzRequester* rqr, DvzId id)

    DvzRequest dvz_create_dat(DvzRequester* rqr, DvzBufferType type, DvzSize size, int flags)

    DvzRequest dvz_create_tex(DvzRequester* rqr, DvzTexDims dims, uvec3 shape, DvzFormat format, int flags)

    DvzRequest dvz_create_graphics(DvzRequester* rqr, DvzId board, DvzGraphicsType type, int flags)

    DvzRequest dvz_set_vertex(DvzRequester* rqr, DvzId graphics, DvzId dat)

    DvzRequest dvz_upload_dat(DvzRequester* rqr, DvzId dat, DvzSize offset, DvzSize size, void* data)

    DvzRequest dvz_set_begin(DvzRequester* rqr, DvzId board)

    DvzRequest dvz_set_viewport(DvzRequester* rqr, DvzId board, vec2 offset, vec2 shape)

    DvzRequest dvz_set_draw(DvzRequester* rqr, DvzId board, DvzId graphics, uint32_t first_vertex, uint32_t vertex_count)

    DvzRequest dvz_set_end(DvzRequester* rqr, DvzId board)


    # FUNCTION END