# CMakeLists.txt for Scarab/library/param
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/configurator.hh
    ${dir}/global_config.hh
    ${dir}/param.hh
    ${dir}/parsable.hh
    ${dir}/parser.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/configurator.cc
    ${dir}/global_config.cc
    ${dir}/param.cc
    ${dir}/parsable.cc
    ${dir}/parser.cc
    PARENT_SCOPE )
