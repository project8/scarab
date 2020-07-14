# CMakeLists.txt for Scarab/library/test
# Author: N. Oblath
# Created: Jan 4, 2016

if( Scarab_ENABLE_TESTING )

    set( PACKAGE_LIBS 
        Scarab
    )
    set( PUBLIC_EXT_LIBS )
    set( PRIVATE_EXT_LIBS )

    set( test_sources
        test_terminate.cc
    )

    

endif( Scarab_ENABLE_TESTING )
