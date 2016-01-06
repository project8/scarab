# CMakeLists.txt for Scarab/library
# Author: N. Oblath
# Created: Jan 4, 2016

# Minimum cmake verison 3.0 required by the way CMake handles rpath information on Mac OSX.
cmake_minimum_required (VERSION 3.0)

#########
# setup #
#########

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Scarab VERSION 1.0.0 )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake )

include( PackageBuilder )

pbuilder_prepare_project()


##################
# Scarab options #
##################

set( Scarab_BUILD_CODEC_JSON CACHE BOOL "Flag to enable building the JSON codec" )

set( Scarab_BUILD_CODEC_MSGPACK CACHE BOOL "Flag to enable building the msgpack codec" )



#######################
# Scarab dependencies #
#######################

if( Scarab_BUILD_CODEC_JSON )
    # RapidJSON
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/json/RapidJSON )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_MSGPACK )
    # msgpack-c
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack/msgpack-c )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack/msgpack-c/include )
endif( Scarab_BUILD_CODEC_MSGPACK )



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

##########

pbuilder_library( Scarab Scarab_SOURCES "" )
#add_library( Scarab ${Scarab_SOURCES} )
#target_link_libraries( Scarab ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${Scarab_HEADERS} )
#pbuilder_install_libraries( Scarab )


###########
# Testing #
###########

add_subdirectory( test )


################################
# Variables for parent project #
################################

pbuilder_variables_for_parent()
