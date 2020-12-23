Package Builder
===============

What is the Package Builder (PB)?
---------------------------------

Package Builder (PB) is a framework built on top of CMake for putting together software packages that specializes in nesting packages.

PB is implemented in a series of CMake macros that standardize how things like building a library, or building an executable.  
Part of the motivation behind PB is to simplify the process of making a software package.  

While CMake is extremely powerful, it has a lot of options for how to make a software package, 
and those choices can make the process more complicated.  PB tries to find a particular balance between how complicated or 
simple it is to build a package, and how many choices are left available to the user or are made by PB.  
For cases not handled by the vanilla PB interface, the user always has the option to use the raw CMake interface.

Submodules
^^^^^^^^^^

The nesting infrastructure in PB assumes one is using git submodules, or something similar.  
It assumes packages will use submodules for internal (i.e. within the same codebase) dependencies.  
For example, the Scarab package in which PB lives is the base utility package for all of the C++ packages used by Project 8, 
and Scarab is included as a submodule (or as a nested submodule) in all of the organization's C++ packages.


Requirements
------------

PB requires CMake v3.12 because it uses FindPython3.


Basic Usage
-----------

This section will explain the basic usage of PB by setting up an example software package called Insecta.


Package Structure
^^^^^^^^^^^^^^^^^

Since Insecta is a PB-based package, it requires Scarab.  So Scarab should be setup as a submodule of Insecta.  
The file structure should look like this::

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


Install Structure
^^^^^^^^^^^^^^^^^

PB uses the variable ``CMAKE_INSTALL_PREFIX`` to control where headers/libraries/binaries/etc are installed.  
The default install prefix is set to ``${PROJECT_BINARY_DIR}``.

A number of different types of installed files are handled specifically by PB:

- Headers are installed in ``<prefix>/${INCLUDE_INSTALL_SUBDIR}``
- Libraries are installed in ``<prefix>/${LIB_INSTALL_SUBDIR}``
- CMake package configuration files are installed in ``<prefix>/${PACKAGE_CONFIG_SUBDIR}``
- Executables are installed in ``<prefix>/${BIN_INSTALL_SUBDIR}``
- Runtime configuration files are installed in ``<prefix>/${CONFIG_INSTALL_SUBDIR}``
- Data files are installed in ``<prefix>/${DATA_INSTALL_SUBDIR}``

Default subdirectories are:

- ``INCLUDE_INSTALL_SUBDIR``: ``include/${PROJECT_NAME}``
- ``LIB_INSTALL_SUBDIR`` (non-Windows): ``lib``
- ``LIB_INSTALL_SUBDIR`` (Windows): ``bin``
- ``PACKAGE_CONFIG_SUBDIR``: ``${LIB_INSTALL_SUBDIR}/cmake/${PROJECT_NAME}``
- ``BIN_INSTALL_SUBDIR``: ``bin``
- ``CONFIG_INSTALL_SUBDIR``: ``config``
- ``DATA_INSTALL_SUBDIR``: ``data``


Initial Setup
^^^^^^^^^^^^^

The initial part of the CMakeList file should be setup as follows::

    cmake_minimum_version( VERSION 3.12 )  # Minimum CMake version mentioned above
    cmake_policy( SET CMP0048 NEW )  # Package version to be specified in the project() command (optional but recommended)
    project( Insecta VERSION 1.0.0 )  # Setup the package.  Package name is `Insecta`

    list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake )  # This is required so that PB can be included
    include( PackageBuilder )  # Loads the PackageBuilder.cmake file

    # Create options or set options from submodules

    # Find other dependencies

    pbuilder_prepare_project()  # Processes PB options

    pbuilder_add_submodule( Scarab Scarab )  # Insecta will use the Scarab library.  See below for more details on this macro.

Building a Library
^^^^^^^^^^^^^^^^^^

Depending on the size and organization of a project, the library definition might be in the main CMakeList file, 
or it might be in a subdirectory with its own CMakeList file.

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

    # This function adds the library, sets the include directories as a target property and links the library.
    pbuilder_library( 
        TARGET Insecta 
        SOURCES ${Insecta_SOURCES}
        PACKAGE_LIBRARIES ${PACKAGE_LIBS}
        PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS} 
    )

    # If the build does not actually use components, but this function is being used multiple times in the project (we'll use it below), 
    # then specifying a component is required.  Here, since we're building the library, we'll call it ``Library``.
    pbuilder_component_install_and_export( 
        COMPONENT Library
        LIBTARGETS Insecta
    )

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
        set( programs )
        pbuilder_executables( 
            TARGETS_VAR programs
            SOURCES ${Insecta_SOURCES}
            PACKAGE_LIBRARIES ${PACKAGE_LIBS}
            PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
            PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS} 
        )

    # If the build does not actually use components, but this function is being used multiple times in the project (we used it above), 
    # then specifying a component is required.  Here, since we're building the executables, we'll call it ``Library``.
    pbuilder_component_install_and_export( 
        COMPONENT Executables
        LIBTARGETS ${programs}
    )

    endif( Insecta_ENABLE_EXECUTABLES )

Nesting with Submodules
-----------------------

