# CMakeLists.txt for Scarab
# Author: N. Oblath
# Created: Jan 4, 2016

# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required (VERSION 3.1)

#########
# setup #
#########

# load the version number from the Scarab/VERSION file
file( STRINGS ${PROJECT_SOURCE_DIR}/VERSION scarab_version_from_file )
string( REPLACE " " "." scarab_version ${scarab_version_from_file} )

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Scarab VERSION ${scarab_version} )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

include( PackageBuilder )

pbuilder_prepare_project()


##################
# Scarab options #
##################

option( Scarab_BUILD_CODEC_JSON "Flag to enable building the JSON codec (requires Scarab_BUILD_PARAM)" TRUE )

option( Scarab_BUILD_CODEC_YAML "Flag to enable building the YAML codec (requires Scarab_BUILD_PARAM)" TRUE )

option( Scarab_BUILD_AUTHENTICATION "Flag to enable building of the authentication class (requires Scarab_BUILD_CODEC_JSON)" TRUE )

option( Scarab_BUILD_PARAM "Flag to enable building of the param class" TRUE )

option( Scarab_BUILD_CLI "Flag to enable the building of the command-line interface class" TRUE )

option( Scarab_BUILD_PYTHON "Build python bindings" TRUE )

if( (Scarab_BUILD_CODEC_JSON OR Scarab_BUILD_CODEC_MSGPACK OR Scarab_BUILD_CODEC_YAML OR Scarab_BUILD_AUTHENTICATION OR Scarab_BUILD_CLI) AND NOT Scarab_BUILD_PARAM )
    message( FATAL_ERROR "Invalid combination of build options.  Building the Codecs, Authentication, and CLI requires Param.  If you want these elements, turn on Param.  If you want Param off, turn these elements off." )
endif()

if( Scarab_BUILD_AUTHENTICATION AND NOT Scarab_BUILD_CODEC_JSON )
    message( FATAL_ERROR "Invalid combination of build options.  Building Authentication requires the JSON Codec.  If you want Authentication, turn on the JSON Codec.  If you want the JSON Codec off, turn Authentication off.")
endif()

#######################
# Scarab dependencies #
#######################

set( PUBLIC_EXT_LIBS )
set( PRIVATE_EXT_LIBS )

if( Scarab_BUILD_CODEC_JSON )
    set( RAPIDJSON_FILE_BUFFER_SIZE 65536 CACHE STRING "Buffer size for reading and writing files using RapidJSON (in Bytes)" )
    
    add_definitions( -DRAPIDJSON_FILE_BUFFER_SIZE=${RAPIDJSON_FILE_BUFFER_SIZE} )
    add_definitions( -DUSE_CODEC_JSON )
    
    # have to use a module find here to use the custom FindRapidJSON.cmake provided with Scarab
    find_package( RapidJSON MODULE REQUIRED )

    list( APPEND PUBLIC_EXT_LIBS RapidJSON::RapidJSON )
else( Scarab_BUILD_CODEC_JSON )
    remove_definitions( -DUSE_CODEC_JSON )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_YAML )
    find_package( yaml-cpp CONFIG REQUIRED )
	#set_option( USE_CPP11 TRUE )

    # Yaml-cpp
    #option( YAML_CPP_BUILD_TOOLS "yaml-cpp option setting" OFF )
	#option( YAML_CPP_BUILD_TESTS "Enable parse tools" OFF )
    #option( YAML_CPP_BUILD_CONTRIB "yaml-cpp option setting" OFF )
    #option( BUILD_SHARED_LIBS "yaml-cpp option setting" ON )
    #add_subdirectory( param/codec/yaml/yaml-cpp )
    # I'm hard-coding the include directory and library because I ran into an error when I tried to include ${CMAKE_CURRENT_BINARY_DIR}/param/codec/yaml/yaml-cpp/yaml-cpp-config.cmake )
    #include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/yaml/yaml-cpp/include )
    #pbuilder_add_ext_libraries( yaml-cpp )

    list( APPEND PUBLIC_EXT_LIBS yaml-cpp )
endif( Scarab_BUILD_CODEC_YAML )

list( APPEND boost_components filesystem system )
list( APPEND PUBLIC_EXT_LIBS Boost::filesystem )

# making sure boost_components is not empty and remove duplicate
if (boost_components)
    list( REMOVE_DUPLICATES boost_components )
endif ( boost_components)

# Boost (1.46 required for filesystem version 3)
find_package( Boost 1.46.0 REQUIRED COMPONENTS ${boost_components} )
# make sure dynamic linking is assumed for all boost libraries
#add_definitions( -DBOOST_ALL_DYN_LINK )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Boost_ALL_DYN_LINK ON )
include_directories( ${Boost_INCLUDE_DIRS} )
# No compiled libraries are currently required
###pbuilder_add_ext_libraries( ${Boost_LIBRARIES} )
    
# For gethostname in version_semantic
if( WIN32 )
    pbuilder_add_ext_libraries( "Ws2_32.lib" )
endif( WIN32 )

# date library
include_directories( BEFORE ${PROJECT_SOURCE_DIR}/external/date )

# cli11 library
if( Scarab_BUILD_CLI )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/external/cli11 )
endif( Scarab_BUILD_CLI )

# Pybind11
if( Scarab_BUILD_PYTHON )
    find_package( pybind11 REQUIRED )
endif( Scarab_BUILD_PYTHON )


########################
# Build Scarab library #
########################

add_subdirectory( library )


##########
# Python #
##########

if( Scarab_BUILD_PYTHON )
    list( APPEND Scarab_SOURCE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/python )
    add_subdirectory( python )
endif( Scarab_BUILD_PYTHON )


###########
# Testing #
###########

#add_subdirectory( testing )
add_subdirectory( library/test )


##################
# Package Config #
##################

configure_file( ${PROJECT_SOURCE_DIR}/ScarabConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ScarabConfig.cmake )

pbuilder_do_package_config( ${CMAKE_CURRENT_BINARY_DIR}/ScarabConfig.cmake )


################################
# Variables for parent project #
################################

#pbuilder_variables_for_parent()

