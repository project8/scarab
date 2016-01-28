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

option( Scarab_BUILD_CODEC_JSON "Flag to enable building the JSON codec" FALSE )

option( Scarab_BUILD_CODEC_MSGPACK "Flag to enable building the msgpack codec" FALSE )

option( Scarab_BUILD_CODEC_YAML "Flag to enable building the yaml codec" FALSE )

option( Scarab_BUILD_AUTHENTICATION "Flag to enable building of the authentication class (requires boost::filesystem)" TRUE )


#######################
# Scarab dependencies #
#######################

if( Scarab_BUILD_CODEC_JSON OR Scarab_BUILD_AUTHENTICATION )
    # RapidJSON
    include_directories( BEFORE 
        ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/json/RapidJSON
        ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/json/RapidJSON/internal 
    )
    install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/json/RapidJSON ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/json/RapidJSON/internal
        DESTINATION ${INCLUDE_INSTALL_DIR}/RapidJSON 
        FILES_MATCHING PATTERN "*.h" 
    )
endif( Scarab_BUILD_CODEC_JSON OR Scarab_BUILD_AUTHENTICATION )

if( Scarab_BUILD_CODEC_MSGPACK )
    # msgpack-c
    include_directories( BEFORE 
        ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/msgpack/msgpack-c 
        ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/msgpack/msgpack-c/include
    )
    install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/msgpack/msgpack-c ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/msgpack/msgpack-c/include
        DESTINATION ${INCLUDE_INSTALL_DIR}/msgpack-c 
        FILES_MATCHING PATTERN "*.h*" 
    )
endif( Scarab_BUILD_CODEC_MSGPACK )

if( Scarab_BUILD_CODEC_YAML )
    # Yaml-cpp
    add_subdirectory( param/codec/yaml/yaml-cpp )
    # I'm hard-coding the include directory and library because I ran into an error when I tried to include ${CMAKE_CURRENT_BINARY_DIR}/param/codec/yaml/yaml-cpp/yaml-cpp-config.cmake )
    include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/yaml/yaml-cpp/include )
    pbuilder_add_ext_libraries( yaml-cpp )
endif( Scarab_BUILD_CODEC_YAML )

if( Scarab_BUILD_AUTHENTICATION )
    # Boost (1.46 required for filesystem version 3)
    find_package( Boost 1.46.0 REQUIRED COMPONENTS filesystem system )
    # make sure dynamic linking is assumed for all boost libraries
    add_definitions( -DBOOST_ALL_DYN_LINK )
    include_directories( ${Boost_INCLUDE_DIRS} )
    # No compiled libraries are currently required
    pbuilder_add_ext_libraries( ${Boost_LIBRARIES} )
endif( Scarab_BUILD_AUTHENTICATION )
    

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

if( Scarab_BUILD_CODEC_JSON OR Scarab_BUILD_AUTHENTICATION )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/json )
    add_subdirectory( param/codec/json )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_MSGPACK )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/msgpack )
    add_subdirectory( param/codec/msgpack )
endif( Scarab_BUILD_CODEC_MSGPACK )

if( Scarab_BUILD_CODEC_YAML )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/param/codec/yaml )
    add_subdirectory( param/codec/yaml )
endif( Scarab_BUILD_CODEC_YAML )

if( Scarab_BUILD_AUTHENTICATION )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/authentication )
    add_subdirectory( authentication )
endif( Scarab_BUILD_AUTHENTICATION )


##########

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

