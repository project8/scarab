
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

        // if an_arg starts with '-', then it's either an option or it's a negative number.
        if( an_arg[0] == f_option_starter ) 
        {
            param_ptr_t t_parsed( simple_parser::parse_value( an_arg ) );
            param_value& t_value = (*t_parsed)();
            if( t_value.is_int() || t_value.is_double() )
            {
                // if it parses as a number, then it's a negative number (no need to check uint, because presumably it's negative)
                f_ord_args.push_back( std::move(t_parsed) );
                return;
            }
            // if it's not a negtive number, then it's an option and shouldn't be considered here
            throw error() << "Cannot parse an option with the nonoption_parser: " << an_arg;
        }

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
