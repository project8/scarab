# CMakeLists.txt for Scarab
# Author: N. Oblath
# Created: Jan 4, 2016

# Minimum cmake verison 3.12 required for FindPython3
cmake_minimum_required (VERSION 3.12)

#########
# setup #
#########

# load the version number from the Scarab/VERSION file
file( STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/VERSION scarab_version_from_file )
string( REPLACE " " "." scarab_version ${scarab_version_from_file} )

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Scarab VERSION ${scarab_version} )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

include( PackageBuilder )
include( FetchContent )

# Require C++17 (minimum set by: Quill dependency)
set_to_max( CMAKE_CXX_STANDARD 17 )

##################
# Scarab options #
##################

option( Scarab_BUILD_CODEC_JSON "Flag to enable building the JSON codec (requires Scarab_BUILD_PARAM)" TRUE )

option( Scarab_BUILD_CODEC_YAML "Flag to enable building the YAML codec (requires Scarab_BUILD_PARAM)" TRUE )

option( Scarab_BUILD_AUTHENTICATION "Flag to enable building of the authentication class (requires Scarab_BUILD_CODEC_JSON)" TRUE )

option( Scarab_BUILD_PARAM "Flag to enable building of the param class" TRUE )

option( Scarab_BUILD_CLI "Flag to enable the building of the command-line interface class" TRUE )

option( Scarab_BUILD_PYTHON "Build python bindings" TRUE )

option( Scarab_ENABLE_LOGGER_DEBUG "Flag to enable debug printing for the logger itself" FALSE )

if( (Scarab_BUILD_CODEC_JSON OR Scarab_BUILD_CODEC_YAML OR Scarab_BUILD_AUTHENTICATION OR Scarab_BUILD_CLI) AND NOT Scarab_BUILD_PARAM )
    message( FATAL_ERROR "Invalid combination of build options.  Building the Codecs, Authentication, and CLI requires Param.  If you want these elements, turn on Param.  If you want Param off, turn these elements off." )
endif()

if( Scarab_BUILD_AUTHENTICATION AND NOT Scarab_BUILD_CODEC_JSON AND NOT Scarab_BUILD_CODEC_YAML )
    message( FATAL_ERROR "Invalid combination of build options.  Building Authentication requires the either the JSON codec or the YAML codec.  If you want Authentication, turn on the JSON Codec.  If you want the JSON Codec off, turn Authentication off.")
endif()

if( Scarab_BUILD_CLI AND NOT Scarab_BUILD_AUTHENTICATION )
    message( FATAL_ERROR "Invalid combination of build options.  Building the CLI requires Authentication.  If you want Authentication off, turn off the CLI" )
endif()

if( Scarab_ENABLE_TESTING )
    enable_testing()
endif()

if( Scarab_ENABLE_LOGGER_DEBUG ) 
    add_compile_definitions( SCARAB_LOGGER_DEBUG )
else( Scarab_ENABLE_LOGGER_DEBUG )
    remove_definitions( SCARAB_LOGGER_DEBUG )
endif( Scarab_ENABLE_LOGGER_DEBUG )

# Scarab needs C++11
set( CMAKE_CXX_STANDARD 17 )


#######################
# Scarab dependencies #
#######################

set( PUBLIC_EXT_LIBS )
set( PRIVATE_EXT_LIBS )

# logging via spdlog
add_compile_definitions( SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE )
set( SPDLOG_BUILD_SHARED TRUE )
set( SPDLOG_INSTALL TRUE )
FetchContent_Declare( spdlog
    GIT_REPOSITORY https://github.com/project8/spdlog.git
    GIT_TAG v1.x_p8
)
FetchContent_MakeAvailable( spdlog )
list( APPEND PUBLIC_EXT_LIBS spdlog::spdlog )


