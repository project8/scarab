cmake_minimum_required (VERSION 3.1)

cmake_policy( SET CMP0048 NEW ) # version in project()
project( PBTest VERSION 1.0.0 )

### 
if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scarab )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/copy_scarab.sh.in ${CMAKE_CURRENT_BINARY_DIR}/copy_scarab.sh @ONLY )
    execute_process( COMMAND ${CMAKE_CURRENT_BINARY_DIR}/copy_scarab.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scarab )
        message( STATUS "Created submodule `scarab`" )
    else()
        message( FATAL "Did not create submodule" )
    endif()
endif()
###

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/scarab/cmake )

include( PackageBuilder )

pbuilder_prepare_project()

#########
# Options
#########

##############
# Dependencies
##############

set( PUBLIC_EXT_LIBS )
set( PRIVATE_EXT_LIBS )

########
# Scarab
########

pbuilder_add_submodule( Scarab scarab )


#########
# Library
#########

include_directories( BEFORE library )

pbuilder_use_sm_library( Scarab Scarab )

set( PACKAGE_LIBS )

set( PBTest_HEADERS
    library/print.hh
)

set( PBTest_SOURCES 
    library/print.cc
)

pbuilder_library( PBTest PBTest_SOURCES PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

pbuilder_install_headers( ${PBTest_HEADERS} )

############
# Executable
############

if( PBTest_ENABLE_EXECUTABLES )
    set( PACKAGE_LIBS
        PBTest
    )
    set( PUBLIC_EXT_LIBS )
    set( PRIVATE_EXT_LIBS )

    set( PBTest_sources
        executable/do_print.cc
    )

    pbuilder_executables( PBTest_sources PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )
endif()
