# CMakeLists.txt for Scarab/library/test
# Author: N. Oblath
# Created: Jan 4, 2016

if( Scarab_ENABLE_TESTING )

    set( Scarab_test_HEADERS
        test_member_variables.hh
    )

    pbuilder_install_headers( ${Scarab_test_HEADERS} )

    set( PACKAGE_LIBS 
        Scarab
    )
    set( PUBLIC_EXT_LIBS )
    set( PRIVATE_EXT_LIBS )

    set( test_sources
        test_member_variables.cc
        test_logger.cc
        test_terminate.cc
        test_time.cc
        test_verbosity.cc
    )

    set( test_configs
    )
    
    if( Scarab_BUILD_PARAM )
        set( test_sources
            ${test_sources}
            test_param_by_pointer.cc
            test_param_node.cc
        )
    endif( Scarab_BUILD_PARAM )

    if( Scarab_BUILD_CODEC_JSON )
        set( test_sources
            ${test_sources}
            test_json.cc
        )
    endif( Scarab_BUILD_CODEC_JSON )
    
    if( Scarab_BUILD_CODEC_YAML )
        set( test_sources
            ${test_sources}
            test_yaml.cc
        )
        set( test_configs
            ${test_configs}
            test_yaml.yaml
        )
    endif( Scarab_BUILD_CODEC_YAML )

    if( Scarab_BUILD_CLI )
        set( test_sources
            ${test_sources}
            test_app_with_callback.cc
            test_app_with_options.cc
            test_app_with_subcommands.cc
            test_basic_application.cc
        )
    endif( Scarab_BUILD_CLI )

    pbuilder_executables( test_sources PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

    pbuilder_install_config( ${test_configs} )

endif( Scarab_ENABLE_TESTING )
