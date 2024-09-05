/*
 * test_main_app.cc
 *
 *  Created on: May 28, 2024
 *      Author: N.S. Oblath
 */

#include "application.hh"

#include "param.hh"
#include "param_codec.hh"
#include "param_helpers_impl.hh"

#include "logger.hh"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"

#include <cstdio>
#include <stdlib.h>

using_param_args_and_kwargs; // bring _a into namespace

LOGGER( talog, "test_main_app" );
/*
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
*/

/// Creates a temporary file and loads it with data
struct file_holder
{
    /// Creates the file and loads the data; returns the filename and opts via arguments
    file_holder( std::string& a_filename, scarab::param_node& a_file_opts, const scarab::param_node& a_data )
    {
        a_file_opts = scarab::param_node(
            "encoding"_a="json"
        );

        std::string t_buffer;
        scarab::param_translator t_translator;
        t_translator.write_string( a_data, t_buffer, a_file_opts );

        char t_new_name[] = "/tmp/fileXXXXXX";
        f_file_descriptor = mkstemp( t_new_name );
        a_filename = std::string( t_new_name );
        write( f_file_descriptor, t_buffer.c_str(), t_buffer.size() );
    }
    ~file_holder()
    {
        close( f_file_descriptor );
    }
    int f_file_descriptor;
};


TEST_CASE( "configuration", "[main_app]" )
{
    using Catch::Matchers::Equals;

    scarab::main_app t_app;

    REQUIRE( t_app.default_config().empty() );
    REQUIRE( t_app.primary_config().empty() );

    t_app.default_config().add( "value", 5 );

    REQUIRE( t_app.default_config()["value"]().as_int() == 5 );

    // for configuration stage 2: config file
    scarab::param_node t_data(
        "value"_a=scarab::param_value(10)
    );
    std::string t_filename;
    scarab::param_node t_read_opts;
    file_holder t_file( t_filename, t_read_opts, t_data );

    SECTION( "No config" )
    {
        t_app.set_use_config( false );
        REQUIRE_FALSE( t_app.get_use_config() );

        t_app.do_config_stage_1();
        REQUIRE_FALSE( t_app.primary_config().has("value") );

        t_app.config_filename() = t_filename;
        t_app.config_encoding() = t_read_opts["encoding"]().as_string();
        t_app.do_config_stage_2();
        REQUIRE_FALSE( t_app.primary_config().has("value") );

        t_app.nonoption_kw_args().add( "value", 15 );
        // this gets used in stage 5
        t_app.nonoption_kw_args().add( "fromenv", "ENV{MAIN_APP_TEST_VAR}");
        t_app.do_config_stage_3();
        REQUIRE_FALSE( t_app.primary_config().has("value") );

        REQUIRE_NOTHROW( t_app.add_config_option< unsigned >( "--test-value", "test-value", "Set the value keyword" ) );
        CLI::Option* t_opt = t_app.get_option_no_throw( "--test-value" );
        REQUIRE_FALSE( t_opt == nullptr );
        t_opt->add_result("20");
        REQUIRE( t_opt->as< unsigned >() == 20 );
        t_app.do_config_stage_4();
        REQUIRE_FALSE( t_app.app_options().has("value") );
        REQUIRE_FALSE( t_app.primary_config().has("value") );

        REQUIRE_THAT( t_app.nonoption_kw_args()["fromenv"]().as_string(), Equals("ENV{MAIN_APP_TEST_VAR}") );
        setenv( "MAIN_APP_TEST_VAR", "500", true );
        t_app.do_config_stage_5();
        unsetenv( "MAIN_APP_TEST_VAR" );
        REQUIRE_FALSE( t_app.primary_config().has("fromenv") );

    }

    SECTION( "Yes config" )
    {
        REQUIRE( t_app.get_use_config() );

        // Stage 1: default config
        t_app.do_config_stage_1();
        REQUIRE( t_app.primary_config()["value"]().as_int() == 5 );

        // Stage 2: config file
        t_app.config_filename() = t_filename;
        t_app.config_encoding() = t_read_opts["encoding"]().as_string();
        t_app.do_config_stage_2();
        REQUIRE( t_app.primary_config()["value"]().as_int() == 10 );

        // Stage 3: non-option keyword arguments (e.g. value=15, parsed from CL)
        t_app.nonoption_kw_args().add( "value", 15 );
        // this gets used in stage 5
        t_app.nonoption_kw_args().add( "fromenv", "ENV{MAIN_APP_TEST_VAR}");
        t_app.do_config_stage_3();
        REQUIRE( t_app.primary_config()["value"]().as_int() == 15 );

        // Stage 4: application-specified options
        REQUIRE_NOTHROW( t_app.add_config_option< unsigned >( "--value", "value", "Set the value keyword" ) );
        CLI::Option* t_opt = t_app.get_option_no_throw( "--value" );
        REQUIRE_FALSE( t_opt == nullptr );
        t_opt->add_result("20"); // equivalent of providing the value on the CL
        REQUIRE( t_opt->as< unsigned >() == 20 );
        t_opt->run_callback(); // stores the value provided to the option into the variable provided when the option was created
        t_app.do_config_stage_4(); // take the values from each option's variable and push it into t_app.f_app_options; then merge into f_primary_config
        REQUIRE( t_app.app_options()["value"]().as_int() == 20 );
        REQUIRE( t_app.primary_config()["value"]().as_int() == 20 );

        // TODO: flags and option vectors

        // Stage 5: environment-variable replacement
        REQUIRE_THAT( t_app.primary_config()["fromenv"]().as_string(), Equals("ENV{MAIN_APP_TEST_VAR}") );
        setenv( "MAIN_APP_TEST_VAR", "500", true );
        t_app.do_config_stage_5();
        unsetenv( "MAIN_APP_TEST_VAR" );
        REQUIRE( t_app.primary_config()["fromenv"]().as_int() == 500 );
    }

    // TODO: test it all with pre_callback()

}

