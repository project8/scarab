/*
 * param_env_modifier.cc
 *
 *  Created on: Mar 21, 2024
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "param_env_modifier.hh"

#include "param_value.hh"

#include "logger.hh"

#include <cstdlib>
#include <regex>

LOGGER( plog, "param_env_modifier" );

namespace scarab
{
        param_env_modifier::param_env_modifier() :
                f_prefix( "ENV\\{"),
                f_var_pattern( "([A-Za-z0-9_]+)" ),
                f_postfix( "\\}")
        {}

        param_env_modifier::~param_env_modifier() 
        {}

        bool param_env_modifier::modify_string::substitute_env( std::string& a_string ) const
        {
            // first we search for the search pattern
            // this should find exactly one match, if there's a match present
            // t_matches should be 2 because there's one match for the full pattern, and one match for what's in parentheses
            //LWARN( plog, "the pattern: " << f_pattern_buffer );
            if( std::smatch t_matches; std::regex_search( a_string, t_matches, std::regex( f_pattern_buffer ) ) )
            {
                //LWARN( plog, "Number of matches: " << t_matches.size() );
                //for( unsigned i = 0; i < t_matches.size(); ++i )
                //{
                //    LWARN( plog, "Match " << i << ": " << t_matches[i] );
                //}
                if( t_matches.size() != 2 )
                {
                    // something unexpected happened in the match that gave us the wrong size matches array; raise exception
                    throw error() << "Something unexpected happened in the regex match to find environment variables";
                }

                try
                {
                    // get the environment variable value
                    std::string t_value( std::move(get_env( t_matches[1] )) );

                    // now we do the var replacement
                    // this should replace all matches to the ENV{variable_name}
                    std::string t_var_name( t_matches[1] );
                    //LWARN( plog, "Var name is " << t_var_name );
                    std::string t_var_pattern_buffer = f_parent->prefix() + t_var_name + f_parent->postfix();
                    const std::regex t_repl_pattern( t_var_pattern_buffer );
                    a_string = std::move( std::regex_replace( a_string, t_repl_pattern, t_value ) );
                    LDEBUG( plog, "String after env replacement: " << a_string );
                    return true;

                }
                // only catch regex_error here
                // if scarab::error was thrown, then that indicates the environment variable was not found, and we want that to propagate upwards
                catch( const std::regex_error& e )
                {
                    LERROR( plog, "Regex error: " << e.what() );
                    throw scarab::error() << e.what();
                }
                

            }
            //LWARN( plog, "No matches" );
            return false;
        }

        std::string param_env_modifier::modify_string::get_env( const std::string& a_var ) const
        {
            if( const char* t_env_value = std::getenv( a_var.c_str() ) )
            {
                return std::string( t_env_value );
            }
            LERROR( plog, "Environment variable not found: " << a_var );
            throw error() << "Did not find environment variable <" << a_var << ">";
        }
};
