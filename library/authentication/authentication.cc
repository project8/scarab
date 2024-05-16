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

namespace scarab
{
    LOGGER( mtlog, "authentication" );

    authentication::authentication() :
            f_spec(),
            f_data()
    {
        setenv("SCARAB_USER", "some_user", true);
    }

    authentication::~authentication()
    {}

    void authentication::add_group( const std::string& a_group )
    {
        f_spec.add( "groups", param_node() );
        f_spec["groups"].as_node().add( a_group, param_node() );
        LDEBUG( mtlog, "Added group <" << a_group << "> (or it was already there)" );
        return;
    }

    void authentication::add_item( const std::string& a_group, const std::string& a_name, const std::string& a_default, const std::string& an_env )
    {
        add_group( a_group );
        param_node t_item( "default"_a=a_default );
        if( ! an_env.empty() ) t_item.add( "env", an_env );
        f_spec["groups"][a_group].as_node().replace( a_name, t_item );
        LDEBUG( mtlog, "Added item <" << a_group << ":" << a_name << ">" );
        return;
    }

    void authentication::add_groups( const param_node& a_groups_node )
    {
        for( auto it = a_groups_node.begin(); it != a_groups_node.end(); ++it )
        {
            LDEBUG( mtlog, "Found group <" << it.name() << ">" );
            // throws if not a node
            const param_node& t_a_group = it->as_node();
            if( t_a_group.empty() )
            {
                LWARN( mtlog, "Group <" << it.name() << "> is empty; skipping" );
                continue;
            }

            param_node t_new_default_group, t_new_env_group;
            for( auto gr_it = t_a_group.begin(); gr_it != t_a_group.end(); ++gr_it )
            {
                LDEBUG( mtlog, "Processing item <" << gr_it.name() << ">" );
                // throws if not a node
                const param_node& t_an_item = gr_it->as_node();

                // default value; required; throws if not present or not string
                add_item( it.name(), gr_it.name(), t_an_item["default"]().as_string(), t_an_item.get_value("env", "") );
            }
        return;
    }

    void authentication::set_auth_file( const std::string& a_filename, const param_node& a_read_opts )
    {
        f_spec.replace( "file", a_filename );
        f_spec.replace( "file-read-opts", a_read_opts );
        LDEBUG( mtlog, "Added filename: " << f_spec["file"] );
        return;
    }

    void authentication::process_spec()
    {
        try
        {
            LDEBUG( mtlog, "Processing authentication specification" );
            //LWARN( mtlog, f_spec );

            param_ptr_t t_file_data_ptr;
            if( f_spec.has( "file" ) )
            {
                LDEBUG( mtlog, "Loading authentication information from a file" );
                t_file_data_ptr = load_from_file( f_spec["file"]().as_string(), f_spec["file-read-opts"].as_node() );
            }

            param_node t_default_data, t_env_data;

            if( f_spec.has("groups" ) )
            {
                // pull out the "groups" node; throws if not a node
                const param_node& t_groups = f_spec["groups"].as_node();
                for( auto it = t_groups.begin(); it != t_groups.end(); ++it )
                {
                    LDEBUG( mtlog, "Found group <" << it.name() << ">" );
                    // throws if not a node
                    const param_node& t_a_group = it->as_node();
                    if( t_a_group.empty() )
                    {
                        LWARN( mtlog, "Group <" << it.name() << "> is empty; skipping" );
                        continue;
                    }

                    param_node t_new_default_group, t_new_env_group;
                    for( auto gr_it = t_a_group.begin(); gr_it != t_a_group.end(); ++gr_it )
                    {
                        LDEBUG( mtlog, "Processing item <" << gr_it.name() << ">" );
                        // throws if not a node
                        const param_node& t_an_item = gr_it->as_node();

                        // default value; required; throws if not present or not string
                        t_new_default_group.add( gr_it.name(), t_an_item["default"]().as_string() );

                        // environment variable; not required; throws if present and not a string
                        if( t_an_item.has("env") )
                        {
                            std::string t_env_var( t_an_item["env"]().as_string() );
                            LDEBUG( mtlog, "Checking for environment variable < " << t_env_var << ">" );
                            if( const char* t_env_value = std::getenv( t_env_var.c_str() ) )
                            {
                                LDEBUG( mtlog, "Found environment variable <" << t_env_var << ">" );
                                t_new_env_group.add( gr_it.name(), t_env_value );
                            }
                        }

                    }
                    t_default_data.add( it.name(), t_new_default_group );
                    if( ! t_new_env_group.empty() ) t_env_data.add( it.name(), t_new_env_group );
                }
            } // end loop over groups

            //LWARN( mtlog, "Default data:\n" << t_default_data );
            //LWARN( mtlog, "Env data:\n" << t_env_data );
            
            // order of precedence: default --> file --> env
            f_data.clear();
            f_data.merge( t_default_data );
            if( t_file_data_ptr ) f_data.merge( t_file_data_ptr->as_node() );
            f_data.merge( t_env_data );
            //LWARN( mtlog, "Final data:\n" << f_data );
        }
        catch( const std::exception& e )
        {
            std::cerr << e.what() << '\n';
        }
        
        return;
    }

    param_ptr_t authentication::load_from_file( const std::string& a_auth_file, const param_node& a_read_opts )
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

        param_ptr_t t_file_data_ptr = std::make_unique< param_node >();

        t_file_data_ptr->as_node().merge( t_read_file->as_node() );
        //LWARN( mtlog, "File loaded:\n" << t_file_data_ptr->as_node() );
        return t_file_data_ptr;
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
