# CMakeLists.txt for Scarab/library
# Author: N. Oblath
# Created: Jan 4, 2016

# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required (VERSION 3.1)

#########
# setup #
#########

# load the version number from the Scarab/VERSION file
file( STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/../VERSION scarab_version_from_file )
string( REPLACE " " "." scarab_version ${scarab_version_from_file} )

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Scarab VERSION ${scarab_version} )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake )

include( PackageBuilder )

pbuilder_prepare_project()


##################
# Scarab options #
##################

set( Scarab_BUILD_CODEC_JSON FALSE CACHE BOOL "Flag to enable building the JSON codec" )

set( Scarab_BUILD_CODEC_MSGPACK FALSE CACHE BOOL "Flag to enable building the msgpack codec" )

set( Scarab_BUILD_AUTHENTICATION TRUE CACHE BOOL "Flag to enable building of the authentication class (requires boost::filesystem)" )


#######################
# Scarab dependencies #
#######################

# filesystem and system for param_value's use of boost::filesystem::path
# thread and date_time for concurrent_queue
set( boost_components filesystem system thread date_time )

if( Scarab_BUILD_CODEC_JSON )
    # RapidJSON
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/json/RapidJSON )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_MSGPACK )
    # msgpack-c
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack/msgpack-c )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack/msgpack-c/include )
endif( Scarab_BUILD_CODEC_MSGPACK )

if( Scarab_BUILD_AUTHENTICATION )
    list( APPEND boost_components filesystem system )
endif( Scarab_BUILD_AUTHENTICATION )

# Boost (1.46 required for filesystem version 3)
list( REMOVE_DUPLICATES boost_components )
find_package( Boost 1.46.0 REQUIRED COMPONENTS ${boost_components} )
# make sure dynamic linking is assumed for all boost libraries
add_definitions( -DBOOST_ALL_DYN_LINK )
include_directories( ${Boost_INCLUDE_DIRS} )
# No compiled libraries are currently required
pbuilder_add_ext_libraries( ${Boost_LIBRARIES} )
    
# For gethostname in version_semantic
if( WIN32 )
    pbuilder_add_ext_libraries( "Ws2_32.lib" )
endif( WIN32 )


########################
# Build Scarab library #
########################

# Unlike many other libraries in the P8 codebase, Scarab builds its library here, 
# only using subdirectories to add relevant files to the Scarab_HEADERS and Scarab_SOURCES list variables.


include_directories( BEFORE
    ${PROJECT_SOURCE_DIR}/utility
    ${PROJECT_SOURCE_DIR}/logger
    ${PROJECT_SOURCE_DIR}/param
)

##########

set( Scarab_HEADERS )
set( Scarab_SOURCES )

add_subdirectory( utility )
add_subdirectory( logger )
add_subdirectory( param )

if( Scarab_BUILD_CODEC_JSON )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/json )
    add_subdirectory( param/codec/json )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_MSGPACK )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack )
    add_subdirectory( param/codec/msgpack )
endif( Scarab_BUILD_CODEC_MSGPACK )

if( Scarab_BUILD_AUTHENTICATION )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/authentication )
    add_subdirectory( authentication )
endif( Scarab_BUILD_AUTHENTICATION )


##########

#message(STATUS "Scarab sources: ${Scarab_SOURCES}" )

pbuilder_library( Scarab Scarab_SOURCES "" )

pbuilder_install_headers( ${Scarab_HEADERS} )


###########
# Testing #
###########

add_subdirectory( test )


################################
# Variables for parent project #
################################

pbuilder_variables_for_parent()

