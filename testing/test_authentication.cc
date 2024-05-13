/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "authentication.hh"

#include "param.hh"
#include "param_codec.hh"
#include "param_helpers_impl.hh"

#include "logger.hh"

#include "catch.hpp"

#include <cstdio>
#include <stdlib.h>

using_param_args_and_kwargs; // bring _a into namespace

LOGGER( talog, "test_auth" );

// Note that we have to go through the whole trouble with file-reading options to specify the codec because 
// the temporary filename can't get an extension (the last characters need to be the randomly-assigned characters).

/// Creates a temporary file and loads it with authentication data
int create_temp_auth_file( std::string& a_filename, scarab::param_node& a_file_opts )
{
    scarab::param_node t_auth(
        "scarab"_a=scarab::param_node(
            "user"_a="file_user",
            "password"_a="p@55w0rd"
        )
    );

    a_file_opts = scarab::param_node(
        "encoding"_a="json"
    );

    std::string t_buffer;
    scarab::param_translator t_translator;
    t_translator.write_string( t_auth, t_buffer, a_file_opts );

    int fd;
    char t_new_name[] = "/tmp/fileXXXXXX";
    fd = mkstemp( t_new_name );
    a_filename = std::string( t_new_name );
    write( fd, t_buffer.c_str(), t_buffer.size() );

    return fd;
}


TEST_CASE( "authentication", "[authentication]" )
{
    using Catch::Matchers::Equals;

    scarab::authentication t_auth;

    REQUIRE( t_auth.design().empty() );
    REQUIRE( t_auth.data().empty() );

    SECTION( "Loading from a file" )
    {
        std::string t_filename;
        scarab::param_node t_read_opts;
        int fd = create_temp_auth_file( t_filename, t_read_opts );

        t_auth.set_auth_file( t_filename, t_read_opts );
        REQUIRE( t_auth.design().has("file") );

        t_auth.process_design();

        REQUIRE( t_auth.data().has("scarab") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );

        close( fd );
    }

    SECTION( "Set design" )
    {
        // group1 will be empty
        // It will not get added to the data
        t_auth.add_group( "group1" );
        REQUIRE( t_auth.design()["groups"].as_node().has("group1") );

        // group2 will have an item, but the environment variable will not be set
        // The value should be the default
        t_auth.add_group( "group2" );
        t_auth.add_item( "group2", "name2", "default2", "SCARAB_AUTH_TEST_ENV2" );
        REQUIRE( t_auth.design()["groups"].as_node().has("group2") ); // group should be there
        REQUIRE( t_auth.design()["groups"]["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.design()["groups"]["group2"]["name2"]["default"]().as_string(), Equals("default2") );
        REQUIRE_THAT( t_auth.design()["groups"]["group2"]["name2"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV2") );
        
        // group3 will have an item, and the environment variable will be set
        // The group is not separately added; it should still be there
        // The value should be the environment variable's value
        t_auth.add_item( "group3", "name3", "default3", "SCARAB_AUTH_TEST_ENV3" );
        setenv( "SCARAB_AUTH_TEST_ENV3", "env_value_3", true );
        REQUIRE( t_auth.design()["groups"].as_node().has("group3") ); // group should be there
        REQUIRE( t_auth.design()["groups"]["group3"].as_node().has("name3") ); // item should be there
        REQUIRE_THAT( t_auth.design()["groups"]["group3"]["name3"]["default"]().as_string(), Equals("default3") );
        REQUIRE_THAT( t_auth.design()["groups"]["group3"]["name3"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV3") );

        t_auth.process_design();
        unsetenv( "SCARAB_AUTH_TEST_ENV3" );

        REQUIRE_FALSE( t_auth.data().has("group1") ); // group should not be there (was empty in the design)

        REQUIRE( t_auth.data().has("group2") ); // group should be there
        REQUIRE( t_auth.data()["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals(t_auth.data()["group2"]["name2"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals("default2") );

        REQUIRE( t_auth.data().has("group3") ); // group should be there
        REQUIRE( t_auth.data()["group3"].as_node().has("name3") ); // item should be there
        REQUIRE_THAT( t_auth.get("group3", "name3"), Equals(t_auth.data()["group3"]["name3"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group3", "name3"), Equals("env_value_3") );
    }

    SECTION( "Precedence: default --> file --> env var" )
    {
        // scarab.user is determined by the file
        // scarab.password gets is determined by the environment variable

        std::string t_filename;
        scarab::param_node t_read_opts;
        int fd = create_temp_auth_file( t_filename, t_read_opts );

        t_auth.set_auth_file( t_filename, t_read_opts );
        REQUIRE( t_auth.design().has("file") );

        t_auth.add_item( "scarab", "user", "a_user", "SCARAB_AUTH_TEST_USER" );
        t_auth.add_item( "scarab", "password", "a_user", "SCARAB_AUTH_TEST_PASSWORD" );

        setenv( "SCARAB_AUTH_TEST_PASSWORD", "123456", true );

        t_auth.process_design();
        unsetenv( "SCARAB_AUTH_TEST_PASSWORD" );

        REQUIRE( t_auth.data().has( "scarab" ) );
        REQUIRE( t_auth.data()["scarab"].as_node().has("user") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("password") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );
        REQUIRE_THAT( t_auth.get("scarab", "password"), Equals("123456") );

    }

}
