
#define SCARAB_API_EXPORTS

#include "nonoption_parser.hh"

#include "logger.hh"

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

        size_t t_val_pos = an_arg.find_first_of( f_value_separator );
        if( t_val_pos != std::string::npos )
        {
            // this argument has an '=' (or whatever f_value_separator is), so parse as a keyword argument
            param_ptr_t t_new_param = parse_kw_arg( an_arg.substr( 0, t_val_pos ), an_arg.substr( t_val_pos+1 ) ); // can throw scarab::error
            f_kw_args.merge( t_new_param->as_node() );
        }
        else
        {
            // this argument has no '=', so parse it as an ordered argument
            f_ord_args.push_back( parse_value( an_arg ) );
        }

        return;
    }

    param_ptr_t nonoption_parser::parse_kw_arg( const std::string& an_addr, const std::string& a_value )
    {
        // this method can throw scarab::error

        if( an_addr.empty() )
        {
            throw error() << "No address for keyword argument with value <" < a_value << ">";
        }

        unsigned t_div_pos = an_addr.find( f_node_separator );
        std::string t_top_addr = an_addr.substr( 0, t_div_pos );

        param_ptr_t t_new_param = new_param_from_addr( t_top_addr );

        if( t_new_param->is_node() )
        {
            // only nodes are allowed at the top level
            add_next( *t_new_param, t_top_addr, an_addr.substr( t_div_pos+1 ), a_value ); // this can throw scarab::error
            return t_new_param;
        }

        throw error() << "Top param type must be a node; this address is invalid: <" << an_addr << ">";
    }

    void nonoption_parser::add_next( param& a_parent, const std::string& an_addr_in_parent, const std::string& a_next_addr, const std::string& a_value )
    {
        // this method can throw scarab::error

        param_ptr_t t_child;

        // check if the next address is empty;
        // if so, parse the value
        // if not, parse the child
        if( a_next_addr.empty() )
        {
            // the child is a param_value or param
            t_child = parse_value( a_value );
        }
        else
        {
            // the child is a param_array or param_node
            unsigned t_div_pos = a_next_addr.find( f_node_separator );
            std::string t_top_addr = a_next_addr.substr( 0, t_div_pos );
            t_child = new_param_from_addr( t_top_addr );
            add_next( *t_child, t_top_addr, a_next_addr.substr( t_div_pos+1 ), a_value ); // this can throw scarab::error
        }

        // add the child as appropriate for the parent type
        if( a_parent.is_array() )
        {
            unsigned index = std::stoul( an_addr_in_parent );
            a_parent.as_array().assign( std::stoul( an_addr_in_parent ), t_child );
        }
        else if( a_parent.is_node() )
        {
            a_parent.as_node().replace( an_addr_in_parent, t_child );
        }
        else
        {
            throw error() << "Invalid param type: only nodes and arrays accepted";
        }

        return;
    }

    param_ptr_t nonoption_parser::new_param_from_addr( const std::string& an_addr )
    {
        if( an_addr.empty() ) return param_ptr_t( new param_value() );

        // check whether all of the characters in the address are digits
        // if any are not, then the address cannot be an integer array index, so return a param_node
        for( const char& ch : an_addr )
        {
            if( ! std::isdigit( ch ) ) return new param_node();
        }
        // if we're here, then the address is an integer, so return a param_array
        return new param_array();
    }

    param_ptr_t nonoption_parser::parse_value( const std::string& a_value )
    {
        // we've found the value; now check if it's a number or a string
        if( a_value.empty() )
        {
            LTRACE( parselog, "Parsed value as NULL" << *this );
            return new param();
        }
        // if "true" or "false", then bool
        else if( a_value == "true" )
        {
            LTRACE( parselog, "Parsed value (" << a_value << ") as bool(true)" << *this );
            return new param_value( true );
        }
        else if( a_value == "false" )
        {
            LTRACE( parselog, "Parsed value (" << a_value << ") as bool(false):" << *this );
            return new param_value( false );
        }
        else
        {
            // To test if the string is numeric:
            //   1. if it has 2 decimal points, it's not numeric (IP addresses, for example, would pass the second test)
            //   2. double is the most general form of number, so if it fails that conversion, it's not numeric.
            double t_double;
            std::stringstream t_conv_double( a_value );
            if( a_value.find( '.' ) == a_value.rfind( '.' ) &&
                    a_value.find( '-' ) == a_value.rfind( '-' ) &&
                    ! (t_conv_double >> t_double).fail() )
            {
                // now we know the value is numeric
                if( a_value.find( '.' ) != std::string::npos ||
                    a_value.find( 'e' ) != std::string::npos ||
                    a_value.find( 'E' ) != std::string::npos )
                {
                    // value is a floating-point number, since it has a decimal point
                    LTRACE( parselog, "Parsed value (" << a_value << ") as double(" << t_double << "):" << *this );
                    return new param_value( t_double );
                }
                else if( a_value[ 0 ] == '-' )
                {
                    // value is a signed integer if it's negative
                    LTRACE( parselog, "Parsed value (" << a_value << ") as int(" << (int64_t)t_double << "):" << *this );
                    return new param_value( (int64_t)t_double );
                }
                else
                {
                    // value is assumed to be unsigned if it's positive
                    LTRACE( parselog, "Parsed value (" << a_value << ") as uint(" << (uint64_t)t_double << ");" << *this );
                    return new param_value( (uint64_t)t_double );
                }
            }
            else
            {
                // value is not numeric; treat as a string
                LTRACE( parselog, "Parsed value (" << a_value << ") as a string:" << *this );
                return new param_value( a_value );
            }
        }
    }

}
