# CMakeLists.txt for Scarab/library
# Author: N. Oblath
# Created: Jan 4, 2016


include_directories( BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/utility
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/version
)

##########

set( Scarab_HEADERS )
set( Scarab_SOURCES )

include_directories( BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}/utility 
    ${CMAKE_CURRENT_BINARY_DIR}/version 
)
add_subdirectory( utility )
add_subdirectory( version )
add_subdirectory( logger )

if( Scarab_BUILD_PARAM )
    include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/param )
    add_subdirectory( param )
endif( Scarab_BUILD_PARAM )

if( Scarab_BUILD_CODEC_JSON )
    include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/json )
    add_subdirectory( param/codec/json )
endif( Scarab_BUILD_CODEC_JSON )

if( Scarab_BUILD_CODEC_YAML )
    include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/param/codec/yaml )
    add_subdirectory( param/codec/yaml )
endif( Scarab_BUILD_CODEC_YAML )

if( Scarab_BUILD_AUTHENTICATION )
    include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/authentication )
    add_subdirectory( authentication )
endif( Scarab_BUILD_AUTHENTICATION )

if( Scarab_BUILD_CLI )
    include_directories( BEFORE 
        ${CMAKE_CURRENT_SOURCE_DIR}/cli
    )
    add_subdirectory( cli )
endif( Scarab_BUILD_CLI )

##########

set( PACKAGE_LIBS )

pbuilder_library( Scarab Scarab_SOURCES PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

pbuilder_install_headers( ${Scarab_HEADERS} )
