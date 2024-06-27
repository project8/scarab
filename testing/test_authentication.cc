/*
 * test_authentication.cc
 *
 *  Created on: May 6, 2024
 *      Author: N.S. Oblath
 */

#include "authentication.hh"

#include "param.hh"
#include "param_codec.hh"
#include "param_helpers_impl.hh"

#include "logger.hh"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <cstdio>
#include <stdlib.h>

using_param_args_and_kwargs; // bring _a into namespace

LOGGER( talog, "test_auth" );

/// Creates a temporary file and loads it with the provided contents
int create_temp_file( const std::string& a_contents, std::string& a_filename )
{
    int fd;
    char t_new_name[] = "/tmp/fileXXXXXX";
    fd = mkstemp( t_new_name );
    a_filename = std::string( t_new_name );
    write( fd, a_contents.c_str(), a_contents.size() );

    return fd;
}

/// Creates a temporary file and loads it with authentication data
/// Note that we have to go through the whole trouble with file-reading options to specify the codec because 
/// the temporary filename can't get an extension (the last characters need to be the randomly-assigned characters).
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

    return create_temp_file( t_buffer, a_filename );
}


/// This class provides access to protected functions to enable testing
class auth_revealer : public scarab::authentication
{
    public:
        std::string pub_read_from_file( const std::string& a_filename ) const
        {
            return this->read_from_file( a_filename );
        }

        scarab::param_ptr_t pub_load_from_auth_file( const std::string& a_auth_file, const scarab::param_node& a_read_opts ) const
        {
            return this->load_from_auth_file( a_auth_file, a_read_opts );
        }
};


