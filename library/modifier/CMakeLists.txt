# CMakeLists.txt for Scarab/library/modifier
# Author: N. Buzinsky
# Created: Feb 12, 2020

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/modifier.hh
    ${dir}/null_modifier.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/modifier.cc
    ${dir}/null_modifier.cc
    PARENT_SCOPE )
