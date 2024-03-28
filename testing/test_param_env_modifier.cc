/*
 * test_param_env_modifier.cc
 *
 *  Created on: Mar 26, 2024
 *      Author: N.S. Oblath
 *
 */

#include "param_env_modifier.hh"

#include "error.hh"

#include "catch.hpp"

#include <stdlib.h>

using scarab::param_env_modifier;
using scarab::param_value;

TEST_CASE( "param_env_modifier::modify_string", "[param]" )
{
    param_env_modifier t_modifier;
    param_env_modifier::modify_string t_str_modifier( &t_modifier );

    std::string t_target( "AAAAA" );
    std::string t_var( "SCARAB_TEST_VAR" );

    std::string t_target_2( "BBBBB" );
    std::string t_var_2( "SCARAB_TEST_VAR_2" );

    setenv( t_var.c_str(), t_target.c_str(), false );
    setenv( t_var_2.c_str(), t_target_2.c_str(), false );

    SECTION( "environment variable" )
    {
        // test getting of the environment variable
        REQUIRE( t_str_modifier.get_env( t_var ) == t_target );
        REQUIRE_THROWS_AS( t_str_modifier.get_env( "blah" ), scarab::error );
    }

    SECTION( "use of substitute_env" )
    {
        std::string t_test_string, t_expected;

        // no var in string: nothing to see here --> nothing to see here
        // false return means that there was no variable substitution requested
        t_test_string = "nothing to see here";
        t_expected = t_test_string;
        REQUIRE_FALSE( t_str_modifier.substitute_env( t_test_string ) );
        REQUIRE( t_test_string == t_expected );

        // var in string but not in env: ENV{VAR_THATS_NOT_SET} --> [throw scarab::error]
        t_test_string = "ENV{VAR_THATS_NOT_SET}";
        REQUIRE_THROWS_AS( t_str_modifier.substitute_env( t_test_string ), scarab::error );

        // one substitution: ENV{SCARAB_TEST_VAR} --> AAAAA
        t_test_string = "ENV{" + t_var + "}";
        REQUIRE( t_str_modifier.substitute_env( t_test_string ) );
        REQUIRE( t_test_string == t_target );

        // one substitution, var in string twice: ENV{SCARAB_TEST_VAR}ENV{SCARAB_TEST_VAR} --> AAAAAAAAAA
        t_test_string = "ENV{" + t_var + "}ENV{" + t_var + "}";
        t_expected = t_target + t_target;
        REQUIRE( t_str_modifier.substitute_env( t_test_string ) );
        REQUIRE( t_test_string == t_expected );

        // one substitution, two vars in string: ENV{SCARAB_TEST_VAR}ENV{SOME_OTHER_VAR} --> AAAAAENV{SOME_OTHER_VAR}
        t_test_string = "ENV{" + t_var + "}ENV{SOME_OTHER_VAR}";
        t_expected = t_target + "ENV{SOME_OTHER_VAR}";
        REQUIRE_NOTHROW( t_str_modifier.substitute_env( t_test_string ) );
        REQUIRE( t_test_string == t_expected );
    }

    SECTION( "use of visitor's operator()" )
    {
        std::string t_test_string, t_expected;

        // no var in string: nothing to see here --> nothing to see here
        // false return means that there was no variable substitution requested
        t_test_string = "nothing to see here";
        t_expected = t_test_string;
        REQUIRE_NOTHROW( t_str_modifier( t_test_string ) );
        REQUIRE( t_test_string == t_expected );

        // var in string but not in env: ENV{VAR_THATS_NOT_SET} --> [throw scarab::error]
        t_test_string = "ENV{VAR_THATS_NOT_SET}";
        REQUIRE_THROWS_AS( t_str_modifier( t_test_string ), scarab::error );

        // one substitution: ENV{SCARAB_TEST_VAR} --> AAAAA
        t_test_string = "ENV{" + t_var + "}";
        REQUIRE_NOTHROW( t_str_modifier( t_test_string ) );
        REQUIRE( t_test_string == t_target );

        // one substitution, var in string twice: ENV{SCARAB_TEST_VAR}ENV{SCARAB_TEST_VAR} --> AAAAAAAAAA
        t_test_string = "ENV{" + t_var + "}ENV{" + t_var + "}";
        t_expected = t_target + t_target;
        REQUIRE_NOTHROW( t_str_modifier( t_test_string ) );
        REQUIRE( t_test_string == t_expected );

        // one substitution, one known, one unknown var in string: ENV{SCARAB_TEST_VAR}ENV{SOME_OTHER_VAR} --> [throw scarab::error]
        t_test_string = "ENV{" + t_var + "}ENV{SOME_OTHER_VAR}";
        t_expected = t_target + "ENV{SOME_OTHER_VAR}";
        REQUIRE_THROWS_AS( t_str_modifier( t_test_string ), scarab::error );

        // two variables, two substitutions: ENV{SCARAB_TEST_VAR}ENV{SOME_OTHER_VAR} --> [throw scarab::error]
        t_test_string = "ENV{" + t_var + "}ENV{" + t_var_2 + "}";
        t_expected = t_target + t_target_2;
        REQUIRE_NOTHROW( t_str_modifier( t_test_string ) );
        REQUIRE( t_test_string == t_expected );
     }

    unsetenv( t_var.c_str() );
    unsetenv( t_var_2.c_str() );
}

