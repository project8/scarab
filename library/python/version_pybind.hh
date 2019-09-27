#ifndef VERSION_PYBIND_HH_
#define VERSION_PYBIND_HH_

#include "version_semantic.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_version( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "VersionSemantic" );
        pybind11::class_< scarab::version_semantic, std::shared_ptr< scarab::version_semantic > >( mod, "VersionSemantic", "data structure for representing semantic version information" )
            .def( pybind11::init<>() )
            .def( pybind11::init< unsigned, unsigned, unsigned >(),
                    pybind11::arg( "major"),
                    pybind11::arg( "minor"),
                    pybind11::arg( "patch"))
            .def( pybind11::init< const scarab::version_semantic& >() )

            .def( "__eq__", &scarab::version_semantic::operator== )
            .def( "__lt__", &scarab::version_semantic::operator< )
            .def( "__gt__", &scarab::version_semantic::operator> )

            .def_property_readonly( "major_version", &scarab::version_semantic::major_version )
            .def_property_readonly( "minor_version", &scarab::version_semantic::minor_version )
            .def_property_readonly( "patch_version", &scarab::version_semantic::patch_version )
            .def_property_readonly( "version", &scarab::version_semantic::version_str )

            .def_property_readonly( "package", (std::string& (scarab::version_semantic::*)()) &scarab::version_semantic::package )
            .def_property_readonly( "commit", (std::string& (scarab::version_semantic::*)()) &scarab::version_semantic::commit )

            .def_property_readonly( "exe_name", (std::string& (scarab::version_semantic::*)()) &scarab::version_semantic::exe_name )
            .def_property_readonly( "hostname", (std::string& (scarab::version_semantic::*)()) &scarab::version_semantic::hostname )
            .def_property_readonly( "username", (std::string& (scarab::version_semantic::*)()) &scarab::version_semantic::username )

            .def_property_readonly( "version_info", &scarab::version_semantic::version_info_string )

            .def( "parse", &scarab::version_semantic::parse, "parse a version string into this object" )
            .def( "combine", &scarab::version_semantic::combine, pybind11::arg( "major" ), pybind11::arg( "minor" ), pybind11::arg( "patch" ), "combine integer represetnations of major, minor, and patch into object")
            .def( "update_exe_host_user", &scarab::version_semantic::update_exe_host_user, "attempt to determine execution properties: executable name, username, hostname" )
        ;

        return all_items;
    }

} /* namespace scarab_pybind */

#endif /* VERSION_PYBIND_HH_ */
