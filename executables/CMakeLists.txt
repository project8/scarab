# CMakeLists.txt for scarab/executables
# Author: N. Oblath
# Created: Jul 13, 2020

##########

set( lib_dependencies 
    Scarab
)

set( sources
    scarab-parse.cc
)

set( programs )
pbuilder_executables( 
    SOURCES ${sources}
    TARGETS_VAR programs
    PROJECT_LIBRARIES ${lib_dependencies}
    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
    PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_component_install_and_export( 
    COMPONENT Executables
    EXETARGETS ${programs}
)
