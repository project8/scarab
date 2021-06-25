#ifndef SCARAB_PYBIND_BINDING_HELPERS
#define SCARAB_PYBIND_BINDING_HELPERS

#include "pybind11/iostream.h"

#define SCARAB_BIND_CALL_GUARD_STREAMS \
    pybind11::call_guard< pybind11::scoped_ostream_redirect, pybind11::scoped_estream_redirect >()

#define SCARAB_BIND_CALL_GUARD_GIL \
    pybind11::call_guard< pybind11::gil_scoped_release >()

#define SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL \
    pybind11::call_guard< pybind11::scoped_ostream_redirect, pybind11::scoped_estream_redirect, pybind11::gil_scoped_release >()

#endif /* SCARAB_PYBIND_BINDING_HELPERS */
