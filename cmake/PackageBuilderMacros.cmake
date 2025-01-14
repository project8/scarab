# PackageBuilderMacros.cmake
# Author: Noah Oblath
#
# Convenient macros for PackageBuilder builds
#
# Requires: CMake v3.12 or better (FindPython3)

# Convenience function for overriding the value of an option (aka a cached bool variable)
macro( set_option VARIABLE VALUE )
    set( ${VARIABLE} ${VALUE} CACHE BOOL "" FORCE )
endmacro()

# Convenience function for setting a variable to the maximum between itself and another value
function( set_to_max VARIABLE VALUE )
    if( ${VALUE} GREATER ${${VARIABLE}} )
        set( ${VARIABLE} ${VALUE} )
    endif()
endfunction()

# This should be called after setting options, and before adding submodules and building the project
macro( pbuilder_prepare_project )

    # Deprecated C++ version options
    if( DEFINED USE_CPP17 )
        message( AUTHOR_WARNING "USE_CPP%% variables are deprecated as of Scarab v3.4; Please set CMAKE_CXX_STANDARD directly" )
        set( CMAKE_CXX_STANDARD 17 )
    elseif( DEFINED USE_CPP14 )
        message( AUTHOR_WARNING "USE_CPP%% variables are deprecated as of Scarab v3.4; Please set CMAKE_CXX_STANDARD directly" )
        set( CMAKE_CXX_STANDARD 14 )
    elseif( DEFINED USE_CPP11 )
        message( AUTHOR_WARNING "USE_CPP%% variables are deprecated as of Scarab v3.4; Please set CMAKE_CXX_STANDARD directly" )
        set( CMAKE_CXX_STANDARD 11 )
    endif()

    # Add/remove C++ version macros
    if( CMAKE_CXX_STANDARD GREATER 20 AND NOT CMAKE_CXX_STANDARD EQUAL 98 )
        add_definitions( -DUSE_CPP20 -DUSE_CPP17 -DUSE_CPP14 -DUSE_CPP11 )
        message( AUTHOR_WARNING "PackageBuilder does not currently include \"USE_CPP%%\" macros for higher that C++20" )
    elseif( CMAKE_CXX_STANDARD EQUAL 20 )
        add_definitions( -DUSE_CPP20 -DUSE_CPP17 -DUSE_CPP14 -DUSE_CPP11 )
    elseif( CMAKE_CXX_STANDARD EQUAL 17 )
        remove_definitions( -DUSE_CPP20 )
        add_definitions( -DUSE_CPP17 -DUSE_CPP14 -DUSE_CPP11 )
    elseif( CMAKE_CXX_STANDARD EQUAL 14 )
        remove_definitions( -DUSE_CPP20 -DUSE_CPP17 )
        add_definitions( -DUSE_CPP14 -DUSE_CPP11 )
    elseif( CMAKE_CXX_STANDARD EQUAL 11 )
        remove_definitions( -DUSE_CPP20 -DUSE_CPP17 -DUSE_CPP14 )
        add_definitions( -DUSE_CPP11 )
    elseif( CMAKE_CXX_STANDARD EQUAL 98 )
        remove_definitions( -DUSE_CPP20 -DUSE_CPP17 -DUSE_CPP14 -DUSE_CPP11 )
    endif()


endmacro()

