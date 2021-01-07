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
#include "version_wrapper.hh"

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

    config_decorator::~config_decorator()
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
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eTrace) ] = logger::ELevel::eTrace;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eDebug) ] = logger::ELevel::eDebug;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eInfo) ] = logger::ELevel::eInfo;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eProg) ] = logger::ELevel::eProg;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eWarn) ] = logger::ELevel::eWarn;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eError) ] = logger::ELevel::eError;
        t_map[ static_cast< main_app::verbosity_t >(logger::ELevel::eFatal) ] = logger::ELevel::eFatal;
        return t_map;
    }

    main_app::verbosity_map_t main_app::s_verbosities( fill_verbosities() );

    main_app::main_app( bool a_use_config ) :
            config_decorator( this, this ),
            app(),
            f_primary_config(),
            f_default_config(),
            f_config_filename(),
            f_nonoption_kw_args(),
            f_nonoption_ord_args(),
            f_app_options(),
            f_app_option_holders(),
            f_use_config( a_use_config )
    {
        set_global_verbosity( logger::ELevel::eProg );

        allow_extras(); // allow unrecognized options, which are parsed into the nonoption args

        if( f_use_config )
        {
            add_option( "-c,--config", f_config_filename, "Config file filename" )->check(CLI::ExistingFile);
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
    }

    main_app::~main_app()
    {
    }

    void main_app::set_version( scarab::version_semantic_ptr_t a_ver )
    {
        version_wrapper::get_instance()->set_imp( a_ver );
    }

    void main_app::pre_callback()
    {
        if( f_use_config )
        {
            do_config_stage_1();

            do_config_stage_2();

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

            do_config_stage_3();

            do_config_stage_4();

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
        f_primary_config.merge( f_default_config );
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
            std::unique_ptr< param > t_config_from_file( t_translator.read_file( t_config_filepath.string() ));
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
        return;
    }

    void main_app::do_config_stage_3()
    {
        // third configuration stage: keyword args

        if( ! f_use_config ) return;

        LDEBUG( applog, "third configuration stage" );
        f_primary_config.merge( f_nonoption_kw_args );
        return;
    }

    void main_app::do_config_stage_4()
    {
        // fourth configuration stage: application options

        if( ! f_use_config ) return;

        std::for_each( f_app_option_holders.begin(), f_app_option_holders.end(),
                       [this]( std::shared_ptr< app_option_holder > a_ptr ){ a_ptr->add_to_app_options(f_app_options); } );
        f_primary_config.merge( f_app_options );
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
        logger::SetGlobalLevel( f_global_verbosity->second );
        return;
    }

    void main_app::set_global_verbosity( logger::ELevel a_verbosity )
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
        applog.SetGlobalLevel( f_global_verbosity->second );
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
        applog.SetGlobalLevel( f_global_verbosity->second );
        return;
    }


} /* namespace scarab */
