# CMakeLists.txt file for Scarab/testing/python
# Author: N.S. Oblath
# Created on: Apr 23, 2025

message( "Building Python testing bindings" )

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

# Python binding headers
set( PYBINDING_HEADERFILES
    ${dir}/test_weak_holder_pybind.hh
)

# Python bindings
set( PYBINDING_SOURCEFILES
    ${dir}/scarab_testing_pybind.cc
)

set( PYBINDING_PROJECT_LIBRARIES Scarab )

pbuilder_add_pybind11_module( 
    MODULE_NAME scarab_testing
    SOURCE_FILES ${PYBINDING_SOURCEFILES}
    PROJECT_LIBRARIES ${PYBINDING_PROJECT_LIBRARIES} 
)

pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

#if( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )
#    message( STATUS "Installing add_lib_python_path.sh since python modules will not be installed in the site-package directory" )
#    configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY)
#    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )
#endif( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )

message( "Adding target for all pytest scripts" )

# developer's note: we really just want to collect the .py files, but we'll just use cmake -E copy_directory for simplicity
add_custom_target(
    pytest ALL
    COMMENT "Copying pytest files to the build directory"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
)

