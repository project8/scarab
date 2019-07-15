/*
 * scarab_pybind.cc
 *
 * Created on: April 18, 2019
 *     Author: B.H. LaRoque
 */

#include "param_pybind.hh"
#include "param_value_pybind.hh"
#include "param_array_pybind.hh"
#include "param_node_pybind.hh"

PYBIND11_MODULE( scarab, scarab_mod )
{
    pybind11::module param_mod = scarab_mod.def_submodule( "param", "Param data structures" );
    scarab_pybind::export_param( param_mod );
    scarab_pybind::export_param_value( param_mod );
    scarab_pybind::export_param_array( param_mod );
    scarab_pybind::export_param_node( param_mod );
}
