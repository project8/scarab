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

# mockup of what will go in package builder
# call will be: 
## pbuilder_add_submodule( Scarab scarab )
# mock up the input parameters
set( SM_NAME "Scarab" )
set( SM_LOCATION "scarab" )

# start macro

if( NOT IS_ABSOLUTE ${SM_LOCATION} )
    set( SM_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${SM_LOCATION}" )
endif()

# Conditions that let us in this loop:
#  1. Submodule SM_NAME has not yet been found
#  2. This is the location of the submodule SM_NAME
message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
message( STATUS "SM_LOCATION: ${SM_LOCATION}" )
if( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${SM_LOCATION}" )
    message( "Adding submodule ${SM_NAME} in location ${SM_LOCATION}" )

    set( ${SM_NAME}_FOUND TRUE CACHE INTERNAL "" )
    set( ${SM_NAME}_LOCATION ${SM_LOCATION} CACHE INTERNAL "" )

    # Determine the library name suffix for this submodule with respect to its parent if it's not already defined
    if( NOT DEFINED ${SM_NAME}_PARENT_LIB_NAME_SUFFIX )
        set( ${SM_NAME}_PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}${PARENT_LIB_NAME_SUFFIX}" CACHE INTERNAL "Scoped libraryname suffix for submodule ${SM_NAME}" )
        message( STATUS "PARENT_LIB_NAME_SUFFIX being set for SM ${SM_NAME}: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}" )
    endif()

    # Set the library name suffix that will be used by the submodule
    set( PARENT_LIB_NAME_SUFFIX ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX} )
#[[
    if( NOT DEFINED PARENT_LIB_NAME_SUFFIX )
        #set( PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}" CACHE INTERNAL "Library name suffix for submodules" )
        set( PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}" )
    else( NOT DEFINED PARENT_LIB_NAME_SUFFIX )
        #set( PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}${PARENT_LIB_NAME_SUFFIX}" CACHE INTERNAL "" )
        set( PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}${PARENT_LIB_NAME_SUFFIX}" )
    endif( NOT DEFINED PARENT_LIB_NAME_SUFFIX )
    message( STATUS "PARENT_LIB_NAME_SUFFIX being set for SM ${SM_NAME}: ${PARENT_LIB_NAME_SUFFIX}" )

    set( ${SM_NAME}_PARENT_LIB_NAME_SUFFIX ${PARENT_LIB_NAME_SUFFIX} CACHE INTERNAL "Scoped library name suffix for submodule ${SM_NAME}" )
]]
    # Set submodule include subdirectory
    set( SM_INCLUDE_SUBDIR "/${SM_NAME}" )
    message( STATUS "Include files for submodule ${SM_NAME} will be installed in ${TOP_PROJECT_INCLUDE_INSTALL_DIR}/${SM_NAME}" )

#[[
    if( NOT DEFINED PARENT_INC_DIR_PATH )
    set( PARENT_INC_DIR_PATH "/${SM_NAME}" CACHE INTERNAL "Include directory path for submodules" )
    else( NOT DEFINED PARENT_INC_DIR_PATH )
    set( PARENT_INC_DIR_PATH "${PARENT_INC_DIR_PATH}/${SM_NAME}" CACHE INTERNAL "" )
    endif( NOT DEFINED PARENT_INC_DIR_PATH )
    #message( STATUS "PARENT_INC_DIR_PATH being set for SM ${SM_NAME}: ${PARENT_INC_DIR_PATH}" )
]]

    message( STATUS "SM ${SM_NAME} cached variables:" )
    message( STATUS "${SM_NAME}_FOUND: ${${SM_NAME}_FOUND}" )
    message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
    message( STATUS "${SM_NAME}_PARENT_LIB_NAME_SUFFIX: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}")

    message( STATUS "Proceeding into subdirectory: ${SM_LOCATION}" )
    add_subdirectory( ${SM_LOCATION} )

    # Need to unset the submodule include subdirectory since we're in a macro
    unset( SM_INCLUDE_SUBDIR )

    # Now that we're going back up the stack of submodules, unset these cache variables
    #unset( PARENT_LIB_NAME_SUFFIX CACHE )


endif( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${SM_LOCATION}" )

if( TARGET Scarab )
    message( STATUS "******* Target Scarab exists" )
endif()

#end macro

# after package builder; this stuff is for the parent project

#find_package( Scarab )

#list( APPEND PUBLIC_EXT_LIBS Scarab )

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
