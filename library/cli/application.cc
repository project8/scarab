/*
 * application.cc
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "application.hh"

#include "logger.hh"
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


    main_app::main_app() :
            config_decorator( this, this ),
            app(),
            f_master_config(),
            f_default_config(),
            f_config_filename(),
            f_global_verbosity( 1 ),
            f_nonoption_kw_args(),
            f_nonoption_ord_args(),
            f_app_options(),
            f_app_option_holders()
    {
        allow_extras(); // allow unrecognized options, which are parsed into the nonoption args

        add_option( "-c,--config", f_config_filename, "Config file filename" )->check(CLI::ExistingFile);
        add_option( "--verbosity", f_global_verbosity, "Global logger verosity" );

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
        do_config_stage_1();

        applog.SetGlobalLevel( (logger::ELevel)f_global_verbosity );

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

        LPROG( applog, "Final configuration:\n" << f_master_config );
        LPROG( applog, "Ordered args:\n" << f_nonoption_ord_args );
    }

    void main_app::do_config_stage_1()
    {
        // first configuration stage: defaults
        LDEBUG( applog, "first configuration stage" );
        f_master_config.merge( f_default_config );
        return;
    }

    void main_app::do_config_stage_2()
    {
        // second configuration stage: config file
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
            f_master_config.merge( t_config_from_file->as_node() );
        }
        return;
    }

    void main_app::do_config_stage_3()
    {
        // third configuration stage: keyword args
        LDEBUG( applog, "third configuration stage" );
        f_master_config.merge( f_nonoption_kw_args );
        return;
    }

    void main_app::do_config_stage_4()
    {
        // fourth configuration stage: application options
        std::for_each( f_app_option_holders.begin(), f_app_option_holders.end(),
                       [this]( std::shared_ptr< app_option_holder > a_ptr ){ a_ptr->add_to_app_options(f_app_options); } );
        f_master_config.merge( f_app_options );
        return;
    }


} /* namespace scarab */
