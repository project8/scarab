#ifndef APPLICATION_PYBIND_HH_
#define APPLICATION_PYBIND_HH_

#include "pybind11/pybind11.h"
#include "pybind11/functional.h"
#include "pybind11/stl.h"

#include "application.hh"
#include "scarab_binding_helpers.hh"

namespace scarab_pybind
{
    std::list< std::string > export_application( pybind11::module& mod )
    {
        std::list< std::string > all_members;

        all_members.push_back( "MainApp" );
        pybind11::class_< scarab::main_app >( mod, "MainApp", "Base class for creating CLI utilities" )
            .def( pybind11::init< bool >(),
                  pybind11::arg( "use_config" ) = true,
                  pybind11::return_value_policy::take_ownership,
                  SCARAB_BIND_CALL_GUARD_STREAMS )

            .def( "set_callback", [](scarab::main_app* an_app, std::function< void() > a_fun){ an_app->callback( a_fun );} )

            // member variables
            .def_property_readonly( "primary_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::primary_config )
            .def_property( "default_config", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::default_config,
                           [](scarab::main_app& an_obj, scarab::param_node& a_config){ an_obj.default_config() = a_config; } )
            .def_property( "config_filename", (std::string& (scarab::main_app::*)()) &scarab::main_app::config_filename,
                           [](scarab::main_app& an_obj, std::string& a_filename){ an_obj.config_filename() = a_filename; } )
            .def_property_readonly( "global_verbosity", &scarab::main_app::get_global_verbosity )
            .def_property_readonly( "nonoption_kw_args", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::nonoption_kw_args )
            .def_property_readonly( "nonoption_ord_args", (scarab::param_array& (scarab::main_app::*)()) &scarab::main_app::nonoption_ord_args )
            .def_property_readonly( "app_options", (scarab::param_node& (scarab::main_app::*)()) &scarab::main_app::app_options )
            .def_property( "use_config", &scarab::main_app::get_use_config, &scarab::main_app::set_use_config )
            .def_property( "auth_groups_key", (std::string& (scarab::main_app::*)()) &scarab::main_app::auth_groups_key,
                           [](scarab::main_app& an_obj, std::string& a_key){ an_obj.auth_groups_key() = a_key; } )
            .def_property( "auth_file_key", (std::string& (scarab::main_app::*)()) &scarab::main_app::auth_file_key,
                           [](scarab::main_app& an_obj, std::string& a_key){ an_obj.auth_file_key() = a_key; } )
            .def_property( "auth", (scarab::authentication& (scarab::main_app::*)()) &scarab::main_app::auth,
                           [](scarab::main_app& an_obj, scarab::authentication& an_auth){ an_obj.auth() = an_auth; } )

            .def( "set_default_auth_spec_groups", &scarab::main_app::set_default_auth_spec_groups )
            .def( "add_default_auth_spec_group", &scarab::main_app::add_default_auth_spec_group )
            .def( "set_default_auth_file", &scarab::main_app::set_default_auth_file )

            .def( "do_config_stage_1", &scarab::main_app::do_config_stage_1, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "do_config_stage_2", &scarab::main_app::do_config_stage_2, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "do_config_stage_3", &scarab::main_app::do_config_stage_3, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "do_config_stage_4", &scarab::main_app::do_config_stage_4, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "do_config_stage_5", &scarab::main_app::do_config_stage_5, SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "do_authentication", &scarab::main_app::do_authentication, SCARAB_BIND_CALL_GUARD_STREAMS )

            //TODO: add_config_subcommand?

            //add_config_options... what template versions do we need?
            .def( "add_config_flag",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_flag< bool >(a_name, a_addr, a_description);},
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, which is toggled as a bool",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_counted_flag",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_flag< unsigned >(a_name, a_addr, a_description);},
                  pybind11::arg( "flag" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a CLI flag, the config will contain a count of occurrances",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_string_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_option< std::string >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a single-string argument",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_int_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_option< int >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a single-integer argument",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_float_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_option< double >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a single-float argument",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_multi_string_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_multi_option< std::string >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_multi_int_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_multi_option< int >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of ints arguments",
                  SCARAB_BIND_CALL_GUARD_STREAMS )
            .def( "add_config_multi_float_option",
                  [](scarab::main_app* an_app, std::string a_name, std::string a_addr, std::string a_description)
                        {an_app->add_config_multi_option< double >(a_name, a_addr, a_description);},
                  pybind11::arg( "option" ),
                  pybind11::arg( "config_address" ),
                  pybind11::arg( "description" ) = "",
                  "add a sequence of string arguments",
                  SCARAB_BIND_CALL_GUARD_STREAMS )

            .def( "set_version", &scarab::main_app::set_version, SCARAB_BIND_CALL_GUARD_STREAMS )

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
                    // CLI11_PARSE returns an integer in the event of an error, the are meant to be
                    // valid shell return codes, so 0 is success
                    CLI11_PARSE( an_app, args.size(), argv );
                    return 0;
                  },
                  "parse arguments and execute the application",
                  SCARAB_BIND_CALL_GUARD_STREAMS_AND_GIL )

            ;

        return all_members;
    }
} /* namespace scarab_pybind */
#endif /* APPLICATION_PYBIND_HH_ */
