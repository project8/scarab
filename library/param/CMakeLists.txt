# CMakeLists.txt for Scarab/library/param
# Author: N. Oblath
# Created: Jan 1, 2016

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( Scarab_HEADERS ${Scarab_HEADERS}
    ${dir}/configurator.hh
    ${dir}/global_config.hh
    ${dir}/param.hh
    ${dir}/param_array.hh
    ${dir}/param_base.hh
    ${dir}/param_base_impl.hh
    ${dir}/param_codec.hh
    ${dir}/param_node.hh
    ${dir}/param_value.hh
    ${dir}/parsable.hh
    ${dir}/parser.hh
    PARENT_SCOPE )

set( Scarab_SOURCES ${Scarab_SOURCES}
    ${dir}/configurator.cc
    ${dir}/global_config.cc
    ${dir}/param.cc
    ${dir}/param_codec.cc
    ${dir}/parsable.cc
    ${dir}/parser.cc
    PARENT_SCOPE )
