#ifndef APPLICATION_PYBIND_HH_
#define APPLICATION_PYBIND_HH_

#include "application.hh"

#include "pybind11/pybind11.h"

namespace scarab_pybind
{
    void export_application( pybind11::module& mod )
    {
        pybind11::class_< scarab::main_app >( mod, "MainApp", "Base class for creating CLI utilities" )
            .def( pybind11::init< >() )

            // member variables
            .def_property_noset( "master_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::master_config )
            .def_property_noset( "default_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::default_config )
            .def_property_noset( "config_filename", (std::string& (scarab::main_app::*)()) &scarab::main_app::config_filename )
            .def_property_noset( "global_verbosity", &scarab::main_app::get_global_verbosity )
            .def_property_noset( "nonoption_kw_args", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::nonoption_kw_args )
            .def_property_noset( "nonoption_ord_args", (scarab::param_array& (scarab::main_app::*)()) &scarab::main_app::nonoption_ord_args )
            .def_property_noset( "app_options", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::app_options )

            ;

    }
} /* namespace scarab_pybind */
#endif /* APPLICATION_PYBIND_HH_ */
