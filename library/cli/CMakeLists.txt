# CMakeLists.txt for Scarab/library/param
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/application.hh
    ${dir}/nonoption_parser.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/application.cc
    ${dir}/nonoption_parser.cc
    PARENT_SCOPE )
