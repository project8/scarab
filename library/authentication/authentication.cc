/*
 * mt_authentication.cc
 *
 *  Created on: Jun 15, 2015
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "authentication.hh"

#include "logger.hh"
#include "param_helpers_impl.hh"
#include "param_codec.hh"
#include "path.hh"

#include <cstdlib> // for getenv()
#include <stdlib.h> // for setenv(); remove after debugging

namespace scarab
{
    LOGGER( mtlog, "authentication" );

    authentication::authentication() :
//            f_design(
//                "groups"_a=param_node(
//                    "scarab"_a=param_node(
//                        "user"_a=param_node(
//                            "default"_a="scarab_user",
//                            "env"_a="SCARAB_USER"
//                        )
//                    )
//                ),
//                "file"_a="my_auth.yaml"
//            ),
            f_design(),
            f_data()
    {
        setenv("SCARAB_USER", "some_user", true);
    }

    authentication::~authentication()
    {}

    void authentication::set_auth_file( const std::string& a_filename, const param_node& a_read_opts )
    {
        f_design.replace( "file", a_filename );
        f_design.replace( "file-read-opts", a_read_opts );
        return;
    }

    void authentication::process_design()
    {
        try
        {
            f_data.clear();

            LDEBUG( mtlog, "Processing authentication design" );

            if( f_design.has( "file" ) )
            {
                LDEBUG( mtlog, "Loading authentication information from a file" );
                load_from_file( f_design["file"]().as_string(), f_design["file-read-opts"].as_node() );
            }

            // pull out the "groups" node; throws if not present or not a node
            const param_node& t_groups = f_design["groups"].as_node();
            for( auto it = t_groups.begin(); it != t_groups.end(); ++it )
            {
                LDEBUG( mtlog, "Found group <" << it.name() << ">" );
                // throws if not a node
                const param_node& t_a_group = it->as_node();

                param_node t_new_group;
                for( auto gr_it = t_a_group.begin(); gr_it != t_a_group.end(); ++gr_it )
                {
                    LDEBUG( mtlog, "Processing item <" << gr_it.name() << ">" );
                    // throws if not a node
                    const param_node& t_an_item = gr_it->as_node();

                    // default value; required; throws if not present or not string
                    std::string t_value( t_an_item["default"]().as_string() );
                    // environment variable; not required; throws if present and not a string
                    if( t_an_item.has("env") )
                    {
                        std::string t_env_var( t_an_item["env"]().as_string() );
                        LDEBUG( mtlog, "Found environment variable <" << t_env_var << "> with value <" << std::getenv( t_env_var.c_str() ) << ">" );
                        if( const char* t_env_value = std::getenv( t_env_var.c_str() ) )
                        {
                            t_value = t_env_value;
                        }
                    }

                    t_new_group.add( gr_it.name(), param_value(t_value) );
                    LDEBUG( mtlog, "Value is <" << t_value << ">" );
                }
                f_data.add( it.name(), t_new_group );
            }
            
        }
        catch( const std::exception& e )
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    void authentication::load_from_file( const std::string& a_auth_file, const param_node& a_read_opts )
    {
        // expand path with standard assumptions
        path t_auth_file_path = expand_path( a_auth_file );
        LINFO( mtlog, "Loading authentication file <" << t_auth_file_path << ">" );

        // is the file there?
        if( ! t_auth_file_path.empty() )
        {
            LDEBUG( mtlog, "Looking for authentication file: <" << t_auth_file_path << ">" );
            try
            {
                if( ! (exists( t_auth_file_path ) && is_regular_file( t_auth_file_path )) )
                {
                    throw error( __FILE__, __LINE__ ) << "File either doesn't exist (" << exists( t_auth_file_path ) << ") or isn't a regular file (" << is_regular_file( t_auth_file_path ) << ")";
                }
            }
            catch( boost::filesystem::filesystem_error& e )
            {
                throw error( __FILE__, __LINE__ ) << "Unable to determine if the authentication file is a regular file: " << e.what();
            }
        }

        // if so, load it
        param_translator t_translator;
        std::unique_ptr< param > t_read_file( t_translator.read_file( t_auth_file_path.string(), a_read_opts ) );
        if( t_read_file == NULL )
        {
            throw error( __FILE__, __LINE__ ) << "Unable to parse the authentication file <" << t_auth_file_path << ">";
        }
        if( ! t_read_file->is_node() )
        {
            throw error( __FILE__, __LINE__ ) << "Authentication file must translate to a node";
        }

        f_data.merge( t_read_file->as_node() );
        LDEBUG( mtlog, "File loaded; auth is now:\n" << f_data );
        return;
    }

    std::string authentication::get(const std::string& a_group, const std::string& an_item ) const
    {
        return f_data[a_group][an_item]().as_string();
    }

/*
    authentication::authentication( const string& a_auth_filename ) :
                    param_node(),
                    f_auth_filename( a_auth_filename ),
                    f_is_loaded( false )
    {
        load( a_auth_filename );
    }

    authentication::~authentication()
    {
    }

    bool authentication::load( const string& a_auth_file )
    {
        this->clear();

        f_auth_filename = a_auth_file;

        // expand path with standard assumptions
        path t_auth_file_path = expand_path( a_auth_file );

        // is the file there?
        bool t_auth_file_present = false;
        if( ! t_auth_file_path.empty() )
        {
            LDEBUG( mtlog, "Looking for authentication file: <" << t_auth_file_path << ">" );
            try
            {
                t_auth_file_present = exists( t_auth_file_path ) && is_regular_file( t_auth_file_path );
                if( ! t_auth_file_present )
                {
                    LERROR( mtlog, "File either doesn't exist (" << exists( t_auth_file_path ) << ") or isn't a regular file (" << is_regular_file( t_auth_file_path ) << ")" );
                    return false;
                }
            }
            catch( boost::filesystem::filesystem_error& e )
            {
                LERROR( mtlog, "Unable to determine if the authentication file is a regular file: " << e.what() );
                return false;
            }
        }

        // if so, load it
        if( t_auth_file_present )
        {
            param_translator t_translator;
            std::unique_ptr< param > t_read_file( t_translator.read_file( t_auth_file_path.string() ) );
            if( t_read_file == NULL )
            {
                LERROR( mtlog, "Unable to parse authentication file" );
                return false;
            }
            else if( ! t_read_file->is_node() )
            {
                LERROR( mtlog, "Authentication file must translate to a node" );
                return false;
            }
            else
            {
                this->param_node::operator=( std::move(t_read_file->as_node()) );
                LDEBUG( mtlog, "Authentications:\n" << *this );
                f_is_loaded = true;
                return true;
            }
        }
        else
        {
            LWARN( mtlog, "Authentications not loaded" );
            return false;
        }
    }
*/
} /* namespace scarab */