macro( pbuilder_add_submodule SM_NAME SM_LOCATION )
    if( NOT IS_ABSOLUTE ${SM_LOCATION} )
        set( SM_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${SM_LOCATION}" )
    endif()

    # Conditions that let us in this loop:
    #  1. Submodule SM_NAME has not yet been found
    #  2. This is the location of the submodule SM_NAME
    message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
    message( STATUS "CMAKE_CURRENT_LIST_DIR/SM_LOCATION: ${CMAKE_CURRENT_LIST_DIR}/${SM_LOCATION}" )
    if( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/${SM_LOCATION}" )
        message( "Adding submodule ${SM_NAME} in location ${CMAKE_CURRENT_LIST_DIR}/${SM_LOCATION}" )

        set( ${SM_NAME}_FOUND TRUE CACHE INTERNAL "" )
        set( ${SM_NAME}_LOCATION ${CMAKE_CURRENT_LIST_DIR}/${SM_LOCATION} CACHE INTERNAL "" )
        set( ${SM_NAME}_BINARY_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${SM_LOCATION} CACHE INTERNAL "" )

        # Determine the library name suffix for this submodule with respect to its parent if it's not already defined
        if( NOT DEFINED ${SM_NAME}_PARENT_LIB_NAME_SUFFIX )
            set( ${SM_NAME}_PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}${${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX}" CACHE INTERNAL "Scoped library name suffix for submodule ${SM_NAME}" )
            message( STATUS "PARENT_LIB_NAME_SUFFIX being set for SM ${SM_NAME}: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}" )
        endif()

        # Set submodule include subdirectory
        set( SM_INCLUDE_SUBDIR "/${SM_NAME}" )
        message( STATUS "Include files for submodule ${SM_NAME} will be installed in ${TOP_PROJECT_INCLUDE_INSTALL_DIR}${SM_INCLUDE_SUBDIR}" )

        # Set CMake config subdirectory
        set( ${SM_NAME}_CMAKE_CONFIG_DIR "${PACKAGE_CONFIG_PREFIX}/${SM_NAME}" CACHE INTERNAL "${SM_NAME} CMake config installation path" )
    
        message( STATUS "SM ${SM_NAME} cached variables:" )
        message( STATUS "${SM_NAME}_FOUND: ${${SM_NAME}_FOUND}" )
        message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
        message( STATUS "${SM_NAME}_BINARY_LOCATION: ${${SM_NAME}_BINARY_LOCATION}" )
        message( STATUS "${SM_NAME}_PARENT_LIB_NAME_SUFFIX: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}")
        message( STATUS "${SM_NAME}_CMAKE_CONFIG_DIR: ${${SM_NAME}_CMAKE_CONFIG_DIR}" )

        message( STATUS "Proceeding into subdirectory: ${SM_LOCATION}" )
        add_subdirectory( ${SM_LOCATION} )

        if( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${SM_LOCATION}/${SM_NAME}Config.cmake )
            message( STATUS "Loading config file for submodule ${SM_NAME}" )
            include( ${CMAKE_CURRENT_BINARY_DIR}/${SM_LOCATION}/${SM_NAME}Config.cmake )
        else()
            message( STATUS "No config file present for submodule ${SM_NAME} (${CMAKE_CURRENT_BINARY_DIR}/${SM_LOCATION}/${SM_NAME}Config.cmake)" )
        endif()

        # Need to unset the submodule subdirectories since we're in a macro
        unset( SM_INCLUDE_SUBDIR )
        unset( SM_CMAKE_CONFIG_SUBDIR )

    endif( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${CMAKE_CURRENT_LIST_DIR}/${SM_LOCATION}" )
endmacro()

macro( pbuilder_get_link_options VAR )
    # Fill a variable with link options that should be used for libraries and executables

    if( LINUX )
        if( OVERRIDE_LIB_PATH )
            set( ${VAR} "LINKER:--disable-new-dtags" )
        else( OVERRIDE_LIB_PATH )
            set( ${VAR} "LINKER:--enable-new-dtags" )
        endif( OVERRIDE_LIB_PATH )
    endif()
endmacro()

macro( pbuilder_expand_lib_name_2 LIB_NAME SM_NAME )
    # Output is in the form of the variable FULL_LIB_NAME
    set( FULL_LIB_NAME "${LIB_NAME}${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}" )
endmacro()

macro( pbuilder_expand_lib_name LIB_NAME )
    # Output is in the form of the variable FULL_LIB_NAME
    pbuilder_expand_lib_name_2( ${LIB_NAME} ${PROJECT_NAME} )
endmacro()

macro( pbuilder_expand_lib_names )
    # Supply library targets as additional macro arguments
    # Output is in the form of the variable FULL_LIB_NAMES
    set( LIB_NAMES ${ARGN} )
    #message( STATUS "expanding these project libraries: ${LIB_NAMES}" )
    set( FULL_LIB_NAMES )
    foreach( lib ${LIB_NAMES} )
        pbuilder_expand_lib_name( ${lib} )
        list( APPEND FULL_LIB_NAMES ${FULL_LIB_NAME} )
    endforeach( lib )
    message( STATUS "expanded to full project library names: ${FULL_LIB_NAMES}" )
endmacro()

macro( pbuilder_use_sm_library LIB_NAME SM_NAME )
    pbuilder_expand_lib_name_2( ${LIB_NAME} ${SM_NAME} )
    list( APPEND ${PROJECT_NAME}_SM_LIBRARIES ${FULL_LIB_NAME} )
    #message( STATUS "Added SM library ${FULL_LIB_NAME} to ${PROJECT_NAME}_SM_LIBRARIES" )
endmacro()

function( pbuilder_library )
    # Builds a shared-object library
    #
    # Combines pbuilder_add_library() and pbuilder_link_library()
    #
    # Parameters:
    #     TARGET: library target name
    #     SOURCES: source files to include
    #     PROJECT_LIBRARIES: libraries from the same project to be linked against
    #     PUBLIC_EXTERNAL_LIBRARIES: public external libraries to be linked against
    #     PRIVATE_ETERNAL_LIBRARIES: private external libraries to be linked against
    #     COMPILE_DEFINITIONS: compile definitions to add to this target

    set( OPTIONS )
    set( ONEVALUEARGS TARGET )
    set( MULTIVALUEARGS SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES COMPILE_DEFINITIONS )
    cmake_parse_arguments( LIB "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Building library ${LIB_TARGET}" )
    message( STATUS "${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX is ${${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX}" )

    pbuilder_add_library(
        TARGET ${LIB_TARGET}
        SOURCES ${LIB_SOURCES}
    )

    pbuilder_link_library(
        TARGET ${LIB_TARGET}
        PROJECT_LIBRARIES ${LIB_PROJECT_LIBRARIES}
        PUBLIC_EXTERNAL_LIBRARIES ${LIB_PUBLIC_EXTERNAL_LIBRARIES}
        PRIVATE_EXTERNAL_LIBRARIES ${LIB_PRIVATE_EXTERNAL_LIBRARIES}
        COMPILE_DEFINITIONS ${LIB_COMPILE_DEFINITIONS}
    )

endfunction()

function( pbuilder_add_library )
    # Adding a shared-object library
    #
    # Parameters:
    #     TARGET: library target name
    #     SOURCES: source files to include

    set( OPTIONS )
    set( ONEVALUEARGS TARGET )
    set( MULTIVALUEARGS SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES COMPILE_DEFINITIONS )
    cmake_parse_arguments( LIB "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Adding library ${LIB_TARGET}" )

    pbuilder_expand_lib_name( ${LIB_TARGET} )
    set( FULL_LIB_TARGET ${FULL_LIB_NAME} )
    set( ${LIB_TARGET}_FULL_TARGET_NAME  ${FULL_LIB_NAME} CACHE INTERNAL "Full target name for library ${LIB_TARGET}" )
    message( STATUS "lib target: ${LIB_TARGET}" )
    message( STATUS "full lib target: ${FULL_LIB_TARGET}" )
    message( STATUS "internal cache ${LIB_TARGET}_FULL_TARGET_NAME: ${${LIB_TARGET}_FULL_TARGET_NAME}" )
    message( STATUS "SM library dependencies (public): ${${PROJECT_NAME}_SM_LIBRARIES}" )
    message( STATUS "external library dependencies (public): ${LIB_PUBLIC_EXTERNAL_LIBRARIES}" )
    message( STATUS "external library dependencies (private): ${LIB_PRIVATE_EXTERNAL_LIBRARIES}" )

    message( STATUS "pbuilder: will build library <${FULL_LIB_TARGET}>" )
    add_library( ${FULL_LIB_TARGET} ${LIB_SOURCES} )

    # Set the version of the library
    set_target_properties( ${FULL_LIB_TARGET} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION} )

    # Grab the include directories, which will be used for the build-interface target includes
    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${FULL_LIB_TARGET} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )
    
    # this will set the INTERFACE_INCLUDE_DIRECTORIES property using the INTERFACE option
    # it's assumed that the include_directories() command was used to set the INCLUDE_DIRECTORIES property for the private side.
    target_include_directories( ${FULL_LIB_TARGET} 
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}>"
    )


endfunction()

function( pbuilder_link_library )
    # Linking a shared-object library
    #
    # Parameters:
    #     TARGET: library target name
    #     PROJECT_LIBRARIES: libraries from the same project to be linked against
    #     PUBLIC_EXTERNAL_LIBRARIES: public external libraries to be linked against
    #     PRIVATE_ETERNAL_LIBRARIES: private external libraries to be linked against
    #     COMPILE_DEFINITIONS: compile definitions to add to this target

    set( OPTIONS )
    set( ONEVALUEARGS TARGET )
    set( MULTIVALUEARGS SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES COMPILE_DEFINITIONS )
    cmake_parse_arguments( LIB "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Linking library ${LIB_TARGET}" )

    pbuilder_expand_lib_name( ${LIB_TARGET} )
    set( FULL_LIB_TARGET ${FULL_LIB_NAME} )
    pbuilder_get_link_options( LINK_OPTIONS )

    pbuilder_expand_lib_names( ${LIB_PROJECT_LIBRARIES} )
    set( FULL_PROJECT_LIBRARIES ${FULL_LIB_NAMES} )
    message( STATUS "full project library dependencies (lib): ${FULL_PROJECT_LIBRARIES}" )

    target_link_options( ${FULL_LIB_TARGET} PUBLIC ${LINK_OPTIONS} )
    target_link_libraries( ${FULL_LIB_TARGET} 
        PUBLIC
            ${FULL_PROJECT_LIBRARIES} ${${PROJECT_NAME}_SM_LIBRARIES} ${LIB_PUBLIC_EXTERNAL_LIBRARIES}
        PRIVATE
            ${LIB_PRIVATE_EXTERNAL_LIBRARIES} 
    )

    # Add target compile definitions
    if( LIB_COMPILE_DEFINITIONS )
        target_compile_definitions( ${FULL_LIB_TARGET}
            PUBLIC
                ${LIB_COMPILE_DEFINITIONS}
        )
    endif()

endfunction()

function( pbuilder_executables )
    # Builds multiple executables, assuming one executable per source file
    # Strips extensino from source file and uses that as the executable name (target and file)
    #
    # Parameters
    #     TARGETS_VAR (output, optional): variable in which to store the names of the executables created
    #     SOURCES: source files to be built
    #     PROJECT_LIBRARIES: libraries from the same project to be linked against
    #     PUBLIC_EXTERNAL_LIBRARIES: public external libraries to be linked against
    #     PRIVATE_ETERNAL_LIBRARIES: private external libraries to be linked against


    set( OPTIONS )
    set( ONEVALUEARGS TARGETS_VAR )
    set( MULTIVALUEARGS SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES )
    cmake_parse_arguments( EXES "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Building multiple executables" )
    message( STATUS "executable source files: ${EXES_SOURCES}" )
    message( STATUS "project library dependencies: ${EXES_PROJECT_LIBRARIES}" )
    message( STATUS "submodule library dependencies (public): ${${PROJECT_NAME}_SM_LIBRARIES}" )
    message( STATUS "external library dependencies (public): ${EXES_PUBLIC_EXTERNAL_LIBRARIES}" )
    message( STATUS "external library dependencies (private): ${EXES_PRIVATE_EXTERNAL_LIBRARIES}" )

    set( targets )

    foreach( source ${EXES_SOURCES} )
        get_filename_component( program ${source} NAME_WE )
        set( target "${program}_exe" )
        if( NOT TARGET ${target} )
            pbuilder_executable( EXECUTABLE ${target} 
                SOURCES ${source} 
                PROJECT_LIBRARIES ${EXES_PROJECT_LIBRARIES} 
                PUBLIC_EXTERNAL_LIBRARIES ${EXES_PUBLIC_EXTERNAL_LIBRARIES} 
                PRIVATE_EXTERNAL_LIBRARIES ${EXES_PRIVATE_EXTERNAL_LIBRARIES} 
            )
            # set the output name to ${program} so it doesn't have the _exe appended
            set_target_properties( ${target} PROPERTIES OUTPUT_NAME ${program} )
            list( APPEND targets ${target} )
        else()
            message( FATAL "Duplicate target: ${program}" )
        endif()
    endforeach( source )

    message( STATUS "Targets produced: ${targets}" )
    if( EXES_TARGETS_VAR )
        message( STATUS "output var for targets: ${EXES_TARGETS_VAR}" )
        set( ${EXES_TARGETS_VAR} ${targets} PARENT_SCOPE )
    endif()

endfunction()

function( pbuilder_executable )
    # Builds a single executable from one or more source files
    #
    # Parameters
    #     EXECUTABLE: executable target
    #     SOURCES: list of sources to be built into one executable
    #     PROJECT_LIBRARIES: libraries from the same project to be linked against
    #     PUBLIC_EXTERNAL_LIBRARIES: public external libraries to be linked against
    #     PRIVATE_ETERNAL_LIBRARIES: private external libraries to be linked against

    set( OPTIONS )
    set( ONEVALUEARGS EXECUTABLE )
    set( MULTIVALUEARGS SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES )
    cmake_parse_arguments( EXE "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Building executable ${EXE_EXECUTABLE} with ${EXE_SOURCES}" )
    message( STATUS "\tlinking with: (project) ${EXE_PROJECT_LIBRARIES} -- (submodule) ${${PROJECT_NAME}_SM_LIBRARIES} --  (public ext) ${EXE_PUBLIC_EXTERNAL_LIBRARIES} -- (private ext) ${EXE_PRIVATE_EXTERNAL_LIBRARIES}")

    add_executable( ${EXE_EXECUTABLE} ${EXE_SOURCES} )

    pbuilder_expand_lib_names( ${EXE_PROJECT_LIBRARIES} )
    set( FULL_PROJECT_LIBRARIES ${FULL_LIB_NAMES} )
    message( STATUS "full project library dependencies (exe): ${FULL_PROJECT_LIBRARIES}" )
    pbuilder_get_link_options( LINK_OPTIONS )

    target_link_options( ${EXE_EXECUTABLE} PUBLIC ${LINK_OPTIONS} )
    target_link_libraries( ${EXE_EXECUTABLE} 
        PUBLIC
            ${FULL_PROJECT_LIBRARIES} ${${PROJECT_NAME}_SM_LIBRARIES} ${EXE_PUBLIC_EXTERNAL_LIBRARIES}
        PRIVATE
            ${EXE_PRIVATE_EXTERNAL_LIBRARIES} 
    )

    # Grab the include directories, which will be used for the build-interface target includes
    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${EXE_EXECUTABLE} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )

    # this will set the INTERFACE_INCLUDE_DIRECTORIES property using the INTERFACE option
    # it's assumed that the include_directories() command was used to set the INCLUDE_DIRECTORIES property for the private side.
    target_include_directories( ${EXE_EXECUTABLE} 
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}>"
    )
    
