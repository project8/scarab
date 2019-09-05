# CMakeLists.txt file for Scarab/library/python
# Author: N.S. Oblath
# Created on: Sept 2, 2019

if( Scarab_BUILD_PYTHON )

    set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

    # Python binding headers
    set( PYBINDING_HEADERFILES
        ${dir}/cancelable_pybind.hh
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
    pbuilder_expand_lib_names( FULL_LIB_DEPENDENCIES LIB_DEPENDENCIES )

    configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY)
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )

    # Potential point of confusion: the C++ library is "Scarab" and the python library is "scarab"
    # Other possible naming schemes seemed less desirable, and we'll hopefully avoid confusion with these comments
    pybind11_add_module( scarab ${PYBINDING_SOURCEFILES} )
    target_link_libraries( scarab PRIVATE ${FULL_LIB_DEPENDENCIES} ${EXTERNAL_LIBRARIES} )
    install( TARGETS scarab DESTINATION ${LIB_INSTALL_DIR} )
    pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

endif( Scarab_BUILD_PYTHON )
