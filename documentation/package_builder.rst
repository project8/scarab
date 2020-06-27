Package Builder
===============

What is the Package Builder (PB)?
---------------------------------

Package Builder (PB) is a framework built on top of CMake for putting together software packages that specializes in nesting packages.

PB is implemented in a series of CMake macros that standardize how things like building a library, or building an executable.  Part of the motivation behind PB is to simplify the process of making a software package.  

While CMake is extremely powerful, it has a lot of options for how to make a software package, and those choices can make the process more complicated.  PB tries to find a particular balance between how complicated or simple it is to build a package, and how many choices are left available to the user or are made by PB.  For cases not handled by the vanilla PB interface, the user always has the option to use the raw CMake interface.

Submodules
^^^^^^^^^^

The nesting infrastructure in PB assumes one is using git submodules, or something similar.  It assumes packages will use submodules for internal (i.e. within the same codebase) dependencies.  For example, the Scarab package in which PB lives is the base utility package for all of the C++ packages used by Project 8, and Scarab is included as a submodule (or as a nested submodule) in all of the organization's C++ packages.


Requirements
------------

PB requires CMake v3.1 because it uses ``CMAKE_CXX_STANDARD``.


Basic Usage
-----------

This section will explain the basic usage of PB by setting up an example software package called Insecta.

Package Structure
^^^^^^^^^^^^^^^^^

Since Insecta is a PB-based package, it requires Scarab.  So Scarab should be setup as a submodule of Insecta.  The file structure should look like this::

    Insecta
    |
    + Scarab
    |  |
    |  + cmake
    |  |  | . . .
    |  |
    |  + library
    |  |  | . . .
    |  |
    |  + CMakeLists.txt
    |
    + Source
    |  | . . .
    |
    + CMakeLists.txt
    
Note that the Scarab directory listing is incomplete, and there are probably going to be more files and directories in Insecta as well.


Initial Setup
^^^^^^^^^^^^^

The initial part of the CMakeList file should be setup as follows::

    cmake_minimum_version( VERSION 3.1 )  # Minimum CMake version mentioned above
    cmake_policy( SET CMP0048 NEW )  # Package version to be specified in the project() command (optional but recommended)
    project( Insecta VERSION 1.0.0 )  # Setup the package.  Package name is `Insecta`

    list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake )  # This is required so that PB can be included
    include( PackageBuilder )  # Loads the PackageBuilder.cmake file

    pbuilder_prepare_project()  # Sets up the PB environment

    pbuilder_add_submodule( Scarab Scarab )  # Insecta will use the Scarab library.  See below for more details on this macro.

Building a Library
^^^^^^^^^^^^^^^^^^

Depending on the size and organization of a project, the library definition might be in the main CMakeList file, or it might be in a subdirectory with its own CMakeList file.

Directory inclusion should be setup in the main CMakeList file::

    include_directories( BEFORE Source )

The remainder should go wherever the library is being defined::

    pbuilder_use_sm_library( Scarab Scarab )  # This library depends on the Scarab library;
    # The first argument is the submodule name
    # The second argument is the target name for the library

    set( PACKAGE_LIBS )  # If there were other libraries in this package on which this library depends, then they would be put in this variable
    set( PUBLIC_EXT_LIBS )  # If there were public external dependencies, the targets would go here
    set( PRIVATE_EXT_LIBS )  # If there were private external dependencies, the targets would go here

    # It's convenient to put the header and source files in their own respective lists
    set( Insecta_HEADERS 
        file1.hh
        file2.hh
    )

    set( Insecta_SOURCES
        file1.cc
        file2.cc
    )

    # Variables storing library dependencies are passed as variables, not variable values
    # This function adds the library, sets the include directories as a target property, links the library, exports the target, and installs the library.
    pbuilder_library( Insecta Insecta_SOURCES PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

    # Headers are passed as a list, so we pass the value of the HEADERS variable
    # This function installs the headers
    pbuilder_install_headers( ${Insecta_HEADERS} )


Building an Executable
^^^^^^^^^^^^^^^^^^^^^^

CMake provides a flag variable ``${PROJECT_NAME}_ENABLE_EXECUTABLES`` that should be used to control whether executables are built.

For the Insecta project, the executable section of the build (again, in its own file or in the main CMakeList file) looks like this::

    if( Insecta_ENABLE_EXECUTABLES )

        # Package libraries required by these executables
        set( PACKAGE_LIBS
            Insecta
        )
        # Public external dependencies
        set( PUBLIC_EXT_LIBS )  # Public external dependencies
        set( PRIVATE_EXT_LIBS )  # Private external dependencies

        # Storing the relevant source files in a variable
        set( Insecta_SOURCES
            executable1.cc
            executable2.cc
        )

        # All variables are passed as variables, not their contents
        # This will create the executables and link it
        pbuilder_executables( Insecta_SOURCES PACKAGE_LIBS PUBLIC_EXT_LIBS PRIVATE_EXT_LIBS )

    endif( Insecta_ENABLE_EXECUTABLES )

Nesting with Submodules
-----------------------

PB was designed with submodules in mind.  It takes care of avoiding conflicts between repeated submodules (i.e. diamond dependency pattern).  It also avoids conflicting library names and include installation between packages with the same submodules, as is explained in the following sections.

Submodules are added with a PB macro::

    pbuilder_add_submodule( [submodule package name], [submodule location] )

This macro will make PB aware of the submodule, and take care of determining the names and install locations as mentioned below.


Repeated Submodules
^^^^^^^^^^^^^^^^^^^

Imagine a situation where Package A includes packages B and C as submodules, and both B and C include package D as a submodule.  PB defines which version of package D is used: whichever is encountered by CMake first.  So if A's CMakeList file includes this::

    pbuilder_add_submodule( B B )
    pbuilder_add_submodule( C C )

then package A's version of D will be used.  It's up to the developer to ensure that the version of D is usable by packages A, B, and C.

Library Names
^^^^^^^^^^^^^

Include Install Directories
^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake Configure Scripts
^^^^^^^^^^^^^^^^^^^^^^^

Please refer to the next section.


CMake Configuration Setup
-------------------------