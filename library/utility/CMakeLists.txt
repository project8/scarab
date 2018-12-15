# CMakeLists.txt for Scarab/library/utility
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/cancelable.hh
    ${dir}/concurrent_queue.hh
    ${dir}/destroyer.hh
    ${dir}/digital.hh
    ${dir}/error.hh
    ${dir}/factory.hh
    ${dir}/macros.hh
    ${dir}/member_variables.hh
    ${dir}/path.hh
    ${dir}/scarab_api.hh
    ${dir}/scarab_version.hh
    ${dir}/signal_handler.hh
    ${dir}/singleton.hh
    ${dir}/terminate.hh
    ${dir}/time.hh
    ${dir}/typename.hh
    ${dir}/unique_typelist.hh
    ${dir}/version_base.hh
    ${dir}/version_wrapper.hh
)

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/cancelable.cc
    ${dir}/digital.cc
    ${dir}/error.cc
    ${dir}/path.cc
    ${dir}/signal_handler.cc
    ${dir}/terminate.cc
    ${dir}/time.cc
    ${dir}/typename.cc
    ${dir}/version_base.cc
)

set( macro_prefix camel_case_)
set( set_prefix Set )
set( get_prefix Get )
set( var_prefix f )
set( static_prefix s )
configure_file( _member_variables.hh.in _camel_case_member_variables.hh @ONLY )    
        
set( macro_prefix snake_case_)
set( set_prefix set_ )
set( get_prefix get_ )
set( var_prefix f_ )
set( static_prefix s_ )
configure_file( _member_variables.hh.in _snake_case_member_variables.hh @ONLY )

set( Scarab_HEADERS
    ${Scarab_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/_camel_case_member_variables.hh
    ${CMAKE_CURRENT_BINARY_DIR}/_snake_case_member_variables.hh
    PARENT_SCOPE )    


configure_file( ${dir}/scarab_version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/scarab_version.cc )
set( Scarab_SOURCES
    ${Scarab_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/scarab_version.cc
    PARENT_SCOPE )
    