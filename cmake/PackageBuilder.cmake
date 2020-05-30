# PackageBuilder.cmake
# Author: Noah Oblath
# Parts of this script are based on work done by Sebastian Voecking and Marco Haag in the Kasper package
# Convenient macros and default variable settings for the Nymph-based build.
#
# Requires: CMake v3.1 or better (CMAKE_CXX_STANDARD)

# CMake policies
cmake_policy( SET CMP0011 NEW )
cmake_policy( SET CMP0012 NEW ) # how if-statements work
cmake_policy( SET CMP0042 NEW ) # rpath on mac os x
cmake_policy( SET CMP0048 NEW ) # version in project()

include( CMakeParseArguments )

# check if this is a stand-alone build
set( PBUILDER_STANDALONE FALSE CACHE INTERNAL "Flag for whether or not this is a stand-alone build" )
set( PBUILDER_CHILD_NAME_EXTENSION "${PROJECT_NAME}" CACHE INTERNAL "Submodule library name modifier" )
if( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
    set( PBUILDER_STANDALONE TRUE )
    
    if( CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*" AND NOT CMAKE_BUILD_TYPE )
  		set( CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel" FORCE )
  		message( STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build" )
    endif()

    # option to force linking when using g++
    if( CMAKE_COMPILER_IS_GNUCXX )
        option( GCC_FORCE_LINKING "Fix linker errors with some GCC versions by adding the --no-as-needed flag" ON )
        if( GCC_FORCE_LINKING )
            set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed" )
        endif( GCC_FORCE_LINKING )
    endif( CMAKE_COMPILER_IS_GNUCXX )
endif( ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )

# define a variable pointing to the directory containing this file
set( PBUILDER_DIR ${CMAKE_CURRENT_LIST_DIR} )

# preprocessor defintion for debug build
if( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    add_definitions(-D${PROJECT_NAME}_DEBUG )
else( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )
    remove_definitions(-D${PROJECT_NAME}_DEBUG )    
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG" )

# Setup the default install prefix
# This gets set to the binary directory upon first configuring.
# If the user changes the prefix, but leaves the flag OFF, then it will remain as the user specified.
# If the user wants to reset the prefix to the default (i.e. the binary directory), then the flag should be set ON.
if( NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT )
    set( SET_INSTALL_PREFIX_TO_DEFAULT ON )
endif( NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT )
if( SET_INSTALL_PREFIX_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} CACHE PATH "Install prefix" FORCE )
    set( SET_INSTALL_PREFIX_TO_DEFAULT OFF CACHE BOOL "Reset default install path when when configuring" FORCE )
endif( SET_INSTALL_PREFIX_TO_DEFAULT )

# install subdirectories
set( INCLUDE_INSTALL_SUBDIR "include/${PROJECT_NAME}" CACHE PATH "Install subdirectory for headers" )
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( LIB_INSTALL_SUBDIR "bin" CACHE PATH "Install subdirectory for libraries" )
else( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
    set( LIB_INSTALL_SUBDIR "lib" CACHE PATH "Install subdirectory for libraries" )
endif( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
set( PACKAGE_CONFIG_SUBDIR "${LIB_INSTALL_SUBDIR}/cmake/${PROJECT_NAME}" CACHE PATH "Install subdirectory for CMake config files" )
set( BIN_INSTALL_SUBDIR "bin" CACHE PATH "Install subdirectory for binaries" )
set( CONFIG_INSTALL_SUBDIR "config" CACHE PATH "Install subdirectory for config files" )
set( DATA_INSTALL_SUBDIR "data" CACHE PATH "Install subdirectory for data files" )

set( INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_SUBDIR}" )
set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_SUBDIR}" )
set( PACKAGE_CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}/${PACKAGE_CONFIG_SUBDIR}" )
set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_SUBDIR}" )
set( CONFIG_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CONFIG_INSTALL_SUBDIR}" )
set( DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_SUBDIR}" )

#if( NOT DEFINED TOP_PROJECT_INCLUDE_INSTALL_DIR )
if( PBUILDER_STANDALONE )
    # this is for standalone builds, but has to be done down here rather than in the standalone if block above
    set( TOP_PROJECT_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}" CACHE INTERNAL "Top-project include installation path" )
    set( TOP_PROJECT_INCLUDE_INSTALL_SUBDIR "${INCLUDE_INSTALL_SUBDIR}" CACHE INTERNAL "Top-project include installation subdirectory" )
    message( STATUS "TOP_PROJECT_INCLUDE_INSTALL_DIR being set to ${TOP_PROJECT_INCLUDE_INSTALL_DIR}" )

    set( TOP_PROJECT_CMAKE_CONFIG_DIR "${PACKAGE_CONFIG_PREFIX}" CACHE INTERNAL "Top-project CMake config installation path" )
    message( STATUS "TOP_PROJECT_CMAKE_CONFIG_DIR being set to ${TOP_PROJECT_CMAKE_CONFIG_DIR}" )
endif()


# flag for building test programs
option( ${PROJECT_NAME}_ENABLE_TESTING "Turn on or off the building of test programs" OFF )

# flag for building executables (other than test programs)
# this is particularly useful if a project is used multiple times and installed in a general location, where executables would overwrite each other.
option( ${PROJECT_NAME}_ENABLE_EXECUTABLES "Turn on or off the building of executables (other than test programs)" ON )

# flag for using C++11
option( USE_CPP11 "Flag for building with C++11" ON )
option( USE_CPP14 "Flag for building with C++14" OFF )
option( USE_CPP17 "Flag for building with C++17" OFF )
if( USE_CPP17 )
    add_definitions( -DUSE_CPP17 )
    add_definitions( -DUSE_CPP14 )
    add_definitions( -DUSE_CPP11 )
    set( CMAKE_CXX_STANDARD 17 )
elseif( USE_CPP14 )
    add_definitions( -DUSE_CPP14 )
    add_definitions( -DUSE_CPP11 )
    set( CMAKE_CXX_STANDARD 14 )
elseif( USE_CPP11 )
    add_definitions( -DUSE_CPP11 )
    set( CMAKE_CXX_STANDARD 11 )
else()
    remove_definitions( -DUSE_CPP11 )
    remove_definitions( -DUSE_CPP14 )
    set( CMAKE_CXX_STANDARD 98 )
endif()

# build shared libraries
set( BUILD_SHARED_LIBS ON )

# global property to hold the names of nymph library targets
set_property( GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES )

# turn on RPATH for Mac OSX
set( CMAKE_MACOSX_RPATH ON )

# add the library install directory to the rpath
SET(CMAKE_INSTALL_RPATH "${LIB_INSTALL_DIR}" )

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

set( LIB_POSTFIX )
set( INC_PREFIX )

# in windows, disable the min and max macros
if( WIN32 )
    # disable the min and max macros
    add_definitions( -DNOMINMAX )
	# libraries and linking
	set( CMAKE_SHARED_LIBRARY_PREFIX "" )
	set( CMAKE_IMPORT_LIBRARY_PREFIX "" )
	set( CMAKE_LINK_DEF_FILE_FLAG "" )
	add_definitions( -DBOOST_ALL_DYN_LINK )
endif( WIN32 )

##########
# MACROS #
##########

# Conveniece function for overriding the value of an option (aka a cached bool variable)
macro( set_option VARIABLE VALUE )
    set( ${VARIABLE} ${VALUE} CACHE BOOL "" FORCE )
endmacro()

# This should be called immediately after setting the project name
macro( pbuilder_prepare_project )
    # default package name is the project name
    set( ${PROJECT_NAME}_PACKAGE_NAME "${PROJECT_NAME}" )

    # Full project name, expanded according to the submodule hierarchy
    pbuilder_expand_lib_name( ${PROJECT_NAME} )
    set( ${PROJECT_NAME}_FULL_PROJECT_NAME ${FULL_LIB_NAME} )
    message( STATUS "?????? Full project name: ${PROJECT_NAME}_FULL_PROJECT_NAME = ${${PROJECT_NAME}_FULL_PROJECT_NAME}" )

    # if git is used, get the commit SHA1
    find_package( Git )
    if( GIT_FOUND )
        # check whether this is a git repo
        execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --is-inside-git-dir WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE IS_GIT_REPO )
        if( IS_GIT_REPO )
            execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_COMMIT  OUTPUT_STRIP_TRAILING_WHITESPACE )
            execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --long  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE ${PROJECT_NAME}_GIT_DESCRIBE  OUTPUT_STRIP_TRAILING_WHITESPACE )
            execute_process( COMMAND ${GIT_EXECUTABLE} remote get-url origin WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE GIT_ORIGIN OUTPUT_STRIP_TRAILING_WHITESPACE )
            if( GIT_ORIGIN )
                message( STATUS "Git origin: ${GIT_ORIGIN}")
                string( REGEX MATCH "[a-zA-Z0-9_-]+/[a-zA-Z0-9_-]+$" GIT_PACKAGE ${GIT_ORIGIN} )
            else( GIT_ORIGIN )
                execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE TOP_DIR OUTPUT_STRIP_TRAILING_WHITESPACE )
                execute_process( COMMAND basename ${TOP_DIR} OUTPUT_VARIABLE GIT_PACKAGE )
            endif( GIT_ORIGIN )
            message( STATUS "Git package: ${GIT_PACKAGE}" )

            # override package name with git package
            set( ${PROJECT_NAME}_PACKAGE_NAME "${GIT_PACKAGE}" )
        endif( IS_GIT_REPO )
    endif( GIT_FOUND )

    # define the variables to describe the package (will go in the [ProjectName]Config.hh file)
    set( ${PROJECT_NAME}_PACKAGE_STRING "${PROJECT_NAME} ${${PROJECT_NAME}_VERSION}" )