TEST_CASE( "options", "[main_app]" )
{
    scarab::main_app t_app;

    // check options:
    // --help
    // --config
    // --config-encoding
    // --verbose
    // --quiet
    // --version
}

TEST_CASE( "app-authentication", "[main_app]" )
{
    using Catch::Matchers::Equals;

    scarab::main_app t_app;

    REQUIRE_THAT( t_app.auth_groups_key(), Equals("auth-groups") );
    REQUIRE( t_app.auth().spec().has("groups") );
    REQUIRE( t_app.auth().spec()["groups"].as_node().empty() );
    REQUIRE_FALSE( t_app.primary_config().has(t_app.auth_groups_key() ) );

    SECTION( "by groups" )
    {
        t_app.primary_config().add( "auth-groups", scarab::param_node(
            "backend"_a=scarab::param_node(
                "user"_a=scarab::param_node(
                    "default"_a="a_backend_user",
                    "env"_a="SCARAB_AUTH_TEST_BACKEND_USER"
                ),
                "password"_a=scarab::param_node(
                    "default"_a="security_hole",
                    "env"_a="SCARAB_AUTH_TEST_BACKEND_PASSWORD"
                )
            )
        ) );

        t_app.do_authentication();

        // testing the authentication spec
        LWARN( talog, "app's auth spec:\n" << t_app.auth().spec() );
        REQUIRE( t_app.auth().spec().has("groups") );
        REQUIRE( t_app.auth().spec()["groups"].as_node().has("backend") );
        REQUIRE( t_app.auth().spec()["groups"]["backend"].as_node().has("user") );
        REQUIRE( t_app.auth().spec()["groups"]["backend"].as_node().has("password") );

        // testing the authentication data
        LWARN( talog, "app's auth data:\n" << t_app.auth().data() );
        REQUIRE( t_app.auth().has("backend") );
        REQUIRE( t_app.auth().has("backend", "user") );
        REQUIRE( t_app.auth().has("backend", "password") );
        REQUIRE_THAT( t_app.auth().get("backend", "user"), Equals("a_backend_user") );
        REQUIRE_THAT( t_app.auth().get("backend", "password"), Equals("security_hole") );
    }
/*
    SECTION( "by auth file" )
    {
        //TODO
    }
    */
}

