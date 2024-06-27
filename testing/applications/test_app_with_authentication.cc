/*
 * test_app_with_options.cc
 *
 *  Created on: Oct 28, 2018
 *      Author: N.S. Oblath
 *
 *  Output examples:
 *
    > bin/test_app_with_options -f 5 -s "hello" -t "world"
    2018-10-29 11:15:20 [ PROG] (tid 0x7fffb0945380) i/application.cc(97): Final configuration:

    {
        third-value : world
    }

    2018-10-29 11:15:20 [ PROG] (tid 0x7fffb0945380) i/application.cc(98): Ordered args:

    [
    ]

    2018-10-29 11:15:20 [ PROG] (tid 0x7fffb0945380) _with_options.cc(69): First value: 5
    2018-10-29 11:15:20 [ PROG] (tid 0x7fffb0945380) _with_options.cc(70): Second value: hello
    2018-10-29 11:15:20 [ PROG] (tid 0x7fffb0945380) _with_options.cc(71): Third value: world
 *
 */

#include "application.hh"

#include "logger.hh"
#include "param_codec.hh"
#include "param_helpers_impl.hh"

#include <cstdio>
#include <stdlib.h>


using namespace scarab;

LOGGER( testlog, "test_app_with_authentication" );

class test_app : public main_app
{
    public:
        test_app(bool a_use_config = true) :
            main_app(a_use_config)
        {

            f_default_config.add( "auth-groups", scarab::param_node(
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
        }
        virtual ~test_app() {}

        void execute()
        {
            // Print the authentication information, both specification and data
            LPROG( testlog, "Authentication specification: " << f_auth.spec() );
            LPROG( testlog, "Authentication data: " << f_auth.data() );
            return;
        }
};


/// Creates a temporary file and loads it with authentication specification
int create_temp_config_file( std::string& a_filename, scarab::param_node& a_file_opts )
{
    scarab::param_node t_auth(
        "auth-groups"_a=scarab::param_node(
            "database"_a=scarab::param_node(
                "user"_a=scarab::param_node(
                    "default"_a="a_db_user",
                    "env"_a="SCARAB_AUTH_TEST_DB_USER"
                ),
                "password"_a=scarab::param_node(
                    "default"_a="",
                    "env"_a="SCARAB_AUTH_TEST_DB_PASSWORD"
                )
            )
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


int main( int argc, char **argv )
{
    std::string t_filename;
    scarab::param_node t_read_opts;
    int fd = create_temp_config_file( t_filename, t_read_opts );

    test_app the_main( true ); // boolean option is for skipping use of a config file
    auto t_executor = [&](){
        the_main.execute();
    };
    the_main.callback( t_executor );

    // provide a filename directly 
    // In real life would be provided on CL w/ -c or --config
    the_main.config_filename() = t_filename; 
    the_main.config_encoding() = t_read_opts["encoding"]().as_string();

    setenv( "SCARAB_AUTH_TEST_DB_PASSWORD", "5up3r 53cr3t", true );

    CLI11_PARSE( the_main, argc, argv );

    unsetenv( "SCARAB_AUTH_TEST_DB_PASSWORD" );

    close( fd );

    return 0;
}