TEST_CASE( "authentication", "[authentication]" )
{
    using Catch::Matchers::Equals;

    auth_revealer t_auth;

    REQUIRE( t_auth.spec().has("groups") );
    REQUIRE( t_auth.data().empty() );

    const std::string t_text( "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." );

    SECTION( "Utility functions" )
    {
        // Reading a file
        std::string t_filename_1;
        int fd_1 = create_temp_file( t_text, t_filename_1 );

        std::string t_read_from_file( t_auth.pub_read_from_file( t_filename_1 ) );
        //LDEBUG( talog, "read text:\n" << t_read_from_file );
        REQUIRE_THAT( t_read_from_file, Equals(t_text) );

        close( fd_1 );

        // Reading an auth file
        std::string t_filename_2;
        scarab::param_node t_read_opts_2;
        int fd_2 = create_temp_auth_file( t_filename_2, t_read_opts_2 );

        scarab::param_ptr_t t_loaded = t_auth.pub_load_from_auth_file( t_filename_2, t_read_opts_2 );
        REQUIRE( t_loaded->is_node() );
        REQUIRE( (*t_loaded)["scarab"].is_node() );
        REQUIRE( (*t_loaded)["scarab"]["user"]().as_string() == "file_user" );
        REQUIRE( (*t_loaded)["scarab"]["password"]().as_string() == "p@55w0rd" );

        close( fd_2 );
    }

    SECTION( "Loading from a file" )
    {
        std::string t_filename;
        scarab::param_node t_read_opts;
        int fd = create_temp_auth_file( t_filename, t_read_opts );

        t_auth.set_auth_file( t_filename, t_read_opts );
        REQUIRE( t_auth.spec().has("file") );

        t_auth.process_spec();

        REQUIRE( t_auth.data().has("scarab") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );
        REQUIRE_THAT( t_auth.get("scarab", "password"), Equals("p@55w0rd") );

        close( fd );
    }

    SECTION( "Set spec" )
    {
        // group1 will be empty
        // It will not get added to the data
        t_auth.add_group( "group1" );
        REQUIRE( t_auth.spec()["groups"].as_node().has("group1") );
        REQUIRE( t_auth.spec()["groups"]["group1"].as_node().empty() );

        // group2 will have an item, but the environment variable will not be set
        // After processing the value should be the default
        t_auth.add_group( "group2" );
        t_auth.add_item( "group2", "name2", "default2", "SCARAB_AUTH_TEST_ENV2" );
        REQUIRE( t_auth.spec()["groups"].as_node().has("group2") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["default"]().as_string(), Equals("default2") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV2") );
        
        // group3 will have three times that will test overriding the default with
        //  1. an environment variable
        //  2. a file override
        //  3. a value override
        // The group is not separately added; it should still be there
        setenv( "SCARAB_AUTH_TEST_ENV3_1", "env_value_3_1", true );
        std::string t_filename_3_2;
        int fd_3_2 = create_temp_file( t_text, t_filename_3_2 );

        // adding an override file or value should throw before a group exists
        REQUIRE_THROWS_AS( t_auth.set_override_file( "group3", "name3_2", t_filename_3_2 ), scarab::error );
        REQUIRE_THROWS_AS( t_auth.set_override_value( "group3", "name3_3", "value3_3" ), scarab::error );
        t_auth.add_item( "group3", "name3_1", "default3_1", "SCARAB_AUTH_TEST_ENV3_1" );
        // adding an override file or value should throw if the group exists, but the relevant item does not
        REQUIRE_THROWS_AS( t_auth.set_override_file( "group3", "name3_2", t_filename_3_2 ), scarab::error );
        REQUIRE_THROWS_AS( t_auth.set_override_value( "group3", "name3_3", "value3_3" ), scarab::error );
        t_auth.add_item( "group3", "name3_2", "default3_2" );
        REQUIRE_NOTHROW( t_auth.set_override_file( "group3", "name3_2", t_filename_3_2 ) );
        t_auth.add_item( "group3", "name3_3", "default3_3" );
        REQUIRE_NOTHROW( t_auth.set_override_value( "group3", "name3_3", "value3_3" ) );
        REQUIRE( t_auth.spec()["groups"].as_node().has("group3") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group3"].as_node().has("name3_1") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_1"]["default"]().as_string(), Equals("default3_1") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_1"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV3_1") );
        REQUIRE( t_auth.spec()["groups"]["group3"].as_node().has("name3_2") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_2"]["default"]().as_string(), Equals("default3_2") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_2"]["override-file"]().as_string(), Equals(t_filename_3_2) );
        REQUIRE( t_auth.spec()["groups"]["group3"].as_node().has("name3_3") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_3"]["default"]().as_string(), Equals("default3_3") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3_3"]["override"]().as_string(), Equals("value3_3") );

        t_auth.process_spec();

        unsetenv( "SCARAB_AUTH_TEST_ENV3_1" );
        close( fd_3_2 );

        REQUIRE_FALSE( t_auth.data().has("group1") ); // group should not be there (was empty in the specification)

        REQUIRE( t_auth.data().has("group2") ); // group should be there
        REQUIRE( t_auth.data()["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals(t_auth.data()["group2"]["name2"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals("default2") );

        REQUIRE( t_auth.data().has("group3") ); // group should be there
        REQUIRE( t_auth.data()["group3"].as_node().has("name3_1") ); // item should be there
        REQUIRE_THAT( t_auth.get("group3", "name3_1"), Equals(t_auth.data()["group3"]["name3_1"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group3", "name3_1"), Equals("env_value_3_1") );
        REQUIRE( t_auth.data()["group3"].as_node().has("name3_2") ); // item should be there
        REQUIRE_THAT( t_auth.get("group3", "name3_2"), Equals(t_auth.data()["group3"]["name3_2"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group3", "name3_2"), Equals(t_text) );
        REQUIRE( t_auth.data()["group3"].as_node().has("name3_2") ); // item should be there
        REQUIRE_THAT( t_auth.get("group3", "name3_3"), Equals(t_auth.data()["group3"]["name3_3"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group3", "name3_3"), Equals("value3_3") );
    }

    SECTION( "Add spec via add_groups" )
    {
        scarab::param_node t_groups(
            "group1"_a=scarab::param_node(),
            "group2"_a=scarab::param_node(
                "name2"_a=scarab::param_node(
                    "default"_a="default2",
                    "env"_a="SCARAB_AUTH_TEST_ENV2"
                )
            ),
            "group3"_a=scarab::param_node(
                "name3"_a=scarab::param_node(
                    "default"_a="default3",
                    "env"_a="SCARAB_AUTH_TEST_ENV3",
                    "override"_a="override3",
                    "override-file"_a="override-file3"
                )
            )
        );
        t_auth.add_groups( t_groups );

        REQUIRE( t_auth.spec()["groups"].as_node().has("group1") );
        REQUIRE( t_auth.spec()["groups"]["group1"].as_node().empty() );

        REQUIRE( t_auth.spec()["groups"].as_node().has("group2") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["default"]().as_string(), Equals("default2") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV2") );

        REQUIRE( t_auth.spec()["groups"].as_node().has("group3") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group3"].as_node().has("name3") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["default"]().as_string(), Equals("default3") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV3") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["override"]().as_string(), Equals("override3") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["override-file"]().as_string(), Equals("override-file3") );
    }

    SECTION( "Precedence: default --> file --> env var --> override file --> override value" )
    {
        // scarab.favorite-color is determined by the default
        // scarab.user is determined by the file
        // scarab.password is determined by the environment variable
        // scarab.lorem is determined by an override file
        // scarab.licks-to-get-to-center-of-tootsie-pop is determined by an override value

        std::string t_auth_filename;
        scarab::param_node t_read_opts;
        int fd_auth = create_temp_auth_file( t_auth_filename, t_read_opts );

        std::string t_lorem_filename;
        int fd_lorem = create_temp_file( t_text, t_lorem_filename );

        t_auth.set_auth_file( t_auth_filename, t_read_opts );
        REQUIRE( t_auth.spec().has("file") );

        t_auth.add_item( "scarab", "favorite-color", "blue", "SCARAB_FAVORITE_COLOR" );
        t_auth.add_item( "scarab", "user", "default_user", "SCARAB_AUTH_TEST_USER" );
        t_auth.add_item( "scarab", "password", "default_password", "SCARAB_AUTH_TEST_PASSWORD" );
        t_auth.add_item( "scarab", "lorem", "Lorem ipsum", "SCARAB_AUTH_TEST_LOREM_IPSUM" );
        t_auth.set_override_file( "scarab", "lorem", t_lorem_filename );
        t_auth.add_item( "scarab", "licks-to-get-to-center-of-tootsie-pop", "one", "SCARAB_AUTH_TEST_LICKS_FOR_TOOTSIE_POP" );
        t_auth.set_override_file( "scarab", "licks-to-get-to-center-of-tootsie-pop", t_lorem_filename );
        t_auth.set_override_value( "scarab", "licks-to-get-to-center-of-tootsie-pop", "three" );

        unsetenv( "SCARAB_FAVORITE_COLOR" );
        unsetenv( "SCARAB_AUTH_TEST_USER" );
        setenv( "SCARAB_AUTH_TEST_PASSWORD", "123456", true );
        setenv( "SCARAB_AUTH_TEST_LOREM_IPSUM", "Lorem ipsum dolor sit amet", true );
        setenv( "SCARAB_AUTH_TEST_LICKS_FOR_TOOTSIE_POP", "two", true );

        t_auth.process_spec();

        unsetenv( "SCARAB_AUTH_TEST_PASSWORD" );
        unsetenv( "SCARAB_AUTH_TEST_LOREM_IPSUM" );
        unsetenv( "SCARAB_AUTH_TEST_LICKS_FOR_TOOTSIE_POP" );
        close( fd_auth );
        close( fd_lorem );

        REQUIRE( t_auth.data().has( "scarab" ) );

        REQUIRE( t_auth.data()["scarab"].as_node().has("favorite-color") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("user") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("password") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("lorem") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("licks-to-get-to-center-of-tootsie-pop") );

        REQUIRE_THAT( t_auth.get("scarab", "favorite-color"), Equals("blue") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );
        REQUIRE_THAT( t_auth.get("scarab", "password"), Equals("123456") );
        REQUIRE_THAT( t_auth.get("scarab", "lorem"), Equals(t_text) );
        REQUIRE_THAT( t_auth.get("scarab", "licks-to-get-to-center-of-tootsie-pop"), Equals("three") );
    }

}
