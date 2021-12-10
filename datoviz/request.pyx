# cython: c_string_type=unicode, c_string_encoding=ascii

# -------------------------------------------------------------------------------------------------
# Imports
# -------------------------------------------------------------------------------------------------

from . cimport request as rq
import logging


logger = logging.getLogger('datoviz')


# -------------------------------------------------------------------------------------------------
# Requester
# -------------------------------------------------------------------------------------------------

cdef class Requester:
    cdef rq.DvzRequester _c_rqr

    def __cinit__(self):
        self._c_rqr = rq.dvz_requester()

    def __dealloc__(self):
        self.destroy()

    def destroy(self):
        """Destroy the requester."""
        rq.dvz_requester_destroy(&self._c_rqr)

    def create_board(self, int width, int height):
        cdef rq.DvzRequest req = rq.dvz_create_board(&self._c_rqr, width, height, 0)
        rq.dvz_request_print(&req)
