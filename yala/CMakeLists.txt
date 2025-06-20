# CMakeLists.txt for Scarab/testing/applications
# Author: N. Oblath
# Created: Jan 4, 2016


include_directories( BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

pbuilder_executable( 
        SOURCES yala.cc test_class.cc test_yala.cc
        EXECUTABLE test_yala
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_executable( 
        SOURCES yala.cc test_class.cc test_yala_static.cc
        EXECUTABLE test_yala_static
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
)

pbuilder_component_install_and_export( 
    COMPONENT Executables
    EXETARGETS test_yala test_yala_static
)
