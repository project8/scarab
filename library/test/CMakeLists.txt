# CMakeLists.txt for Scarab/library/test
# Author: N. Oblath
# Created: Jan 4, 2016

if( Scarab_ENABLE_TESTING )

    set( Scarab_test_HEADERS
        test_member_variables.hh
    )

    pbuilder_install_headers( ${Scarab_test_HEADERS} )

    set( lib_dependencies 
        Scarab
    )
    
    set( test_programs
        test_digital
        test_member_variables
        test_logger
        test_terminate
        test_time
    )

    set( test_configs
    )
    
    if( Scarab_BUILD_PARAM )
        set( test_programs
            ${test_programs}
            test_param_array
            test_param_by_pointer
            test_param_node
            test_param_value
        )
    endif( Scarab_BUILD_PARAM )

    if( Scarab_BUILD_CODEC_JSON )
        set( test_programs
            ${test_programs}
            test_json
        )
    endif( Scarab_BUILD_CODEC_JSON )
    
    if( Scarab_BUILD_CODEC_YAML )
        set( test_programs
            ${test_programs}
            test_yaml
        )
        set( test_configs
            ${test_configs}
            test_yaml.yaml
        )
    endif( Scarab_BUILD_CODEC_YAML )

    if( Scarab_BUILD_CLI )
        set( test_programs
            ${test_programs}
            test_app_with_callback
            test_app_with_options
            test_app_with_subcommands
            test_basic_application
            test_nonoption_parser
        )
    endif( Scarab_BUILD_CLI )

    pbuilder_executables( test_programs lib_dependencies )

    pbuilder_install_config( ${test_configs} )

endif( Scarab_ENABLE_TESTING )
