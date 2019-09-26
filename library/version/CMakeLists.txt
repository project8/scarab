# CMakeLists.txt for Scarab/library/version
# Author: N.S. Oblath
# Created: Sep 26, 2019

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/scarab_version.hh
    ${dir}/version_ifc.hh
    ${dir}/version_semantic.hh
    ${dir}/version_wrapper.hh
    PARENT_SCOPE 
)

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/version_ifc.cc
    ${dir}/version_semantic.cc
) # not PARENT_SCOPE so that we can set it to that scope in the next set command

configure_file( ${dir}/scarab_version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/scarab_version.cc )
set( Scarab_SOURCES
    ${Scarab_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/scarab_version.cc
    PARENT_SCOPE )
    