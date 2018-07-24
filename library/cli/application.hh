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
     @class application
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
     */
    class main_app : public app
    {
        public:
            main_app();
            virtual ~main_app();

        public:
            /// parses positional arguments into the global config
            /// called after parsing, before running callbacks
            virtual void pre_callback();

            void set_version( version_semantic* a_ver );

            mv_referrable_const( param_node, master_config );
            mv_referrable( param_node, default_config );

            mv_referrable_const( std::string, config_filename );
            mv_accessible( unsigned, global_verbosity );

            mv_referrable( param_node, nonoption_kw_args );
            mv_referrable( param_array, nonoption_ord_args );
    };

} /* namespace scarab */

#endif /* SCARAB_APPLICATION_HH_ */
