# CMakeLists.txt file for Scarab/library/python
# Author: N.S. Oblath
# Created on: Sept 2, 2019

if( Scarab_BUILD_PYTHON )

    set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

    # Python binding headers
    set( PYBINDING_HEADERFILES
        ${dir}/signal_handler_pybind.hh
    )

    # Python bindings
    set( PYBINDING_SOURCEFILES
        ${dir}/scarab_namespace_pybind.cc
    )

    if( Scarab_BUILD_PARAM )
        add_definitions( -DBUILD_PARAM_PYBINDING )
        set( PYBINDING_HEADERFILES 
            ${PYBINDING_HEADERFILES}
            ${dir}/param_pybind.hh
            ${dir}/param_value_pybind.hh
            ${dir}/param_array_pybind.hh
            ${dir}/param_node_pybind.hh
        )
    endif( Scarab_BUILD_PARAM )

    set( LIB_DEPENDENCIES Scarab )

    configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY)
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )

    pybind11_add_module( py_scarab MODULE ${PYBINDING_SOURCEFILES} )
    target_link_libraries( py_scarab PRIVATE ${LIB_DEPENDENCIES} ${EXTERNAL_LIBRARIES} )
    pbuilder_install_libraries( py_scarab )
    pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

endif( Scarab_BUILD_PYTHON )
