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
#include "param.hh"

namespace scarab
{
    typedef CLI::App app;

    class version_semantic;

    /*!
     @class main_app
     @author N. S. Oblath

     @brief Primary application class

     @details
     See CLI11 documentation for the CLI::App class:
     - GitHub Readme: https://github.com/CLIUtils/CLI11
     - Tutorial: https://cliutils.gitlab.io/CLI11Tutorial
     - API: https://cliutils.github.io/CLI11/index.html

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

     Notes for setting options in an application:

       Stages for setting the configuration
       1. Default options
       2. Config file
       3. Non-option arguments
       4. Application-specified options

       In stage 3, arguments can have two forms:
         - Keyword arguments, in the form [name]=[value], are merged with the config.
           They're also separately accessible as `nonoption_kw_args`
         - Ordered arguments, in the form [value], are not merged with the config.
           They're accessible as `nonoption_ord_args`

       Application specified options are merged with the config and
       are separately accessible as `app_options`.

       Example:
         > my_app -c config.yaml --an_opt 20 nested.value="hello"

         What happens:
         1. my_app will have default values hard-coded
         2. File `config.yaml` will be parsed and merged with the defaults
         3. Configuration `nested { value: "hello" }` will be merged with the config
         4. Option `an_opt` will set something specified in the config to 20

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

            template< typename XValType >
            void set_app_option( const std::string& a_parsable );

            mv_referrable( param_node, master_config );

            // configuration stage 1
            mv_referrable( param_node, default_config );

            // configuration stage 2
            mv_referrable_const( std::string, config_filename );
            mv_accessible( unsigned, global_verbosity );

            // configuration stage 3
            mv_referrable( param_node, nonoption_kw_args );
            mv_referrable( param_array, nonoption_ord_args );

            // configuration stage 4
            mv_referrable( param_node, app_options );
    };

    template< typename XValType >
    void main_app::set_app_option< XValType >( const std::string& a_parsable )
    {

    }

} /* namespace scarab */

#endif /* SCARAB_APPLICATION_HH_ */
