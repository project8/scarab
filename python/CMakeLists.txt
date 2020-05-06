# CMakeLists.txt file for Scarab/library/python
# Author: N.S. Oblath
# Created on: Sept 2, 2019

if( Scarab_BUILD_PYTHON )
    message( "Building Python bindings" )

    set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

    # Python binding headers
    set( PYBINDING_HEADERFILES
        ${dir}/cancelable_pybind.hh
        ${dir}/signal_handler_pybind.hh
        ${dir}/application_pybind.hh
        ${dir}/version_pybind.hh
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
            ${dir}/param_binding_helpers.hh
            ${dir}/param_value_pybind.hh
            ${dir}/param_array_pybind.hh
            ${dir}/param_node_pybind.hh
        )
    endif( Scarab_BUILD_PARAM )

    set( PROJECT_LIBRARIES Scarab )

    set( PY_MODULE_NAME scarab ) # mock-up
    # start macro pbuilder_add_pybind11_library(PY_MODULE_NAME, PROJECT_LIBRARIES)

    pbuilder_expand_lib_names( ${PROJECT_LIBRARIES} )

    set( PROJECT_INCLUDE_DIRS )
    foreach( LIBRARY ${FULL_PROJECT_LIBRARIES} )
        get_target_property( LIB_INCLUDE_DIRS ${FULL_PROJECT_LIBRARIES} INTERFACE_INCLUDE_DIRECTORIES )
        list( APPEND PROJECT_INCLUDE_DIRS ${LIB_INCLUDE_DIRS} )
    endforeach()
    list( REMOVE_DUPLICATES PROJECT_INCLUDE_DIRS )
    message( STATUS "Main project library include directories: ${PROJECT_INCLUDE_DIRS}")

    include_directories( ${PROJECT_INCLUDE_DIRS} )

    # Potential point of confusion: the C++ library is "Scarab" and the python library is "scarab"
    # Other possible naming schemes seemed less desirable, and we'll hopefully avoid confusion with these comments
    pybind11_add_module( ${PY_MODULE_NAME} ${PYBINDING_SOURCEFILES} )

    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${PY_MODULE_NAME} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${INCLUDE_INSTALL_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )

    target_include_directories( ${PY_MODULE_NAME}
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INCLUDE_INSTALL_SUBDIR}>"
    )
    get_target_property(iids ${PY_MODULE_NAME} INTERFACE_INCLUDE_DIRECTORIES)
    message( STATUS ">>>>>> ${iids}")

    target_link_libraries( ${PY_MODULE_NAME} PRIVATE ${${PROJECT_NAME}_SM_LIBRARIES} ${FULL_PROJECT_LIBRARIES} ${PUBLIC_EXT_LIBS} ${PRIVATE_EXT_LIBS} )
    install( TARGETS ${PY_MODULE_NAME} DESTINATION ${LIB_INSTALL_DIR} )

    # end macro pbuilder_add_pybind11_library

    pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

    configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY)
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )

    message( "Done with Python bindings" )
endif( Scarab_BUILD_PYTHON )
