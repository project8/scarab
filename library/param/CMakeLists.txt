# CMakeLists.txt for Scarab/library/param
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/param.hh
    ${dir}/param_array.hh
    ${dir}/param_base.hh
    ${dir}/param_base_impl.hh
    ${dir}/param_codec.hh
    ${dir}/param_env_modifier.hh
    ${dir}/param_fwd.hh
    ${dir}/param_helpers.hh
    ${dir}/param_helpers_impl.hh
    ${dir}/param_modifier.hh
    ${dir}/param_node.hh
    ${dir}/param_value.hh
    ${dir}/param_visitor.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/param_array.cc
    ${dir}/param_base.cc
    ${dir}/param_codec.cc
    ${dir}/param_env_modifier.cc
    ${dir}/param_helpers.cc
    ${dir}/param_modifier.cc
    ${dir}/param_node.cc
    ${dir}/param_value.cc
    ${dir}/param_visitor.cc
    PARENT_SCOPE )
