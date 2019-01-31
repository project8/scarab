
#define SCARAB_API_EXPORTS

#include "nonoption_parser.hh"

#include "logger.hh"
#include "param_helpers.hh"

#include <cctype>
#include <sstream>

LOGGER( parselog, "nonoption_parser" );

namespace scarab
{
    nonoption_parser::nonoption_parser( std::vector< std::string > an_args ) :
            f_ord_args(),
            f_kw_args()
    {
        for( const std::string& arg : an_args )
        {
            parse( arg ); // can throw scarab::error
        }
    }

    nonoption_parser::~nonoption_parser()
    {
    }

    void nonoption_parser::parse( const std::string& an_arg )
    {
        // this method can throw scarab::error

        // if an_arg starts with '-', then it's an option and shouldn't be considered here
        if( an_arg[0] == f_option_starter ) throw error() << "Cannot parse an option with the nonoption_parser: " << an_arg;

        size_t t_val_pos = an_arg.find_first_of( f_value_separator );
        if( t_val_pos != std::string::npos )
        {
            // this argument has an '=' (or whatever f_value_separator is), so parse as a keyword argument
            param_ptr_t t_new_param = simple_parser::parse_address(
                    an_arg.substr(0, t_val_pos),
                    simple_parser::parse_value( an_arg.substr(t_val_pos+1) ) ); // can throw scarab::error
            f_kw_args.merge( t_new_param->as_node() );
        }
        else
        {
            // this argument has no '=', so parse it as an ordered argument
            f_ord_args.push_back( simple_parser::parse_value( an_arg ) );
        }

        return;
    }

}