endmacro()

macro( pbuilder_add_submodule SM_NAME SM_LOCATION )
    if( NOT IS_ABSOLUTE ${SM_LOCATION} )
        set( SM_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${SM_LOCATION}" )
    endif()

    # Conditions that let us in this loop:
    #  1. Submodule SM_NAME has not yet been found
    #  2. This is the location of the submodule SM_NAME
    message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
    message( STATUS "SM_LOCATION: ${SM_LOCATION}" )
    if( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${SM_LOCATION}" )
        message( "Adding submodule ${SM_NAME} in location ${SM_LOCATION}" )

        set( ${SM_NAME}_FOUND TRUE CACHE INTERNAL "" )
        set( ${SM_NAME}_LOCATION ${SM_LOCATION} CACHE INTERNAL "" )

        # Determine the library name suffix for this submodule with respect to its parent if it's not already defined
        if( NOT DEFINED ${SM_NAME}_PARENT_LIB_NAME_SUFFIX )
            set( ${SM_NAME}_PARENT_LIB_NAME_SUFFIX "_${PROJECT_NAME}${${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX}" CACHE INTERNAL "Scoped library name suffix for submodule ${SM_NAME}" )
            message( STATUS "PARENT_LIB_NAME_SUFFIX being set for SM ${SM_NAME}: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}" )
        endif()

        # Set submodule include subdirectory
        set( SM_INCLUDE_SUBDIR "/${SM_NAME}" )
        message( STATUS "Include files for submodule ${SM_NAME} will be installed in ${TOP_PROJECT_INCLUDE_INSTALL_DIR}${SM_INCLUDE_SUBDIR}" )

        # Set CMake config subdirectory
        set( SM_CMAKE_CONFIG_SUBDIR "/${SM_NAME}" )
        message( STATUS "CMake config files for submodule ${SM_NAME} will be installed in ${TOP_PROJECT_CMAKE_CONFIG_DIR}${SM_CMAKE_CONFIG_SUBDIR}" )

        message( STATUS "SM ${SM_NAME} cached variables:" )
        message( STATUS "${SM_NAME}_FOUND: ${${SM_NAME}_FOUND}" )
        message( STATUS "${SM_NAME}_LOCATION: ${${SM_NAME}_LOCATION}" )
        message( STATUS "${SM_NAME}_PARENT_LIB_NAME_SUFFIX: ${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}")

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

    endif( NOT ${SM_NAME}_FOUND OR "${${SM_NAME}_LOCATION}" STREQUAL "${SM_LOCATION}" )
endmacro()

### Updated for Scarab3
macro( pbuilder_expand_lib_name_2 LIB_NAME SM_NAME )
    set( FULL_LIB_NAME "${LIB_NAME}${${SM_NAME}_PARENT_LIB_NAME_SUFFIX}" )
endmacro()

### Updated for Scarab3
macro( pbuilder_expand_lib_name LIB_NAME )
    pbuilder_expand_lib_name_2( ${LIB_NAME} ${PROJECT_NAME} )
endmacro()

### Updated for Scarab3
macro( pbuilder_expand_lib_names )
    # Supply library targets as additional macro arguments
    set( PROJECT_LIBRARIES ${ARGN} )
    message( STATUS "expanding these project libraries: ${PROJECT_LIBRARIES}" )
    set( FULL_PROJECT_LIBRARIES )
    foreach( project_lib ${PROJECT_LIBRARIES} )
        pbuilder_expand_lib_name( ${project_lib} )
        list( APPEND FULL_PROJECT_LIBRARIES ${FULL_LIB_NAME} )
    endforeach( project_lib )
    message( STATUS "expanded to full project library names: ${FULL_PROJECT_LIBRARIES}" )
endmacro()

### Updated for Scarab3
macro( pbuilder_use_sm_library LIB_NAME SM_NAME )
    pbuilder_expand_lib_name_2( ${LIB_NAME} ${SM_NAME} )
    list( APPEND ${PROJECT_NAME}_SM_LIBRARIES ${FULL_LIB_NAME} )
    message( STATUS "Added SM library ${FULL_LIB_NAME} to ${PROJECT_NAME}_SM_LIBRARIES" )
endmacro()

### Updated for Scarab3
macro( pbuilder_library LIB_BASENAME SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES )
    #message( STATUS "Building library ${LIB_BASENAME}; PARENT_LIB_NAME_SUFFIX is ${PARENT_LIB_NAME_SUFFIX}; ${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX is ${${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX}" )
    #set( FULL_LIB_NAME "${LIB_BASENAME}${${PROJECT_NAME}_PARENT_LIB_NAME_SUFFIX}" )
    pbuilder_expand_lib_name( ${LIB_BASENAME} )
    #message( STATUS "lib basename: ${LIB_BASENAME}" )
    message( STATUS "full lib name: ${FULL_LIB_NAME}" )
    message( STATUS "SM libraries (public): ${${PROJECT_NAME}_SM_LIBRARIES}" )
    message( STATUS "external libraries (public): ${${PUBLIC_EXTERNAL_LIBRARIES}}" )
    message( STATUS "external libraries (private): ${${PRIVATE_EXTERNAL_LIBRARIES}}" )

    pbuilder_expand_lib_names( ${${PROJECT_LIBRARIES}} )
    message( STATUS "full project libraries (lib): ${FULL_PROJECT_LIBRARIES}" )

    message( STATUS "pbuilder: will build library <${FULL_LIB_NAME}>" )
    add_library( ${FULL_LIB_NAME} ${${SOURCES}} )
    ###target_compile_options( ${FULL_LIB_NAME} INTERFACE ${GLOBAL_COMPILE_OPTIONS} )

    # Grab the include directories, which will be used for the build-interface target includes
    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${FULL_LIB_NAME} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )

    # this will set the INTERFACE_INCLUDE_DIRECTORIES property using the INTERFACE option
    # it's assumed that the include_directories() command was used to set the INCLUDE_DIRECTORIES property for the private side.
    target_include_directories( ${FULL_LIB_NAME} 
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${TOP_PROJECT_INCLUDE_INSTALL_SUBDIR}${SM_INCLUDE_SUBDIR}>"
    )

    target_link_libraries( ${FULL_LIB_NAME} 
        PUBLIC
            ${${PROJECT_NAME}_SM_LIBRARIES} ${FULL_PROJECT_LIBRARIES} ${${PUBLIC_EXTERNAL_LIBRARIES}}
        PRIVATE
            ${${PRIVATE_EXTERNAL_LIBRARIES}} 
    )

    message( STATUS "Resetting ${PROJECT_NAME}_SM_LIBRARIES" )
    set( ${PROJECT_NAME}_SM_LIBRARIES )

    # Make exported targets available during the build phase
    message( STATUS "####### build-phase targets file going to: ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake" )
    export( TARGETS ${FULL_LIB_NAME}
        FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
    )

    pbuilder_install_libraries( ${FULL_LIB_NAME} )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_libraries )
    # Unspecified arguments: any libraries to install
    #message( STATUS "installing libs: ${ARGN}" )
    install( TARGETS ${ARGN} 
        EXPORT ${PROJECT_NAME}Targets 
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_executables SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES )
    ###message( STATUS "programs: ${${PROGRAMS}}" )
    message( STATUS "executable source files: ${${SOURCES}}" )
    message( STATUS "project libraries: ${${PROJECT_LIBRARIES}}" )
    message( STATUS "external libraries (public): ${${PUBLIC_EXTERNAL_LIBRARIES}}" )
    message( STATUS "external libraries (private): ${${PRIVATE_EXTERNAL_LIBRARIES}}" )

    foreach( source ${${SOURCES}} )
        get_filename_component( program ${source} NAME_WE )
        if( NOT TARGET ${program} )
            pbuilder_executable( ${program} source ${PROJECT_LIBRARIES} ${PUBLIC_EXTERNAL_LIBRARIES} ${PRIVATE_EXTERNAL_LIBRARIES} )
        else()
            message( FATAL "Duplicate target: ${program}" )
        endif()
    endforeach( source )
