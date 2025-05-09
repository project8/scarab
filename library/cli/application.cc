/*
 * application.cc
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "application.hh"

#include "nonoption_parser.hh"
#include "param_codec.hh"
#include "param_env_modifier.hh"
#include "version_wrapper.hh"

#include <thread>
//#include <iostream>  // used for printing debugging statements sometimes

using std::string;

LOGGER(applog, "application");

namespace scarab
{

    config_decorator::config_decorator( main_app* a_main, app* a_this_app ) :
            f_main( a_main ),
            f_this( a_this_app ),
            f_subcommand_decorators()
    {
    }

    config_decorator* config_decorator::add_config_subcommand( std::string a_subcommand_name, std::string a_description )
    {
        app* t_subcommand = f_this->add_subcommand( a_subcommand_name, a_description );
        //config_decorator* t_decorator = new config_decorator( f_main, t_subcommand );
        f_subcommand_decorators.emplace_back( new config_decorator( f_main, t_subcommand ) );
        return f_subcommand_decorators.back().get();
    }


    // utility function to fill the verbosity map
    main_app::verbosity_map_t fill_verbosities()
    {
        main_app::verbosity_map_t t_map;
        // have to hard-code the filling of the verbosity map because you can't iterate over an enum
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eTrace) ] = ELevel::eTrace;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eDebug) ] = ELevel::eDebug;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eInfo) ] = ELevel::eInfo;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eProg) ] = ELevel::eProg;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eWarn) ] = ELevel::eWarn;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eError) ] = ELevel::eError;
        t_map[ static_cast< main_app::verbosity_t >(ELevel::eFatal) ] = ELevel::eFatal;
        return t_map;
    }

    main_app::verbosity_map_t main_app::s_verbosities( fill_verbosities() );

    main_app::main_app( bool a_use_config ) :
            config_decorator( this, this ),
            app(),
            f_primary_config(),
            f_default_config(),
            f_config_filename(),
            f_config_encoding(),
            f_nonoption_kw_args(),
            f_nonoption_ord_args(),
            f_app_options(),
            f_app_option_holders(),
            f_use_config( a_use_config ),
            f_auth_groups_key( "auth-groups" ),
            f_auth_file_key( "auth-file" ),
            f_auth()
    {
        set_global_verbosity( ELevel::eProg );

        allow_extras(); // allow unrecognized options, which are parsed into the nonoption args

        if( f_use_config )
        {
            add_option( "-c,--config", f_config_filename, "Config file filename" )->check(CLI::ExistingFile);
            add_option( "--config-encoding", f_config_encoding, "Config file encoding" );
        }

        auto t_verbose_callback = [this]( unsigned a_count )
            {
                this->increase_global_verbosity( a_count );
            };
        add_flag_function( "-v,--verbose", t_verbose_callback, "Increase verbosity" );

        auto t_quiet_callback = [this]( unsigned a_count )
            {
                this->decrease_global_verbosity( a_count );
            };
        add_flag_function( "-q,--quiet", t_quiet_callback, "Decrease verbosity" );

        auto t_version_callback = [](int)
            {
                LPROG( applog, '\n' << version_wrapper::get_instance()->version_info_string() );
                throw CLI::Success();
            };
        add_flag_function( "-V,--version", t_version_callback, "Print the version message and exit" );

        // This preparse callback adds a small delay to every program execution that allows the logging thread to startup.
        // The pre-parse stage is used so that it executes even for --help and --version calls.
        // Without it, for short-running executions (e.g. calling --help, --version, or with invalid parameters that quickly exits) may segfault before printing log messages.
        // If you want to use the preparse_callback for something, please include this delay (or ensure there's an equivalent delay)
        auto t_preparse_callback = [](int)
            {
                std::this_thread::sleep_for( std::chrono::milliseconds(500) );
            };
        preparse_callback( t_preparse_callback );

        // I attempted to use this to add the delay to avoid the segfault problem (see above, regarding the preparse callback).
        // But for some reason it was not being run at all
        // If we could use this function, we could implement something that only delays if the program was short-running. 
        // Pre-parse, we could record the time.  Then we sleep_until we reach the preparse time + 500 ms, for instance.
        //auto t_final_callback = []()
        //    {
        //        std::cerr << "final_callback delay" << std::endl;
        //        std::this_thread::sleep_for( std::chrono::milliseconds(5000) );
        //    };
        //final_callback( t_final_callback );
    }

    void main_app::set_version( scarab::version_semantic_ptr_t a_ver )
    {
        version_wrapper::get_instance()->set_imp( a_ver );
    }

    void main_app::pre_callback()
    {
        if( f_use_config )
        {
            do_config_stage_1(); // defaults

            do_config_stage_2(); // config file

            try
            {
                nonoption_parser t_no_parser( remaining() );
                f_nonoption_kw_args = t_no_parser.kw_args();
                f_nonoption_ord_args = t_no_parser.ord_args();
            }
            catch( error& e )
            {
                LERROR( applog, "Unable to parse remaining arguments: " << e.what() );
                throw CLI::ParseError( std::string("Unable to parse remaining arguments due to parse error or unknown option: ") + e.what(), CLI::ExitCodes::ArgumentMismatch );
            }

            do_config_stage_3(); // keyword arguments

            do_config_stage_4(); // application options

            do_config_stage_5(); // additional modifiers (e.g. environment variables)

            do_authentication();
            // remove auth spec from the configuation
            f_primary_config.erase( f_auth_groups_key );

            LPROG( applog, "Final configuration:\n" << f_primary_config );
            LPROG( applog, "Ordered args:\n" << f_nonoption_ord_args );
        }
        return;
    }

    void main_app::do_config_stage_1()
    {
        // first configuration stage: defaults

        if( ! f_use_config ) return;

        LDEBUG( applog, "first configuration stage" );
        //LWARN( applog, "merging default config: " << f_default_config );
        f_primary_config.merge( f_default_config );
        //LWARN( applog, "Primary config, after stage 1:\n" << f_primary_config )
        return;
    }

    void main_app::do_config_stage_2()
    {
        // second configuration stage: config file

        if( ! f_use_config ) return;

        LDEBUG( applog, "second configuration stage" );
        if( ! f_config_filename.empty() )
        {
            path t_config_filepath = scarab::expand_path( f_config_filename );
            LDEBUG( applog, "Loading config file <" << t_config_filepath << "> from filename <" << f_config_filename << ">" );
            param_translator t_translator;
            std::unique_ptr< param > t_config_from_file;
            if( f_config_encoding.empty() )
            {
                t_config_from_file = t_translator.read_file( t_config_filepath.string() );
            }
            else
            {
                param_node t_file_opts;
                t_file_opts.add( "encoding", f_config_encoding );
                t_config_from_file = t_translator.read_file( t_config_filepath.string(), t_file_opts );
            }
            if( t_config_from_file == NULL )
            {
                throw error() << "[application] error parsing config file";
            }
            if( ! t_config_from_file->is_node() )
            {
                throw error() << "[application] configuration file must consist of an object/node";
            }
            f_primary_config.merge( t_config_from_file->as_node() );
        }
        //LWARN( applog, "Primary config, after stage 2:\n" << f_primary_config )
        return;
    }

    void main_app::do_config_stage_3()
    {
        // third configuration stage: keyword args

        if( ! f_use_config ) return;

        LDEBUG( applog, "third configuration stage" );
        f_primary_config.merge( f_nonoption_kw_args );
        //LWARN( applog, "Primary config, after stage 3:\n" << f_primary_config )
        return;
    }

    void main_app::do_config_stage_4()
    {
        // fourth configuration stage: application options

        if( ! f_use_config ) return;

        LDEBUG( applog, "fourth configuration stage" );
        std::for_each( f_app_option_holders.begin(), f_app_option_holders.end(),
                       [this]( std::shared_ptr< app_option_holder > a_ptr ){ a_ptr->add_to_app_options(f_app_options); } );
        //LWARN( applog, "App options, before stage-4 merge:\n" << f_app_options );
        f_primary_config.merge( f_app_options );
        //LWARN( applog, "Primary config, after stage 4:\n" << f_primary_config )
        return;
    }

    void main_app::do_config_stage_5()
    {
        // fifth configuration stage: additional modification of param_node:
        // * environment variable substitution for ENV{[var]} tokens

        if( ! f_use_config ) return;

        LDEBUG( applog, "fifth configuration stage" );
        param_env_modifier t_modifier;
        t_modifier( f_primary_config );
        //LWARN( applog, "Primary config, after stage 5:\n" << f_primary_config )
        return;
    }

    void main_app::do_authentication()
    {
        LDEBUG( applog, "Handling authentication" );
        // Load the authentication specification into the authentication object
        // Will throw scarab::error if the spec is not properly formatted
        if( f_primary_config.has( f_auth_groups_key ) )
        {
            f_auth.add_groups( f_primary_config[f_auth_groups_key].as_node() );
        }
        if( f_primary_config.has( f_auth_file_key ) )
        {
            f_auth.set_auth_file( f_primary_config[f_auth_file_key]().as_string() );
        }

        f_auth.process_spec();
        return;
    }

    void main_app::set_default_auth_spec_groups( const scarab::param_node& a_groups )
    {
        f_default_config.replace( f_auth_groups_key, a_groups );
        return;
    }

    void main_app::add_default_auth_spec_group( const std::string& a_group_name, const scarab::param_node& a_spec_group )
    {
        //LWARN( applog, "Adding default auth spec group: " << a_group_name << " -- " << a_spec_group );
        if( ! f_default_config.has(f_auth_groups_key) ) f_default_config.add( f_auth_groups_key, scarab::param_node() );
        f_default_config[f_auth_groups_key].as_node().replace( a_group_name, a_spec_group );
        //LWARN( applog, "Default config: " << f_default_config );
        return;
    }

    void main_app::set_default_auth_file( const std::string& a_filename )
    {
        f_default_config.replace( f_auth_file_key, a_filename );
        return;
    }

    main_app::verbosity_t main_app::get_global_verbosity() const
    {
        return f_global_verbosity->first;
    }
 
    void main_app::set_global_verbosity( main_app::verbosity_t a_verbosity )
    {
        auto t_it = s_verbosities.find( a_verbosity );

        // if we don't match a value, then find the next higher value
        if( t_it == s_verbosities.end() )
        {
            t_it = s_verbosities.upper_bound( a_verbosity );
            // make sure we don't go off the end
            if( t_it == s_verbosities.end() ) t_it = std::prev(s_verbosities.end());
        }

        f_global_verbosity = t_it;

        // set the global verbosity in the logger
        logger::set_global_threshold( f_global_verbosity->second );
        return;
    }

    void main_app::set_global_verbosity( ELevel a_verbosity )
    {
        set_global_verbosity( static_cast< verbosity_t >(a_verbosity) );
        return;
    }

    void main_app::increase_global_verbosity( unsigned an_offset )
    {
        // increased verbosity is lower in verbosity number
        if( an_offset == 0 ) return;
        // Move down, but don't go past the beginning
        for( unsigned i_count = 0; i_count < an_offset; ++i_count )
        {
            if( f_global_verbosity == s_verbosities.begin() ) break;
            --f_global_verbosity;
        }
        // set the global verbosity in the logger
        logger::set_global_threshold( f_global_verbosity->second );
        return;
    }

    void main_app::decrease_global_verbosity( unsigned an_offset )
    {
        // decreased verbosity is higher in verbosity number
        if( an_offset == 0 ) return;
        // Move up, but not past the end
        for( unsigned i_count = 0; i_count < an_offset; ++i_count )
        {
            if( std::next(f_global_verbosity) == s_verbosities.end() ) break;
            ++f_global_verbosity;
        }
        // set the global verbosity in the logger
        logger::set_global_threshold( f_global_verbosity->second );
        return;
    }

} /* namespace scarab */
