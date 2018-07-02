# CMakeLists.txt for Scarab/library/test
# Author: N. Oblath
# Created: Jan 4, 2016

if( Scarab_ENABLE_TESTING )

    set( Scarab_test_HEADERS
        test__member_variables.hh
    )

    pbuilder_install_headers( ${Scarab_test_HEADERS} )

    set( lib_dependencies 
        Scarab
    )
    
    set( test_programs
        test__member_variables
        test_logger
        test_terminate
        test_time
    )
    
    if( Scarab_BUILD_PARAM )
        set( test_programs
            ${test_programs}
            test_configurator
        )
    endif( Scarab_BUILD_PARAM )

    if( Scarab_BUILD_CODEC_YAML )
        set( test_programs
            ${test_programs}
            test_yaml
        )
    endif( Scarab_BUILD_CODEC_YAML )
    
    pbuilder_executables( test_programs lib_dependencies )

endif( Scarab_ENABLE_TESTING )