endmacro()

### Updated for Scarab3
macro( pbuilder_executable THIS_PROGRAM SOURCES PROJECT_LIBRARIES PUBLIC_EXTERNAL_LIBRARIES PRIVATE_EXTERNAL_LIBRARIES )
    # Builds a single executable from one or more source files
    message( STATUS "pbuilder will build executable ${THIS_PROGRAM} with ${${SOURCES}}" )
    message( STATUS "\tlinking with: (project) ${${PROJECT_LIBRARIES}} -- (public ext) ${${PUBLIC_EXTERNAL_LIBRARIES}} -- (private ext) ${${PRIVATE_EXTERNAL_LIBRARIES}}")

    add_executable( ${THIS_PROGRAM} ${${SOURCES}} )

    pbuilder_expand_lib_names( ${${PROJECT_LIBRARIES}} )
    message( STATUS "full project libraries (exe): ${FULL_PROJECT_LIBRARIES}" )

    target_link_libraries( ${THIS_PROGRAM} 
        PUBLIC
            ${FULL_PROJECT_LIBRARIES} ${${PUBLIC_EXTERNAL_LIBRARIES}}
        PRIVATE
            ${${PRIVATE_EXTERNAL_LIBRARIES}} 
    )

    export( TARGETS ${THIS_PROGRAM}
        FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
    )

    pbuilder_install_executables( ${THIS_PROGRAM} )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_executables )
    install( TARGETS ${ARGN} 
        EXPORT ${PROJECT_NAME}Targets 
        RUNTIME DESTINATION ${BIN_INSTALL_DIR} 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_headers )
    #message( STATUS "installing headers in ${INCLUDE_INSTALL_DIR}${${PROJECT_NAME}_PARENT_INC_DIR_PATH}" )
    install( FILES ${ARGN} 
        DESTINATION ${TOP_PROJECT_INCLUDE_INSTALL_DIR}${SM_INCLUDE_SUBDIR}
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_header_dirs FILE_PATTERN )
    install( DIRECTORY ${ARGN} 
        DESTINATION ${INCLUDE_INSTALL_DIR}${${PROJECT_NAME}_PARENT_INC_DIR_PATH} 
        FILES_MATCHING PATTERN "${FILE_PATTERN}" 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_config )
    install( FILES ${ARGN} 
        DESTINATION ${CONFIG_INSTALL_DIR} 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_data )
    install( FILES ${ARGN} 
        DESTINATION ${DATA_INSTALL_DIR} 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_install_files DEST_DIR )
    install( FILES ${ARGN} 
        DESTINATION ${DEST_DIR} 
    )
