/*
 * application.cc
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#include "application.hh"

#include "nonoption_parser.hh"

#include "param_codec.hh"

#include "logger.hh"

using std::string;

namespace scarab
{
    LOGGER( applog, "application" );

    application::application() :
            command(),
            f_master_config(),
            f_default_config(),
            f_config_filename(),
            f_global_verbosity( 1 ),
            f_nonoption_kw_args(),
            f_nonoption_ord_args()
    {
        add_option( "-c,--config", f_config_filename, "Config file filename" )->check(CLI::ExistingFile);
        add_option( "--verbosity", f_global_verbosity, "Global logger verosity" );

        auto t_version_callback = [](int count){
            LPROG( applog, "PRINT VERSION INFORMATION HERE" );
            // TODO: implement version callback
        };
        add_flag_function( "-v,--version", t_version_callback, "Print the version message and exit" );
    }

    application::~application()
    {
    }

    void application::pre_callback()
    {
        // first configuration: defaults
        f_master_config.merge( f_default_config );

        //std::cout << "first configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );

        string t_name_exe( "executable" ); // the name used to specify the executable in parser
        string t_name_json( "json" );

        applog.SetGlobalLevel( (logger::ELevel)f_global_verbosity );

        // TODO: decide on whether we want to keep this

        // name of executable
        //f_exe_name = t_parser.get_value( t_name_exe, f_exe_name );
#ifdef __APPLE__
        char t_exe_buf[ 2048 ];
        uint32_t t_bufsize = sizeof( t_exe_buf );
        if( _NSGetExecutablePath( t_exe_buf, &t_bufsize ) != 0 )
#elif __linux
        const size_t t_bufsize = 2048;
        char t_exe_buf[ t_bufsize ];
        if( readlink( "/proc/self/exe", t_exe_buf, t_bufsize ) < 0 )
#endif
        {
            LWARN( applog, "Could not retrieve executable file name" );
#ifdef __APPLE__
            LWARN( applog, "Executable name buffer is too small; needs size %u\n" << t_bufsize );
#endif
        }
        f_exe_name = string( t_exe_buf );

        // second configuration: config file
        path t_config_filepath = scarab::expand_path( f_config_filename );
        if( ! t_config_filepath.empty() )
        {
            param_translator t_translator;
            std::unique_ptr< param > t_config_from_file( t_translator.read_file( t_config_filepath.native() ));
            if( t_config_from_file == NULL )
            {
                throw error() << "[application] error parsing config file";
            }
            if( ! t_config_from_file->is_node() )
            {
                throw error() << "[application] configuration file must consist of an object/node";
            }
            f_master_config.merge( t_config_from_file->as_node() );
        }

        //std::cout << "second configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );


        //std::cout << "third configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );

        nonoption_parser t_no_parser( remaining() );
        f_nonoption_kw_args = t_no_parser.kw_args();
        f_nonoption_ord_args = t_no_parser.ord_args();

        //LDEBUG( applog, "adding command-line parser:\n" << t_parser << *f_master_config );
        f_master_config.merge( f_nonoption_kw_args );

        //std::cout << "fourth configuration complete" << std::endl;
        LPROG( applog, "Final configuration:\n" << f_master_config );
    }

} /* namespace scarab */
