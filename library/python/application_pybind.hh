#ifndef APPLICATION_PYBIND_HH_
#define APPLICATION_PYBIND_HH_

#include "pybind11/pybind11.h"
#include "pybind11/functional.h"
#include "pybind11/stl.h"

#include "application.hh"

namespace scarab_pybind
{
    std::list< std::string > export_application( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        all_members.push_back( "MainApp" );
        pybind11::class_< scarab::main_app >( mod, "MainApp", "Base class for creating CLI utilities" )
            .def( pybind11::init< >() )

            .def( "set_callback", [](scarab::main_app* an_app, std::function< void() > a_fun){ an_app->callback( a_fun );} )

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
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_option< std::string >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments" )
            .def( "add_config_multi_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_multi_option< std::string >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments" )
            .def( "add_config_flag",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_flag< bool >(a_name, a_addr, a_description);},
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, which is toggled as a bool" )
            .def( "add_config_counted_flag",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_flag< unsigned >(a_name, a_addr, a_description);},
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, the config will contain a count of occurrances" )

            .def( "set_version", &scarab::main_app::set_version )

            // implement execute as a lambda, it is a macro in CLI11
            .def( "execute",
                  [](scarab::main_app& an_app, std::list< std::string > args )
                  {
                    //TODO: this feels... super ugly, also how do I deal with errors/return codes?
                    const char* argv[args.size()];
                    std::list< std::string >::iterator an_arg = args.begin();
                    for (unsigned i_arg=0; i_arg<args.size(); ++i_arg)
                    {
                        argv[i_arg] = an_arg->c_str();
                        ++an_arg;
                    }
                    CLI11_PARSE( an_app, args.size(), argv );
                  },
                  "parse arguments and execute the application" )

            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* APPLICATION_PYBIND_HH_ */
