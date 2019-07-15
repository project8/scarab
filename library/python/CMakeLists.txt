# CMakeLists.txt for scarab/library/python

message( STATUS "project libraries: ${FULL_PROJECT_LIBRARIES}" )

include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/param )

# pybind11 bindings of scarab::param
set( PB_PARAM_HEADERFILES
    param/param_pybind.hh
    param/param_value.hh
    param/param_array.hh
    param/param_node.hh
)

set( PB_SCARAB_SOURCEFILES
    scarab_pybind.cc
)

pybind11_add_module( scarab ${PB_PARAM_SOURCEFILES} )
target_link_libraries( scarab PRIVATE ${FULL_PROJECT_LIBRARIES} ${EXTERNAL_LIBRARIES} )
pbuilder_install_libraries( scarab )
