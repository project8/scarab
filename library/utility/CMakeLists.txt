# CMakeLists.txt for Scarab/library/utility
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/base_exception.hh
    ${dir}/cancelable.hh
    ${dir}/concurrent_queue.hh
    ${dir}/destroyer.hh
    ${dir}/digital.hh
    ${dir}/error.hh
    ${dir}/exponential_backoff.hh
    ${dir}/factory.hh
    ${dir}/indexed_factory.hh
    ${dir}/macros.hh
    ${dir}/map_at_default.hh
    ${dir}/member_variables.hh
    ${dir}/path.hh
    ${dir}/scarab_api.hh
    ${dir}/signal_handler.hh
    ${dir}/singleton.hh
    ${dir}/thread_singleton.hh
    ${dir}/time.hh
    ${dir}/typename.hh
    ${dir}/unique_typelist.hh
) # not PARENT_SCOPE so that we can set it to that scope in the set command below

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/cancelable.cc
    ${dir}/digital.cc
    ${dir}/path.cc
    ${dir}/signal_handler.cc
    ${dir}/time.cc
    ${dir}/typename.cc
    PARENT_SCOPE 
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
