/*
 * param_binding_helpers.hh
 *
 *  Created on: Jan 23, 2020
 *      Author: N. Oblath, L. Gladstone, B.H. LaRoque
 */

#ifndef PARAM_BINDING_HELPERS_HH_
#define PARAM_BINDING_HELPERS_HH_

#include <algorithm> //for std::replace on string

#include "param.hh"
#include "error.hh"

#include "pybind11/pybind11.h"
#include "pybind11/pytypes.h"


namespace scarab_pybind
{

    scarab::param_ptr_t to_param( const pybind11::object& an_object, bool hyphenate_keys=false );
    pybind11::object to_python( const scarab::param& a_param, bool underscore_keys = false );

} /* namespace scarab_pybind */
#endif /* PARAM_BINDING_HLEPERS_HH_ */
