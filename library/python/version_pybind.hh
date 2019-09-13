#ifndef VERSION_PYBIND_HH_
#define VERSION_PYBIND_HH_

#include "version_base.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_version( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "VersionSemantic" );
        pybind11::class_< scarab::version_semantic >( mod, "VersionSemantic", "data structure for representing semantic version information" )
            .def( pybind11::init<>() )
            .def( pybind11::init< unsigned, unsigned, unsigned >(),
                    pybind11::arg( "major"),
                    pybind11::arg( "minor"),
                    pybind11::arg( "patch"))
            .def( pybind11::init< const scarab::version_semantic& >() )

            .def( "__eq__", &scarab::version_semantic::operator== )
            .def( "__lt__", &scarab::version_semantic::operator< )
            //TODO: base class doesn't seem to have a __gt__ == operator>, can we add it?
        ;

        return all_items;
    }

} /* namespace scarab_pybind */

#endif /* VERSION_PYBIND_HH_ */