endmacro()

### Updated for Scarab3
macro( pbuilder_do_package_config )
    # This macro sets up and installs the configuration files used tospecify the install information for the project.
    # It installs an already-created "config" file.
    # It creates and installs the "version-config" and "targets" files.
    # Arguments: 
    #   
    #   CONFIG_LOCATION -- Directory in which to find the already-created config file.
    #                      If not specified, the default is ${PROJECT_BINARY_DIR}.
    #   FILE_PREFIX -- Portion of the filename that preceeds `Config.cmake`, `Targets.cmake`, and `ConfigVersion.cmake` for the 
    #                  config, targets, and config-version files, respectively.  If not specified, the default is ${PROJECT_NAME}.
    #   CONFIG_FILENAME -- Optional specification of the full config filename.  If specified, overrules the default described above.
    #   TARGETS_FILENAME -- Optional specification of the full targets filename.  If specified, overrules the default described above.
    #   VERSION_FILENAME -- Optional specification of the full version-config filename.  If specified, overrules the default described above.

    # Parse macro arguments
    set( oneValueArgs CONFIG_LOCATION FILE_PREFIX CONFIG_FILENAME TARGETS_FILENAME VERSION_FILENAME )
    cmake_parse_arguments( PKG_CONF "" "${oneValueArgs}" "" ${ARGN} )

    # Handle arguments and apply defaults

    if( NOT PKG_CONF_CONFIG_LOCATION )
        set( PKG_CONF_CONFIG_LOCATION ${PROJECT_BINARY_DIR} )
    endif()

    if( NOT PKG_CONF_FILE_PREFIX )
        set( PKG_CONF_FILE_PREFIX ${PROJECT_NAME} )
    endif()

    if( NOT PKG_CONF_CONFIG_FILENAME )
        set( PKG_CONF_CONFIG_FILENAME ${PKG_CONF_FILE_PREFIX}Config.cmake )
    endif()
    set( CONFIG_PATH ${PKG_CONF_CONFIG_LOCATION}/${PKG_CONF_CONFIG_FILENAME} )
    message( STATUS "Config file path: ${CONFIG_PATH}" )

    if( NOT PKG_CONF_TARGETS_FILENAME )
        set( PKG_CONF_TARGETS_FILENAME ${PKG_CONF_FILE_PREFIX}Targets.cmake )
    endif()

    if( NOT PKG_CONF_VERSION_FILENAME )
        set( PKG_CONF_VERSION_FILENAME ${PKG_CONF_FILE_PREFIX}ConfigVersion.cmake )
    endif()
    set( CONFIG_VERSION_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PKG_CONF_VERSION_FILENAME} )
    message( STATUS "Config version file path: ${CONFIG_VERSION_PATH}" )

    # Config file must exist already
    if( NOT EXISTS ${CONFIG_PATH} )
        message( FATAL_ERROR "Package config file does not exist: ${CONFIG_PATH}" )
    endif()

    install( EXPORT ${PROJECT_NAME}Targets
        FILE
            ${PKG_CONF_TARGETS_FILENAME}
        NAMESPACE
            ${PROJECT_NAME}::
        DESTINATION
            ${TOP_PROJECT_CMAKE_CONFIG_DIR}${SM_CMAKE_CONFIG_SUBDIR}
    )

    include( CMakePackageConfigHelpers )
    write_basic_package_version_file(
        ${CONFIG_VERSION_PATH}
        COMPATIBILITY SameMajorVersion
    )

    install( 
        FILES 
            ${CONFIG_VERSION_PATH}
            ${CONFIG_PATH}
        DESTINATION 
            ${TOP_PROJECT_CMAKE_CONFIG_DIR}${SM_CMAKE_CONFIG_SUBDIR}
    )
