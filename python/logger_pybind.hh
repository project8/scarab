#ifndef LOGGER_PYBIND_HH_
#define LOGGER_PYBIND_HH_

#include "logger.hh"
#include "scarab_binding_helpers.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    std::list< std::string > export_logger( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        all_members.push_back( "Logger" );
        pybind11::class_< scarab::logger >( mod, "Logger", "Utility class for logging to stdout and stderr" )
            .def( pybind11::init< >(), SCARAB_BIND_CALL_GUARD_STREAMS )

            .def_static( "SetGlobalLevel", &scarab::logger::SetGlobalLevel, "Set the global verbosity level" )
            .def_static( "GetGlobalLevel", &scarab::logger::GetGlobalLevel, "Get the global verbosity level" )
            ;

        pybind11::enum_<scarab::logger::ELevel>(mod, "ELevel")
            .value("eTrace", scarab::logger::ELevel::eTrace)
            .value("eDebug", scarab::logger::ELevel::eDebug)
            .value("eInfo", scarab::logger::ELevel::eInfo)
            .value("eProg", scarab::logger::ELevel::eProg)
            .value("eWarn", scarab::logger::ELevel::eWarn)
            .value("eError", scarab::logger::ELevel::eError)
            .value("eFatal", scarab::logger::ELevel::eFatal)
            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* LOGGER_PYBIND_HH_ */
