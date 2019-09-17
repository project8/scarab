#ifndef APPLICATION_PYBIND_HH_
#define APPLICATION_PYBIND_HH_

#include "application.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    std::list< std::string > export_application( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        //TODO: this class does not actually belong in this namespace... where should it go?
        pybind11::class_< CLI::Option >( mod, "_CliOption", "non-scarab class" );

        all_members.push_back( "MainApp" );
        pybind11::class_< scarab::main_app >( mod, "MainApp", "Base class for creating CLI utilities" )
            .def( pybind11::init< >() )

            // member variables
            .def_property_readonly( "master_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::master_config )
            .def_property( "default_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::default_config,
                           [](scarab::main_app& an_obj, scarab::param_node& a_config){ an_obj.default_config() = a_config; } )
            .def_property_readonly( "config_filename", (std::string& (scarab::main_app::*)()) &scarab::main_app::config_filename )
            .def_property_readonly( "global_verbosity", &scarab::main_app::get_global_verbosity )
            .def_property_readonly( "nonoption_kw_args", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::nonoption_kw_args )
            .def_property_readonly( "nonoption_ord_args", (scarab::param_array& (scarab::main_app::*)()) &scarab::main_app::nonoption_ord_args )
            .def_property_readonly( "app_options", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::app_options )

            //TODO: add_config_subcommand?

            //add_config_options... what template versions do we need?
            .def( "add_config_option",
                  &scarab::config_decorator::add_config_option< std::string >,
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments" )
            .def( "add_config_multi_option",
                  &scarab::config_decorator::add_config_multi_option< std::string >,
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments" )
            .def( "add_config_flag",
                  &scarab::config_decorator::add_config_flag< bool >,
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, which is toggled as a bool" )
            .def( "add_config_counted_flag",
                  &scarab::config_decorator::add_config_flag< unsigned >,
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, the config will contain a count of occurrances" )

            .def( "set_version", &scarab::main_app::set_version )

            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* APPLICATION_PYBIND_HH_ */
