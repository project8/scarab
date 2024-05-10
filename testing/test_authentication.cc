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

using_param_args_and_kwargs; // bring _a into namespace

LOGGER( talog, "test_auth" );

// Note that we have to go through the whole trouble with file-reading options to specify the codec because 
// the temporary filename can't get an extension (the last characters need to be the randomly-assigned characters).

/// Creates a temporary file and loads it with authentication data
int create_temp_auth_file( std::string& a_filename, scarab::param_node& a_file_opts )
{
    scarab::param_node t_auth(
        "scarab"_a=scarab::param_node(
            "user"_a="file_user"
        )
    );

    a_file_opts = scarab::param_node(
        "encoding"_a="json"
    );

    std::string t_buffer;
    scarab::param_translator t_translator;
    t_translator.write_string( t_auth, t_buffer, a_file_opts );
    LWARN( talog, "buffer string:\n" << t_buffer );
    LWARN( talog, "size: " << t_buffer.size() );

    int fd;
    char t_new_name[] = "/tmp/fileXXXXXX";
    fd = mkstemp( t_new_name );
    a_filename = std::string( t_new_name );
    write( fd, t_buffer.c_str(), t_buffer.size() );

    LWARN( talog, "temp file: " << a_filename );
    LWARN( talog, "read opts:\n" << a_file_opts )

    return fd;
}


TEST_CASE( "authentication", "[authentication]" )
{
    using Catch::Matchers::Equals;

    scarab::authentication t_auth;

    SECTION( "Loading from a file" )
    {
        LWARN( talog, "in loading file section" );

        std::string t_filename;
        scarab::param_node t_read_opts;
        int fd = create_temp_auth_file( t_filename, t_read_opts );

        LWARN( talog, "created file <" << t_filename << ">" );
        LWARN( talog, "file read opts are\n" << t_read_opts );

        t_auth.set_auth_file( t_filename, t_read_opts );
        REQUIRE( t_auth.design().has("file") );

        LWARN( talog, "added filename:\n" << t_auth.design() );

        t_auth.process_design();

        LWARN( talog, t_auth.data() );
        LWARN( talog, t_auth.data().has("scarab") );

        REQUIRE( t_auth.data().has("scarab") );
        REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("file_user") );

        close( fd );
    }

    //t_auth.process_design();

    //REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("some_user") );
}
