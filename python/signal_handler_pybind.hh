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

        all_members.push_back( "SignalHandler" );
        pybind11::class_< scarab::signal_handler >( mod, "SignalHandler", "handle system signals and pass cancel status to associated objects" )
            .def( pybind11::init< >(), 
                  pybind11::return_value_policy::take_ownership,
                  SCARAB_BIND_CALL_GUARD_STREAMS )

//        .def("set_wp_potentially_slicing",
//             [](WpOwner<VB> &self, py::handle obj) {
//                 self.set_wp(py::potentially_slicing_weak_ptr<VB>(obj));
//             })
            .def_static( "add_cancelable", 
                         [](pybind11::handle a_cancelable) {
                             scarab::signal_handler::add_cancelable(pybind11::potentially_slicing_weak_ptr<scarab::cancelable>(a_cancelable).lock());
                         },
                         "add a cancelable object to the list to be canceled in the event of a SIGINT" )
            .def_static( "remove_cancelable", static_cast<void (*)(std::shared_ptr<scarab::cancelable>)>( &scarab::signal_handler::remove_cancelable ), "remove a cancelable object from the list to be canceled in the event of a SIGINT" )
            .def_static( "reset", &scarab::signal_handler::reset, "remove all cancelable objects", SCARAB_BIND_CALL_GUARD_STREAMS )
            .def_static( "cancel_all", &scarab::signal_handler::cancel_all, "cancel all cancelable objects known to the signal_handler", SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def_static( "print_cancelables", &scarab::signal_handler::print_cancelables, "print pointers to the cancelables known to the signal_handler", SCARAB_BIND_CALL_GUARD_STREAMS )
            .def_static( "start_waiting_thread", &scarab::signal_handler::start_waiting_thread, "starts the thread that waits for signals", SCARAB_BIND_CALL_GUARD_STREAMS )
            .def_static( "join_waiting_thread", &scarab::signal_handler::join_waiting_thread, "joins the thread that waits for signals", SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* SIGNAL_HANDLER_PYBIND_HH_ */