/*
TEST_CASE( "authentication", "[authentication]" )
{
    using Catch::Matchers::Equals;

    scarab::authentication t_auth;

    REQUIRE( t_auth.spec().has("groups") );
    REQUIRE( t_auth.data().empty() );

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
        // The value should be the default
        t_auth.add_group( "group2" );
        t_auth.add_item( "group2", "name2", "default2", "SCARAB_AUTH_TEST_ENV2" );
        REQUIRE( t_auth.spec()["groups"].as_node().has("group2") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["default"]().as_string(), Equals("default2") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group2"]["name2"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV2") );
        
        // group3 will have an item, and the environment variable will be set
        // The group is not separately added; it should still be there
        // The value should be the environment variable's value
        t_auth.add_item( "group3", "name3", "default3", "SCARAB_AUTH_TEST_ENV3" );
        setenv( "SCARAB_AUTH_TEST_ENV3", "env_value_3", true );
        REQUIRE( t_auth.spec()["groups"].as_node().has("group3") ); // group should be there
        REQUIRE( t_auth.spec()["groups"]["group3"].as_node().has("name3") ); // item should be there
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["default"]().as_string(), Equals("default3") );
        REQUIRE_THAT( t_auth.spec()["groups"]["group3"]["name3"]["env"]().as_string(), Equals("SCARAB_AUTH_TEST_ENV3") );

        t_auth.process_spec();
        unsetenv( "SCARAB_AUTH_TEST_ENV3" );

        REQUIRE_FALSE( t_auth.data().has("group1") ); // group should not be there (was empty in the specification)

        REQUIRE( t_auth.data().has("group2") ); // group should be there
        REQUIRE( t_auth.data()["group2"].as_node().has("name2") ); // item should be there
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals(t_auth.data()["group2"]["name2"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group2", "name2"), Equals("default2") );

        REQUIRE( t_auth.data().has("group3") ); // group should be there
        REQUIRE( t_auth.data()["group3"].as_node().has("name3") ); // item should be there
        REQUIRE_THAT( t_auth.get("group3", "name3"), Equals(t_auth.data()["group3"]["name3"]().as_string()) );
        REQUIRE_THAT( t_auth.get("group3", "name3"), Equals("env_value_3") );
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
                    "env"_a="SCARAB_AUTH_TEST_ENV3"
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
    }

    SECTION( "Precedence: default --> file --> env var" )
    {
        // scarab.user is determined by the file
        // scarab.password gets is determined by the environment variable

        std::string t_filename;
        scarab::param_node t_read_opts;
        int fd = create_temp_auth_file( t_filename, t_read_opts );

        t_auth.set_auth_file( t_filename, t_read_opts );
        REQUIRE( t_auth.spec().has("file") );

        t_auth.add_item( "scarab", "user", "a_user", "SCARAB_AUTH_TEST_USER" );
        t_auth.add_item( "scarab", "password", "a_user", "SCARAB_AUTH_TEST_PASSWORD" );

        setenv( "SCARAB_AUTH_TEST_PASSWORD", "123456", true );

        t_auth.process_spec();
        unsetenv( "SCARAB_AUTH_TEST_PASSWORD" );

        REQUIRE( t_auth.data().has( "scarab" ) );
        REQUIRE( t_auth.data()["scarab"].as_node().has("user") );
        REQUIRE( t_auth.data()["scarab"].as_node().has("password") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );
        REQUIRE_THAT( t_auth.get("scarab", "password"), Equals("123456") );

    }
}
*/