endmacro()

#[[macro( pbuilder_variables_for_parent )
    if( NOT ${PBUILDER_STANDALONE} )
        set( ${PROJECT_NAME}_FOUND TRUE CACHE INTERNAL "" )
        set( ${PROJECT_NAME}_LOCATION ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "" )
        get_property( LIBRARIES GLOBAL PROPERTY ${PROJECT_NAME}_LIBRARIES )
        set( ${PROJECT_NAME}_LIBRARIES ${LIBRARIES} ${SUBMODULE_LIBRARIES} CACHE INTERNAL "" )
        set( ${PROJECT_NAME}_LIBRARY_DIR ${LIB_INSTALL_DIR} CACHE INTERNAL "" )
        set( ${PROJECT_NAME}_INCLUDE_DIR ${INCLUDE_INSTALL_DIR} CACHE INTERNAL "" )
        get_property( DEP_INCLUDE_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )
        list( REMOVE_DUPLICATES DEP_INCLUDE_DIRS )
        set( ${PROJECT_NAME}_DEP_INCLUDE_DIRS ${DEP_INCLUDE_DIRS} CACHE INTERNAL "" )
        get_directory_property( COMPILE_DEFINITIONS COMPILE_DEFINITIONS )
        set( ${PROJECT_NAME}_COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS} CACHE INTERNAL "" )
    endif( NOT ${PBUILDER_STANDALONE} )
