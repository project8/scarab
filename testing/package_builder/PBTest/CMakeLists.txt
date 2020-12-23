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

message( STATUS "Full project name for <${PROJECT_NAME}: ${${PROJECT_NAME}_FULL_PROJECT_NAME}" )


#########
# Options
#########

##############
# Dependencies
##############



# before adding submodules and building the project
pbuilder_prepare_project()

########
# Scarab
########

pbuilder_add_submodule( Scarab scarab )


#########
# Library
#########

include_directories( BEFORE library )

pbuilder_use_sm_library( Scarab Scarab )

set( PBTest_HEADERS
    library/print.hh
)

set( PBTest_SOURCES 
    library/print.cc
)

pbuilder_library( 
    TARGET PBTest 
    SOURCES ${PBTest_SOURCES}
#    PROJECT_LIBRARIES ${PACKAGE_LIBS} 
#    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
#    PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_component_install_and_export( 
    COMPONENT Library
    LIBTARGETS PBTest
)

pbuilder_install_headers( ${PBTest_HEADERS} )

############
# Executable
############

if( PBTest_ENABLE_EXECUTABLES )
    set( PACKAGE_LIBS
        PBTest
    )

    set( PBTest_sources
        executable/do_print.cc
    )

    set( programs )
    pbuilder_executables( 
        SOURCES ${PBTest_sources}
        TARGETS_VAR programs
        PROJECT_LIBRARIES ${PACKAGE_LIBS}
#        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
#        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
    )

    pbuilder_component_install_and_export( 
        COMPONENT Executables
        EXETARGETS ${programs}
    )
endif()

########
# Config
########

configure_file( ${PROJECT_SOURCE_DIR}/PBTestConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PBTestConfig.cmake @ONLY )

pbuilder_do_package_config()
