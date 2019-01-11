/*
 * application.cc
 *
 *  Created on: Jul 20, 2018
 *      Author: N.S. Oblath
 */

#include "application.hh"

#include "logger.hh"
#include "nonoption_parser.hh"
#include "param_codec.hh"
#include "version_wrapper.hh"

using std::string;

namespace scarab
{
    LOGGER( applog, "application" );

    main_app::main_app() :
            app(),
            f_master_config(),
            f_default_config(),
            f_config_filename(),
            f_global_verbosity( 1 ),
            f_nonoption_kw_args(),
            f_nonoption_ord_args(),
            f_app_options()
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

    void main_app::set_version( version_semantic* a_ver )
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
        f_master_config.merge( f_default_config );
        return;
    }

    void main_app::do_config_stage_2()
    {
        // second configuration stage: config file
        if( ! f_config_filename.empty() )
        {
            path t_config_filepath = scarab::expand_path( f_config_filename );
            LDEBUG( applog, "Loading config file <" << t_config_filepath << "> from filename <" << f_config_filename << ">" );
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
        return;
    }

    void main_app::do_config_stage_3()
    {
        // third configuration stage: keyword args
        //LDEBUG( applog, "adding command-line parser:\n" << t_parser << *f_master_config );
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
