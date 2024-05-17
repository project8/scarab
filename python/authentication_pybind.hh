#ifndef AUTHENTICATION_PYBIND_HH_
#define AUTHENTICATION_PYBIND_HH_

#include "authentication.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_authentication( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "Authentication" );
        pybind11::class_< scarab::authentication >( mod, "Authentication", "class for defining and processing authentication information" )
            .def( pybind11::init<>() )
            .def( pybind11::init< const scarab::authentication& >() )

            .def( "add_group", &scarab::authentication::add_group )
            .def( "add_item", &scarab::authentication::add_item, pybind11::arg("group"), pybind11::arg("name"), pybind11::arg("default"), pybind11::arg("env")="" )
            .def( "add_groups", &scarab::authentication::add_groups )
            .def( "set_auth_file", &scarab::authentication::set_auth_file )
            .def( "process_spec", &scarab::authentication::process_spec )
            .def_property( "spec", (scarab::param_node& (scarab::authentication::*)()) &scarab::authentication::spec,
                           [](scarab::authentication& an_obj, const scarab::param_node& a_spec ) {an_obj.spec() = a_spec; } )

            .def( "get", &scarab::authentication::get, pybind11::arg("group"), pybind11::arg("item") )
            .def_property( "data", (scarab::param_node& (scarab::authentication::*)()) &scarab::authentication::data,
                           [](scarab::authentication& an_obj, const scarab::param_node& a_data ) {an_obj.data() = a_data; } )

        ;

        return all_items;
    }

} /* namespace scarab_pybind */

#endif /* AUTHENTICATION_PYBIND_HH_ */
