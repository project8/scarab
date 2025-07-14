/*!
   @file scarab-parse.cc
   @author N.S. Oblath
  
   @brief Parser: Parse a file and verify its conversion to a param structure
  
   @details
   Usage:
   ~~~~
   $> scarab-parse [format] [file]
   ~~~~
  
   Use `scarab-parse -h` to get the full help information.
*/


#include "application.hh"
#include "logger.hh"
#include "param_json.hh"
#include "param_yaml.hh"
#include "scarab_version.hh"

LOGGER( parselog, "scarab-parse" );

namespace scarab
{
    struct do_parse
    {
        do_parse() :
                f_return( 0 )
        {}

        void setup_subcommands( main_app& an_app )
        {
            scarab::config_decorator* t_sc_json = an_app.add_config_subcommand( "json",  "Parse a JSON file" );
            t_sc_json->add_config_option< std::string >( "filename", "filename", "File to parse" )->required();
            t_sc_json->this_app()->callback([&an_app,  this]() { this->json( an_app.primary_config() ); } );

            scarab::config_decorator* t_sc_yaml = an_app.add_config_subcommand( "yaml", "Parse a YAML file" );
            t_sc_yaml->add_config_option< std::string >( "filename", "filename", "File to parse" )->required();
            t_sc_yaml->this_app()->callback([&an_app, this]() { this->yaml( an_app.primary_config() ); } );

            return;
        }

        void json( const param_node& a_config )
        {
            parse< param_input_json >( a_config );
            return;
        }

        void yaml( const param_node& a_config )
        {
            parse< param_input_yaml >( a_config );
            return;
        }

        template< typename parser >
        void parse( const param_node& a_config )
        {
            parser t_input;
            if( ! a_config.has( "filename" ) )
            {
                LERROR( parselog, "Filename was not specified" );
                f_return = -1;
                return;
            }

            param_ptr_t t_param_location( t_input.read_file( a_config["filename"]().as_string() ) );

            if( ! t_param_location )
            {
                LERROR( parselog, "File did not parse!" );
                f_return = -1;
                return;
            }

            LPROG( parselog, "Parsed file:\n" << *t_param_location );

            return;
        }

        int f_return;
    };
}


using namespace scarab;

int main( int argc, char **argv )
{
    main_app the_main;
    the_main.require_subcommand();
    the_main.fallthrough();

    do_parse t_parse;
    t_parse.setup_subcommands( the_main );

    // Package version
    the_main.set_version( version_semantic_ptr_t( new version() ) );

    // Parse CL options and run the application
    CLI11_PARSE( the_main, argc, argv );

    return t_parse.f_return;
}
