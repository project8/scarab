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
        test_configurator
        test_logger
        test_yaml
    )
    
    
    pbuilder_executables( test_programs lib_dependencies )

endif( Scarab_ENABLE_TESTING )
