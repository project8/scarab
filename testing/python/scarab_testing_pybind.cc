/*
 * scarab_testing_namespace_pybind.cc
 *
 * Created on: April 23, 2025
 *     Author: N.S. Oblath
 */

#include "pybind11/stl.h"

#include "test_weak_holder_pybind.hh"


PYBIND11_MODULE( scarab_testing, scarab_mod )
{
    std::list< std::string > all_members;
    // utility
    all_members.splice( all_members.end(), scarab_testing_pybind::export_test_weak_holder( scarab_mod ) );

    scarab_mod.attr( "__all__" ) = all_members;
}
