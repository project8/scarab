#ifndef LOGGING_PYBIND_HH_
#define LOGGING_PYBIND_HH_

#include "logger.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    using verbosity_t = std::underlying_type< scarab::ELevel >::type;
    int s2py_verbosity( verbosity_t a_verbosity )
    {
        pybind11::module_ pylog = pybind11::module_::import("logging");
        switch( a_verbosity )
        {
            case static_cast< verbosity_t >(scarab::ELevel::eTrace):
                return pylog.attr("DEBUG").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eDebug):
                return pylog.attr("DEBUG").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eInfo):
                return pylog.attr("INFO").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eProg):
                return pylog.attr("INFO").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eWarn):
                return pylog.attr("WARNING").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eError):
                return pylog.attr("ERROR").cast<int>();
            case static_cast< verbosity_t >(scarab::ELevel::eFatal):
                return pylog.attr("CRITICAL").cast<int>();
            default:
                return pylog.attr("NOTSET").cast<int>();
        }
    }

    std::list< std::string > export_logging( pybind11::module& mod )
    {
        std::list< std::string> all_members;

        all_members.push_back( "s2py_verbosity" );
        mod.def( "s2py_verbosity",
                 &s2py_verbosity,
                 pybind11::arg("verbosity"),
                 "Convert Scarab's logging verbosity to Python's logging level." );

        all_members.push_back( "stop_logging" );
        mod.def( "stop_logging",
                &scarab::logger::stop_quill,
                "Stop the C++ logging thread -- Use this at the end of your application." );


        return all_members;
    }
}

#endif /* LOGGING_PYBIND_HH_ */
