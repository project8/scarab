# CMakeLists.txt for Scarab/library/external
# Author: N. Oblath
# Created: May 29, 2020

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/date/date.h
    ${dir}/tinyexpr/tinyexpr.h
    PARENT_SCOPE )

if( Scarab_BUILD_CLI )
    set( Scarab_HEADERS ${Scarab_HEADERS}
        ${dir}/cli11/CLI11.hpp
        PARENT_SCOPE )
endif( Scarab_BUILD_CLI )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/tinyexpr/tinyexpr.c
    PARENT_SCOPE )
