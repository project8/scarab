/*
 * scarab_namespace_pybind.cc
 *
 * Created on: April 18, 2019
 *     Author: B.H. LaRoque
 */

#include "pybind11/stl.h"

#ifdef BUILD_CLI_PYBINDING
#include "application_pybind.hh"
#endif

#include "cancelable_pybind.hh"
#include "signal_handler_pybind.hh"
#include "version_pybind.hh"

#ifdef BUILD_PARAM_PYBINDING
#include "param_pybind.hh"
#include "param_value_pybind.hh"
#include "param_array_pybind.hh"
#include "param_node_pybind.hh"
#endif

PYBIND11_MODULE( scarab, scarab_mod )
{
    std::list< std::string > all_members;
    // cancelable
    all_members.splice( all_members.end(), scarab_pybind::export_cancelable( scarab_mod ) );
    // signal handler
    all_members.splice( all_members.end(), scarab_pybind::export_signal_handler( scarab_mod ) );
#ifdef BUILD_CLI_PYBINDING
    // application
    all_members.splice( all_members.end(), scarab_pybind::export_application( scarab_mod ) );
#endif
    // utility
    all_members.splice( all_members.end(), scarab_pybind::export_version( scarab_mod ) );
#ifdef BUILD_PARAM_PYBINDING
    // Param-related data objects
    all_members.splice( all_members.end(), scarab_pybind::export_param( scarab_mod ) );
    all_members.splice( all_members.end(), scarab_pybind::export_param_value( scarab_mod ) );
    all_members.splice( all_members.end(), scarab_pybind::export_param_array( scarab_mod ) );
    all_members.splice( all_members.end(), scarab_pybind::export_param_node( scarab_mod ) );
#endif
    scarab_mod.attr( "__all__" ) = all_members;
}
