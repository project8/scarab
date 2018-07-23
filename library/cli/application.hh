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
    typedef CLI::App command;

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
     int main(int argc, char **argv)
     {
         application app;

         // Add new options/flags here

         // Set the default config file
         app.default_config() = t_default_config;

         // This includes calling application::pre_callback() to finalize the global config
         CLI11_PARSE(app, argc, argv);

    return 0;
}
     */
    class application : public command
    {
        public:
            application();
            virtual ~application();

        public:
            /// parses positional arguments into the global config
            /// called after parsing, before running callbacks
            virtual void pre_callback();

            mv_referrable_const( scarab::param_node, master_config );
            mv_referrable( scarab::param_node, default_config );

            mv_referrable_const( std::string, config_filename );
            mv_accessible( unsigned, global_verbosity );

    };

} /* namespace scarab */

#endif /* SCARAB_APPLICATION_HH_ */
