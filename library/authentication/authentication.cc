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
#include <fstream>

namespace scarab
{
    LOGGER( mtlog, "authentication" );

    authentication::authentication() :
            f_spec(),
            f_data()
    {
        f_spec.add( "groups", param_node() );
    }

    void authentication::add_group( const std::string& a_group )
    {
        if( f_spec["groups"].as_node().add( a_group, param_node() ) )
        {
            LDEBUG( mtlog, "Added group <" << a_group << ">" );
        }
        else
        {
            LDEBUG( mtlog, "Group <" << a_group << "> already exists" );
        }
        return;
    }

    void authentication::add_item( const std::string& a_group, const std::string& a_name, const std::string& a_default, const std::string& an_env )
    {
        add_group( a_group );
        if( f_spec["groups"][a_group].as_node().has(a_name) )
        {
            LDEBUG( mtlog, "Item <" << a_group << ":" << a_name << "> already exists; it will be overwritten" );
        }
        else
        {
            f_spec["groups"][a_group].as_node().add( a_name, param_node() );
        }

        param_node t_item( "default"_a=a_default );
        if( ! an_env.empty() ) t_item.add( "env", an_env );
        //LWARN( mtlog, "new item:\n" << t_item );
        f_spec["groups"][a_group][ a_name ].as_node().merge(t_item);
        LDEBUG( mtlog, "Added item <" << a_group << ":" << a_name << ">" );
        //LWARN( mtlog, f_spec["groups"][a_group] );
        
        return;
    }

    void authentication::add_groups( const param_node& a_groups_node )
    {
        //LWARN( mtlog, "adding groups: " << a_groups_node );
        for( auto it = a_groups_node.begin(); it != a_groups_node.end(); ++it )
        {
            LDEBUG( mtlog, "Found group <" << it.name() << ">" );
            // throws if not a node
            const param_node& t_a_group = it->as_node();
            if( t_a_group.empty() )
            {
                add_group( it.name() );
                continue;
            }

            param_node t_new_default_group, t_new_env_group;
            for( auto gr_it = t_a_group.begin(); gr_it != t_a_group.end(); ++gr_it )
            {
                LDEBUG( mtlog, "Processing item <" << gr_it.name() << ">" );

                if( gr_it->is_node() )
                {
                    const param_node& t_an_item = gr_it->as_node();

                    // default value; required; throws if not present or not string
                    add_item( it.name(), gr_it.name(), t_an_item["default"]().as_string(), t_an_item.get_value("env", "") );
                    if( t_an_item.has("value") ) set_value( it.name(), gr_it.name(), t_an_item["value"]().as_string() );
                    if( t_an_item.has("file") ) set_file( it.name(), gr_it.name(), t_an_item["file"]().as_string() );
                }
                else if( gr_it->is_value() )
                {
                    const param_value& t_an_item = gr_it->as_value();

                    // default value; required; throws if not present or not string
                    add_item( it.name(), gr_it.name(), t_an_item.as_string() );
                    set_value( it.name(), gr_it.name(), t_an_item.as_string() );
                }
                else
                {
                    throw scarab::error( __FILE__, __LINE__ ) << "Invalid group data type: " << gr_it->type();
                }
            }
        }
        return;
    }

    void authentication::set_file( const std::string& a_group, const std::string& a_name, const std::string& a_filename )
    {
        f_spec["groups"][a_group][a_name].as_node().replace("file", a_filename);
        return;
    }

    void authentication::set_value( const std::string& a_group, const std::string& a_name, const std::string& a_value )
    {
        f_spec["groups"][a_group][a_name].as_node().replace("value", a_value);
        return;
    }

    void authentication::set_auth_file( const std::string& a_filename, const param_node& a_read_opts )
    {
        f_spec.replace( "file", a_filename );
        f_spec.replace( "file-read-opts", a_read_opts );
        LDEBUG( mtlog, "Added auth filename: " << f_spec["file"] );
        return;
    }