TEST_CASE( "param_env_modifier", "[param]" )
{
    param_env_modifier t_modifier;

    std::string t_target( "AAAAA" );
    std::string t_var( "SCARAB_TEST_VAR" );

    std::string t_target_2( "BBBBB" );
    std::string t_var_2( "SCARAB_TEST_VAR_2" );

    setenv( t_var.c_str(), t_target.c_str(), false );
    setenv( t_var_2.c_str(), t_target_2.c_str(), false );

    param_value t_test_string, t_expected;

    // no var in string: nothing to see here --> nothing to see here
    // false return means that there was no variable substitution requested
    t_test_string = "nothing to see here";
    t_expected = t_test_string;
    REQUIRE_NOTHROW( t_modifier( t_test_string ) );
    REQUIRE( t_test_string == t_expected );

    // var in string but not in env: ENV{VAR_THATS_NOT_SET} --> [throw scarab::error]
    t_test_string = "ENV{VAR_THATS_NOT_SET}";
    REQUIRE_THROWS_AS( t_modifier( t_test_string ), scarab::error );

    // one substitution: ENV{SCARAB_TEST_VAR} --> AAAAA
    t_test_string = "ENV{" + t_var + "}";
    REQUIRE_NOTHROW( t_modifier( t_test_string ) );
    REQUIRE( t_test_string == t_target );

    // one substitution, var in string twice: ENV{SCARAB_TEST_VAR}ENV{SCARAB_TEST_VAR} --> AAAAAAAAAA
    t_test_string = "ENV{" + t_var + "}ENV{" + t_var + "}";
    t_expected = t_target + t_target;
    REQUIRE_NOTHROW( t_modifier( t_test_string ) );
    REQUIRE( t_test_string == t_expected );

    // one substitution, one known, one unknown var in string: ENV{SCARAB_TEST_VAR}ENV{SOME_OTHER_VAR} --> [throw scarab::error]
    t_test_string = "ENV{" + t_var + "}ENV{SOME_OTHER_VAR}";
    t_expected = t_target + "ENV{SOME_OTHER_VAR}";
    REQUIRE_THROWS_AS( t_modifier( t_test_string ), scarab::error );

    // two variables, two substitutions: ENV{SCARAB_TEST_VAR}ENV{SOME_OTHER_VAR} --> [throw scarab::error]
    t_test_string = "ENV{" + t_var + "}ENV{" + t_var_2 + "}";
    t_expected = t_target + t_target_2;
    REQUIRE_NOTHROW( t_modifier( t_test_string ) );
    REQUIRE( t_test_string == t_expected );

    unsetenv( t_var.c_str() );
    unsetenv( t_var_2.c_str() );
}
