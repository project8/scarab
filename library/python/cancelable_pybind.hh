#include "cancelable.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    void export_cancelable( pybind11::module& mod )
    {

        pybind11::class_< scarab::cancelable >( mod, "Cancelable" )
            .def( pybind11::init< >() )

            .def( "cancel", &scarab::cancelable::cancel )
            .def( "reset_cancel", &scarab::cancelable::reset_cancel )
            .def( "is_canceled", &scarab::cancelable::is_canceled )

            ;

    }
}
