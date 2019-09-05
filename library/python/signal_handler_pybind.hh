#include "signal_handler.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    void export_signal_handler( pybind11::module& mod )
    {
        mod.def( "cancel_all", [](int a_code){ return scarab::signal_handler::cancel_all( a_code ); },
                    pybind11::call_guard< pybind11::gil_scoped_release >(),
                   "Asynchronous call to exit the process with the given exit code" );


        pybind11::class_< scarab::signal_handler >( mod, "SignalHandler" )
            .def( pybind11::init< >() )
            
            .def( "add_cancelable", &scarab::signal_handler::add_cancelable )
            .def( "remove_cancelable", &scarab::signal_handler::remove_cancelable )
            .def( "reset", &scarab::signal_handler::reset )

            ;

    }
} /* namespace scarab_pybind */
