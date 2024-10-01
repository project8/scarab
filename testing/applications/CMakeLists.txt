# CMakeLists.txt for Scarab/testing/applications
# Author: N. Oblath
# Created: Jan 4, 2016

set( testing_applications_SOURCES
    test_app_with_authentication.cc
    test_app_with_callback.cc
    test_app_with_options.cc
    test_app_with_subcommands.cc
    test_basic_application.cc
    test_raise_sigabrt.cc
    test_raise_sigint.cc
    test_raise_sigquit.cc
    test_raise_sigterm.cc
    test_unhandled_exception.cc
    print_something.cc
)

pbuilder_executables( SOURCES ${testing_applications_SOURCES}
        TARGETS_VAR programs
        PROJECT_LIBRARIES ${testing_LIB_DEPENDENCIES}
)
set( programs ${programs} PARENT_SCOPE )

if( Scarab_ENABLE_TESTING )
    add_test( NAME app_with_authentication COMMAND test_app_with_authentication WORKING_DIRECOTRY ${BIN_INSTALL_DIR} )
    add_test( NAME app_with_callback COMMAND test_app_with_callback WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME app_with_options COMMAND test_app_with_options -f 5 -s "hello" -t "world" WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME app_with_subcommands_get COMMAND test_app_with_subcommands get WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME app_with_subcommands_set COMMAND test_app_with_subcommands set value=10 WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME basic_application COMMAND test_basic_application WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME raise_sigabrt COMMAND test_raise_sigabrt WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME raise_sigint COMMAND test_raise_sigint WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME raise_sigquit COMMAND test_raise_sigquit WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME raise_sigterm COMMAND test_raise_sigterm WORKING_DIRECTORY ${BIN_INSTALL_DIR} )
    add_test( NAME unhandled_exception COMMAND test_unhandled_exception WORKING_DIRECTORY ${BIN_INSTALL_DIR} )

    set_tests_properties( unhandled_exception PROPERTIES WILL_FAIL TRUE )
endif()

