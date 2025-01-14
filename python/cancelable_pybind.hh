#ifndef CANCELABLE_PYBIND_HH_
#define CANCELABLE_PYBIND_HH_

#include "cancelable.hh"
#include "scarab_binding_helpers.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    // utility class to expose do_cancellation and do_reset_cancellation so they can be bound
    class cancelable_publicist : public scarab::cancelable
    {
        public:
            using scarab::cancelable::do_cancellation;
            using scarab::cancelable::do_reset_cancellation;
    };

    class cancelable_trampoline : public scarab::cancelable, public pybind11::trampoline_self_life_support
    {
        public:
            using scarab::cancelable::cancelable;

            void do_cancellation( int a_code ) override 
            {
                PYBIND11_OVERRIDE(void, scarab::cancelable, do_cancellation, a_code );
            }

            void do_reset_cancellation() override 
            {
                PYBIND11_OVERRIDE(void, scarab::cancelable, do_reset_cancellation, );
            }

    };

    std::list< std::string > export_cancelable( pybind11::module& mod )
    {
        std::list< std::string> all_members;

        all_members.push_back( "Cancelable" );
        pybind11::classh< scarab::cancelable, cancelable_trampoline >( mod, "Cancelable" )
            .def( pybind11::init< >() )

            .def( "cancel", &scarab::cancelable::cancel, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "reset_cancel", &scarab::cancelable::reset_cancel, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "is_canceled", &scarab::cancelable::is_canceled )

            .def( "wait", &scarab::cancelable::wait, 
                  SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL,
                  pybind11::arg("wait_ms"),
                  pybind11::arg("check_ms") = 500
                )

            .def( "do_cancellation", &cancelable_publicist::do_cancellation, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "do_reset_cancellation", &cancelable_publicist::do_reset_cancellation, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            ;

        return all_members;
    }
}

#endif /* CANCELABLE_PYBIND_HH_ */
