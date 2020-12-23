/*
 * application.hh
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_APPLICATION_HH_
#define SCARAB_APPLICATION_HH_

#include "CLI11.hpp"

#include "logger.hh"
#include "member_variables.hh"
#include "param_helpers.hh"
#include "version_semantic.hh"

#include <type_traits>

namespace scarab
{
    typedef CLI::App app;

    class version_semantic;

    class main_app;

    /*!
     @class config_decorator
     @author N. S. Oblath

     @brief Adds the ability to create options and subcommands that are tied to a main_app's primary config

     @details
     Note that while this class is called a decorator, it does not follow the actual decorator pattern.
     It does, however, follow the spirit of that pattern in that it adds a capability to the CLI::App class.

     Subcommands and options are tied to the main_app in slightly different ways because of the different ways
     in which those two CLI11 classes are used.

     Subcommands are CLI::App objects, and any App can have subcommands.  Therefore the collection of Apps in
     use in a given application can form a multi-level parent-child structure.  Each parent App can have multiple
     children, and owns those child App objects.  The children can also be parents, of course.
     The scarab::config_decorator for each subcommand App object is created by the parent config_decorator's
     add_config_subcommand() function.  That decorator has a link to its App and is owned by its parent decorator.

     At the top of the App hierarchy is the primary App.  In Scarab, that's the main_app, which also inherits from
     config_decorator.  So main_app is also the top of the config_decorator hierarchy.

     Options, on the other hand, do not form their own hierarchy.  Each App owns its options.  On the Scarab side,
     though, even options that belong to subcommands contribute to the main_app's primary config.  Therefore when
     config_decorator::add_config_option() is used, the app_option_holder that corresponds to the new option is
     given directly to the main_app.
    */
    class SCARAB_API config_decorator
    {
        public:
            config_decorator( main_app* a_main, app* a_this_app );
            config_decorator( const config_decorator& ) = delete;
            config_decorator( config_decorator&& ) = delete;
            virtual ~config_decorator();

            config_decorator& operator=( const config_decorator& ) = delete;
            config_decorator& operator=( config_decorator&& ) = delete;

            main_app* main() const;
            app* this_app() const;

            /// Add a subcommand that is linked to a particular main_app and can create options that modify that main_app's primary config
            config_decorator* add_config_subcommand( std::string a_subcommand_name, std::string a_description="" );

            /// Add an option that gets automatically added to the primary config of a main_app
            template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_option( std::string a_name,
                                            std::string a_primary_config_addr,
                                            std::string a_description = "" );

            /// Add an option that gets automatically added to the primary config of a main_app
            template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_multi_option( std::string a_name,
                                                  std::string a_primary_config_addr,
                                                  std::string a_description = "" );

            /// Add a flag that gets automatically added to the primary config of a main_app
            /// The flag can be passed multiple times, and the number of instances will be recorded in the config
            template< typename T, CLI::enable_if_t< std::is_integral<T>::value && ! CLI::is_bool<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_flag( std::string a_name,
                                          std::string a_primary_config_addr,
                                          std::string a_description = "" );

            /// Add a flag that gets automatically added to the primary config of a main_app
            /// This flag is recorded in the config as a boolean: false if it doesn't appear in the command line, and true if it does
            template< typename T, CLI::enable_if_t< CLI::is_bool<T>::value, CLI::detail::enabler > = CLI::detail::dummy >
            CLI::Option* add_config_flag( std::string a_name,
                                          std::string a_primary_config_addr,
                                          std::string a_description = "" );

        public:
            // these structs provide a type erasure mechanism that:
            //  - holds the actual value provided on the CL, and
            //  - adds the value to the f_app_options, and then to the global config, from the pre_callback function
            struct app_option_holder
            {
                virtual void add_to_app_options( param_node& a_app_options ) = 0;
                CLI::Option* f_option;
                std::string f_primary_config_addr;
                virtual ~app_option_holder() {}
            };

            template< typename T >
            struct app_option_holder_typed : app_option_holder
            {
                void add_to_app_options( param_node& a_app_options )
                {
                    if( ! (*f_option) ) return;
                    param_ptr_t t_new_config_ptr = simple_parser::parse_address(
                            f_primary_config_addr,
                            param_ptr_t( new param_value(f_value) ) ); // throws scarab::error if top-level param object is not a node
                    a_app_options.merge( t_new_config_ptr->as_node() );
                    return;
                }
                T f_value;
                virtual ~app_option_holder_typed() {}
            };

            template< typename T >
            struct app_option_holder_vector_typed : app_option_holder
            {
                void add_to_app_options( param_node& a_app_options )
                {
                    if( ! (*f_option) ) return;
                    param_array t_array;
                    std::for_each( f_values.begin(), f_values.end(), [&t_array]( T a_value ){ t_array.push_back( a_value ); } );
                    param_ptr_t t_new_config_ptr = simple_parser::parse_address(
                            f_primary_config_addr,
                            param_ptr_t( new param_array(std::move(t_array)) ) ); // throws scarab::error if top-level param object is not a node
                    a_app_options.merge( t_new_config_ptr->as_node() );
                    return;
                }
                std::vector< T > f_values;
                virtual ~app_option_holder_vector_typed() {}
            };

            struct app_option_holder_bool_flag : app_option_holder
            {
                void add_to_app_options( param_node& a_app_options )
                {
                    if( ! (*f_option) ) return;
                    param_ptr_t t_new_config_ptr = simple_parser::parse_address(
                            f_primary_config_addr,
                            param_ptr_t( new param_value(*f_option) ) ); // throws scarab::error if top-level param object is not a node
                    a_app_options.merge( t_new_config_ptr->as_node() );
                    return;
                }
                virtual ~app_option_holder_bool_flag() {}
            };

        protected:
            main_app* f_main;
            app* f_this;

            // config_decorator owns the decorators of subcommands just like app owns the apps of subcommands
            using conf_dec_ptr_t = std::unique_ptr< config_decorator >;
            std::vector< conf_dec_ptr_t > f_subcommand_decorators;
    };


    /*!
     @class main_app
     @author N. S. Oblath

     @brief Primary application class

     @details
     This class is designed to provide an entry point for an application, and to setup the application's configuration
     based on information provided by the user at the command line.

     Note that there is a constructor option, `use_config`, which allows `main_app` to be used without using the configuration file 
     if it's set to false (the default is `true`). 
     There will not be a `-c` CLI option, and the configuration steps will not be run.  Functionality is much closer 
     to that of `CLI::App` in this case.

     The application's primary "configuration" is in the form of a scarab::param_node object that takes input from
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
         - Keyword arguments, in the form [name]=[value], are merged with the primary config.
           They're also separately accessible as `nonoption_kw_args`
         - Ordered arguments, in the form [value], are not merged with the primary config.
           They're accessible as `nonoption_ord_args`

       Application specified options (stage 4) are merged with the config and
       are separately accessible as `app_options`.  They have to be specified
       in the application using `add_config_option()`, which will map
       the option name to an address in the primary config.

       The functionality for each stage is implemented in its own virtual function,
       so a subclass can customize the procedure as needed.

       Example:
         > my_app -c config.yaml --an_opt 20 nested.value="hello"

         What happens:
         1. my_app will have default values hard-coded
         2. File `config.yaml` will be parsed and merged with the defaults
         3. Configuration `nested { value: "hello" }` will be merged with the primary config
         4. Option `an_opt` will set something specified in the config to 20

     See CLI11 documentation for the CLI::App class:
     - GitHub Readme: https://github.com/CLIUtils/CLI11
     - Tutorial: https://cliutils.gitlab.io/CLI11Tutorial
     - API: https://cliutils.github.io/CLI11/index.html

     There are several test examples that can be used as useful references on different ways that
     applications can be setup.  You'll find them in the scarab/testing/applications directory.
     - test_app_with_callback.cc -- an example that runs a program using a callback function
     - test_app_with_options.cc -- an example using basic CLI11 options (i.e. options that don't modify the configuration)
     - test_app_with_subcommands.cc -- an example with two subcommands.  e.g. my_app do_action_A --an_option=6
     - test_basic_application.cc -- the example above
     */
    class SCARAB_API main_app : public config_decorator, public app
    {
        public:
            main_app( bool a_use_config = true );
            virtual ~main_app();

        public:
            /// parses positional arguments into the global config
            /// called after parsing, before running callbacks
            virtual void pre_callback();

            // Functions to perform each configuration stage
            /// Load default values
            virtual void do_config_stage_1();
            /// Load the config file
            virtual void do_config_stage_2();
            /// Load the directly-addressed non-option arguments
            virtual void do_config_stage_3();
            /// Load the application-specific options
            virtual void do_config_stage_4();

            void set_version( scarab::version_semantic_ptr_t a_ver );

            /// Master configuration tree for the application
            mv_referrable( param_node, primary_config );

            // configuration stage 1
            /// Default configuration values
            mv_referrable( param_node, default_config );

            // configuration stage 2
            /// Configuration file name
            mv_referrable_const( std::string, config_filename );

            // configuration stage 3
            /// Keyword configuration values coming from the command line, in the form: config.address=value
            mv_referrable( param_node, nonoption_kw_args );
            /// Ordered configuration values coming in an application-specific order from the command line, in the form: value
            mv_referrable( param_array, nonoption_ord_args );

            // configuration stage 4
            /// Application-specific options that are specified using add_config_option() functions
            mv_referrable( param_node, app_options );

            /// Store the app option holder structs from this app and any subcommands
            mv_referrable( std::vector< std::shared_ptr< app_option_holder > >, app_option_holders );

            mv_accessible_noset( bool, use_config );

            //*************************
            // Verbosity
            //*************************
        public:
            using verbosity_t = std::underlying_type< logger::ELevel >::type; // typically is unsigned

            /// Get the global verbosity
            verbosity_t get_global_verbosity() const;
            /// Set the global verbosity with the verbosity value
            void set_global_verbosity( verbosity_t a_verbosity );
            /// Set the global verbosity with the verbosity enum
            void set_global_verbosity( logger::ELevel a_verbosity );

            void increase_global_verbosity( unsigned an_offset );
            void decrease_global_verbosity( unsigned an_offset );

            using verbosity_map_t = std::map< verbosity_t, logger::ELevel >;
            using verbosity_iterator_t = verbosity_map_t::iterator;
            static verbosity_map_t s_verbosities;

        protected:
            verbosity_iterator_t f_global_verbosity;

    };


    template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > >
    CLI::Option* config_decorator::add_config_option( std::string a_name,
                                                      std::string a_primary_config_addr,
                                                      std::string a_description )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_typed<T> >();
        t_opt_holder_ptr->f_option = f_this->add_option( a_name, t_opt_holder_ptr->f_value, a_description ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_primary_config_addr = a_primary_config_addr;
        f_main->app_option_holders().push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }

    template< typename T, CLI::enable_if_t< ! CLI::is_vector<T>::value, CLI::detail::enabler > >
    CLI::Option* config_decorator::add_config_multi_option( std::string a_name,
                                                            std::string a_primary_config_addr,
                                                            std::string a_description )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_vector_typed<T> >();
        t_opt_holder_ptr->f_option = f_this->add_option( a_name, t_opt_holder_ptr->f_values, a_description ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_primary_config_addr = a_primary_config_addr;
        f_main->app_option_holders().push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }

    template< typename T, CLI::enable_if_t< std::is_integral<T>::value && ! CLI::is_bool<T>::value, CLI::detail::enabler > >
    CLI::Option* config_decorator::add_config_flag( std::string a_name,
                                                    std::string a_primary_config_addr,
                                                    std::string a_description )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_typed<T> >();
        t_opt_holder_ptr->f_option = f_this->add_flag( a_name, t_opt_holder_ptr->f_value, a_description ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_primary_config_addr = a_primary_config_addr;
        f_main->app_option_holders().push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }

    template< typename T, CLI::enable_if_t< CLI::is_bool<T>::value, CLI::detail::enabler > >
    CLI::Option* config_decorator::add_config_flag( std::string a_name,
                                                    std::string a_primary_config_addr,
                                                    std::string a_description )
    {
        auto t_opt_holder_ptr = std::make_shared< app_option_holder_bool_flag >();
        t_opt_holder_ptr->f_option = f_this->add_flag( a_name, a_description ); // throws CLI::OptionAlreadyAdded if the option's already there
        t_opt_holder_ptr->f_primary_config_addr = a_primary_config_addr;
        f_main->app_option_holders().push_back( t_opt_holder_ptr );
        return t_opt_holder_ptr->f_option;
    }

    inline app* config_decorator::this_app() const
    {
        return f_this;
    }

    inline main_app* config_decorator::main() const
    {
        return f_main;
    }

} /* namespace scarab */

#endif /* SCARAB_APPLICATION_HH_ */
