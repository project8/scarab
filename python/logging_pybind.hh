#ifndef LOGGING_PYBIND_HH_
#define LOGGING_PYBIND_HH_

#include "logging.hh"
#include "scarab_binding_helpers.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    std::list< std::string > export_logging( pybind11::module& mod )
    {
        std::list< std::string> all_members;

        all_members.push_back( "stop_logging" );

        return all_members;
    }
}

#endif /* LOGGING_PYBIND_HH_ */
