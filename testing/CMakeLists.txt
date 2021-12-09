# CMakeLists.txt for scarab/testing
# Author: N.S. Oblath
# Created: Jul 7, 2020

##########

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
        use_catch.cc
        stream_check.cc
        test_digital.cc
        test_exponential_backoff.cc
        test_logger.cc
        test_member_variables.cc
        test_signal_handler.cc
        test_time.cc
        test_verbosity.cc
        ${CMAKE_CURRENT_BINARY_DIR}/test_path.cc 
    )

    if( Scarab_BUILD_PARAM )
        set( testing_SOURCES
            ${testing_SOURCES}
            test_param_array.cc
            test_param_by_pointer.cc
            test_param_node.cc
            test_param_translator.cc
            test_param_value.cc
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
            test_nonoption_parser.cc
        )
    endif( Scarab_BUILD_CLI )


set( testing_LIB_DEPENDENCIES 
    Scarab
    Catch
)

# Interface library was recommended for Catch:
# https://stackoverflow.com/questions/34896891/catch-lib-unit-testing-and-ctest-cmake-integration/34900012#34900012
add_library( Catch INTERFACE )
target_include_directories( Catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/catch )

add_executable( run_tests ${testing_SOURCES} )
target_link_libraries( run_tests PUBLIC ${testing_LIB_DEPENDENCIES} )

if( Scarab_BUILD_CLI )
    add_subdirectory( applications )
endif( Scarab_BUILD_CLI )

# So far it seems sufficient to use Catch2's ability to run tests, and not CTest.
# Here are two ways it could potentially be integrated with CTest, if we change our minds.

# Normal CMake Integration
#enable_testing()
add_test(NAME unit_tests COMMAND run_tests )

# Automated CMake Integration
#include(CTest)
#include(Catch)
#catch_discover_tests(foo) 