# PythonPackage.cmake
# Author: B. H. LaRoque
#
# expands on package builder to script support for python modules
include (PackageBuilder)

# macro to get python develop dependencies
# TODO for now this is a copy/paste of what I'm already doing, should make it sarter
macro( python_package_find_python VALUE )
    execute_process(COMMAND python${VALUE}-config --includes COMMAND awk "{print substr($1,3)}" OUTPUT_VARIABLE PYTHON_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND python${VALUE}-config --ldflags COMMAND awk "{print substr($1,3)\"/lib\"substr($2,3)}" OUTPUT_VARIABLE PYTHON_LIBRARY_base OUTPUT_STRIP_TRAILING_WHITESPACE)
    #MESSAGE("python include dir: ${PYTHON_INCLUDE_DIR}")
    #MESSAGE("python lib base dir: ${PYTHON_LIBRARY_base}")
    file(GLOB PYTHON_LIBRARY "${PYTHON_LIBRARY_base}.so"  "${PYTHON_LIBRARY_base}.dylib")
    #MESSAGE("python lib: ${PYTHON_LIBRARY}")
    if ("${PYTHON_LIBRARY}" STREQUAL "")
        UNSET(PYTHON_LIBRARY)
    endif ("${PYTHON_LIBRARY}" STREQUAL "")
    find_package(PythonLibs ${VALUE} REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    pbuilder_add_ext_libraries(${PYTHON_LIBRARIES})
endmacro()

# macro to add python library
macro( python_package_add_module VALUE )
    get_filename_component(this_py_lib ${VALUE} NAME_WE)
    add_library(${this_py_lib} MODULE ${VALUE})
    get_property(THIS_PACKAGE_LIBS GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES)
    target_link_libraries(${this_py_lib} ${EXTERNAL_LIBRARIES} ${THIS_PACKAGE_LIBS})

    set_target_properties(${this_py_lib} PROPERTIES PREFIX "")
    #####################################
    # TODO, this should probably go into lib/python2.7/<...>
    install( TARGETS ${this_py_lib} EXPORT ${PROJECT_NAME}Targets DESTINATION ${LIB_INSTALL_DIR} )
    set_target_properties( ${this_py_lib} PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR} )
endmacro()