endfunction()

function( pbuilder_component_install_and_export )
    # This function installs library and executable targets, and makes sure they're exported correctly.
    #
    # Parameters:
    #     COMPONENT: build component being installed; required if this function is used multiple times in a project
    #                if a build does not actually have multiple components, a name still needs to be given (e.g. "library")
    #     NAMESPACE: optional namespace specification to add to the library on export, with colons included
    #     LIBTARGETS: all library targets to be installed
    #     EXETARGETS: all executable targets to be installed

    set( OPTIONS )
    set( ONEVALUEARGS COMPONENT NAMESPACE )
    set( MULTIVALUEARGS LIBTARGETS EXETARGETS )
    cmake_parse_arguments( CIE "${OPTIONS}" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    if( CIE_COMPONENT )
        set( INSERT_COMPONENT "_${CIE_COMPONENT}" )
    endif()

    if( CIE_NAMESPACE )
        message( "Doing installation of targets with namespace ${CIE_NAMESPACE}" )
    else()
        message( "Doing installation of targets with no namespace" )
    endif()
    message( "Targets are: (libs) ${CIE_LIBTARGETS} and (exes) ${CIE_EXETARGETS}" )

    # Libraries
    if( CIE_LIBTARGETS )
        message( "Installing and exporting libraries for component <${CIE_COMPONENT}>" )
        message( STATUS "Library targets are: ${CIE_LIBTARGETS}" )

        # expand library names
        pbuilder_expand_lib_names( ${CIE_LIBTARGETS} )
        set( FULL_LIBTARGETS ${FULL_LIB_NAMES} )
        message( STATUS "Expanded lib names: ${FULL_LIBTARGETS}" )


        # export to make targets available at build time
        if( CIE_NAMESPACE )
            export( TARGETS ${FULL_LIBTARGETS}
                NAMESPACE ${CIE_NAMESPACE}
                APPEND
                FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake
            )
        else()
            export( TARGETS ${FULL_LIBTARGETS}
                APPEND
                FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake
            )
        endif()

        # install libraries and add to export for installation
        install( TARGETS ${FULL_LIBTARGETS} 
            EXPORT ${PROJECT_NAME}${INSERT_COMPONENT}_Targets
            COMPONENT ${CIE_COMPONENT}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        )
    endif()

    # Executables
    if( CIE_EXETARGETS )
        message( "Installing and exporting executables for component <${CIE_COMPONENT}>" )
        message( STATUS "Executable targets are: ${CIE_EXETARGETS}" )
        
        # make targets available at build time
        #message( STATUS "******* build-time exe targets file: ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake" )
        if( CIE_NAMESPACE )
            export( TARGETS ${CIE_EXETARGETS}
                NAMESPACE ${CIE_NAMESPACE}
                APPEND
                FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake
            )
        else()
            export( TARGETS ${CIE_EXETARGETS}
                APPEND
                FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake
            )
        endif()

        # install executables and add to export for installation
        #message( STATUS "******* installed exe targets export: ${PROJECT_NAME}${INSERT_COMPONENT}_Targets")
        install( TARGETS ${CIE_EXETARGETS}
            EXPORT ${PROJECT_NAME}${INSERT_COMPONENT}_Targets
            COMPONENT ${CIE_COMPONENT}
            RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        )

    endif()

    # Export installation
    message( "Installing export ${PROJECT_NAME}${INSERT_COMPONENT}_Targets" )
    message( STATUS "Output file will be ${${PROJECT_NAME}_CMAKE_CONFIG_DIR}/${PROJECT_NAME}${INSERT_COMPONENT}_Targets.cmake" )
    install( EXPORT ${PROJECT_NAME}${INSERT_COMPONENT}_Targets
        NAMESPACE
            ${PROJECT_NAME}::
        DESTINATION
            ${${PROJECT_NAME}_CMAKE_CONFIG_DIR}
    )


endfunction()

macro( pbuilder_install_headers )
    #message( STATUS "installing headers in ${INCLUDE_INSTALL_DIR}${${PROJECT_NAME}_PARENT_INC_DIR_PATH}" )
    install( FILES ${ARGN} 
        DESTINATION ${TOP_PROJECT_INCLUDE_INSTALL_DIR}${SM_INCLUDE_SUBDIR}
    )
endmacro()

macro( pbuilder_install_header_dirs FILE_PATTERN )
    install( DIRECTORY ${ARGN} 
        DESTINATION ${INCLUDE_INSTALL_DIR}${${PROJECT_NAME}_PARENT_INC_DIR_PATH} 
        FILES_MATCHING PATTERN "${FILE_PATTERN}" 
    )
endmacro()

macro( pbuilder_install_config )
    install( FILES ${ARGN} 
        DESTINATION ${CONFIG_INSTALL_DIR} 
    )
endmacro()

macro( pbuilder_install_data )
    install( FILES ${ARGN} 
        DESTINATION ${DATA_INSTALL_DIR} 
    )
endmacro()

macro( pbuilder_install_files DEST_DIR )
    install( FILES ${ARGN} 
        DESTINATION ${DEST_DIR} 
    )
endmacro()

function( pbuilder_do_package_config )
    # This macro sets up and installs the configuration files used tospecify the install information for the project.
    # It installs an already-created "config" file.
    # It creates and installs the "version-config" and "targets" files.
    # Arguments: 
    #   
    #   INPUT_FILE -- Path of the input config file (typically ends in .cmake.in).  Absolute path or relative to the current source directory.
    #   OUTPUT_FILE -- Filename for the output config file (no file path)
    #   VERSION_FILENAME -- Optional specification of the full version-config filename.  If specified, overrules the default described above.

    # Parse macro arguments
    set( oneValueArgs INPUT_FILE OUTPUT_FILE VERSION_FILENAME )
    cmake_parse_arguments( PKG_CONF "" "${oneValueArgs}" "" ${ARGN} )

    # Handle arguments and apply defaults

    if( NOT PKG_CONF_INPUT_FILE )
        set( PKG_CONF_INPUT_FILE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in )
    endif()
    message( STATUS "Config input file: ${PKG_CONF_INPUT_FILE}")

    if( NOT PKG_CONF_OUTPUT_FILE )
        set( PKG_CONF_OUTPUT_FILE ${PROJECT_NAME}Config.cmake )
    endif()
    message( STATUS "Config output file: ${PKG_CONF_OUTPUT_FILE}")

    if( NOT PKG_CONF_VERSION_FILENAME )
        set( PKG_CONF_VERSION_FILENAME ${PROJECT_NAME}ConfigVersion.cmake )
    endif()
    set( CONFIG_VERSION_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_VERSION_FILENAME} )
    message( STATUS "Config version file path: ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_VERSION_FILENAME}" )

    # Config file must exist already
    if( NOT EXISTS ${PKG_CONF_INPUT_FILE} )
        message( FATAL_ERROR "Package config file does not exist: ${PKG_CONF_INPUT_FILE}" )
    endif()

    include( CMakePackageConfigHelpers )
    configure_package_config_file( ${PKG_CONF_INPUT_FILE} ${PKG_CONF_OUTPUT_FILE} 
        INSTALL_DESTINATION ${PACKAGE_CONFIG_PREFIX}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_VERSION_FILENAME}
        COMPATIBILITY SameMajorVersion
    )

    install( 
        FILES 
            ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_VERSION_FILENAME}
            ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_OUTPUT_FILE}
        DESTINATION 
            ${${PROJECT_NAME}_CMAKE_CONFIG_DIR}
    )

