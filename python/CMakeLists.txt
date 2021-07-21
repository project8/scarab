# CMakeLists.txt file for Scarab/library/python
# Author: N.S. Oblath
# Created on: Sept 2, 2019

if( Scarab_BUILD_PYTHON )
    message( "Building Python bindings" )

    set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

    # Python binding headers
    set( PYBINDING_HEADERFILES
        ${dir}/scarab_binding_helpers.hh
        ${dir}/cancelable_pybind.hh
        ${dir}/signal_handler_pybind.hh
        ${dir}/version_pybind.hh
    )

    # Python bindings
    set( PYBINDING_SOURCEFILES
        ${dir}/scarab_namespace_pybind.cc
    )

    if( Scarab_BUILD_CLI )
        add_definitions( -DBUILD_CLI_PYBINDING )
        set( PYBINDING_HEADERFILES
            ${PYBINDING_HEADERFILES}
            ${dir}/application_pybind.hh
        )
    endif( Scarab_BUILD_CLI )

    if( Scarab_BUILD_PARAM )
        add_definitions( -DBUILD_PARAM_PYBINDING )
        set( PYBINDING_HEADERFILES
            ${PYBINDING_HEADERFILES}
            ${dir}/param_pybind.hh
            ${dir}/param_binding_helpers.hh
            ${dir}/param_value_pybind.hh
            ${dir}/param_array_pybind.hh
            ${dir}/param_node_pybind.hh
        )
    endif( Scarab_BUILD_PARAM )

    set( PROJECT_LIBRARIES Scarab )

    pbuilder_add_pybind11_module( scarab ${PROJECT_LIBRARIES} )

    pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

    if( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )
        message( STATUS "Installing add_lib_python_path.sh since python modules will not be installed in the site-package directory" )
        configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY)
        install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )
    endif( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )

    message( "Done with Python bindings" )
endif( Scarab_BUILD_PYTHON )
