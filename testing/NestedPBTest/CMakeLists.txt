cmake_minimum_required (VERSION 3.1)

cmake_policy( SET CMP0048 NEW ) # version in project()
project( NestedPBTest VERSION 1.0.0 )

### 
if( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scarab )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/create_nesting.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_nesting.sh @ONLY )
    execute_process( COMMAND ${CMAKE_CURRENT_BINARY_DIR}/create_nesting.sh
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

message( STATUS "@@@@@@@ NestedPBTEst full project name: ${NestedPBTest_FULL_PROJECT_NAME}" )


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

pbuilder_add_submodule( Scarab scarab ) # first use of Scarab is the direct submodule, so the scarab submodule of PBTest should be skipped
pbuilder_add_submodule( PBTest PBTest )


#########
# Library
#########

include_directories( BEFORE library )

pbuilder_use_sm_library( Scarab Scarab )
pbuilder_use_sm_library( PBTest PBTest )

set( PACKAGE_LIBS )

set( NestedPBTest_HEADERS
    library/nested_print.hh
)

set( NestedPBTest_SOURCES 
    library/nested_print.cc
)

pbuilder_library( NestedPBTest NestedPBTest_SOURCES PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

pbuilder_install_headers( ${NestedPBTest_HEADERS} )

############
# Executable
############

if( NestedPBTest_ENABLE_EXECUTABLES )
    set( PACKAGE_LIBS
        NestedPBTest
    )
    set( PUBLIC_EXT_LIBS )
    set( PRIVATE_EXT_LIBS )

    set( NestedPBTest_sources
        executable/nested_do_print.cc
    )

    pbuilder_executables( NestedPBTest_sources PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )
endif()

########
# Config
########

configure_file( ${PROJECT_SOURCE_DIR}/NestedPBTestConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/NestedPBTestConfig.cmake @ONLY )

message( STATUS "@@@@@@@!! NestedPBTest full project name: ${NestedPBTest_FULL_PROJECT_NAME}" )
pbuilder_do_package_config()

