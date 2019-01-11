/*
 * application.hh
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_APPLICATION_HH_
#define SCARAB_APPLICATION_HH_

#include "CLI11.hpp"

#include "member_variables.hh"
#include "param_helpers.hh"

namespace scarab
{
    typedef CLI::App app;

    class version_semantic;

    /*!
     @class main_app
     @author N. S. Oblath

     @brief Primary application class

     @details
     This class is designed to provide an entry point for an application, and to setup the application's configuration
     based on information provided by the user at the command line.

     The application's master "configuration" is in the form of a scarab::param_node object that takes input from
     default values, a configuration file, directly-addressed arguments, and application-specific options.

     The `main_app` class uses and makes available all of the functionality of the CLI11 library, and
     in particular it directly extends the CLI::App class.  Links to documentation on how to use CLI11
     are provided below.

     Basic usage (customized from CLI11's basic usage):

     int main( int argc, char **argv )
     {
         main_app the_main;

         // Add new options/flags here

         // Set the default config file
         the_main.default_config() = t_default_config;

         // This includes calling application::pre_callback() to finalize the global config
         CLI11_PARSE( the_main, argc, argv );

         return 0;
     }

     Notes on specifying and using command-line arguments:

       There are two main types of command-line arguments:
       1. "Options" are named arguments.  On the command line they look like `-b [value]`, `--broker [value]`, or `-r` (flag with no value)
       2. "Non-option" arguments have no name, and typically rely on the order they fall on the command line to interpret their meaning.

       Options need to be specified in an application (i.e. CLI11 needs to know about them at compile time).
       All of the functionality of CLI11 is available when using `main_app`,
       so you can create options and flags (options with no values) that do not affect the configuration.
       Alternatively, `main_app` provides the ability to define options that are automatically added to the configuration (see below).

     Notes for setting the configuration in an application:

       Stages for setting the configuration
       1. Default options
       2. Config file
       3. Non-option arguments
       4. Application-specified options

       In stage 3, arguments can have two forms:
         - Keyword arguments, in the form [name]=[value], are merged with the master config.
           They're also separately accessible as `nonoption_kw_args`
         - Ordered arguments, in the form [value], are not merged with the master config.
           They're accessible as `nonoption_ord_args`

       Application specified options are merged with the config and
       are separately accessible as `app_options`.  They have to be specified
       in the application using `add_config_option()`, which will map
       the option name to an address in the master config.

       Example:
         > my_app -c config.yaml --an_opt 20 nested.value="hello"

         What happens:
         1. my_app will have default values hard-coded
         2. File `config.yaml` will be parsed and merged with the defaults
         3. Configuration `nested { value: "hello" }` will be merged with the master config
         4. Option `an_opt` will set something specified in the config to 20

     See CLI11 documentation for the CLI::App class:
     - GitHub Readme: https://github.com/CLIUtils/CLI11
     - Tutorial: https://cliutils.gitlab.io/CLI11Tutorial
     - API: https://cliutils.github.io/CLI11/index.html

     There are several test examples that can be used as useful references on different ways that
     applications can be setup.  You'll find them in the scarab/library/test directory.
     - test_app_with_callback.cc -- an example that runs a program using a callback function
     - test_app_with_options.cc -- an example using basic CLI11 options (i.e. options that don't modify the configuration)
     - test_app_with_subcommands.cc -- an example with two subcommands.  e.g. my_app do_action_A --an_option=6
     - test_basic_application.cc -- the example above
     */
    class SCARAB_API main_app : public app
    {
        public:
            main_app();
            virtual ~main_app();

        public:
            /// parses positional arguments into the global config
            /// called after parsing, before running callbacks
            virtual void pre_callback();

            void set_version( version_semantic* a_ver );

            template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_option( std::string a_name,
                               std::string a_master_config_addr,
                               std::string a_description = "" );

            template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_option( std::string a_name,
                               std::string a_master_config_addr,
                               std::string a_description,
                               bool a_defaulted );

            /// Master configuration tree for the application
            mv_referrable( param_node, master_config );

            // configuration stage 1
            /// Default configuration values
            mv_referrable( param_node, default_config );

            // configuration stage 2
            /// Configuration file name
            mv_referrable_const( std::string, config_filename );
            /// Global verbosity value
            mv_accessible( unsigned, global_verbosity );

            // configuration stage 3
            /// Keyword configuration values coming from the command line, in the form: config.address=value
            mv_referrable( param_node, nonoption_kw_args );
            /// Ordered configuration values coming in an application-specific order from the command line, in the form: value
            mv_referrable( param_array, nonoption_ord_args );

            // configuration stage 4
            /// Application-specific options that are specified using add_config_option() functions
            mv_referrable( param_node, app_options );

        protected:
            // these structs provide a type erasure mechanism that:
            //  - holds the actual value provided on the CL, and
            //  - adds the value to the f_app_options, and then to the global config, from the pre_callback function
            struct app_option_holder
            {
                virtual void add_to_app_options( param_node& a_app_options ) = 0;
                CLI::Option* f_option;
                std::string f_master_config_addr;
            };

            template< typename T >
            struct app_option_holder_typed : app_option_holder
            {
                void add_to_app_options( param_node& a_app_options )
                {
                    param_ptr_t t_new_config_ptr = simple_parser::parse_address(
                            f_master_config_addr,
                            param_ptr_t( new param_value(f_value) ) ); // throws scarab::error if top-level param object is not a node
                    a_app_options.merge( t_new_config_ptr->as_node() );
                    return;
                }
                T f_value;
            };

            std::vector< std::shared_ptr< app_option_holder > > f_app_option_holders;
    };

    template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > >
    CLI::Option* main_app::add_config_option( std::string a_name,
                       std::string a_master_config_addr,
                       std::string a_description )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_typed<T> >();
        t_opt_holder_ptr->f_option = add_option( a_name, t_opt_holder_ptr->f_value, a_description ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_master_config_addr = a_master_config_addr;
        f_app_option_holders.push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }

    template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > >
    CLI::Option* main_app::add_config_option( std::string a_name,
                       std::string a_master_config_addr,
                       std::string a_description,
                       bool a_defaulted )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_typed<T> >();
        t_opt_holder_ptr->f_option = add_option( a_name, t_opt_holder_ptr->f_value, a_description, a_defaulted ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_master_config_addr = a_master_config_addr;
        f_app_option_holders.push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }


} /* namespace scarab */

#endif /* SCARAB_APPLICATION_HH_ */
