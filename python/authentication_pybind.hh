#ifndef AUTHENTICATION_PYBIND_HH_
#define AUTHENTICATION_PYBIND_HH_

#include "authentication.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{

    std::list< std::string > export_authentication( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        using has_sig1 = bool(scarab::authentication::*)( const std::string& ) const;
        using has_sig2 = bool(scarab::authentication::*)( const std::string&, const std::string& ) const;
        using get_sig1 = std::string(scarab::authentication::*)( const std::string&, const std::string& ) const;
        using get_sig2 = std::string(scarab::authentication::*)( const std::string&, const std::string&, const std::string& ) const;

        all_items.push_back( "Authentication" );
        pybind11::class_< scarab::authentication >( mod, "Authentication", "class for defining and processing authentication information" )
            .def( pybind11::init<>() )
            .def( pybind11::init< const scarab::authentication& >() )

            .def( "add_group", &scarab::authentication::add_group, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_item", &scarab::authentication::add_item, pybind11::arg("group"), pybind11::arg("name"), pybind11::arg("default"), pybind11::arg("env")="", SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_groups", &scarab::authentication::add_groups, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "set_override_file", &scarab::authentication::set_override_file, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "set_override_value", &scarab::authentication::set_override_value, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "set_auth_file", &scarab::authentication::set_auth_file, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "process_spec", &scarab::authentication::process_spec, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def_property( "spec", (scarab::param_node& (scarab::authentication::*)()) &scarab::authentication::spec,
                           [](scarab::authentication& an_obj, const scarab::param_node& a_spec ) {an_obj.spec() = a_spec; } )
            .def( "has", static_cast<has_sig1>(&scarab::authentication::has) )
            .def( "has", static_cast<has_sig2>(&scarab::authentication::has) )
            .def( "get", static_cast<get_sig1>(&scarab::authentication::get) )
            .def( "get", static_cast<get_sig2>(&scarab::authentication::get) )
            .def_property( "data", (scarab::param_node& (scarab::authentication::*)()) &scarab::authentication::data,
                           [](scarab::authentication& an_obj, const scarab::param_node& a_data ) {an_obj.data() = a_data; } )

        ;

        return all_items;
    }

} /* namespace scarab_pybind */

#endif /* AUTHENTICATION_PYBIND_HH_ */
