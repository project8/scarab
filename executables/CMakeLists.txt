# CMakeLists.txt for scarab/executables
# Author: N. Oblath
# Created: Jul 13, 2020

##########

set( lib_dependencies 
    Scarab
)
set( programs
    scarab-parse
)
pbuilder_executables( programs lib_dependencies PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )
