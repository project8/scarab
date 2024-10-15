# CMakeLists.txt for scarab/testing
# Author: N.S. Oblath
# Created: Jul 7, 2020

##########

# Install Catch2

include( FetchContent )

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)

FetchContent_MakeAvailable( Catch2 )
list( APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras )

##########

# Unit testing

set( testing_HEADERS
    stream_check.hh
    test_member_variables.hh
)

pbuilder_install_headers( ${testing_HEADERS} )

configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/test_path.cc.in 
    ${CMAKE_CURRENT_BINARY_DIR}/test_path.cc 
    @ONLY
)

set( testing_SOURCES
    run_tests.cc
    stream_check.cc
    test_digital.cc
    test_exponential_backoff.cc
    test_logger.cc
    test_member_variables.cc
    test_signal_handler.cc
    test_time.cc
    ${CMAKE_CURRENT_BINARY_DIR}/test_path.cc 
)

if( Scarab_BUILD_PARAM )
    set( testing_HEADERS
        ${testing_HEADERS}
        test_param_modifiers_visitors.hh
    )
    set( testing_SOURCES
        ${testing_SOURCES}
        test_param_array.cc
        test_param_by_pointer.cc
        test_param_env_modifier.cc
        test_param_modifier.cc
        test_param_nested.cc
        test_param_node.cc
        test_param_translator.cc
        test_param_value.cc
        test_param_visitor.cc
)
endif( Scarab_BUILD_PARAM )

if( Scarab_BUILD_CODEC_JSON )
    set( testing_SOURCES
        ${testing_SOURCES}
        test_json.cc
    )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_YAML )
    set( testing_SOURCES
        ${testing_SOURCES}
        test_yaml.cc
    )
endif( Scarab_BUILD_CODEC_YAML )

if( Scarab_BUILD_CLI )
    set( testing_SOURCES
        ${testing_SOURCES}
        test_main_app.cc
        test_nonoption_parser.cc
        test_verbosity.cc
    )
endif( Scarab_BUILD_CLI )

if( Scarab_BUILD_AUTHENTICATION )
    set( testing_SOURCES
        ${testing_SOURCES}
        test_authentication.cc
    )
endif( Scarab_BUILD_AUTHENTICATION )

set( testing_LIB_DEPENDENCIES 
    Scarab
)

pbuilder_executable( 
    EXECUTABLE run_tests
    SOURCES ${testing_SOURCES}
    PROJECT_LIBRARIES ${testing_LIB_DEPENDENCIES}
    PRIVATE_EXTERNAL_LIBRARIES Catch2::Catch2
)

##########

# Other tests

if( Scarab_BUILD_PYTHON )
    add_custom_target(
        python_testing ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/python" "${CMAKE_CURRENT_BINARY_DIR}/python"
        COMMENT "Copying testing/python to the build directory"
    )
endif( Scarab_BUILD_PYTHON )

if( Scarab_BUILD_CLI )
    # this sets the programs list
    add_subdirectory( applications )
endif( Scarab_BUILD_CLI )

# Add the run_tests executable to the programs list, which has now been populated
list( APPEND programs "run_tests" )

##########

# Installation

pbuilder_component_install_and_export( 
    COMPONENT Catch2
    LIBTARGETS Catch2
    NAMESPACE Catch2::
)

#message(STATUS "##### programs: ${programs}")
pbuilder_component_install_and_export( 
    COMPONENT Executables
    EXETARGETS ${programs}
)

##########

# CTest integration

# Normal CMake Integration
add_test( NAME unit_tests COMMAND run_tests WORKING_DIRECTORY ${BIN_INSTALL_DIR} )

# Another option would be to do automated CMake Integration
#include(CTest)
#include(Catch)
#catch_discover_tests(foo) 
