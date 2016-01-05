# CMakeLists.txt for Scarab/library/utility
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/scarab_api.hh
    ${dir}/error.hh
    ${dir}/mutex.hh
    ${dir}/time.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/error.cc
    ${dir}/mutex.cc
    ${dir}/time.cc
    PARENT_SCOPE )
