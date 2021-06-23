#ifndef CANCELABLE_PYBIND_HH_
#define CANCELABLE_PYBIND_HH_

#include "cancelable.hh"
#include "scarab_binding_helpers.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    std::list< std::string > export_cancelable( pybind11::module& mod )
    {
        std::list< std::string> all_members;

        all_members.push_back( "Cancelable" );
        pybind11::class_< scarab::cancelable, std::shared_ptr<scarab::cancelable> >( mod, "Cancelable" )
            .def( pybind11::init< >() )

            .def( "cancel", &scarab::cancelable::cancel, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "reset_cancel", &scarab::cancelable::reset_cancel, SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "is_canceled", &scarab::cancelable::is_canceled )

            ;

        return all_members;
    }
}

#endif /* CANCELABLE_PYBIND_HH_ */