endfunction()

function( pbuilder_add_pybind11_module )
    # Adds a pybind11 module that is linked to the specified project libraries, PUBLIC_EXT_LIBS, and PRIVATE_EXT_LIBS
    # Installs the library in the standard lib directory unless indicated by the definition of the variable PBUILDER_PY_INSTALL_IN_SITELIB
    #
    # Parameters
    #     MODULE_NAME: name of the Python module to be built
    #     SOURCE_FILES: list of sources to be built into one module
    #     PROJECT_LIBRARIES: libraries from the same project to be linked against
    #     PUBLIC_EXTERNAL_LIBRARIES: public external libraries to be linked against
    #     PRIVATE_ETERNAL_LIBRARIES: private external libraries to be linked against

    set( ONEVALUEARGS MODULE_NAME )
    set( MULTIVALUEARGS SOURCE_FILES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_ETERNAL_LIBRARIES )
    cmake_parse_arguments( PB11 "" "${ONEVALUEARGS}" "${MULTIVALUEARGS}" ${ARGN} )

    message( "Doing python binding for module ${PB11_MODULE_NAME}")

    pbuilder_expand_lib_names( ${PB11_PROJECT_LIBRARIES} )
    set( FULL_PROJECT_LIBRARIES ${FULL_LIB_NAMES} )
    message( STATUS "full project libraries (pybind11): ${FULL_PROJECT_LIBRARIES}" )
    message( STATUS "submodule libraries (pybind11): ${${PROJECT_NAME}_SM_LIBRARIES}" )
    message( STATUS "source files: ${PB11_SOURCE_FILES}" )

    # Potential point of confusion: the C++ library is "Scarab" and the python library is "scarab"
    # Other possible naming schemes seemed less desirable, and we'll hopefully avoid confusion with these comments
    pybind11_add_module( ${PB11_MODULE_NAME} ${PB11_SOURCE_FILES} )

    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${PB11_MODULE_NAME} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${INCLUDE_INSTALL_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )

    target_include_directories( ${PB11_MODULE_NAME}
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_SUBDIR}>"
    )

    target_link_libraries( ${PB11_MODULE_NAME} 
        PUBLIC
            ${FULL_PROJECT_LIBRARIES} ${${PROJECT_NAME}_SM_LIBRARIES} ${PB11_PUBLIC_EXTERNAL_LIBRARIES}
        PRIVATE
            ${PB11_PRIVATE_EXTERNAL_LIBRARIES} 
    )

    set( PY_MODULE_INSTALL_DIR ${LIB_INSTALL_DIR} )
    # Override that install location if specified by the user
    if( DEFINED PBUILDER_PY_INSTALL_IN_SITELIB AND DEFINED Python3_SITELIB )
        set( PY_MODULE_INSTALL_DIR ${Python3_SITELIB} )
    endif( DEFINED PBUILDER_PY_INSTALL_IN_SITELIB AND DEFINED Python3_SITELIB )
    message( STATUS "Installing module ${PY_MODULE_NAME} in ${PY_MODULE_INSTALL_DIR}" )

    install( TARGETS ${PB11_MODULE_NAME} DESTINATION ${PY_MODULE_INSTALL_DIR} )

endfunction()
