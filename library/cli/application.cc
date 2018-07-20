/*
 * application.cc
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#include "application.hh"

#include "logger.hh"

namespace scarab
{
    LOGGER( applog, "application" );

    application::application() :
            CLI::App(),
            f_master_config(),
            f_default_config(),
            f_config_filename()
    {
        // TODO: add option -c,--config to set the config_filename
        // TODO: add logger option (including member variable)
    }

    application::~application()
    {
    }

    void application::pre_callback()
    {
        parser t_parser( an_argc, an_argv );
        //std::cout << "options parsed" << std::endl;
        //cout << t_parser );

        // first configuration: defaults
        f_master_config.merge( f_default_config );

        //std::cout << "first configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );

        string t_name_logger( "logger" );
        string t_name_exe( "executable" ); // the name used to specify the executable in parser
        string t_name_config( "config" );
        string t_name_json( "json" );

        // TODO: convert this to use the local value of the logger verbosity

        if( t_parser.has( t_name_logger ) )
        {
            try
            {
                slog.SetGlobalLevel( (logger::ELevel)t_parser[t_name_logger]["verbosity"]().get< unsigned >() );
            }
            catch( std::exception& e ) {}
        }

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
            LWARN( slog, "Could not retrieve executable file name" );
#ifdef __APPLE__
            LWARN( slog, "Executable name buffer is too small; needs size %u\n" << t_bufsize );
#endif
        }
        f_exe_name = string( t_exe_buf );

        // TODO: convert this to use the local variable

        // second configuration: config file
        if( t_parser.has( t_name_config ) )
        {
            path t_config_filename = scarab::expand_path( t_parser.get_value( t_name_config ) );
            if( ! t_config_filename.empty() )
            {
                param_translator t_translator;
                std::unique_ptr< param > t_config_from_file( t_translator.read_file( t_config_filename.native() ));
                if( t_config_from_file == NULL )
                {
                    throw error() << "[configurator] error parsing config file";
                }
                if( ! t_config_from_file->is_node() )
                {
                    throw error() << "[configurator] configuration file must consist of an object/node";
                }
                f_master_config.merge( t_config_from_file->as_node() );
            }
        }

        //std::cout << "second configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );


        //std::cout << "third configuration complete" << std::endl;
        //cout << f_master_config );
        //cout << t_parser );

        // fourth configuration: command line arguments
        t_parser.erase( t_name_exe );
        t_parser.erase( t_name_config );
        t_parser.erase( t_name_json );

        // TODO: convert this to use the App::remaining feature

        //std::cout << "removed config and json from parsed options" << std::endl;
        //cout << t_parser );
        //LDEBUG( slog, "adding command-line parser:\n" << t_parser << *f_master_config );
        f_master_config.merge( t_parser );

        // check for help and version flags
        if( f_master_config.has( "help" ) )
        {
            f_help_flag = true;
            f_master_config.erase( "help" );
        }
        if( f_master_config.has( "version" ) )
        {
            f_version_flag = true;
            f_master_config.erase( "version" );
        }

        //std::cout << "fourth configuration complete" << std::endl;
        LPROG( slog, "Final configuration:\n" << f_master_config );
    }

} /* namespace scarab */