if( Scarab_BUILD_CODEC_JSON )
    set( RAPIDJSON_FILE_BUFFER_SIZE 65536 CACHE STRING "Buffer size for reading and writing files using RapidJSON (in Bytes)" )
    
    add_definitions( -DRAPIDJSON_FILE_BUFFER_SIZE=${RAPIDJSON_FILE_BUFFER_SIZE} )
    add_definitions( -DUSE_CODEC_JSON )
    
    find_package( RapidJSON 1.0 REQUIRED )

    # The config file that comes with RapidJSON does not define an interface target, so we'll do so here.
    # We use the name `rapidjson` to match the post-v1.1.0 version.
    # The v1-1.1 include-dir variable is RAPIDJSON_INCLUDE_DIRS.
    if( NOT TARGET rapidjson )
        add_library(rapidjson INTERFACE IMPORTED)
        set_target_properties(rapidjson PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${RAPIDJSON_INCLUDE_DIRS}"
        )
    endif()

    list( APPEND PUBLIC_EXT_LIBS rapidjson )
else( Scarab_BUILD_CODEC_JSON )
    remove_definitions( -DUSE_CODEC_JSON )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_YAML )
    find_package( yaml-cpp CONFIG REQUIRED )

    # sometimes the target is yaml-cpp and somtimes it's yaml-cpp::yaml-cpp
    if( TARGET yaml-cpp::yaml-cpp )
        list( APPEND PUBLIC_EXT_LIBS yaml-cpp::yaml-cpp )
    else()
        list( APPEND PUBLIC_EXT_LIBS yaml-cpp )
    endif()
    
    add_definitions( -DUSE_CODEC_YAML )
else( Scarab_BUILD_CODEC_YAML )
    remove_definitions( -DUSE_CODEC_YAML )
endif( Scarab_BUILD_CODEC_YAML )

# Create a cache variable for boost components that can be set from dependent projects
set( Scarab_BOOST_COMPONENTS "${Scarab_BOOST_COMPONENTS};filesystem" CACHE INTERNAL "Boost components to be found by Scarab" )

#list( APPEND Scarab_BOOST_COMPONENTS filesystem system )
list( APPEND PUBLIC_EXT_LIBS Boost::filesystem )

# making sure Scarab_BOOST_COMPONENTS is not empty and remove duplicate
if( Scarab_BOOST_COMPONENTS )
    list( REMOVE_DUPLICATES Scarab_BOOST_COMPONENTS )
endif( Scarab_BOOST_COMPONENTS )

# Boost (1.46 required for filesystem version 3)
find_package( Boost 1.46.0 REQUIRED COMPONENTS ${Scarab_BOOST_COMPONENTS} )
# make sure dynamic linking is assumed for all boost libraries
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Boost_ALL_DYN_LINK ON )
    
# For gethostname in version_semantic
if( WIN32 )
    pbuilder_add_ext_libraries( "Ws2_32.lib" )
endif( WIN32 )

# Pybind11
if( Scarab_BUILD_PYTHON )
    find_package( Python3 REQUIRED COMPONENTS Interpreter Development )
    set(PYBIND11_PYTHON_VERSION ${Python3_VERSION} CACHE STRING "")
    find_package( pybind11 REQUIRED )
    #if( TARGET Python3::Python )
    #    message( STATUS "%%%%% Found Python::Python" )
    #else()
    #    message( STATUS "%%%%% Did NOT find Python::Python" )
    #endif()
    include_directories( ${Python3_INCLUDE_DIRS} )
endif( Scarab_BUILD_PYTHON )


###################
# Process options #
###################

pbuilder_prepare_project()


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


###############
# Executables #
###############

if( Scarab_ENABLE_EXECUTABLES )
    add_subdirectory( executables )
endif( Scarab_ENABLE_EXECUTABLES )


###########
# Testing #
###########

if( Scarab_ENABLE_TESTING )
    add_subdirectory( testing )
endif()


##################
# Package Config #
##################

pbuilder_do_package_config(
    INPUT_FILE ${PROJECT_SOURCE_DIR}/ScarabConfig.cmake.in
    OUTPUT_FILE ScarabConfig.cmake
)

