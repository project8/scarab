#ifndef SIGNAL_HANDLER_PYBIND_HH_
#define SIGNAL_HANDLER_PYBIND_HH_

#include "signal_handler.hh"
#include "scarab_binding_helpers.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    std::list< std::string > export_signal_handler( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        all_members.push_back( "cancel_all" );
        mod.def( "cancel_all", [](int a_code){ return scarab::signal_handler::cancel_all( a_code ); },
                    SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL,
                    "Asynchronous call to exit the process with the given exit code" );

        all_members.push_back( "SignalHandler" );
        pybind11::class_< scarab::signal_handler >( mod, "SignalHandler", "handle system signals and pass cancel status to associated objects" )
            .def( pybind11::init< >(), SCARAB_BIND_CALL_GUARD_STREAMS )

            .def( "add_cancelable", &scarab::signal_handler::add_cancelable, "add a cancelable object to the list to be canceled in the event of a SIGINT" )
            .def( "remove_cancelable", static_cast<void (scarab::signal_handler::*)(std::shared_ptr<scarab::cancelable>)>( &scarab::signal_handler::remove_cancelable ), "remove a cancelable object from the list to be canceled in the event of a SIGINT" )
            .def( "reset", &scarab::signal_handler::reset, "remove all cancelable objects", SCARAB_BIND_CALL_GUARD_STREAMS )
            .def_static( "cancel_all", &scarab::signal_handler::cancel_all, "cancel all cancelable objects known to the signal_handler", SCARAB_BIND_CALL_GUARD_STREAMS )
            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* SIGNAL_HANDLER_PYBIND_HH_ */
