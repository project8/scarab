# CMakeLists.txt for Scarab/testing/applications
# Author: N. Oblath
# Created: Jan 4, 2016

set( testing_applications_SOURCES
    test_app_with_callback.cc
    test_app_with_options.cc
    test_app_with_subcommands.cc
    test_basic_application.cc
    test_raise_sigabrt.cc
    test_raise_sigint.cc
    test_raise_sigquit.cc
    test_raise_sigterm.cc
    test_unhandled_exception.cc
)

pbuilder_executables( testing_applications_SOURCES testing_LIB_DEPENDENCIES PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )
