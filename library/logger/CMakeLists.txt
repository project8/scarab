# CMakeLists.txt for Scarab/library/logger
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/local_logger.hh
    ${dir}/logger.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/local_logger.cc
    ${dir}/logger.cc
    PARENT_SCOPE )
