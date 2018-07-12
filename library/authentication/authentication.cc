/*
 * mt_authentication.cc
 *
 *  Created on: Jun 15, 2015
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "authentication.hh"

#include "logger.hh"
#include "param_codec.hh"
#include "path.hh"

using std::string;

namespace scarab
{
    LOGGER( mtlog, "authentication" );

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

} /* namespace scarab */
