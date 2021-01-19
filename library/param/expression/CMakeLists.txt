# CMakeLists.txt for Scarab/expression
# Author: N. Oblath
# Created: Jan 18, 2021

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/expression.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/expression.cc
    PARENT_SCOPE )