endmacro()]]

macro( pbuilder_add_pybind11_module PY_MODULE_NAME PROJECT_LIBRARIES )
    # Adds a pybind11 module that is linked to the specified project libraries, PUBLIC_EXT_LIBS, and PRIVATE_EXT_LIBS
    # Installs the library in the standard lib directory

    pbuilder_expand_lib_names( ${PROJECT_LIBRARIES} )

    set( PROJECT_INCLUDE_DIRS )
    foreach( LIBRARY ${FULL_PROJECT_LIBRARIES} )
        get_target_property( LIB_INCLUDE_DIRS ${LIBRARY} INTERFACE_INCLUDE_DIRECTORIES )
        list( APPEND PROJECT_INCLUDE_DIRS ${LIB_INCLUDE_DIRS} )
    endforeach()
    list( REMOVE_DUPLICATES PROJECT_INCLUDE_DIRS )
    #message( STATUS "Main project library include directories: ${PROJECT_INCLUDE_DIRS}")

    include_directories( ${PROJECT_INCLUDE_DIRS} )

    # Potential point of confusion: the C++ library is "Scarab" and the python library is "scarab"
    # Other possible naming schemes seemed less desirable, and we'll hopefully avoid confusion with these comments
    pybind11_add_module( ${PY_MODULE_NAME} ${PYBINDING_SOURCEFILES} )

    get_target_property( SOURCE_TREE_INCLUDE_DIRS ${PY_MODULE_NAME} INCLUDE_DIRECTORIES )
    message( STATUS "Adding install interface include dir: ${INCLUDE_INSTALL_SUBDIR}" )
    message( STATUS "Adding build interface include dirs: ${SOURCE_TREE_INCLUDE_DIRS}" )

    target_include_directories( ${PY_MODULE_NAME}
        INTERFACE 
            "$<BUILD_INTERFACE:${SOURCE_TREE_INCLUDE_DIRS}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${INCLUDE_INSTALL_SUBDIR}>"
    )

    target_link_libraries( ${PY_MODULE_NAME} PRIVATE ${${PROJECT_NAME}_SM_LIBRARIES} ${FULL_PROJECT_LIBRARIES} ${PUBLIC_EXT_LIBS} ${PRIVATE_EXT_LIBS} )
    install( TARGETS ${PY_MODULE_NAME} DESTINATION ${LIB_INSTALL_DIR} )
endmacro()