PB was designed with submodules in mind.  It takes care of avoiding conflicts between repeated submodules (i.e. diamond dependency pattern).  
It also avoids conflicting library names and include installation between packages with the same submodules, as is explained in the following sections.

Submodules are added with a PB macro::

    pbuilder_add_submodule( [submodule package name], [submodule location] )

This macro will make PB aware of the submodule, and take care of determining the names and install locations as mentioned below.


Repeated Submodules
^^^^^^^^^^^^^^^^^^^

Imagine a situation where Package A includes packages B and C as submodules, and both B and C include package D as a submodule.  
PB defines which version of package D is used: whichever is encountered by CMake first.  So if A's CMakeList file includes this::

    pbuilder_add_submodule( B B )
    pbuilder_add_submodule( C C )

then package A's version of D will be used.  It's up to the developer to ensure that the version of D is usable by packages A, B, and C.

Library Names
^^^^^^^^^^^^^

PB manages library file names to avoid conflicts between libraries installed as submodules of different packages.  
For example, if both packages B and C have package D as a submodule, they both would generate libD.so (assuming a system that uses .so libraries).  
So PB renames the library file according to the parent packages.  If a user builds and installs packages B and C, 
if they install to a common location, they will find these libraries:

- ``libB.so``
- ``libD_B.so``
- ``libC``
- ``libD_C.so``

They could also separately build and install package D, which would build ``libD.so``.

PB supports arbitrary levels of nesting for library names.  In the example in the section above, package A would build these libraries:

- ``libA.so``
- ``libB_A.so``
- ``libD_B_A.so``
- ``libC_A.so``


Include Install Directories
^^^^^^^^^^^^^^^^^^^^^^^^^^^

PB manages include install directories to avoid conflicts between headers installed as submodules of different packages.  
Headers from all submodules of a parent project are installed in subdirectories of the parent project's header install directory.

Continuing the A/B/C/D example from above, the include directories would be structured like this::

    <prefix>
    |
    + include
      |
      + A
        |
        + <A's header files>
        |
        + B
        | + <B's header files>
        |
        + C
        | + <C's header files>
        |
        + D
          + <D's header files>

Unlike with the library naming, the submodule include-directory structure does not follow the structure of the submodule nesting: 
all submodules are setup as subdirectories of the top parent project's include directory.  Unlike with libraries, 
which tend to be installed in a single directory (if using a common install prefix), header files are installed in 
project-specific subdirectories of the ``include`` directory, so submodule include subdirectories will avoid conflicts by being 
all within the parent project's directory.


CMake Configure Scripts
^^^^^^^^^^^^^^^^^^^^^^^

Please refer to the next section.


CMake Configuration Setup
-------------------------

In modern CMake usage, package configurations for installed packages are discovered by loading ``.cmake`` files describing the installed package, 
rather than by using ``FindPackage`` scripts.  PB supports the creation of CMake configuration scripts.  
PB uses one of the standard configuration-file locations for installing the package-config files:  ``<prefix>/${LIB_INSTALL_SUBDIR}/cmake/${PROJECT_NAME}``.

The package author is responsible for creating a configurable package-config file (apologies for the confusing wording).  
The file is "configurable" in that CMake will customize the file using the ``configure_file()`` command during the CMake configure stage.  
The user should include this in their main CMakeList file::

    configure_file( ${PROJECT_SOURCE_DIR}/[project name]Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/[project name]Config.cmake @ONLY )

Then the user calls a PB macro that will install the above configuration file, and create configuration files that 
describe the version and targets of the package::

    pbuilder_do_package_config()

Here is an example of a simple package-config template file, taken from the PBTest package that comes with Scarab (in the ``testing`` directory)::

    # PBTestConfig.cmake

    get_filename_component( PBTest_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH )

    # Find the dependencies
    include( CMakeFindDependencyMacro )
    find_dependency( Scarab REQUIRED PATHS ${PBTest_CMAKE_DIR}/Scarab @Scarab_BINARY_LOCATION@ )

    # Import targets if they're not already present
    # This nested setup allows the import to be used both in the build tree (i.e. as a submodule) and after installation
    if( NOT TARGET PBTest::@PBTest_FULL_PROJECT_NAME@ )
        if( TARGET @PBTest_FULL_PROJECT_NAME@ )
            add_library( PBTest::@PBTest_FULL_PROJECT_NAME@ ALIAS @PBTest_FULL_PROJECT_NAME@ )
        else()
            include("${PBTest_CMAKE_DIR}/PBTestTargets.cmake")
        endif()
    endif()

For packages that include PB-based submodules, those are considered dependencies in this context.  
In the above example, Scarab is a subumodule of PBTest, and it required to be found by PBTest.  
It provides two path hints: ``${PBTest_CMAKE_DIR}/Scarab`` for occasions when PBTest has been installed, and ``@Scarab_BINARY_LOCATION@`` 
for occasions when PBTest is itself being used as a submodule.

The last section ensures that all of the expected library targets are present.  If they're not there, then usually the ``Targets`` 
config file is included.  If, for some reason, the library target is present but not with the expected namespace, 
then an alias target with the namespace is created.  For a project with multiple libraries, only one ``Targets`` 
config file would be included, but multiple alias libraries would need to be created, one per real library.
