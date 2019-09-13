#ifndef SIGNAL_HANDLER_PYBIND_HH_
#define SIGNAL_HANDLER_PYBIND_HH_

#include "signal_handler.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    void export_signal_handler( pybind11::module& mod )
    {
        mod.def( "cancel_all", [](int a_code){ return scarab::signal_handler::cancel_all( a_code ); },
                    pybind11::call_guard< pybind11::gil_scoped_release >(),
                   "Asynchronous call to exit the process with the given exit code" );


        pybind11::class_< scarab::signal_handler >( mod, "SignalHandler", "handle system signals and pass cancel status to associated objects" )
            .def( pybind11::init< >() )

            .def( "add_cancelable", &scarab::signal_handler::add_cancelable, "add a cancelable object to the list to be canceled in the event of a SIGINT" )
            .def( "remove_cancelable", &scarab::signal_handler::remove_cancelable, "remove a cancelable object from the list to be canceled in the event of a SIGINT" )
            .def( "reset", &scarab::signal_handler::reset, "remove all cancelable objects" )

            ;

    }
} /* namespace scarab_pybind */
#endif /* SIGNAL_HANDLER_PYBIND_HH_ */
