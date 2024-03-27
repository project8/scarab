/*
 * test_param_env_modifier.cc
 *
 *  Created on: Mar 26, 2024
 *      Author: N.S. Oblath
 *
 */

#include "param_env_modifier.hh"

#include "catch.hpp"

using scarab::param_env_modifier;
using scarab::param_value;
/*
class modify_value : public boost::static_visitor< void >
{
    public:
        //typedef void result_type;
        void operator()( std::string& a_value ) const
        {
            a_value = "modified";
            return;
        }
        template< typename T >
        void operator()( T ) const
        {
            return;
        }
};
*/

// to test
//  - regex error caught [direct]
//  - no vars in string (string unchanged) [direct & modifier]
//  - var in string but not in env (throws scarab::error) [direct & modifier]
//  - var in string and in env (changed as predicted) [direct & modifier]
//  - var in string twice (changed as predicted) [direct & modifier]
//  - two vars in string (changed as predicted) [modifier]

TEST_CASE( "param_env_modifier", "[param]" )
{
    param_env_modifier t_modifier;

    SECTION( "direct substitution" )
    {
        //t_modifier.pattern() = "[A-Za-z]+";
        //t_modifier.pattern() = "ENV\\{[A-Za-z_]+\\}";
        std::string test_string( "test ENV{SCARAB_TEST_VAR_1} string ENV{SCARAB_TEST_VAR_2} -- ENV{SCARAB_TEST_VAR_1}" );
        param_env_modifier::modify_string t_str_modifier( &t_modifier );
        REQUIRE( t_str_modifier.substitute_env( test_string ) );
    }
}