    void authentication::process_spec()
    {
        // This function takes the authentication specification and turns it into authentication data.
        // It handles the various sources of information in the following order of precedence (higher numbers override lower numbers):
        // 1. Defaults
        // 2. Authentication file
        // 3. Environment variables
        // 4. Override (file)
        // 5. Override (value)

        try
        {
            LDEBUG( mtlog, "Processing authentication specification" );
            //LWARN( mtlog, f_spec );

            param_ptr_t t_file_data_ptr;
            if( f_spec.has( "file" ) )
            {
                LDEBUG( mtlog, "Loading authentication information from a file" );
                t_file_data_ptr = load_from_auth_file( f_spec["file"]().as_string(), f_spec["file-read-opts"].as_node() );
            }

            param_node t_default_data, t_env_data, t_override_data;

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

                    param_node t_new_default_group, t_new_env_group, t_new_override_group;
                    //LWARN( mtlog, "getting items from group:\n" << t_a_group );
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

                        if( t_an_item.has("file") )
                        {
                            LDEBUG( mtlog, "Reading from file <" << t_an_item["file"]() << ">" );   
                            t_new_override_group.add( gr_it.name(), read_from_file( t_an_item["file"]().as_string() ) );
                        }

                        if( t_an_item.has("value") )
                        {
                            LDEBUG( mtlog, "Found a value" );
                            t_new_override_group.replace( gr_it.name(), t_an_item["value"]().as_string() );
                        }
                    } // end loop over items in the group

                    t_default_data.add( it.name(), t_new_default_group );
                    if( ! t_new_env_group.empty() ) t_env_data.add( it.name(), t_new_env_group );
                    if( ! t_new_override_group.empty() ) t_override_data.add( it.name(), t_new_override_group );
                } // end loop over groups
            } // end if-have-groups

            //LWARN( mtlog, "Default data:\n" << t_default_data );
            //if( t_file_data_ptr ) LWARN( mtlog, "Auth file:\n" << t_file_data_ptr->as_node() );
            //LWARN( mtlog, "Env data:\n" << t_env_data );
            //LWARN( mtlog, "Override data:\n" << t_override_data );
            
            // order of precedence: default --> auth file --> env --> overrides
            f_data.clear();
            f_data.merge( t_default_data );
            if( t_file_data_ptr ) f_data.merge( t_file_data_ptr->as_node() );
            f_data.merge( t_env_data );
            f_data.merge( t_override_data );
            LDEBUG( mtlog, "Authentication data processed" );
            //LWARN( mtlog, "Final data:\n" << f_data );
        }
        catch( const std::exception& e )
        {
            std::cerr << e.what() << '\n';
        }
        
        return;
    }

    param_ptr_t authentication::load_from_auth_file( const std::string& a_auth_file, const param_node& a_read_opts ) const
    {
        path t_auth_file_path = check_path( a_auth_file );
        LINFO( mtlog, "Loading authentication file <" << t_auth_file_path << ">" );

        // if so, load it
        LDEBUG( mtlog, "Opening and reading file <" << t_auth_file_path << ">" );
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

    std::string authentication::read_from_file( const std::string& a_filename ) const
    {
        // expand path with standard assumptions
        path t_path = expand_path( a_filename );
        LDEBUG( mtlog, "Found a file: " << t_path );

        std::ifstream t_op_ifstream( t_path.native(), std::ifstream::binary );
        if( ! t_op_ifstream.is_open() )
        {
            throw error( __FILE__, __LINE__ ) << "Couldn't open file <" << t_path << ">";
        }

        constexpr std::streamsize t_max_buffers = 50;
        std::array<char, 1024> t_buffer;
        std::string t_value;
        while( t_op_ifstream.good() )
        {
            t_op_ifstream.read( t_buffer.data(), t_buffer.size() );
            std::streamsize t_size_read = t_op_ifstream.gcount();
            t_value += std::string( t_buffer.data(), t_size_read );
        }
        if( t_op_ifstream.bad() )
        {
            throw error( __FILE__, __LINE__ ) << "Error while reading file <" << t_path << ">";
        }

        return t_value;
    }

    path authentication::check_path( const std::string& a_filename ) const
    {
        // expand path with standard assumptions
        path t_file_path = expand_path( a_filename );
        LDEBUG( mtlog, "Expanded path: " << t_file_path );

        // is the file there?
        if( ! t_file_path.empty() )
        {
            LDEBUG( mtlog, "Looking for authentication file: <" << t_file_path << ">" );
            try
            {
                if( ! (exists( t_file_path ) && is_regular_file( t_file_path )) )
                {
                    throw error( __FILE__, __LINE__ ) << "File either doesn't exist (" << exists( t_file_path ) << ") or isn't a regular file (" << is_regular_file( t_file_path ) << ")";
                }
            }
            catch( boost::filesystem::filesystem_error& e )
            {
                throw error( __FILE__, __LINE__ ) << "Unable to determine if the authentication file is a regular file: " << e.what();
            }
        }
        return t_file_path;
    }


    bool authentication::has( const std::string& a_group ) const
    {
        return f_data.has( a_group );
    }

    bool authentication::has( const std::string& a_group, const std::string& an_item ) const
    {
        try
        {
            // throws std::out_of_range if a_group is not present
            // throws scarab::error if f_data[a_group] is not a node
            return f_data[a_group].as_node().has( an_item );
        }
        catch( const std::exception& )
        {
            return false;
        }
    }

    std::string authentication::get( const std::string& a_group, const std::string& an_item ) const
    {
        return f_data[a_group][an_item]().as_string();
    }

    std::string authentication::get( const std::string& a_group, const std::string& an_item, const std::string& a_default ) const
    {
        try
        {
            return f_data[a_group][an_item]().as_string();
        }
        catch( const std::exception& )
        {
            return a_default;
        }
        
    }

} /* namespace scarab */
