# FindRapidJSON.cmake
#
# Finds the rapidjson library and defines and interface target for import
#
# This will define the following variables
#
#    RapidJSON_FOUND
#    RapidJSON_INCLUDE_DIRS
#
# and the following imported targets
#
#     RapidJSON::RapidJSON
#
# RapidJSON provides a CMake package config files for finding the 
# installed package, and defines RAPIDJSON_INCLUDE_DIRS.
#
# This find module uses that config-find and defines the above variables and target.
#
# Author: N.S. Oblath
# Based on version by: Pablo Arias - pabloariasal@gmail.com

find_package( RapidJSON CONFIG )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( RapidJSON
    REQUIRED_VARS RAPIDJSON_INCLUDE_DIRS
    VERSION_VAR RapidJSON_VERSION
)

if( RapidJSON_FOUND )
    set( RapidJSON_INCLUDE_DIRS ${RAPIDJSON_INCLUDE_DIRS} )
endif()

if(RapidJSON_FOUND AND NOT TARGET RapidJSON::RapidJSON)
    add_library(RapidJSON::RapidJSON INTERFACE IMPORTED)
    set_target_properties(RapidJSON::RapidJSON PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${RapidJSON_INCLUDE_DIRS}"
    )
endif()
